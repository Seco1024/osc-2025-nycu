import serial
import time

kernel_image_path = "kernel8.img"

dev_name = "/dev/ttyUSB0"
# dev_name = "/dev/pts/23"
tty = serial.Serial(dev_name, baudrate=115200, timeout=1)


with open(kernel_image_path, "rb") as f:
    kernel_image = f.read()

kernel_size = len(kernel_image)
print(f"Sending kernel size: {kernel_size} bytes")
tty.write(f"{kernel_size}\n".encode())

time.sleep(3)

print("Sending kernel image...")
tty.write(kernel_image)
tty.flush()
tty.close()
print("Successfully sent kernel image.")