from concurrent.futures import ThreadPoolExecutor
import threading

import audioop
import pyaudio
import numpy as np
from scipy.signal import resample
from autoworklet.speak import T5TTS
from autoworklet.speech.transcribe.huggingface import SpeechTranscriber
from queue import Queue
import queue


class StoppableSpeechThread(threading.Thread):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._stop_event = threading.Event()
        self._pause_recording_event = threading.Event()
        self.ambient_detected = False
        self.speech_volume = 100
        self.executor = ThreadPoolExecutor(max_workers=1)
        self.speechTranscriber = SpeechTranscriber()
        self.result_queue = Queue()
        self.live_speech_thread = None
        self.tts = T5TTS()

    def run(self):
        self.live_speech_thread = threading.Thread(target=self.live_speech)
        self.live_speech_thread.start()
        while True:
            if self._stop_event.is_set():
                break
            try:
                # Attempt to get a message from the queue with a timeout
                message = self.result_queue.get(timeout=2)
            except queue.Empty:
                # If the queue is empty, the get() method will timeout and raise queue.Empty
                continue
            except Exception as e:
                # Handle other possible exceptions
                print(f"An error occurred: {e}")
                continue
            print(message)
            self._pause_recording_event.set()
            self.tts.speak("I heard you say " + message)
            # playsound.playsound(Path(__file__).parent /
            #                         "sounds" / "detected.mp3")
            self._pause_recording_event.clear()
            self.result_queue.task_done()

    def stop(self):
        self.executor.shutdown()
        self._stop_event.set()
        self.live_speech_thread.join()
        self.result_queue.join()
        self.join()

    def transcribe(self, audio_float):
        result = self.speechTranscriber.transcribe(audio_float)
        # print(result["text"].strip())
        self.result_queue.put(result["text"].strip())

    def live_speech(self, wait_time=10):
        FORMAT = pyaudio.paInt16
        CHANNELS = 1
        RATE = 44100
        CHUNK = 1024

        audio = pyaudio.PyAudio()

        stream = audio.open(
            format=FORMAT,
            channels=CHANNELS,
            rate=RATE,
            input=True,
            frames_per_buffer=CHUNK
        )

        frames = []
        recording = False
        frames_recorded = 0

        try:
            while True:
                if self._stop_event.is_set():
                    break
                frames_recorded += 1
                data = stream.read(CHUNK)
                if self._pause_recording_event.is_set():
                    continue
                rms = audioop.rms(data, 2)

                if not self.ambient_detected:
                    if frames_recorded < 40:
                        if frames_recorded == 1:
                            print("Detecting ambient noise...")
                        if frames_recorded > 5:
                            if self.speech_volume < rms:
                                self.speech_volume = rms
                        continue
                    elif frames_recorded == 40:
                        print("Listening...")
                        self.speech_volume = self.speech_volume * 3
                        self.ambient_detected = True

                if rms > self.speech_volume:
                    recording = True
                    frames_recorded = 0
                elif recording and frames_recorded > wait_time:
                    recording = False
                    # print("transcribing...")

                    # wf = wave.open("audio.wav", 'wb')
                    # wf.setnchannels(CHANNELS)
                    # wf.setsampwidth(audio.get_sample_size(FORMAT))
                    # wf.setframerate(RATE)
                    # wf.writeframes(b''.join(frames))
                    # wf.close()

                    # result = whisper_model.transcribe(
                    #     "audio.wav",
                    #     fp16=False
                    # )
                    # os.remove("audio.wav")

                    # Convert frames to a NumPy array
                    audio_data = b''.join(frames)
                    audio_np = np.frombuffer(audio_data, dtype=np.int16)

                    # Scale int16 array to float32 range [-1.0, 1.0]
                    audio_float = audio_np.astype(
                        np.float32) / np.iinfo(np.int16).max

                    # Optional: Resample the audio if necessary (e.g., from 44100 to 16000 Hz)
                    if RATE != 16000:
                        audio_float = resample(audio_float, int(
                            len(audio_float) * 16000 / RATE))

                    self.executor.submit(self.transcribe, audio_float)
                    # yield result["text"].strip()

                    frames = []

                if recording:
                    frames.append(data)
        except Exception as e:
            print(e)
            # print stack trace
            import traceback
            traceback.print_exc()
        finally:
            stream.stop_stream()
            stream.close()
            audio.terminate()
