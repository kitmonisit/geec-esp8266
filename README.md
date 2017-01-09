# If you're on OS X or Linux, this is a piece of cake:

## To build this project

    $ make build

## To upload

    $ make upload

## To monitor serial

    $ make serial


# If you're on Windows and using the PlatformIO IDE:

From your home folder, go to `.platformio\platforms\espressif8266` and
rename the folder `ldscripts` to `ldscripts.bad` (Yes, it's a bad folder for
our project, we will not use it).

In the PlatformIO IDE, after having loaded the project, there are buttons on
the left edge for Build, Upload, and Serial Monitor.

