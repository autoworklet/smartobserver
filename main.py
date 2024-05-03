import json
from datetime import datetime
from pathlib import Path
import threading
from kivy.uix.textinput import TextInput
from kivy.uix.button import Button
from kivy.uix.label import Label
from kivy.uix.screenmanager import ScreenManager, Screen, SlideTransition
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.gridlayout import GridLayout
from kivy.properties import ObjectProperty, BooleanProperty
from kivy.uix.widget import Widget
from kivy.app import App
from autoworklet.pubsub import PubSub
from autoworklet.recorder import Recorder
from autoworklet.replayer import replay
from autoworklet.speech.speech import StoppableSpeechThread
from uiohook import pyiohook as iohook
from kivy.clock import mainthread
from kivy.core.window import Window
from kivy.uix.popup import Popup
from kivy.uix.filechooser import FileChooserListView
from plyer import filechooser
import os
import bson
from kivymd.app import MDApp
from kivymd.uix.boxlayout import MDBoxLayout
from kivymd.uix.label import MDLabel

# Create a global PubSub instance
pubsub = PubSub()


def event_handler(event):
    pubsub.publish('event_received', event)


# Start the iohook event loop
iohook.start(event_handler)

def thread(function):
    def wrap(*args, **kwargs):
        t = threading.Thread(target=function, args=args, kwargs=kwargs, daemon=True)
        t.start()

        return t
    return wrap

class AutoWorklet(MDBoxLayout):
    status_label = ObjectProperty(None)
    replay_event_data = ObjectProperty(None)
    start_record_button = ObjectProperty(None)
    stop_record_button = ObjectProperty(None)
    replay_button = ObjectProperty(None)
    screenshot_image = ObjectProperty(None)
    generated_tasks = ObjectProperty(None)
    events_label = ObjectProperty(None)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.recorder = None
        self.recording = False
        self.replaying = False
        self.replay_thread = None

        pubsub.subscribe('event_received', self.on_event_received)

    @mainthread
    def start_stop_record_pressed(self):
        if self.replaying:
            return
        if self.recording:
            self.recording = False
            self.status_label.text = "Stopped"
            self.start_record_button.icon = "record"
            self.recorder.stop_screenshot_recorder()
            self.recorder.stop_generating()
        else:
            self.recorder = Recorder()
            self.recorder.start_screenshot_recorder()
            self.recorder.start_generating(self.update_generated_tasks)
            self.recording = True
            self.status_label.text = "Recording..."

            self.start_record_button.icon = "stop"

    def replay_interrupt_pressed(self):
        if self.recording:
            return
        if self.replaying:
            if self.replay_thread is not None and self.replay_thread.is_alive():
                self.replay_thread.stop()
                self.replay_thread.join()
        elif self.recorder is not None:
            self.replaying = True
            self.status_label.text = "Replaying..."
            self.replay_button.icon = "stop"
            events = self.recorder.get_events()
            self.replay_thread = replay(events, self.replay_finished)

    @mainthread
    def replay_finished(self):
        self.replaying = False
        self.status_label.text = "Finished"
        self.replay_button.icon = "play"
        self.replay_thread = None

    @mainthread
    def update_generated_tasks(self, tasks):
        self.generated_tasks.text = tasks

    def generate_pressed(self):
        if self.recorder is not None:
            self.recorder.generate(self.update_generated_tasks)
            
    def save_pressed(self):
        # Get the user's home desktop directory
        home_dir = os.path.expanduser("~/Desktop")
        # Generate file name with YYYY-MM-DD-HH-MM-SS format
        file_name = f"events-{datetime.now().strftime('%Y-%m-%d-%H-%M-%S')}.zip"
        file_path = os.path.join(home_dir, file_name)
        self.recorder.save(file_path)

    def load_pressed(self):
        path = filechooser.open_file(title="Pick a events zip file...",
                                     filters=[("Zip files", "*.zip")])
        if path:
            self.recorder = Recorder()
            self.recorder.load(path[0])

    @mainthread
    def update_events_label(self):
        self.events_label.text = str(self.recorder)

    def on_event_received(self, event):
        # print(json.dumps(event))
        # check if cmd+shift+R
        if event['type'] == 'EVENT_KEY_RELEASED' and event['mask'] == 3 and event['keycode'] == 19:
            self.replay_interrupt_pressed()
        # check if cmd+shift+S is pressed
        elif event['type'] == 'EVENT_KEY_RELEASED' and event['mask'] == 3 and event['keycode'] == 31:
            self.start_stop_record_pressed()

        elif self.recorder is not None and self.recording and not self.replaying:
            self.recorder.record(event)
            # update events label
            self.update_events_label()

        self.update_event_info(event)
        # events = self.recorder.get_events()
        # self.replay_event_data.text = "\n".join(
        #     [json.dumps(event) for event in events])

    @mainthread
    def update_event_info(self, event):
        # display the event info based on type
        if event['type'].startswith('EVENT_KEY'):
            self.replay_event_data.text = f"{event['type']}(time={event['time']}, keychar={event['keychar'] if 'keychar' in event else ''}, rawcode={event['rawcode'] if 'rawcode' in event else ''}, flags={event['flags'] if 'flags' in event else ''}, mask={event['mask'] if 'mask' in event else ''})"
        elif event['type'].startswith('EVENT_MOUSE_WHEEL'):
            self.replay_event_data.text = f"{event['type']}(time={event['time']}, x={event['x'] if 'x' in event else ''}, y={event['y'] if 'y' in event else ''}, wheelType={event['wheelType'] if 'wheelType' in event else ''}, direction={event['direction'] if 'direction' in event else ''}, amount={event['amount'] if 'amount' in event else ''}, rotation={event['rotation'] if 'rotation' in event else ''})"
        elif event['type'].startswith('EVENT_MOUSE'):
            self.replay_event_data.text = f"{event['type']}(time={event['time']}, x={event['x'] if 'x' in event else ''}, y={event['y'] if 'y' in event else ''}, mask={event['mask'] if 'mask' in event else ''}, button={event['button'] if 'button' in event else ''})"
        
        if 'screenshotPath' in event:
            # print(f"Screenshot saved to {event['screenshotPath']}")
            self.screenshot_image.source = event['screenshotPath']


class AutoWorkletApp(MDApp):
    def build(self):

        self.speech_thread = None
        # create a thread to start StoppableSpeechThread
        # self.speech_thread = StoppableSpeechThread()
        # self.speech_thread.start()

        Window.size = (800, 300)
        # position in the top right corner
        Window.top = 0
        Window.left = 0
        # Window.always_on_top = True
        # Window.borderless = True
        # transparent
        # Window.opacity = 0.8
        self.main = AutoWorklet()
        return self.main

    def on_pause(self):
        return True

    def on_resume(self):
        pass

    def on_stop(self):
        print("Stopping...")

        print("Stopping iohook")
        iohook.stop()
        print("Stopping pubsub")
        pubsub.clear()
        print("Stopping speech thread")
        if self.speech_thread is not None:
            self.speech_thread.stop()

        print("Stopped")
        return True


if __name__ == '__main__':
    AutoWorkletApp().run()
