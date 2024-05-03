import base64
import io
import threading
from vertexai.generative_models import (
    GenerationConfig,
    GenerativeModel,
    HarmBlockThreshold,
    HarmCategory,
    Part,
)
from PIL import Image,ImageDraw
import numpy as np
import io

import vertexai
PROJECT_ID = "autoworklet"  # @param {type:"string"}
LOCATION = "us-central1"  # @param {type:"string"}


vertexai.init(project=PROJECT_ID, location=LOCATION)


MODEL_ID = "gemini-1.5-pro-preview-0409"  # @param {type:"string"}

model = GenerativeModel(
    MODEL_ID,
    system_instruction=[
        "You are a helpful smart observer",
        "Given a series of screenshots, mouse and keyboard events, your mission is to infer a list of tasks that are being performed, so that it can be used to assist the user in completing tasks more efficiently",
        "You also offers real-time suggestions to assist the user in completing tasks more efficiently",
        "You can use the previous list of tasks guessed to help you.",
        "You can also use the screenshot and events to help you.",
        "Only guess the tasks that are relevant to the user's actions.",
        "Do not explain the user's actions, just list the tasks.",
        "Users click locations is indicated by a red dot in the screenshot and click area.",
        "Example list of tasks: ",
        "1. Copy a file from folder A to folder B.",
        "2. Open a file.",
        "3. Close a file.",
        "4. Save a file.",
    ],
)

# Set model parameters
generation_config = GenerationConfig(
    temperature=0.9,
    top_p=1.0,
    top_k=32,
    candidate_count=1,
    max_output_tokens=8192,
)

# Set safety settings
safety_settings = {
    HarmCategory.HARM_CATEGORY_HARASSMENT: HarmBlockThreshold.BLOCK_LOW_AND_ABOVE,
    HarmCategory.HARM_CATEGORY_HATE_SPEECH: HarmBlockThreshold.BLOCK_LOW_AND_ABOVE,
    HarmCategory.HARM_CATEGORY_SEXUALLY_EXPLICIT: HarmBlockThreshold.BLOCK_LOW_AND_ABOVE,
    HarmCategory.HARM_CATEGORY_DANGEROUS_CONTENT: HarmBlockThreshold.BLOCK_LOW_AND_ABOVE,
}

def thread(function):
    def wrap(*args, **kwargs):
        t = threading.Thread(target=function, args=args, kwargs=kwargs, daemon=True)
        t.start()

        return t
    return wrap

from icecream import ic

@thread
def generate(previous_list, events, update_generated_tasks):
    print("Generating...")
    ic(previous_list)
    # ic(events)

    # Set contents to send to the model
    contents = ["Previous list of tasks guessed:\n"]
    # if previous_list == "" or previous_list is None or previous_list is not str:
    if previous_list == "" or previous_list is None or not isinstance(previous_list, str):
        contents += ["No previous list of tasks guessed\n"]
    else:
        contents += [previous_list]
    contents += ["\n\nList of events:\n"]
    keystring = ""
    for event in events:
        if 'keychar' in event:
            keystring += event['keychar']
            # contents += [f"Key: {event['type']}({event['keychar']})\n"]
        else:
            if keystring:
                contents += [f"KeyType: {keystring}\n"]
                keystring = ""
            if 'screenshotPath' in event:
                image_file_uri = event['screenshotPath']
                with open(image_file_uri, "rb") as image_file:
                    image_data = image_file.read()
                image = Image.open(io.BytesIO(image_data))
                # draw red dot at x,y
                draw = ImageDraw.Draw(image)
                draw.ellipse((event['x']-5, event['y']-5, event['x']+5, event['y']+5), fill='#ff0000cc')
                byte_arr = io.BytesIO()
                image.save(byte_arr, format='JPEG')
                jpeg_bytes = byte_arr.getvalue()
                
                image.save(f'_events/event_{event["x"]}_{event["y"]}.jpeg')

                image_content = Part.from_data(
                    data=jpeg_bytes, mime_type="image/jpeg"
                )
                contents += [image_content]
            contents += [f"Mouse: {event['type']}({event['x']}, {event['y']}), click area:"]
            
            # crop the image
            x = event['x']
            y = event['y']
            w = 200
            h = 200
            crop = image.crop((x-w/2, y-h/2, x+w/2, y+h/2))
            
            # convert crop to jpeg
            byte_arr = io.BytesIO()
            crop.save(byte_arr, format='JPEG')
            jpeg_bytes = byte_arr.getvalue()
            # save the crop to a file
            crop.save(f'crop_{x}_{y}.jpeg')

            clickArea = Part.from_data(
                data=jpeg_bytes, mime_type="image/jpeg"
            )
            contents += [clickArea]
            contents += ["\n"]
    
    if keystring:
        contents += [f"KeyType: {keystring}\n"]
    contents += ["New list of tasks guessed and suggestions:\n"]

    # ic(contents)
    # Counts tokens
    print(model.count_tokens(contents))

    # Prompt the model to generate content
    response = model.generate_content(
        contents,
        generation_config=generation_config,
        safety_settings=safety_settings,
    )

    print(f"\nAnswer:\n{response.text}")
    update_generated_tasks(response.text)
    return response.text
