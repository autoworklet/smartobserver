from libnut import pylibnut as nut
import os
import tempfile
import zipfile
from datetime import datetime

import threading
import time
from icecream import ic
from autoworklet.llm.gemini import generate

class ScreenShotRecorder:
    '''
    A class to record screenshots every F seconds to buffer of size N
    '''
    bufferDoNotRemove = []
    buffer = []
    def __init__(self, dirName, N, F):
        self.dirName = dirName
        self.N = N
        self.F = F
        self.stop_event = threading.Event()

    def __getitem__(self, key):
        # save to bufferDoNotRemove
        self.bufferDoNotRemove.append(self.buffer[key])
        return self.buffer[key]

    def start(self):
        self.stop_event.clear()
        self.thread = threading.Thread(target=self.run, args=())
        self.thread.start()

    def run(self):
        print("ScreenShotRecorder started")
        while not self.stop_event.is_set():
            self.screenshot()
            time.sleep(self.F)

    def screenshot(self):
        currentTime = datetime.now().strftime("%Y-%m-%d-%H-%M-%S")
        fileName = f"screenshot-{currentTime}.jpg"
        filePath = os.path.join(self.dirName.name, fileName)
        nut.captureScreenToFile(filePath)
        self.buffer.append(filePath)
        print("b:", self.buffer)
        if len(self.buffer) > self.N:
            # remove the oldest screenshot
            if self.buffer[0] not in self.bufferDoNotRemove:
                os.remove(self.buffer.pop(0))

    def stop(self):
        self.stop_event.set()
        self.thread.join()


class Recorder:
    def __init__(self):
        self._stop_generate_event = threading.Event()
        self._events = []
        # create temp directory to store screenshots
        self.dirName = tempfile.TemporaryDirectory()
        self.screenShotRecorder = ScreenShotRecorder(self.dirName, 10, 1)
        self.previous_list = ""

    def start_screenshot_recorder(self):
        self.screenShotRecorder.start()

    def stop_screenshot_recorder(self):
        self.screenShotRecorder.stop()

    def __del__(self):
        self.clear()

    def clear(self):
        self._events = []
        self.dirName.cleanup()

    def record(self, event):
        # if first event, save screenshot
        if len(self._events) == 0:
            if len(self.screenShotRecorder.buffer) > 0:
                event['screenshotPath'] = self.screenShotRecorder[-1]
            else:
                # stamp with time
                currentTime = datetime.now().strftime("%Y-%m-%d-%H-%M-%S")
                fileName = f"screenshot-{currentTime}.jpg"
                filePath = os.path.join(self.dirName.name, fileName)
                nut.captureScreenToFile(filePath)
                event['screenshotPath'] = filePath
        # if mouse event, save screenshot
        elif event['type'] == 'EVENT_MOUSE_PRESSED' or event['type'] == 'EVENT_MOUSE_RELEASED':
            # stamp with time
            # fileName = f"screenshot-{event['time']}.jpg"
            # filePath = os.path.join(self.dirName.name, fileName)
            # nut.captureScreenToFile(filePath)

            # get the latest screenshot before the click from the screenShotRecorder
            event['screenshotPath'] = self.screenShotRecorder[-1]

        # if mouse event, only record mouse pressed events
        if event['type'] == 'EVENT_MOUSE_PRESSED' or event['type'] == 'EVENT_MOUSE_RELEASED' or event['type'] == 'EVENT_MOUSE_MOVED' or event['type'] == 'EVENT_MOUSE_DRAGGED' or event['type'] == 'EVENT_MOUSE_CLICKED':
            if event['type'] == 'EVENT_MOUSE_PRESSED':
                self._events.append(event)
                # self.generate()
        else:
            # if keyboard event, record only key pressed events
            if event['type'] == 'EVENT_KEY_PRESSED':
                event['keychar'] = chr(event['keychar'])
                self._events.append(event)
                # self.generate()

    def generate(self, update_generated_tasks):
        def _update_generated_tasks(tasks):
            self.previous_list = tasks
            update_generated_tasks(tasks)
        generate(self.previous_list, self._events, _update_generated_tasks)
        # ic(new_list)
        # self.previous_list = new_list
    
    def start_generating(self, update_generated_tasks):
        self.update_generated_tasks = update_generated_tasks
        self.generate_thread = threading.Thread(target=self._generate_every_minute)
        self.generate_thread.start()

    def stop_generating(self):
        self._stop_generate_event.set()
        self.generate_thread.join()

    def _generate_every_minute(self):
        while True:
            if self._stop_generate_event.is_set():
                break
            self.generate(self.update_generated_tasks)
            time.sleep(30)  # wait for 30 seconds

    def save(self, filePath):
        # save a zip file containing screenshots and events.json
        # create a zip file
        with zipfile.ZipFile(filePath, 'w') as zipf:
            # create a new events dict by copying the original events
            events = self._events.copy()
            # file all screenshots in events
            for event in events:
                if 'screenshotPath' in event:
                    filename = os.path.basename(event['screenshotPath'])
                    zipf.write(event['screenshotPath'], filename)
            # write events.json with YYYY-MM-DD-HH-MM-SS format
            jsonName = f"events-{datetime.now().strftime('%Y-%m-%d-%H-%M-%S')}.json"
            with open(os.path.join(self.dirName.name, jsonName), 'w') as f:
                f.write("\n".join([str(event) for event in self._events]))
            zipf.write(os.path.join(
                self.dirName.name, jsonName), "events.json")

    def load(self, filePath):
        # load a zip file containing screenshots and events.json
        # clear existing events
        self.clear()
        # extract the zip file
        with zipfile.ZipFile(filePath, 'r') as zipf:
            # clear all files in the temp directory
            self.dirName.cleanup()
            # extract all files
            zipf.extractall(self.dirName.name)
            # read events.json
            with open(os.path.join(self.dirName.name, "events.json"), 'r') as f:
                # read all events
                self._events = [eval(line) for line in f.readlines()]
                # change the screenshot path to the path in the temp directory
                for event in self._events:
                    if 'screenshotPath' in event:
                        event['screenshotPath'] = os.path.join(
                            self.dirName.name, event['screenshotPath'])

    def get_events(self):
        return self._events
    
    def __str__(self):
        text = ""
        for event in self._events:
            if event['type'].startswith('EVENT_KEY'):
                text += f"{event['type']}(time={event['time']}, keychar={event['keychar'] if 'keychar' in event else ''}, rawcode={event['rawcode'] if 'rawcode' in event else ''}, flags={event['flags'] if 'flags' in event else ''}, mask={event['mask'] if 'mask' in event else ''})\n"
            elif event['type'].startswith('EVENT_MOUSE_WHEEL'):
                text += f"{event['type']}(time={event['time']}, x={event['x'] if 'x' in event else ''}, y={event['y'] if 'y' in event else ''}, wheelType={event['wheelType'] if 'wheelType' in event else ''}, direction={event['direction'] if 'direction' in event else ''}, amount={event['amount'] if 'amount' in event else ''}, rotation={event['rotation'] if 'rotation' in event else ''})\n"
            elif event['type'].startswith('EVENT_MOUSE'):
                text += f"{event['type']}(time={event['time']}, x={event['x'] if 'x' in event else ''}, y={event['y'] if 'y' in event else ''}, mask={event['mask'] if 'mask' in event else ''}, button={event['button'] if 'button' in event else ''})\n"
        return text
