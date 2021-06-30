/*
    ChibiOS - Copyright (C) 2006..2016 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    STM32L4xx/stm32_registry.h
 * @brief   STM32L4xx capabilities registry.
 *
 * @addtogroup HAL
 * @{
 */

#ifndef STM32_REGISTRY_H
#define STM32_REGISTRY_H

/*===========================================================================*/
/* Platform capabilities.                                                    */
/*===========================================================================*/

/**
 * @name    STM32L4xx capabilities
 * @{
 */
/* ADC attributes.*/
#define STM32_HAS_ADC1                      TRUE
#define STM32_ADC1_HANDLER                  Vector88
#define STM32_ADC1_NUMBER                   18
#define STM32_ADC1_DMA_MSK                  (STM32_DMA_STREAM_ID_MSK(1, 1) |\
                                             STM32_DMA_STREAM_ID_MSK(2, 3))
#define STM32_ADC1_DMA_CHN                  0x00000000

#define STM32_HAS_ADC2                      TRUE
#define STM32_ADC2_HANDLER                  Vector88
#define STM32_ADC2_NUMBER                   18
#define STM32_ADC2_DMA_MSK                  (STM32_DMA_STREAM_ID_MSK(1, 2) |\
                                             STM32_DMA_STREAM_ID_MSK(2, 4))
#define STM32_ADC2_DMA_CHN                  0x00000000

#define STM32_HAS_ADC3                      TRUE
#define STM32_ADC3_HANDLER                  VectorFC
#define STM32_ADC3_NUMBER                   47
#define STM32_ADC3_DMA_MSK                  (STM32_DMA_STREAM_ID_MSK(1, 3) |\
                                             STM32_DMA_STREAM_ID_MSK(2, 5))
#define STM32_ADC3_DMA_CHN                  0x00000000

#define STM32_HAS_ADC4                      FALSE

/* CAN attributes.*/
#define STM32_HAS_CAN1                      TRUE
#define STM32_HAS_CAN2                      FALSE
#define STM32_CAN_MAX_FILTERS               14
#define STM32_CAN1_TX_HANDLER               Vector8C
#define STM32_CAN1_RX0_HANDLER              Vector90
#define STM32_CAN1_RX1_HANDLER              Vector94
#define STM32_CAN1_SCE_HANDLER              Vector98
#define STM32_CAN1_TX_NUMBER                19
#define STM32_CAN1_RX0_NUMBER               20
#define STM32_CAN1_RX1_NUMBER               21
#define STM32_CAN1_SCE_NUMBER               22

/* DAC attributes.*/
#define STM32_HAS_DAC1_CH1                  TRUE
#define STM32_DAC1_CH1_DMA_MSK              (STM32_DMA_STREAM_ID_MSK(1, 3)|\
                                             STM32_DMA_STREAM_ID_MSK(2, 4))
#define STM32_DAC1_CH1_DMA_CHN              0x00005600

#define STM32_HAS_DAC1_CH2                  TRUE
#define STM32_DAC1_CH2_DMA_MSK              (STM32_DMA_STREAM_ID_MSK(1, 4)|\
                                             STM32_DMA_STREAM_ID_MSK(2, 5))
#define STM32_DAC1_CH2_DMA_CHN              0x00033000

#define STM32_HAS_DAC2_CH1                  FALSE
#define STM32_HAS_DAC2_CH2                  FALSE

/* DMA attributes.*/
#define STM32_ADVANCED_DMA                  TRUE
#define STM32_DMA_SUPPORTS_CSELR            TRUE
#define STM32_DMA1_NUM_CHANNELS             7
#define STM32_DMA1_CH1_HANDLER              Vector6C
#define STM32_DMA1_CH2_HANDLER              Vector70
#define STM32_DMA1_CH3_HANDLER              Vector74
#define STM32_DMA1_CH4_HANDLER              Vector78
#define STM32_DMA1_CH5_HANDLER              Vector7C
#define STM32_DMA1_CH6_HANDLER              Vector80
#define STM32_DMA1_CH7_HANDLER              Vector84
#define STM32_DMA1_CH1_NUMBER               11
#define STM32_DMA1_CH2_NUMBER               12
#define STM32_DMA1_CH3_NUMBER               13
#define STM32_DMA1_CH4_NUMBER               14
#define STM32_DMA1_CH5_NUMBER               15
#define STM32_DMA1_CH6_NUMBER               16
#define STM32_DMA1_CH7_NUMBER               17

