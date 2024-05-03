import io
from pydub import AudioSegment
from pydub.playback import play
import playsound
import os

def speak(text):
    from transformers import VitsModel, AutoTokenizer
    import torch

    model = VitsModel.from_pretrained("facebook/mms-tts-eng")
    tokenizer = AutoTokenizer.from_pretrained("facebook/mms-tts-eng")

    inputs = tokenizer(text, return_tensors="pt")

    with torch.no_grad():
        output = model(**inputs).waveform

    import scipy

    scipy.io.wavfile.write("techno.wav", rate=model.config.sampling_rate, data=output.float().numpy().T)
    playsound.playsound("techno.wav")
    os.remove("techno.wav")
