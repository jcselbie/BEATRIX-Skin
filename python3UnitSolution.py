import serial   # used to access Serial ports (COM)
import time     # used for time delays during calibration

serialIn = serial.Serial('COM5', 9600) # Defines the Input port (from the Skin's Arduino)
serialOut = serial.Serial('COM6', 9600) # Defines the Output port (to BEATRIX's Arduino)

# Sets BEATRIX up for motor control, 
print("Calibrating...")
time.sleep(1)
serialOut.write("@CALNOW\r".encode())
time.sleep(1)
serialOut.write("@ENMOTORS ON\r".encode())
time.sleep(1)
serialOut.write("@CALNOW\r".encode())
time.sleep(1)
print("BEATRIX is ready to go!")

while True:
    try:
        if serialIn.readable():
            received_char = serialIn.read().decode().strip() 
            print("Character received:", received_char)
    except KeyboardInterrupt:
        print("\nExiting...")
        break

    if received_char == 'E':
        serialOut.write("@MOVRALL 200 -200 200 200 200 200\r".encode())
    elif received_char == 'N':
        serialOut.write("@MOVRALL 100 -100 -100 200 200 200\r".encode())
    elif received_char == 'S':
        serialOut.write("@MOVRALL -100 100 100 200 200 200\r".encode())
    elif received_char == 'W':
        serialOut.write("@MOVRALL -200 200 -200 200 200 200\r".encode())


serialIn.close()
serialOut.close()