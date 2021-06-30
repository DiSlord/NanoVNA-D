# List of all the ChibiOS/HAL files, there is no need to remove the files
# from this list, you can disable parts of the HAL by editing halconf.h.
ifeq ($(USE_SMART_BUILD),yes)
HALCONF := $(strip $(shell cat halconf.h | egrep -e "\#define"))

HALSRC := $(CHIBIOS)/os/hal/src/hal.c \
          $(CHIBIOS)/os/hal/src/hal_st.c \
          $(CHIBIOS)/os/hal/src/hal_buffers.c \
          $(CHIBIOS)/os/hal/src/hal_queues.c \
          $(CHIBIOS)/os/hal/src/hal_mmcsd.c
ifneq ($(findstring HAL_USE_ADC TRUE,$(HALCONF)),)
HALSRC += $(CHIBIOS)/os/hal/src/hal_adc.c
endif
ifneq ($(findstring HAL_USE_CAN TRUE,$(HALCONF)),)
HALSRC += $(CHIBIOS)/os/hal/src/hal_can.c
endif
ifneq ($(findstring HAL_USE_DAC TRUE,$(HALCONF)),)
HALSRC += $(CHIBIOS)/os/hal/src/hal_dac.c
endif
ifneq ($(findstring HAL_USE_EXT TRUE,$(HALCONF)),)
HALSRC += $(CHIBIOS)/os/hal/src/hal_ext.c
endif
ifneq ($(findstring HAL_USE_GPT TRUE,$(HALCONF)),)
HALSRC += $(CHIBIOS)/os/hal/src/hal_gpt.c
endif
ifneq ($(findstring HAL_USE_I2C TRUE,$(HALCONF)),)
HALSRC += $(CHIBIOS)/os/hal/src/hal_i2c.c
endif
ifneq ($(findstring HAL_USE_I2S TRUE,$(HALCONF)),)
HALSRC += $(CHIBIOS)/os/hal/src/hal_i2s.c
endif
ifneq ($(findstring HAL_USE_ICU TRUE,$(HALCONF)),)
HALSRC += $(CHIBIOS)/os/hal/src/hal_icu.c
endif
ifneq ($(findstring HAL_USE_MAC TRUE,$(HALCONF)),)
HALSRC += $(CHIBIOS)/os/hal/src/hal_mac.c
endif
ifneq ($(findstring HAL_USE_MMC_SPI TRUE,$(HALCONF)),)
HALSRC += $(CHIBIOS)/os/hal/src/hal_mmc_spi.c
endif
ifneq ($(findstring HAL_USE_PAL TRUE,$(HALCONF)),)
HALSRC += $(CHIBIOS)/os/hal/src/hal_pal.c
endif
ifneq ($(findstring HAL_USE_PWM TRUE,$(HALCONF)),)
HALSRC += $(CHIBIOS)/os/hal/src/hal_pwm.c
endif
ifneq ($(findstring HAL_USE_RTC TRUE,$(HALCONF)),)
HALSRC += $(CHIBIOS)/os/hal/src/hal_rtc.c
endif
ifneq ($(findstring HAL_USE_SDC TRUE,$(HALCONF)),)
HALSRC += $(CHIBIOS)/os/hal/src/hal_sdc.c
endif
ifneq ($(findstring HAL_USE_SERIAL TRUE,$(HALCONF)),)
HALSRC += $(CHIBIOS)/os/hal/src/hal_serial.c
endif
ifneq ($(findstring HAL_USE_SERIAL_USB TRUE,$(HALCONF)),)
HALSRC += $(CHIBIOS)/os/hal/src/hal_serial_usb.c
endif
ifneq ($(findstring HAL_USE_SPI TRUE,$(HALCONF)),)
HALSRC += $(CHIBIOS)/os/hal/src/hal_spi.c
endif
ifneq ($(findstring HAL_USE_UART TRUE,$(HALCONF)),)
HALSRC += $(CHIBIOS)/os/hal/src/hal_uart.c
endif
ifneq ($(findstring HAL_USE_USB TRUE,$(HALCONF)),)
HALSRC += $(CHIBIOS)/os/hal/src/hal_usb.c
endif
ifneq ($(findstring HAL_USE_WDG TRUE,$(HALCONF)),)
HALSRC += $(CHIBIOS)/os/hal/src/hal_wdg.c
endif
else
HALSRC = $(CHIBIOS)/os/hal/src/hal.c \
         $(CHIBIOS)/os/hal/src/hal_buffers.c \
         $(CHIBIOS)/os/hal/src/hal_queues.c \
         $(CHIBIOS)/os/hal/src/hal_mmcsd.c \
         $(CHIBIOS)/os/hal/src/hal_adc.c \
         $(CHIBIOS)/os/hal/src/hal_can.c \
         $(CHIBIOS)/os/hal/src/hal_dac.c \
         $(CHIBIOS)/os/hal/src/hal_ext.c \
         $(CHIBIOS)/os/hal/src/hal_gpt.c \
         $(CHIBIOS)/os/hal/src/hal_i2c.c \
         $(CHIBIOS)/os/hal/src/hal_i2s.c \
         $(CHIBIOS)/os/hal/src/hal_icu.c \
         $(CHIBIOS)/os/hal/src/hal_mac.c \
         $(CHIBIOS)/os/hal/src/hal_mmc_spi.c \
         $(CHIBIOS)/os/hal/src/hal_pal.c \
         $(CHIBIOS)/os/hal/src/hal_pwm.c \
         $(CHIBIOS)/os/hal/src/hal_rtc.c \
         $(CHIBIOS)/os/hal/src/hal_sdc.c \
         $(CHIBIOS)/os/hal/src/hal_serial.c \
         $(CHIBIOS)/os/hal/src/hal_serial_usb.c \
         $(CHIBIOS)/os/hal/src/hal_spi.c \
         $(CHIBIOS)/os/hal/src/hal_st.c \
         $(CHIBIOS)/os/hal/src/hal_uart.c \
         $(CHIBIOS)/os/hal/src/hal_usb.c \
         $(CHIBIOS)/os/hal/src/hal_wdg.c
endif

# Required include directories
HALINC = $(CHIBIOS)/os/hal/include
