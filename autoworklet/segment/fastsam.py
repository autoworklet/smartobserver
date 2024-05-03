import os
from ultralytics import FastSAM
from ultralytics.models.fastsam import FastSAMPrompt

dirname = os.path.dirname(__file__)
# Load the model
model = FastSAM(os.path.join(dirname, 'FastSAM-s.pt'))


def fastsam_segment_image_path(path, points):
    # Run inference on an image
    everything_results = model(
        path, device='cpu', conf=0.4, iou=0.9)
    print(everything_results)

    prompt_process = FastSAMPrompt(path, everything_results, device='cpu')

    ann = prompt_process.point_prompt(points=[points], pointlabel=[1])
    prompt_process.plot(annotations=ann, output='./out')

    return everything_results
