import serial
import serial.tools.list_ports
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

def print_ports(ports):
    global port_num
    port_num = 0
    for port, desc, hwid in ports:
        port_num = port_num + 1
        print(f"{port_num}) {port}: {desc} [{hwid}]")

ports = serial.tools.list_ports.comports()

if not ports:
    print("No ports found!")
    quit(-1)

user_input = 0
while True:
    print_ports(ports)
    user_input = input(f"Choose a port (1 - {port_num}): ")
    if user_input.isdigit() and int(user_input) >= 1 and int(user_input) <= port_num:
        break
    print("\nInvalid port specified!\n")

com = ports[int(user_input) - 1]
print(f"You chose: {com.device} - {com.description}")

# TODO: Add option to select baudrate
ser = serial.Serial(port=com.device, baudrate=115200, timeout=1)
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
                # Get the current directory
                file_tree = get_file_tree_string(r".")
                ser.write(file_tree.encode('utf-8'))
                ser.write(b'\n')


        except UnicodeDecodeError:
            print("Received undecodable bytes.")
