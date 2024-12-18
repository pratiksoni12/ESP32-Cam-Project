from flask import Flask, request, jsonify
import requests
import schedule
import time
import threading
from datetime import datetime
import sys
import os

app = Flask(__name__)

ESP32_URL_TEMPLATE = 'http://192.168.1.10{bed}/display'

def send_data_to_esp32(name, p_id, bed, fg, date, time):
    # Dynamically construct the ESP32 URL based on the bed number
    esp32_url = ESP32_URL_TEMPLATE.format(bed=bed)
    payload = {"name": name, "id": p_id, "bed": bed, "fg": fg, "Date": date, "Time": time}

    try:
        print(f"Sending data to ESP32 ({esp32_url}): {payload}")
        response = requests.post(esp32_url, json=payload, timeout=10)
        if response.status_code == 200:
            print('Data sent successfully:', response.json())
            return True
        else:
            print('Failed to send data:', response.status_code)
            return False

    except Exception as e:
        print('Error:', str(e))
        return False


@app.route('/send-to-esp32', methods=['POST'])
def send_to_esp32():
    try:
        data = request.json
        if not data or 'name' not in data or 'id' not in data:
            return jsonify({"error": "Invalid data. Please provide 'name' and 'id'"}), 400

        name = data['name']
        p_id = data['id']
        p_bed=data.get('bed','')
        p_fg=data.get('fg','')
#         date = data['date']
#         time = data['time']
        current_time = datetime.now()
        formatted_date = current_time.strftime("%d/%m/%Y")
        formatted_time = current_time.strftime("%H:%M:%S")

        # Forward data to ESP32
        print(f"Received API request with data: {data}")
        success =send_data_to_esp32(name, p_id, p_bed, p_fg, formatted_date, formatted_time)
        if success:
            return jsonify({"message": "Data sent to ESP32 successfully"}), 200
        else:
            return jsonify({"error": "Failed to send data to ESP32"}), 500

    except Exception as e:
        return jsonify({"error": str(e)}), 500


def schedule_task():
    current_time = datetime.now()
    formatted_date = current_time.strftime("%d/%m/%Y")
    formatted_time = current_time.strftime("%H:%M:%S")

    print("Scheduled task running: Sending data to ESP32...")
    success = send_data_to_esp32(p_name, p_id, p_bed, p_fg, formatted_date, formatted_time)
    if success:
        print("Data sent successfully. Shutting down server.")
        os._exit(0)  # Forcefully exit the application after successful execution


def run_scheduler():
    schedule.every(2).minutes.do(schedule_task)
    print("Scheduler initialized.")
    while True:
        schedule.run_pending()
        time.sleep(1)


if __name__ == '__main__':
    if len(sys.argv) < 5:
        print("Usage: python script.py <bed_number> <patient_id> <patient_name> <finger_id>")
        sys.exit(1)

    p_bed = sys.argv[1]
    p_id = sys.argv[2]
    p_name = sys.argv[3]
    p_fg = sys.argv[4]

    print(f"Bed Number: {p_bed}")
    print(f"Patient ID: {p_id}")
    print(f"Patient Name: {p_name}")
    print(f"Finger ID: {p_fg}")

    # Start the scheduler in a separate thread
#     scheduler_thread = threading.Thread(target=run_scheduler, daemon=True)
#     scheduler_thread.start()

    current_time = datetime.now()
    formatted_date = current_time.strftime("%d/%m/%Y")
    formatted_time = current_time.strftime("%H:%M:%S")
    
    success = send_data_to_esp32(p_name, p_id, p_bed, p_fg, formatted_date, formatted_time)
    if success:
        print("Data sent successfully. Shutting down server.")
        os._exit(0)  # Forcefully exit the application after successful execution

    print("Starting Flask server...")
    app.run(host='0.0.0.0', port=5000, debug=True)