#define STM32_DMA2_NUM_CHANNELS             7
#define STM32_DMA2_CH1_HANDLER              Vector120
#define STM32_DMA2_CH2_HANDLER              Vector124
#define STM32_DMA2_CH3_HANDLER              Vector128
#define STM32_DMA2_CH4_HANDLER              Vector12C
#define STM32_DMA2_CH5_HANDLER              Vector130
#define STM32_DMA2_CH6_HANDLER              Vector150
#define STM32_DMA2_CH7_HANDLER              Vector154
#define STM32_DMA2_CH1_NUMBER               56
#define STM32_DMA2_CH2_NUMBER               57
#define STM32_DMA2_CH3_NUMBER               58
#define STM32_DMA2_CH4_NUMBER               59
#define STM32_DMA2_CH5_NUMBER               60
#define STM32_DMA2_CH6_NUMBER               68
#define STM32_DMA2_CH7_NUMBER               69

/* ETH attributes.*/
#define STM32_HAS_ETH                       FALSE

/* EXTI attributes.*/
#define STM32_EXTI_NUM_LINES                39
#define STM32_EXTI_IMR_MASK                 0xFF820000U
#define STM32_EXTI_IMR2_MASK                0xFFFFFF87U

#define STM32_EXTI_LINE0_HANDLER            Vector58
#define STM32_EXTI_LINE1_HANDLER            Vector5C
#define STM32_EXTI_LINE2_HANDLER            Vector60
#define STM32_EXTI_LINE3_HANDLER            Vector64
#define STM32_EXTI_LINE4_HANDLER            Vector68
#define STM32_EXTI_LINE5_9_HANDLER          Vector9C
#define STM32_EXTI_LINE10_15_HANDLER        VectorE0
#define STM32_EXTI_LINE1635_38_HANDLER      Vector44
#define STM32_EXTI_LINE18_HANDLER           VectorE4
#define STM32_EXTI_LINE19_HANDLER           Vector48
#define STM32_EXTI_LINE20_HANDLER           Vector4C
#define STM32_EXTI_LINE2122_HANDLER         Vector140

#define STM32_EXTI_LINE0_NUMBER             6
#define STM32_EXTI_LINE1_NUMBER             7
#define STM32_EXTI_LINE2_NUMBER             8
#define STM32_EXTI_LINE3_NUMBER             9
#define STM32_EXTI_LINE4_NUMBER             10
#define STM32_EXTI_LINE5_9_NUMBER           23
#define STM32_EXTI_LINE10_15_NUMBER         40
#define STM32_EXTI_LINE1635_38_NUMBER       1
#define STM32_EXTI_LINE18_NUMBER            41
#define STM32_EXTI_LINE19_NUMBER            2
#define STM32_EXTI_LINE20_NUMBER            3
#define STM32_EXTI_LINE2122_NUMBER          64

/* GPIO attributes.*/
#define STM32_HAS_GPIOA                     TRUE
#define STM32_HAS_GPIOB                     TRUE
#define STM32_HAS_GPIOC                     TRUE
#define STM32_HAS_GPIOD                     TRUE
#define STM32_HAS_GPIOE                     TRUE
#define STM32_HAS_GPIOF                     TRUE
#define STM32_HAS_GPIOG                     TRUE
#define STM32_HAS_GPIOH                     TRUE
#define STM32_HAS_GPIOI                     FALSE
#define STM32_HAS_GPIOJ                     FALSE
#define STM32_HAS_GPIOK                     FALSE
#define STM32_GPIO_EN_MASK                  (RCC_AHB2ENR_GPIOAEN |          \
                                             RCC_AHB2ENR_GPIOBEN |          \
                                             RCC_AHB2ENR_GPIOCEN |          \
                                             RCC_AHB2ENR_GPIODEN |          \
                                             RCC_AHB2ENR_GPIOEEN |          \
                                             RCC_AHB2ENR_GPIOFEN |          \
                                             RCC_AHB2ENR_GPIOGEN |          \
                                             RCC_AHB2ENR_GPIOHEN)

