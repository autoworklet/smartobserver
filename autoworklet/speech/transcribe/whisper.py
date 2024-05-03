import whisper
from huggingface_hub import hf_hub_download


class SpeechTranscriber:
    def __init__(self, model_id="base.en"):
        if model_id == 'base.en':
            self.model = whisper.load_model(model_id)
        else:
            # Load the model model_id=distil-whisper/distil-small.en
            self.model = whisper.load_model(
                hf_hub_download(repo_id=model_id, filename="original-model.bin"))

    def transcribe(self, audio_float):
        return self.model.transcribe(audio_float, fp16=False)
