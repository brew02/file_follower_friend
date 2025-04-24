import serial
import serial.tools.list_ports
import time
from pathlib import Path

# For testing
def print_file_tree(path='.'):
    p = Path(path)
    for file in p.glob('*'):
        print(file)

def get_file_tree_string(path):
    try:
        p = Path(path)
        contents = []
        for content in p.iterdir():
            rel = str(content)
            if content.is_dir():
                rel = rel + '/'
            content = str(content)
            last = content.rfind('\\')
            if last == -1:
                contents.append(rel)
            else:
                contents.append(rel[(last + 1):])
            
            
    except FileNotFoundError:
        return f"Error: '{path}' is not a valid directory."

    return '\n'.join(contents)

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

# Available baud rates
BAUD_RATES = {
    1: 9600,
    2: 19200,
    3: 38400,
    4: 57600,
    5: 115200
}

# Prompt user to choose a baud rate
while True:
    print("\nChoose a baudrate:")
    for key, rate in BAUD_RATES.items():
        print(f"{key}) {rate}")

    user_input = input(f"Choose a baudrate (1 - {len(BAUD_RATES)}): ")

    if user_input.isdigit() and int(user_input) in BAUD_RATES:
        baudrate = BAUD_RATES[int(user_input)]
        print(f"Selected baudrate: {baudrate}\n")
        break
    else:
        print("Invalid baudrate selected. Please choose a valid option.")

ser = serial.Serial(port=com.device, baudrate=baudrate, timeout=0)
time.sleep(2)
ser.reset_input_buffer()

print("Listening...")
current_path = Path(".").resolve()

while True:
    value = ser.readline()
    if value:
        try:
            string_value = value.decode('utf-8').strip()
            if not string_value:
                continue

            print(f"Received: {string_value}")

            if string_value == 'y':
                # Send current directory tree
                print(f"Sending current directory")
                file_tree = get_file_tree_string(r".")
                ser.write(file_tree.encode('utf-8'))
                ser.write(b'\0')

            elif string_value == 'g':
                parent = current_path.parent
                print(f"Moving to parent directory: {parent}")
                if parent.exists() and parent.is_dir():
                    current_path = parent
                    tree = get_file_tree_string(current_path)
                    ser.write(tree.encode('utf-8') + b'\0')
                else:
                    print(f"Error: Parent directory does not exist.\n")

            elif string_value:
                directory_path = current_path / string_value
                if directory_path.is_dir():
                    current_path = directory_path
                    print(f"Sending file tree for {directory_path}...")
                    file_tree = get_file_tree_string(directory_path)
                    ser.write(file_tree.encode('utf-8'))
                    ser.write(b'\0')
                else:
                    ser.write(f"Error: '{directory_path}' is not a valid directory.\n".encode('utf-8'))

        except UnicodeDecodeError:
            print("Received undecodable bytes.")