/* I2C attributes.*/
#define STM32_HAS_I2C1                      TRUE
#define STM32_I2C1_EVENT_HANDLER            VectorBC
#define STM32_I2C1_EVENT_NUMBER             31
#define STM32_I2C1_ERROR_HANDLER            VectorC0
#define STM32_I2C1_ERROR_NUMBER             32
#define STM32_I2C1_RX_DMA_MSK               (STM32_DMA_STREAM_ID_MSK(1, 7) |\
                                             STM32_DMA_STREAM_ID_MSK(2, 6))
#define STM32_I2C1_RX_DMA_CHN               0x03500000
#define STM32_I2C1_TX_DMA_MSK               (STM32_DMA_STREAM_ID_MSK(1, 6) |\
                                             STM32_DMA_STREAM_ID_MSK(2, 7))
#define STM32_I2C1_TX_DMA_CHN               0x05300000

#define STM32_HAS_I2C2                      TRUE
#define STM32_I2C2_EVENT_HANDLER            VectorC4
#define STM32_I2C2_EVENT_NUMBER             33
#define STM32_I2C2_ERROR_HANDLER            VectorC8
#define STM32_I2C2_ERROR_NUMBER             34
#define STM32_I2C2_RX_DMA_MSK               (STM32_DMA_STREAM_ID_MSK(1, 5))
#define STM32_I2C2_RX_DMA_CHN               0x00030000
#define STM32_I2C2_TX_DMA_MSK               (STM32_DMA_STREAM_ID_MSK(1, 4))
#define STM32_I2C2_TX_DMA_CHN               0x00003000

#define STM32_HAS_I2C3                      TRUE
#define STM32_I2C3_EVENT_HANDLER            Vector160
#define STM32_I2C3_EVENT_NUMBER             72
#define STM32_I2C3_ERROR_HANDLER            Vector164
#define STM32_I2C3_ERROR_NUMBER             73
#define STM32_I2C3_RX_DMA_MSK               (STM32_DMA_STREAM_ID_MSK(1, 3))
#define STM32_I2C3_RX_DMA_CHN               0x00000300
#define STM32_I2C3_TX_DMA_MSK               (STM32_DMA_STREAM_ID_MSK(1, 2))
#define STM32_I2C3_TX_DMA_CHN               0x00000030

#define STM32_HAS_I2C4                      FALSE

/* RTC attributes.*/
#define STM32_HAS_RTC                       TRUE
#define STM32_RTC_HAS_SUBSECONDS            TRUE
#define STM32_RTC_HAS_PERIODIC_WAKEUPS      TRUE
#define STM32_RTC_NUM_ALARMS                2
#define STM32_RTC_HAS_INTERRUPTS            FALSE

/* SDMMC attributes.*/
#define STM32_HAS_SDMMC1                    TRUE
#define STM32_SDMMC1_HANDLER                Vector104
#define STM32_SDMMC1_NUMBER                 49
#define STM32_SDC_SDMMC1_DMA_MSK            (STM32_DMA_STREAM_ID_MSK(2, 4) |\
                                             STM32_DMA_STREAM_ID_MSK(2, 5))
#define STM32_SDC_SDMMC1_DMA_CHN            0x00077000

/* SPI attributes.*/
#define STM32_HAS_SPI1                      TRUE
#define STM32_SPI1_SUPPORTS_I2S             FALSE
#define STM32_SPI1_RX_DMA_MSK               (STM32_DMA_STREAM_ID_MSK(1, 2) |\
                                             STM32_DMA_STREAM_ID_MSK(2, 3))
#define STM32_SPI1_RX_DMA_CHN               0x00000410
#define STM32_SPI1_TX_DMA_MSK               (STM32_DMA_STREAM_ID_MSK(1, 3) |\
                                             STM32_DMA_STREAM_ID_MSK(2, 4))
#define STM32_SPI1_TX_DMA_CHN               0x00004100

#define STM32_HAS_SPI2                      TRUE
#define STM32_SPI2_SUPPORTS_I2S             FALSE
#define STM32_SPI2_RX_DMA_MSK               (STM32_DMA_STREAM_ID_MSK(1, 4))
#define STM32_SPI2_RX_DMA_CHN               0x00001000
#define STM32_SPI2_TX_DMA_MSK               (STM32_DMA_STREAM_ID_MSK(1, 5))
#define STM32_SPI2_TX_DMA_CHN               0x00010000

