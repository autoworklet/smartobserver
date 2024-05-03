
from transformers import pipeline
from datasets import load_dataset
import soundfile as sf
import torch
import playsound
import os
from io import BytesIO
from pydub import AudioSegment
from pydub.playback import play

class T5TTS:
    def __init__(self):
        synthesiser = pipeline("text-to-speech", "microsoft/speecht5_tts")

        embeddings_dataset = load_dataset(
            "Matthijs/cmu-arctic-xvectors", split="validation")
        speaker_embedding = torch.tensor(
            embeddings_dataset[7306]["xvector"]).unsqueeze(0)

        self.speaker_embedding = speaker_embedding
        self.synthesiser = synthesiser

    # def speak(self, text):
    #     speech = self.synthesiser(text, forward_params={
    #                               "speaker_embeddings": self.speaker_embedding})

    #     sf.write("speech.wav", speech["audio"],
    #              samplerate=speech["sampling_rate"])
    #     playsound.playsound("speech.wav")
    #     os.remove("speech.wav")

    def speak(self, text):
        speech = self.synthesiser(text, forward_params={"speaker_embeddings": self.speaker_embedding})
        audio = speech["audio"]
        sampling_rate = speech["sampling_rate"]

        # Using BytesIO to avoid writing files
        buffer = BytesIO()
        buffer.write(audio.tobytes())  # Writing raw audio data to buffer
        buffer.seek(0)  # Rewind the buffer to the beginning

        # Convert audio bytes to an AudioSegment for playback
        audio_segment = AudioSegment.from_file(buffer, format="raw",
                                               frame_rate=sampling_rate,
                                               channels=1, sample_width=4)  # Adjust these parameters as per your audio specs
        play(audio_segment)  # Play audio using pydub's play