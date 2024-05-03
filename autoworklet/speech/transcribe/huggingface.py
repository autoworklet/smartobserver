import torch
from transformers import AutoModelForSpeechSeq2Seq, AutoProcessor, pipeline


class SpeechTranscriber:
    def __init__(self, model_id="distil-whisper/distil-small.en"):
        self.device = "cuda:0" if torch.cuda.is_available() else "cpu"
        self.torch_dtype = torch.float16 if torch.cuda.is_available() else torch.float32

        # Load the model
        self.model = AutoModelForSpeechSeq2Seq.from_pretrained(
            model_id, torch_dtype=self.torch_dtype, low_cpu_mem_usage=True, use_safetensors=True
        )
        self.model.to(self.device)

        # Load the processor
        self.processor = AutoProcessor.from_pretrained(model_id)

        # Create the pipeline
        self.pipe = pipeline(
            "automatic-speech-recognition",
            model=self.model,
            tokenizer=self.processor.tokenizer,
            feature_extractor=self.processor.feature_extractor,
            max_new_tokens=128,
            torch_dtype=self.torch_dtype,
            device=self.device,
        )

    def transcribe(self, audio_float):
        # Use the pipeline to transcribe audio
        return self.pipe(audio_float)
