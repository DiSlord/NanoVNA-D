# List of the ChibiOS e200z0 SPC560BCxx startup files.
STARTUPSRC =
          
STARTUPASM = $(CHIBIOS)/os/common/startup/e200/devices/SPC560BCxx/boot.s \
             $(CHIBIOS)/os/common/startup/e200/compilers/GCC/vectors.s \
             $(CHIBIOS)/os/common/startup/e200/compilers/GCC/crt0.s

STARTUPINC = ${CHIBIOS}/os/common/startup/e200/compilers/GCC \
             ${CHIBIOS}/os/common/startup/e200/devices/SPC560BCxx

STARTUPLD  = ${CHIBIOS}/os/common/startup/e200/compilers/GCC/ld
