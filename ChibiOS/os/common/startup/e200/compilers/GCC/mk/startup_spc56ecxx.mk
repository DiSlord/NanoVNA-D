# List of the ChibiOS e200z4 SPC56ECxx startup files.
STARTUPSRC =
          
STARTUPASM = $(CHIBIOS)/os/common/startup/e200/devices/SPC56ECxx/boot.s \
             $(CHIBIOS)/os/common/startup/e200/compilers/GCC/vectors.s \
             $(CHIBIOS)/os/common/startup/e200/compilers/GCC/crt0.s

STARTUPINC = ${CHIBIOS}/os/common/startup/e200/compilers/GCC \
             ${CHIBIOS}/os/common/startup/e200/devices/SPC56ECxx

STARTUPLD  = ${CHIBIOS}/os/common/startup/e200/compilers/GCC/ld
