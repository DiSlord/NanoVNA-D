# List of the ChibiOS/RT Cortex-M0 STM32F0xx port files.
PORTSRC = $(CHIBIOS)/os/common/ports/ARMCMx/chcore.c \
          $(CHIBIOS)/os/common/ports/ARMCMx/chcore_v6m.c
          
PORTASM = $(CHIBIOS)/os/common/ports/ARMCMx/compilers/GCC/chcoreasm_v6m.s

PORTINC = $(CHIBIOS)/os/common/ports/ARMCMx \
          $(CHIBIOS)/os/common/ports/ARMCMx/compilers/GCC
