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
 * @file    SDMMCv1/hal_sdc_lld.h
 * @brief   STM32 SDC subsystem low level driver header.
 *
 * @addtogroup SDC
 * @{
 */

#ifndef HAL_SDC_LLD_H
#define HAL_SDC_LLD_H

#if HAL_USE_SDC || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    Configuration options
 * @{
 */
/**
 * @brief   SDMMC driver enable switch.
 * @details If set to @p TRUE the support for SDMMC1 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(STM32_SDC_USE_SDMMC1) || defined(__DOXYGEN__)
#define STM32_SDC_USE_SDMMC1                FALSE
#endif

/**
 * @brief   Support for unaligned transfers.
 * @note    Unaligned transfers are much slower.
 */
#if !defined(STM32_SDC_SDMMC_UNALIGNED_SUPPORT) || defined(__DOXYGEN__)
#define STM32_SDC_SDMMC_UNALIGNED_SUPPORT   TRUE
#endif

/**
 * @brief   Write timeout in milliseconds.
 */
#if !defined(STM32_SDC_SDMMC_WRITE_TIMEOUT) || defined(__DOXYGEN__)
#define STM32_SDC_SDMMC_WRITE_TIMEOUT       250
#endif

/**
 * @brief   Read timeout in milliseconds.
 */
#if !defined(STM32_SDC_SDMMC_READ_TIMEOUT) || defined(__DOXYGEN__)
#define STM32_SDC_SDMMC_READ_TIMEOUT        25
#endif

/**
 * @brief   Card clock activation delay in milliseconds.
 */
#if !defined(STM32_SDC_SDMMC_CLOCK_DELAY) || defined(__DOXYGEN__)
#define STM32_SDC_SDMMC_CLOCK_DELAY         10
#endif

/**
 * @brief   SDMMC1 DMA priority (0..3|lowest..highest).
 */
#if !defined(STM32_SDC_SDMMC1_DMA_PRIORITY) || defined(__DOXYGEN__)
#define STM32_SDC_SDMMC1_DMA_PRIORITY       3
#endif

/**
 * @brief   SDMMC1 interrupt priority level setting.
 */
#if !defined(STM32_SDC_SDMMC1_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define STM32_SDC_SDMMC1_IRQ_PRIORITY       9
#endif
/** @} */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if STM32_SDC_USE_SDMMC1 && !STM32_HAS_SDMMC1
#error "SDMMC1 not present in the selected device"
#endif

#if !STM32_SDC_USE_SDMMC1
#error "SDC driver activated but no SDMMC peripheral assigned"
#endif

#if !OSAL_IRQ_IS_VALID_PRIORITY(STM32_SDC_SDMMC1_IRQ_PRIORITY)
#error "Invalid IRQ priority assigned to SDIO"
#endif

#if !STM32_DMA_IS_VALID_PRIORITY(STM32_SDC_SDMMC1_DMA_PRIORITY)
#error "Invalid DMA priority assigned to SDIO"
#endif

/* Check on the presence of the DMA streams settings in mcuconf.h.*/
#if !defined(STM32_SDC_SDMMC1_DMA_STREAM)
#error "SDMMC1 DMA streams not defined"
#endif

/* Check on the validity of the assigned DMA channels.*/
#if !STM32_DMA_IS_VALID_ID(STM32_SDC_SDMMC1_DMA_STREAM, STM32_SDC_SDMMC1_DMA_MSK)
#error "invalid DMA stream associated to SDMMC1"
#endif

#if !defined(STM32_DMA_REQUIRED)
#define STM32_DMA_REQUIRED
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   Type of card flags.
 */
typedef uint32_t sdcmode_t;

/**
 * @brief   SDC Driver condition flags type.
 */
typedef uint32_t sdcflags_t;

/**
 * @brief   Type of a structure representing an SDC driver.
 */
typedef struct SDCDriver SDCDriver;

/**
 * @brief   Driver configuration structure.
 * @note    It could be empty on some architectures.
 */
typedef struct {
  /**
   * @brief   Working area for memory consuming operations.
   * @note    Buffer must be word aligned and big enough to store 512 bytes.
   * @note    It is mandatory for detecting MMC cards bigger than 2GB else it
   *          can be @p NULL. SD cards do NOT need it.
   * @note    Memory pointed by this buffer is only used by @p sdcConnect(),
   *          afterward it can be reused for other purposes.
   */
  uint8_t       *scratchpad;
  /**
   * @brief   Bus width.
   */
  sdcbusmode_t  bus_width;
  /* End of the mandatory fields.*/
} SDCConfig;

/**
 * @brief   @p SDCDriver specific methods.
 */
#define _sdc_driver_methods                                                 \
  _mmcsd_block_device_methods

/**
 * @extends MMCSDBlockDeviceVMT
 *
 * @brief   @p SDCDriver virtual methods table.
 */
struct SDCDriverVMT {
  _sdc_driver_methods
};

/**
 * @brief   Structure representing an SDC driver.
 */
struct SDCDriver {
  /**
   * @brief Virtual Methods Table.
   */
  const struct SDCDriverVMT *vmt;
  _mmcsd_block_device_data
  /**
   * @brief Current configuration data.
   */
  const SDCConfig           *config;
  /**
   * @brief Various flags regarding the mounted card.
   */
  sdcmode_t                 cardmode;
  /**
   * @brief Errors flags.
   */
  sdcflags_t                errors;
  /**
   * @brief Card RCA.
   */
  uint32_t                  rca;
  /* End of the mandatory fields.*/
  /**
   * @brief Thread waiting for I/O completion IRQ.
   */
  thread_reference_t        thread;
  /**
   * @brief     DMA mode bit mask.
   */
  uint32_t                  dmamode;
  /**
   * @brief     Transmit DMA channel.
   */
  const stm32_dma_stream_t  *dma;
  /**
   * @brief     Pointer to the SDMMC registers block.
   * @note      Needed for debugging aid.
   */
  SDMMC_TypeDef             *sdmmc;
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if !defined(__DOXYGEN__)
extern SDCDriver SDCD1;
#endif

#ifdef __cplusplus
extern "C" {
#endif
  void sdc_lld_init(void);
  void sdc_lld_start(SDCDriver *sdcp);
  void sdc_lld_stop(SDCDriver *sdcp);
  void sdc_lld_start_clk(SDCDriver *sdcp);
  void sdc_lld_set_data_clk(SDCDriver *sdcp, sdcbusclk_t clk);
  void sdc_lld_stop_clk(SDCDriver *sdcp);
  void sdc_lld_set_bus_mode(SDCDriver *sdcp, sdcbusmode_t mode);
  void sdc_lld_send_cmd_none(SDCDriver *sdcp, uint8_t cmd, uint32_t arg);
  bool sdc_lld_send_cmd_short(SDCDriver *sdcp, uint8_t cmd, uint32_t arg,
                              uint32_t *resp);
  bool sdc_lld_send_cmd_short_crc(SDCDriver *sdcp, uint8_t cmd, uint32_t arg,
                                  uint32_t *resp);
  bool sdc_lld_send_cmd_long_crc(SDCDriver *sdcp, uint8_t cmd, uint32_t arg,
                                 uint32_t *resp);
  bool sdc_lld_read_special(SDCDriver *sdcp, uint8_t *buf, size_t bytes,
                            uint8_t cmd, uint32_t argument);
  bool sdc_lld_read(SDCDriver *sdcp, uint32_t startblk,
                    uint8_t *buf, uint32_t blocks);
  bool sdc_lld_write(SDCDriver *sdcp, uint32_t startblk,
                     const uint8_t *buf, uint32_t blocks);
  bool sdc_lld_sync(SDCDriver *sdcp);
  bool sdc_lld_is_card_inserted(SDCDriver *sdcp);
  bool sdc_lld_is_write_protected(SDCDriver *sdcp);
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_SDC */

#endif /* HAL_SDC_LLD_H */

/** @} */
