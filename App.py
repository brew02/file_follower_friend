import serial
import serial.tools.list_ports
import time
from pathlib import Path
from PIL import Image

"""
Description: App.py is a program created to communicate with the STM32 board utilized for the file follower
friend application. The program utilizes the PySerial library to communicate with the board. The board sends requests
to the PC over UART and the PC responds with the requested data, which are directory contents formatted as a string.

Authors: Brodie Abrew, Lucas Berry
Date: 4/28/2025
"""

# Prints the file tree of the current directory (for testing purposes)
def print_file_tree(path='.'):
    p = Path(path) # creates the path object
    for file in p.glob('*'):
        print(file) #prints file

# Returns a string representation of the contents of the specified directory
def get_file_tree_string(path):
    try:
        p = Path(path) # creates a path object of the provided directory
        contents = [] # creates a list to store contents of directory

        # iterates through directory contents
        for content in p.iterdir():
            rel = str(content) # converts the object to a string

            # checks if the current file is a directory
            if content.is_dir():
                rel = rel + '/' # appends '/' to the directory name for unix naming convention

            content = str(content) # converts the object to a string
            last = content.rfind('\\') # looks for the last instance of '\\'

            # check if index is last index
            if last == -1:
                contents.append(rel) # appends '/' to the end of string
            else:
                contents.append(rel[(last + 1):]) # appends '/' to the end of string
            
    # file not found exception thrown
    except FileNotFoundError:
        return f"Error: '{path}' is not a valid directory."

    # joins the list contents together into a string, separated by newline characters
    return '\n'.join(contents)

# prints the active ports on the PC
def print_ports(ports):
    global port_num
    port_num = 0

    # walks through active ports and prints them
    for port, desc, hwid in ports:
        port_num = port_num + 1
        print(f"{port_num}) {port}: {desc} [{hwid}]")

# turn a 24-bit color into a 16-bit color
def color24to16(r, g, b):
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

# prints ports
ports = serial.tools.list_ports.comports()

# checks if no active ports
if not ports:
    print("No ports found!")
    quit(-1) # exits program

# lets user select ports
user_input = 0
while True:
    print_ports(ports) # prints active ports
    user_input = input(f"Choose a port (1 - {port_num}): ") # lets user select port to use

    # ensures user selected valid port
    if user_input.isdigit() and int(user_input) >= 1 and int(user_input) <= port_num:
        break
    print("\nInvalid port specified!\n")

# stores selected port
com = ports[int(user_input) - 1]
print(f"You chose: {com.device} - {com.description}") # prints selection to the console

# Available baudrates
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

    # showcases baudrates to choose from
    for key, rate in BAUD_RATES.items():
        print(f"{key}) {rate}")

    user_input = input(f"Choose a baudrate (1 - {len(BAUD_RATES)}): ") # user selects baudrate

# ensures user selected valid baudrate
    if user_input.isdigit() and int(user_input) in BAUD_RATES:
        baudrate = BAUD_RATES[int(user_input)] # assigns baudrate
        print(f"Selected baudrate: {baudrate}\n")
        break
    else:
        print("Invalid baudrate selected. Please choose a valid option.")

# sets up serial object for data transfer
ser = serial.Serial(port=com.device, baudrate=baudrate, timeout=None, write_timeout=None)
time.sleep(1) # brief delay
ser.reset_input_buffer() # resets input buffer

# program begins
print("Listening...")
current_path = Path(".").resolve() # gets the users current directory

# while loop which runs until the program is ended
while True:
    value = ser.readline() # reads line over UART

    # checks if value is null
    if value:
        try:
            # removes any invalid bytes
            value = value.replace(b'\xFF', b'')
            string_value = value.decode('utf-8').strip() # gets received string

            # ensures empty strings are ignored
            if not string_value:
                continue

            print(f"Received: {string_value}") # prints receive string for debugging purposes

            # checks if 'y' character received
            if string_value == 'y':
                current_path = Path(".").resolve() # gets the users current directory
                print(f"Sending current directory")
                file_tree = get_file_tree_string(current_path) # gets contents of the current directory
                ser.write(b'd:' + file_tree.encode('utf-8') + b'\0') # writes string over UART

            # checks if 'g' character received
            elif string_value == 'g':
                parent = current_path.parent # gets contents of the parent directory
                
                # checks if the parent directory exists and ensures it is a directory
                if parent.exists() and parent.is_dir():
                    print(f"Moving to parent directory: {parent}")
                    current_path = parent # gets the parent path
                    tree = get_file_tree_string(current_path) # gets contents of parent
                    ser.write(b'd:' + tree.encode('utf-8') + b'\0') # sends parent contents over UART
                else:
                    print(f"Error: Parent directory does not exist.\n") # error checking

            # checks if the user submitted a directory to look at
            elif string_value:
                parts = string_value.split(":", 1)
                if(parts[0] != "o"):
                    print(f"Error: Invalid command '{parts[0]}'")
                    continue

                path = parts[1]
                #if(path[-1] == '/'):
                 #   path = path[:-1]

                directory_path = current_path / path # gets the directory path

                # checks if the specified directory name is a valid directory
                if directory_path.is_dir():
                    current_path = directory_path # gets the path
                    print(f"Sending file tree for {directory_path}...")
                    file_tree = get_file_tree_string(directory_path) # gets the contents of the directory
                    ser.write(b'd:' + file_tree.encode('utf-8') + b'\0') # sends contents over UART
                else:
                    try:
                        # try to open the file as an image
                        image = Image.open(directory_path)
                        image.verify()
                        image = Image.open(directory_path)
                        current_path = directory_path
                        # resize the image
                        new_image = image.resize((128, 130))
                        width, height = new_image.size
                        # load the pixels
                        pixels = new_image.load()
                        processed_pixels = bytearray()

                        for y in range(height):
                            for x in range(width):
                                # grab each pixel and convert to 16-bit
                                r, g, b = pixels[x, y]
                                pixel = color24to16(r, g, b)
                                processed_pixels.extend(pixel.to_bytes(2, 'little'))
                        
                        # send the image
                        ser.write(b'b:' + len(processed_pixels).to_bytes(4, 'little') + bytes(processed_pixels))
                    except:
                        try:
                            # wasn't an image or directory, try opening
                            # as just a file
                            with open(directory_path, 'rb') as file:
                                # read the file and send it
                                file_bytes = file.read()
                                current_path = directory_path
                                ser.write(b'f:' + len(file_bytes).to_bytes(4, 'little') + file_bytes)
                        except:
                            print(f"Error: '{directory_path}' is not a valid path.") # error checking

        # exception checking
        except UnicodeDecodeError:
            print("Received undecodable bytes.")
