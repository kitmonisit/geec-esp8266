# Customizing the ESP8266 board tools for Arduino IDE

In OS X, the root path is

    ~/Library/Arduino15/packages/esp8266/hardware/esp8266/2.3.0

In Linux, the root path is

    ~/.arduino15/packages/esp8266/hardware/esp8266/2.3.0

## Compiler flags

In the `$ROOT_PATH/platform.txt` file, be sure to have the following entries.  Replace the values with <> to the correct paths.

    libsodium.path=<path to your sketch folder>/libsodium

    compiler.cpreprocessor.flags=-D__ets__ -DICACHE_FLASH -U__STRICT_ANSI__ "-I{compiler.sdk.path}/include" "-I{compiler.sdk.path}/lwip/include" "-I{build.path}/core" "-I{libsodium.path}/include"

    compiler.c.elf.libs=-lm -lgcc -lhal -lphy -lpp -lnet80211 -lwpa -lcrypto -lmain -lwps -laxtls -lsmartconfig -lmesh -lwpa2 {build.lwip_lib} -lstdc++ -lsodium

## Linker script

You will need a customized linker script, here provided as
[ld/eagle.app.v6.common.ld](ld/eagle.app.v6.common.ld).  In `$ROOT_PATH/tools/sdk/ld`,
replace `eagle.app.v6.common.ld` with the file of the same name in this
repository.

This custom linker script is necessary so that
[libsodium/libsodium.a](libsodium/libsodium.a) is stored in flash memory and
not in RAM.