#define STM32_HAS_SPI3                      TRUE
#define STM32_SPI3_SUPPORTS_I2S             FALSE
#define STM32_SPI3_RX_DMA_MSK               (STM32_DMA_STREAM_ID_MSK(2, 1))
#define STM32_SPI3_RX_DMA_CHN               0x00000003
#define STM32_SPI3_TX_DMA_MSK               (STM32_DMA_STREAM_ID_MSK(2, 2))
#define STM32_SPI3_TX_DMA_CHN               0x00000030

#define STM32_HAS_SPI4                      FALSE
#define STM32_HAS_SPI5                      FALSE
#define STM32_HAS_SPI6                      FALSE

/* TIM attributes.*/
#define STM32_TIM_MAX_CHANNELS              6

#define STM32_HAS_TIM1                      TRUE
#define STM32_TIM1_IS_32BITS                FALSE
#define STM32_TIM1_CHANNELS                 4
#define STM32_TIM1_UP_HANDLER               VectorA4
#define STM32_TIM1_CC_HANDLER               VectorAC
#define STM32_TIM1_UP_NUMBER                25
#define STM32_TIM1_CC_NUMBER                27

#define STM32_HAS_TIM2                      TRUE
#define STM32_TIM2_IS_32BITS                TRUE
#define STM32_TIM2_CHANNELS                 4
#define STM32_TIM2_HANDLER                  VectorB0
#define STM32_TIM2_NUMBER                   28

#define STM32_HAS_TIM3                      TRUE
#define STM32_TIM3_IS_32BITS                FALSE
#define STM32_TIM3_CHANNELS                 4
#define STM32_TIM3_HANDLER                  VectorB4
#define STM32_TIM3_NUMBER                   29

#define STM32_HAS_TIM4                      TRUE
#define STM32_TIM4_IS_32BITS                FALSE
#define STM32_TIM4_CHANNELS                 4
#define STM32_TIM4_HANDLER                  VectorB8
#define STM32_TIM4_NUMBER                   30

#define STM32_HAS_TIM5                      TRUE
#define STM32_TIM5_IS_32BITS                TRUE
#define STM32_TIM5_CHANNELS                 4
#define STM32_TIM5_HANDLER                  Vector108
#define STM32_TIM5_NUMBER                   50

#define STM32_HAS_TIM6                      TRUE
#define STM32_TIM6_IS_32BITS                FALSE
#define STM32_TIM6_CHANNELS                 0
#define STM32_TIM6_HANDLER                  Vector118
#define STM32_TIM6_NUMBER                   54

#define STM32_HAS_TIM7                      TRUE
#define STM32_TIM7_IS_32BITS                FALSE
#define STM32_TIM7_CHANNELS                 0
#define STM32_TIM7_HANDLER                  Vector11C
#define STM32_TIM7_NUMBER                   55

#define STM32_HAS_TIM8                      TRUE
#define STM32_TIM8_IS_32BITS                FALSE
#define STM32_TIM8_CHANNELS                 6
#define STM32_TIM8_UP_HANDLER               VectorF0
#define STM32_TIM8_CC_HANDLER               VectorF8
#define STM32_TIM8_UP_NUMBER                44
#define STM32_TIM8_CC_NUMBER                46

#define STM32_HAS_TIM15                     TRUE
#define STM32_TIM15_IS_32BITS               FALSE
#define STM32_TIM15_CHANNELS                2
#define STM32_TIM15_HANDLER                 VectorA0
#define STM32_TIM15_NUMBER                  24

#define STM32_HAS_TIM16                     TRUE
#define STM32_TIM16_IS_32BITS               FALSE
#define STM32_TIM16_CHANNELS                2
#define STM32_TIM16_HANDLER                 VectorA4
#define STM32_TIM16_NUMBER                  25

#define STM32_HAS_TIM17                     TRUE
#define STM32_TIM17_IS_32BITS               FALSE
#define STM32_TIM17_CHANNELS                2
#define STM32_TIM17_HANDLER                 VectorA8
#define STM32_TIM17_NUMBER                  26

#define STM32_HAS_TIM9                      FALSE
#define STM32_HAS_TIM10                     FALSE
#define STM32_HAS_TIM11                     FALSE
#define STM32_HAS_TIM12                     FALSE
#define STM32_HAS_TIM13                     FALSE
#define STM32_HAS_TIM14                     FALSE
#define STM32_HAS_TIM18                     FALSE
#define STM32_HAS_TIM19                     FALSE
#define STM32_HAS_TIM20                     FALSE
#define STM32_HAS_TIM21                     FALSE
#define STM32_HAS_TIM22                     FALSE

