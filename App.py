import serial
import time
from pathlib import Path

# For testing
def print_file_tree(path='.'):
    p = Path(path)
    for file in p.glob('*'):
        print(file)

# Returns the file tree as a string
def get_file_tree_string(path='.'):
    p = Path(path)
    folders = [str(folder) for folder in p.iterdir() if folder.is_dir()]
    return '\n'.join(folders)

# run python -m serial.tools.list_ports and replace port with whatever port
# is being used. Also change the baudrate
ser = serial.Serial(port='COM3', baudrate=115200, timeout=1)
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
                print("Sending file tree...")
                file_tree = get_file_tree_string(r"C:\Users\lucas\Computer-Science\CS456")
                ser.write(file_tree.encode('utf-8'))
                ser.write(b'\n')


        except UnicodeDecodeError:
            print("Received undecodable bytes.")
