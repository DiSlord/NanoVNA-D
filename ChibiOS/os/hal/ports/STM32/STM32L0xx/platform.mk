# List of all the STM32L0xx platform files.
ifeq ($(USE_SMART_BUILD),yes)
HALCONF := $(strip $(shell cat halconf.h | egrep -e "\#define"))

PLATFORMSRC := $(CHIBIOS)/os/hal/ports/common/ARMCMx/nvic.c \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/DMAv1/stm32_dma.c \
               $(CHIBIOS)/os/hal/ports/STM32/STM32L0xx/hal_lld.c \
               $(CHIBIOS)/os/hal/ports/STM32/STM32L0xx/hal_ext_lld_isr.c \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/TIMv1/hal_st_lld.c
ifneq ($(findstring HAL_USE_ADC TRUE,$(HALCONF)),)
PLATFORMSRC += $(CHIBIOS)/os/hal/ports/STM32/LLD/ADCv1/hal_adc_lld.c
endif
ifneq ($(findstring HAL_USE_CAN TRUE,$(HALCONF)),)
PLATFORMSRC += $(CHIBIOS)/os/hal/ports/STM32/LLD/CANv1/hal_can_lld.c
endif
ifneq ($(findstring HAL_USE_DAC TRUE,$(HALCONF)),)
PLATFORMSRC += $(CHIBIOS)/os/hal/ports/STM32/LLD/DACv1/hal_dac_lld.c
endif
ifneq ($(findstring HAL_USE_EXT TRUE,$(HALCONF)),)
PLATFORMSRC += $(CHIBIOS)/os/hal/ports/STM32/LLD/EXTIv1/hal_ext_lld.c
endif
ifneq ($(findstring HAL_USE_PAL TRUE,$(HALCONF)),)
PLATFORMSRC += $(CHIBIOS)/os/hal/ports/STM32/LLD/GPIOv2/hal_pal_lld.c
endif
ifneq ($(findstring HAL_USE_I2C TRUE,$(HALCONF)),)
PLATFORMSRC += $(CHIBIOS)/os/hal/ports/STM32/LLD/I2Cv2/hal_i2c_lld.c
endif
ifneq ($(findstring HAL_USE_RTC TRUE,$(HALCONF)),)
PLATFORMSRC += $(CHIBIOS)/os/hal/ports/STM32/LLD/RTCv2/hal_rtc_lld.c
endif
ifneq ($(findstring HAL_USE_SPI TRUE,$(HALCONF)),)
PLATFORMSRC += $(CHIBIOS)/os/hal/ports/STM32/LLD/SPIv1/hal_spi_lld.c
endif
ifneq ($(findstring HAL_USE_GPT TRUE,$(HALCONF)),)
PLATFORMSRC += $(CHIBIOS)/os/hal/ports/STM32/LLD/TIMv1/hal_gpt_lld.c
endif
ifneq ($(findstring HAL_USE_ICU TRUE,$(HALCONF)),)
PLATFORMSRC += $(CHIBIOS)/os/hal/ports/STM32/LLD/TIMv1/hal_icu_lld.c
endif
ifneq ($(findstring HAL_USE_PWM TRUE,$(HALCONF)),)
PLATFORMSRC += $(CHIBIOS)/os/hal/ports/STM32/LLD/TIMv1/hal_pwm_lld.c
endif
ifneq ($(findstring HAL_USE_SERIAL TRUE,$(HALCONF)),)
PLATFORMSRC += $(CHIBIOS)/os/hal/ports/STM32/LLD/USARTv2/hal_serial_lld.c
endif
ifneq ($(findstring HAL_USE_UART TRUE,$(HALCONF)),)
PLATFORMSRC += $(CHIBIOS)/os/hal/ports/STM32/LLD/USARTv2/hal_uart_lld.c
endif
ifneq ($(findstring HAL_USE_USB TRUE,$(HALCONF)),)
PLATFORMSRC += $(CHIBIOS)/os/hal/ports/STM32/LLD/USBv1/hal_usb_lld.c
endif
ifneq ($(findstring HAL_USE_WDG TRUE,$(HALCONF)),)
PLATFORMSRC += $(CHIBIOS)/os/hal/ports/STM32/LLD/xWDGv1/hal_wdg_lld.c
endif
else
PLATFORMSRC := $(CHIBIOS)/os/hal/ports/common/ARMCMx/nvic.c \
               $(CHIBIOS)/os/hal/ports/STM32/STM32L0xx/hal_lld.c \
               $(CHIBIOS)/os/hal/ports/STM32/STM32L0xx/hal_ext_lld_isr.c \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/ADCv1/hal_adc_lld.c \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/CANv1/hal_can_lld.c \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/DMAv1/stm32_dma.c \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/DACv1/hal_dac_lld.c \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/EXTIv1/hal_ext_lld.c \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/GPIOv2/hal_pal_lld.c \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/I2Cv2/hal_i2c_lld.c \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/RTCv2/hal_rtc_lld.c \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/SPIv1/hal_spi_lld.c \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/TIMv1/hal_gpt_lld.c \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/TIMv1/hal_icu_lld.c \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/TIMv1/hal_pwm_lld.c \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/TIMv1/hal_st_lld.c \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/USARTv2/hal_serial_lld.c \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/USARTv2/hal_uart_lld.c \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/USBv1/hal_usb_lld.c \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/xWDGv1/hal_wdg_lld.c
endif

# Required include directories
PLATFORMINC := $(CHIBIOS)/os/hal/ports/common/ARMCMx \
               $(CHIBIOS)/os/hal/ports/STM32/STM32L0xx \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/ADCv1 \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/CANv1 \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/DACv1 \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/DMAv1 \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/EXTIv1 \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/GPIOv2 \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/I2Cv2 \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/RTCv2 \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/SPIv1 \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/TIMv1 \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/USARTv2 \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/USBv1 \
               $(CHIBIOS)/os/hal/ports/STM32/LLD/xWDGv1
