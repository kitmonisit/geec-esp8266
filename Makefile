build:
	platformio run

upload:
	platformio run -t upload

serial:
	platformio device monitor --baud 9600

clean:
	platformio run -t clean

