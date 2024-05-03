# EVENT_HOOK_ENABLED = 1,
# EVENT_HOOK_DISABLED,
# EVENT_KEY_TYPED,
# EVENT_KEY_PRESSED,
# EVENT_KEY_RELEASED,
# EVENT_MOUSE_CLICKED,
# EVENT_MOUSE_PRESSED,
# EVENT_MOUSE_RELEASED,
# EVENT_MOUSE_MOVED,
# EVENT_MOUSE_DRAGGED,
# EVENT_MOUSE_WHEEL

from libnut import pylibnut as nut
from kivy.clock import Clock
# from multiprocessing import Process

# def replay_thread(events):
#     print("Replaying events")
#     diff = 0
#     lastMousePressed = {}
#     lastKeyPressed = {}
#     for i, event in enumerate(events):
#         if diff == 0:
#             diff = event['time']
#         diff_ms = event['time'] - diff
#         diff_ms = diff_ms / 1000000  # Convert to milliseconds
#         print(f"Sleeping for {diff_ms} ms")
#         nut.microsleep(diff_ms)  # Assuming nut.microsleep accepts milliseconds
#         diff = event['time']
#         print(f"Replaying event {i}, {event}")
#         if event['type'] == 'EVENT_MOUSE_MOVED':
#             nut.moveMouse(event['x'], event['y'])
#         elif event['type'] == 'EVENT_MOUSE_PRESSED':
#             h = event['button']
#             lastMousePressed[h] = event
#             nut.mouseToggle(True, event['button'])
#         elif event['type'] == 'EVENT_MOUSE_RELEASED':
#             if len(lastMousePressed.keys()) == 0:
#                 continue
#             h = event['button']
#             if h in lastMousePressed:
#                 del lastMousePressed[h]
#             nut.mouseToggle(False, event['button'])
#         elif event['type'] == 'EVENT_KEY_PRESSED':
#             h = event['mask'] + event['rawcode']
#             lastKeyPressed[h] = event
#             flags = event['flags'] if 'flags' in event else "none"
#             nut.keyToggleRaw(event['rawcode'], True, flags)
#         elif event['type'] == 'EVENT_KEY_RELEASED':
#             if len(lastKeyPressed.keys()) == 0:
#                 continue
#             h = event['mask'] + event['rawcode']
#             if h in lastKeyPressed:
#                 del lastKeyPressed[h]
#             flags = event['flags'] if 'flags' in event else "none"
#             nut.keyToggleRaw(event['rawcode'], False, flags)
#     # Release any pressed mouse buttons or keys
#     for event in lastMousePressed.values():
#         print(f"Releasing mouse {event}")
#         nut.mouseToggle(False, event['button'])
#     for event in lastKeyPressed.values():
#         print(f"Releasing key {event}")
#         flags = event['flags'] if 'flags' in event else "none"
#         nut.keyToggleRaw(event['rawcode'], False)

# def replay(events, callback=None):
#     # json dumps and loads
#     # Create and start a new process to replay the events
#     process = Process(target=replay_thread, args=(events,))
#     process.start()
#     # process.join()  # Wait for the process to complete, if necessary

#     # if callback is not None:
#     #     callback()

import threading


# def replay(events, callback=None):
#     def replay_thread(events):
#         print("Replaying events in a new thread")
#         diff = 0
#         lastMousePressed = {}
#         lastKeyPressed = {}
#         for i, event in enumerate(events):
#             if diff == 0:
#                 diff = event['time']
#             diff_ms = event['time'] - diff
#             diff_us = diff_ms / 1000  # Convert to milliseconds
#             print(f"Sleeping for {diff_us} us")
#             Clock.usleep(diff_us)  # Assuming nut.microsleep accepts milliseconds
#             diff = event['time']
#             print(f"Replaying event {i}, {event}")
#             if event['type'] == 'EVENT_MOUSE_MOVED':
#                 nut.moveMouse(event['x'], event['y'])
#             # elif event['type'] == 'EVENT_MOUSE_CLICKED':
#             #     nut.mouseClick(event['button'])
#             elif event['type'] == 'EVENT_MOUSE_PRESSED':
#                 h = event['button']
#                 lastMousePressed[h] = event
#                 nut.mouseToggle(True, event['button'])
#             elif event['type'] == 'EVENT_MOUSE_RELEASED':
#                 # ignore release if not pressed
#                 if len(lastMousePressed.keys()) == 0:
#                     continue
#                 h = event['button']
#                 if h in lastMousePressed:
#                     del lastMousePressed[h]
#                 nut.mouseToggle(False, event['button'])
#             elif event['type'] == 'EVENT_KEY_PRESSED':
#                 h = event['rawcode']
#                 lastKeyPressed[h] = event
#                 flags = event['flags'] if 'flags' in event else "none"
#                 nut.keyToggleRaw(event['rawcode'], True, flags)
#             elif event['type'] == 'EVENT_KEY_RELEASED':
#                 # ignore release if not pressed
#                 if len(lastKeyPressed.keys()) == 0:
#                     continue
#                 h = event['rawcode']
#                 if h in lastKeyPressed:
#                     del lastKeyPressed[h]
#                 # flags = event['flags'] if 'flags' in event else "none"
#                 nut.keyToggleRaw(event['rawcode'], False)
#         # if last event is EVENT_MOUSE_PRESSED, release it
#         if len(events) > 0:
#             for event in lastMousePressed.values():
#                 print(f"Releasing mouse {event}")
#                 nut.mouseToggle(False, event['button'])
#             lastMousePressed = {}
#             for event in lastKeyPressed.values():
#                 print(f"Releasing key {event}")
#                 # flags = event['flags'] if 'flags' in event else "none"
#                 nut.keyToggleRaw(event['rawcode'], False)
#             lastKeyPressed = {}

