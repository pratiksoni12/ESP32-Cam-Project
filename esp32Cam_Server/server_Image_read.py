from flask import Flask, request
import os

app = Flask(__name__)

# Create a folder to store the images
os.makedirs("images", exist_ok=True)


@app.route('/upload', methods=['POST'])
def upload_image():
    device_id = request.headers.get('Device-ID')
    os.makedirs(f"images/{device_id}", exist_ok=True)
    file = request.data  # Get the raw image data
    if file:
        file_path = f"images/{device_id}/{device_id}_{len(os.listdir(f'images/{device_id}')) + 1}.jpg"
        with open(file_path, 'wb') as f:
            f.write(file)
        print(f"Image received and saved as {file_path}")
        return "Image uploaded successfully!", 200
    else:
        return "No image found", 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)

