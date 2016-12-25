build:
	platformio run

upload:
	platformio run -t upload

serial:
	platformio device monitor --baud 115200

