
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

# download hf models

    transformers-cli download distil-whisper/distil-small.en --cache-dir models

# run

    poetry run python main.py