# List of the ChibiOS e200z3 SPC563Mxx startup files.
STARTUPSRC =
          
STARTUPASM = $(CHIBIOS)/os/common/startup/e200/devices/SPC563Mxx/boot.s \
             $(CHIBIOS)/os/common/startup/e200/compilers/GCC/vectors.s \
             $(CHIBIOS)/os/common/startup/e200/compilers/GCC/crt0.s

STARTUPINC = ${CHIBIOS}/os/common/startup/e200/compilers/GCC \
             ${CHIBIOS}/os/common/startup/e200/devices/SPC563Mxx

STARTUPLD  = ${CHIBIOS}/os/common/startup/e200/compilers/GCC/ld
