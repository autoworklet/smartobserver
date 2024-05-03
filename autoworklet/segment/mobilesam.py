from ultralytics import SAM
import os

dirname = os.path.dirname(__file__)
# Load the model
model = SAM(os.path.join(dirname, 'mobile_sam.pt'))

def segment_image_path(path, points):
    # Predict a segment based on a point prompt
    results = model.predict(path, points=points, labels=[1])
    return results
