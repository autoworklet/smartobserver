
# SmartObserver

As digital interfaces become more integral to our lives, the need for systems that understand and react to user behavior beyond basic inputs grows. SmartObserver bridges this gap by leveraging Google's Generative AI to intuitively interpret user intentions and offers real-time suggestions to assist the user in completing tasks more efficiently

# Install packages (only test on MacOS with Apple Silicon currently)

    # you need to install poetry first
    poetry install
    poetry run python -m pip install "kivy[base]"
    poetry run pip3 install torch torchvision torchaudio
    poetry run pip3 install git+https://github.com/openai/whisper.git
    brew install portaudio
    poetry run pip3 install pyaudio
    poetry run pip3 install playsound PyObjC
    poetry run pip3 install huggingface_hub 
    poetry run pip3 install 'transformers[torch]'
    poetry run pip3 install git+https://github.com/ultralytics/CLIP.git
    poetry run pip3 install ultralytics
    poetry run pip3 install icecream
    poetry run pip3 install setuptools
    poetry run pip3 install pydub
    poetry run pip3 install soundfile
    poetry run pip3 install datasets
    poetry run pip3 install sentencepiece
    poetry run pip3 install imageio
    poetry run pip3 install vertexai

    # You may need to also install the following packages for the app to work
    poetry run pip3 install pyautogui pyobjc-core pyobjc-framework-Quartz
    poetry run pip3 install pyobjc-framework-ApplicationServices

# Login to GCP (this is used to call Gemini Vertex API)

    # need to install gcloud https://cloud.google.com/sdk/docs/install and enable the Vertex AI API https://cloud.google.com/vertex-ai/docs/start/cloud-environment
    poetry run gcloud auth login

# Run the app

This will run the app. It will take a few seconds in the first run to compile cpp extensions.

    poetry run python main.py

# Download hf models

    transformers-cli download distil-whisper/distil-small.en --cache-dir models