#         if callback is not None:
#             callback()

#     # Create and start a new thread to replay the events
#     thread = threading.Thread(target=replay_thread, args=(events,))
#     thread.start()
#     return thread
#     # replay_thread(events)

class StoppableReplayThread(threading.Thread):
    def __init__(self, events, callback=None):
        super().__init__()
        self.events = events
        self.callback = callback
        self._stop_event = threading.Event()

    def run(self):
        events = self.events
        print("Replaying events in a new thread")
        diff = 0
        lastMousePressed = {}
        lastKeyPressed = {}
        lastMousePressedButton = None
        for i, event in enumerate(events):
            # Check if the stop event is set; if so, exit the loop
            if self._stop_event.is_set():
                break

            if diff == 0:
                diff = event['time']
            diff_ms = event['time'] - diff
            diff_us = diff_ms / 1000  # Convert to milliseconds
            print(f"Sleeping for {diff_us} us")
            # Assuming nut.microsleep accepts milliseconds
            Clock.usleep(diff_us)
            diff = event['time']
            print(f"Replaying event {i}, {event}")
            if event['type'] == 'EVENT_MOUSE_MOVED':
                nut.moveMouse(event['x'], event['y'])
            # elif event['type'] == 'EVENT_MOUSE_CLICKED':
            #     nut.mouseClick(event['button'])
            elif event['type'] == 'EVENT_MOUSE_PRESSED':
                h = event['button']
                lastMousePressed[h] = event
                lastMousePressedButton = event['button']
                nut.mouseToggle(True, event['button'])
            elif event['type'] == 'EVENT_MOUSE_RELEASED':
                if lastMousePressedButton is not None and event and lastMousePressedButton == event['button']:
                    lastMousePressedButton = None
                # ignore release if not pressed
                if len(lastMousePressed.keys()) == 0:
                    continue
                h = event['button']
                if h in lastMousePressed:
                    del lastMousePressed[h]
                nut.mouseToggle(False, event['button'])
            elif event['type'] == 'EVENT_MOUSE_DRAGGED':
                button = 'left' if lastMousePressedButton is None else lastMousePressedButton
                nut.dragMouse(event['x'], event['y'], button)
            elif event['type'] == 'EVENT_MOUSE_WHEEL':
                if event['direction'] == 3 and event['rotation'] != 0:
                    nut.scrollMouse(0, event['rotation']*event['amount'])
                elif event['direction'] == 4 and event['rotation'] != 0:
                    nut.scrollMouse(event['rotation']*event['amount'], 0)

            elif event['type'] == 'EVENT_KEY_PRESSED':
                h = event['rawcode']
                lastKeyPressed[h] = event
                flags = event['flags'] if 'flags' in event else "none"
                nut.keyToggleRaw(event['rawcode'], True, flags)
            elif event['type'] == 'EVENT_KEY_RELEASED':
                # ignore release if not pressed
                if len(lastKeyPressed.keys()) == 0:
                    continue
                h = event['rawcode']
                if h in lastKeyPressed:
                    del lastKeyPressed[h]
                # flags = event['flags'] if 'flags' in event else "none"
                nut.keyToggleRaw(event['rawcode'], False)

        # Finalization logic, releasing keys and mouse buttons, if needed
        # This is only reached if the loop exits normally, not by a stop event

        if self._stop_event.is_set():
            print("Thread stopping early")

        print("Cleaning up pressed keys and mouse buttons")
        if len(events) > 0:
            for event in lastMousePressed.values():
                print(f"Releasing mouse {event}")
                nut.mouseToggle(False, event['button'])
            lastMousePressed = {}
            for event in lastKeyPressed.values():
                print(f"Releasing key {event}")
                # flags = event['flags'] if 'flags' in event else "none"
                nut.keyToggleRaw(event['rawcode'], False)
            lastKeyPressed = {}

        if self.callback is not None:
            self.callback()

    def stop(self):
        self._stop_event.set()


def replay(events, callback=None):
    thread = StoppableReplayThread(events, callback)
    thread.start()
    return thread
