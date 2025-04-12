import serial
import time
from pathlib import Path

def print_file_tree(path='.'):
    p = Path(path)
    for file in p.glob('*'):
        print(file)

# run python -m serial.tools.list_ports and replace port with whatever port
# is being used. Also change the baudrate
ser = serial.Serial(port='COM3', baudrate=57600, timeout=1)
time.sleep(2)
ser.reset_input_buffer()

print("Listening...")

while True:
    value = ser.readline()
    if value:
        try:
            string_value = value.decode('utf-8').strip()
            print(f"Received: {string_value}")

            if string_value == 'y':
                print_file_tree(r"C:\Users\lucas\Computer-Science\CS456")


        except UnicodeDecodeError:
            print("Received undecodable bytes.")