/* USART attributes.*/
#define STM32_HAS_USART1                    TRUE
#define STM32_USART1_HANDLER                VectorD4
#define STM32_USART1_NUMBER                 37
#define STM32_USART1_RX_DMA_MSK             (STM32_DMA_STREAM_ID_MSK(1, 5) |\
                                             STM32_DMA_STREAM_ID_MSK(2, 7))
#define STM32_USART1_RX_DMA_CHN             0x02020000
#define STM32_USART1_TX_DMA_MSK             (STM32_DMA_STREAM_ID_MSK(1, 4) |\
                                             STM32_DMA_STREAM_ID_MSK(2, 6))
#define STM32_USART1_TX_DMA_CHN             0x00202000

#define STM32_HAS_USART2                    TRUE
#define STM32_USART2_HANDLER                VectorD8
#define STM32_USART2_NUMBER                 38
#define STM32_USART2_RX_DMA_MSK             (STM32_DMA_STREAM_ID_MSK(1, 6))
#define STM32_USART2_RX_DMA_CHN             0x00200000
#define STM32_USART2_TX_DMA_MSK             (STM32_DMA_STREAM_ID_MSK(1, 7))
#define STM32_USART2_TX_DMA_CHN             0x02000000

#define STM32_HAS_USART3                    TRUE
#define STM32_USART3_HANDLER                VectorDC
#define STM32_USART3_NUMBER                 39
#define STM32_USART3_RX_DMA_MSK             (STM32_DMA_STREAM_ID_MSK(1, 3))
#define STM32_USART3_RX_DMA_CHN             0x00000200
#define STM32_USART3_TX_DMA_MSK             (STM32_DMA_STREAM_ID_MSK(1, 2))
#define STM32_USART3_TX_DMA_CHN             0x00000020

#define STM32_HAS_UART4                     TRUE
#define STM32_UART4_HANDLER                 Vector110
#define STM32_UART4_NUMBER                  52
#define STM32_UART4_RX_DMA_MSK              STM32_DMA_STREAM_ID_MSK(2, 5)
#define STM32_UART4_RX_DMA_CHN              0x00020000
#define STM32_UART4_TX_DMA_MSK              STM32_DMA_STREAM_ID_MSK(2, 3)
#define STM32_UART4_TX_DMA_CHN              0x00000200

#define STM32_HAS_UART5                     TRUE
#define STM32_UART5_HANDLER                 Vector114
#define STM32_UART5_NUMBER                  53
#define STM32_UART5_RX_DMA_MSK              STM32_DMA_STREAM_ID_MSK(2, 2)
#define STM32_UART5_RX_DMA_CHN              0x00000020
#define STM32_UART5_TX_DMA_MSK              STM32_DMA_STREAM_ID_MSK(2, 1)
#define STM32_UART5_TX_DMA_CHN              0x00000002

#define STM32_HAS_LPUART1                   TRUE
#define STM32_LPUART1_HANDLER               Vector158
#define STM32_LPUART1_NUMBER                70

#define STM32_HAS_USART6                    FALSE
#define STM32_HAS_UART7                     FALSE
#define STM32_HAS_UART8                     FALSE

/* USB attributes.*/
#define STM32_HAS_USB                       FALSE
#define STM32_HAS_OTG1                      TRUE
#define STM32_OTG1_HANDLER                  Vector14C
#define STM32_OTG1_NUMBER                   67

#define STM32_HAS_OTG2                      FALSE

/* IWDG attributes.*/
#define STM32_HAS_IWDG                      TRUE
#define STM32_IWDG_IS_WINDOWED              TRUE

/* LTDC attributes.*/
#define STM32_HAS_LTDC                      FALSE

/* DMA2D attributes.*/
#define STM32_HAS_DMA2D                     FALSE

/* FSMC attributes.*/
#define STM32_HAS_FSMC                      TRUE

/* CRC attributes.*/
#define STM32_HAS_CRC                       TRUE
#define STM32_CRC_PROGRAMMABLE              TRUE
/** @} */

#endif /* STM32_REGISTRY_H */

/** @} */
