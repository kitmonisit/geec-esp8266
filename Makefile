build:
	platformio run

upload:
	platformio run -t upload --upload-port /dev/cu.SLAB_USBtoUART

serial:
	platformio device monitor --baud 9600 --port /dev/cu.SLAB_USBtoUART

clean:
	platformio run -t clean

