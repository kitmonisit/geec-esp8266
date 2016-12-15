The ESP8266 board tools root path

In OS X:

    ~/Library/Arduino15/packages/esp8266/hardware/esp8266/2.3.0

In Linux:

    ~/.arduino15/packages/esp8266/hardware/esp8266/2.3.0


## Compiler flags

In the `platform.txt` file, be sure to have the following entries

    libsodium.path=/Users/kit/Documents/Arduino/NodeMakina/libsodium

    compiler.cpreprocessor.flags=-D__ets__ -DICACHE_FLASH -U__STRICT_ANSI__ "-I{compiler.sdk.path}/include" "-I{compiler.sdk.path}/lwip/include" "-I{build.path}/core" "-I{libsodium.path}/include"
    compiler.c.flags=-c {compiler.warning_flags} -Os -g -Wpointer-arith -Wno-implicit-function-declaration -Wl,-EL -fno-inline-functions -nostdlib -mlongcalls -mtext-section-literals -falign-functions=4 -MMD -std=gnu99 -ffunction-sections -fdata-sections

    compiler.c.elf.libs=-lm -lgcc -lhal -lphy -lpp -lnet80211 -lwpa -lcrypto -lmain -lwps -laxtls -lsmartconfig -lmesh -lwpa2 {build.lwip_lib} -lstdc++ -lsodium

    compiler.cpp.flags=-c {compiler.warning_flags} -Os -g -mlongcalls -mtext-section-literals -fno-exceptions -fno-rtti -falign-functions=4 -std=c++11 -MMD -ffunction-sections -fdata-sections

## Linker script

You will need a customized linker script, here provided as
[ld/eagle.app.v6.common.ld](ld/eagle.app.v6.common.ld).  In `tools/sdk/ld`,
replace `eagle.app.v6.common.ld` with the file of the same name in this
repository.

This custom linker script is necessary so that
[libsodium/libsodium.a](libsodium/libsodium.a) is stored in flash memory and
not in RAM.

