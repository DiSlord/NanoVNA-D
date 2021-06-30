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
 * @file    USARTv1/hal_uart_lld.c
 * @brief   STM32 low level UART driver code.
 *
 * @addtogroup UART
 * @{
 */

#include "hal.h"

#if HAL_USE_UART || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#define USART1_RX_DMA_CHANNEL                                               \
  STM32_DMA_GETCHANNEL(STM32_UART_USART1_RX_DMA_STREAM,                     \
                       STM32_USART1_RX_DMA_CHN)

#define USART1_TX_DMA_CHANNEL                                               \
  STM32_DMA_GETCHANNEL(STM32_UART_USART1_TX_DMA_STREAM,                     \
                       STM32_USART1_TX_DMA_CHN)

#define USART2_RX_DMA_CHANNEL                                               \
  STM32_DMA_GETCHANNEL(STM32_UART_USART2_RX_DMA_STREAM,                     \
                       STM32_USART2_RX_DMA_CHN)

#define USART2_TX_DMA_CHANNEL                                               \
  STM32_DMA_GETCHANNEL(STM32_UART_USART2_TX_DMA_STREAM,                     \
                       STM32_USART2_TX_DMA_CHN)

#define USART3_RX_DMA_CHANNEL                                               \
  STM32_DMA_GETCHANNEL(STM32_UART_USART3_RX_DMA_STREAM,                     \
                       STM32_USART3_RX_DMA_CHN)

#define USART3_TX_DMA_CHANNEL                                               \
  STM32_DMA_GETCHANNEL(STM32_UART_USART3_TX_DMA_STREAM,                     \
                       STM32_USART3_TX_DMA_CHN)

#define UART4_RX_DMA_CHANNEL                                                \
  STM32_DMA_GETCHANNEL(STM32_UART_UART4_RX_DMA_STREAM,                      \
                       STM32_UART4_RX_DMA_CHN)

#define UART4_TX_DMA_CHANNEL                                                \
  STM32_DMA_GETCHANNEL(STM32_UART_UART4_TX_DMA_STREAM,                      \
                       STM32_UART4_TX_DMA_CHN)

#define UART5_RX_DMA_CHANNEL                                                \
  STM32_DMA_GETCHANNEL(STM32_UART_UART5_RX_DMA_STREAM,                      \
                       STM32_UART5_RX_DMA_CHN)

#define UART5_TX_DMA_CHANNEL                                                \
  STM32_DMA_GETCHANNEL(STM32_UART_UART5_TX_DMA_STREAM,                      \
                       STM32_UART5_TX_DMA_CHN)

#define USART6_RX_DMA_CHANNEL                                               \
  STM32_DMA_GETCHANNEL(STM32_UART_USART6_RX_DMA_STREAM,                     \
                       STM32_USART6_RX_DMA_CHN)

#define USART6_TX_DMA_CHANNEL                                               \
  STM32_DMA_GETCHANNEL(STM32_UART_USART6_TX_DMA_STREAM,                     \
                       STM32_USART6_TX_DMA_CHN)

#define STM32_UART45_CR2_CHECK_MASK                                         \
  (USART_CR2_STOP_0 | USART_CR2_CLKEN | USART_CR2_CPOL | USART_CR2_CPHA |   \
   USART_CR2_LBCL)

#define STM32_UART45_CR3_CHECK_MASK                                         \
  (USART_CR3_CTSIE | USART_CR3_CTSE | USART_CR3_RTSE | USART_CR3_SCEN |     \
   USART_CR3_NACK)

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/** @brief USART1 UART driver identifier.*/
#if STM32_UART_USE_USART1 || defined(__DOXYGEN__)
UARTDriver UARTD1;
#endif

/** @brief USART2 UART driver identifier.*/
#if STM32_UART_USE_USART2 || defined(__DOXYGEN__)
UARTDriver UARTD2;
#endif

/** @brief USART3 UART driver identifier.*/
#if STM32_UART_USE_USART3 || defined(__DOXYGEN__)
UARTDriver UARTD3;
#endif

/** @brief UART4 UART driver identifier.*/
#if STM32_UART_USE_UART4 || defined(__DOXYGEN__)
UARTDriver UARTD4;
#endif

/** @brief UART5 UART driver identifier.*/
#if STM32_UART_USE_UART5 || defined(__DOXYGEN__)
UARTDriver UARTD5;
#endif

/** @brief USART6 UART driver identifier.*/
#if STM32_UART_USE_USART6 || defined(__DOXYGEN__)
UARTDriver UARTD6;
#endif

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/**
 * @brief   Status bits translation.
 *
 * @param[in] sr        USART SR register value
 *
 * @return  The error flags.
 */
static uartflags_t translate_errors(uint16_t sr) {
  uartflags_t sts = 0;

  if (sr & USART_SR_ORE)
    sts |= UART_OVERRUN_ERROR;
  if (sr & USART_SR_PE)
    sts |= UART_PARITY_ERROR;
  if (sr & USART_SR_FE)
    sts |= UART_FRAMING_ERROR;
  if (sr & USART_SR_NE)
    sts |= UART_NOISE_ERROR;
  if (sr & USART_SR_LBD)
    sts |= UART_BREAK_DETECTED;
  return sts;
}

/**
 * @brief   Puts the receiver in the UART_RX_IDLE state.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 */
static void uart_enter_rx_idle_loop(UARTDriver *uartp) {
  uint32_t mode;
  
  /* RX DMA channel preparation, if the char callback is defined then the
     TCIE interrupt is enabled too.*/
  if (uartp->config->rxchar_cb == NULL)
    mode = STM32_DMA_CR_DIR_P2M | STM32_DMA_CR_CIRC;
  else
    mode = STM32_DMA_CR_DIR_P2M | STM32_DMA_CR_CIRC | STM32_DMA_CR_TCIE;
  dmaStreamSetMemory0(uartp->dmarx, &uartp->rxbuf);
  dmaStreamSetTransactionSize(uartp->dmarx, 1);
  dmaStreamSetMode(uartp->dmarx, uartp->dmamode | mode);
  dmaStreamEnable(uartp->dmarx);
}

/**
 * @brief   USART de-initialization.
 * @details This function must be invoked with interrupts disabled.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 */
static void usart_stop(UARTDriver *uartp) {

  /* Stops RX and TX DMA channels.*/
  dmaStreamDisable(uartp->dmarx);
  dmaStreamDisable(uartp->dmatx);
  
  /* Stops USART operations.*/
  uartp->usart->CR1 = 0;
  uartp->usart->CR2 = 0;
  uartp->usart->CR3 = 0;
}

/**
 * @brief   USART initialization.
 * @details This function must be invoked with interrupts disabled.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 */
static void usart_start(UARTDriver *uartp) {
  uint16_t cr1;
  USART_TypeDef *u = uartp->usart;

  /* Defensive programming, starting from a clean state.*/
  usart_stop(uartp);

  /* Baud rate setting.*/
#if STM32_HAS_USART6
  if ((uartp->usart == USART1) || (uartp->usart == USART6))
#else
  if (uartp->usart == USART1)
#endif
    u->BRR = STM32_PCLK2 / uartp->config->speed;
  else
    u->BRR = STM32_PCLK1 / uartp->config->speed;

  /* Resetting eventual pending status flags.*/
  (void)u->SR;  /* SR reset step 1.*/
  (void)u->DR;  /* SR reset step 2.*/
  u->SR = 0;

  /* Note that some bits are enforced because required for correct driver
     operations.*/
  u->CR2 = uartp->config->cr2 | USART_CR2_LBDIE;
  u->CR3 = uartp->config->cr3 | USART_CR3_DMAT | USART_CR3_DMAR |
                                USART_CR3_EIE;

  /* Mustn't ever set TCIE here - if done, it causes an immediate
     interrupt.*/
  cr1 = USART_CR1_UE | USART_CR1_PEIE | USART_CR1_TE | USART_CR1_RE;
  u->CR1 = uartp->config->cr1 | cr1;

  /* Starting the receiver idle loop.*/
  uart_enter_rx_idle_loop(uartp);
}

/**
 * @brief   RX DMA common service routine.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 * @param[in] flags     pre-shifted content of the ISR register
 */
static void uart_lld_serve_rx_end_irq(UARTDriver *uartp, uint32_t flags) {

  /* DMA errors handling.*/
#if defined(STM32_UART_DMA_ERROR_HOOK)
  if ((flags & (STM32_DMA_ISR_TEIF | STM32_DMA_ISR_DMEIF)) != 0) {
    STM32_UART_DMA_ERROR_HOOK(uartp);
  }
#else
  (void)flags;
#endif

  if (uartp->rxstate == UART_RX_IDLE) {
    /* Receiver in idle state, a callback is generated, if enabled, for each
       received character and then the driver stays in the same state.*/
    _uart_rx_idle_code(uartp);
  }
  else {
    /* Receiver in active state, a callback is generated, if enabled, after
       a completed transfer.*/
    dmaStreamDisable(uartp->dmarx);
    _uart_rx_complete_isr_code(uartp);
  }
}

/**
 * @brief   TX DMA common service routine.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 * @param[in] flags     pre-shifted content of the ISR register
 */
static void uart_lld_serve_tx_end_irq(UARTDriver *uartp, uint32_t flags) {

  /* DMA errors handling.*/
#if defined(STM32_UART_DMA_ERROR_HOOK)
  if ((flags & (STM32_DMA_ISR_TEIF | STM32_DMA_ISR_DMEIF)) != 0) {
    STM32_UART_DMA_ERROR_HOOK(uartp);
  }
#else
  (void)flags;
#endif

  dmaStreamDisable(uartp->dmatx);

  /* A callback is generated, if enabled, after a completed transfer.*/
  _uart_tx1_isr_code(uartp);
}

/**
 * @brief   USART common service routine.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 */
static void serve_usart_irq(UARTDriver *uartp) {
  uint16_t sr;
  USART_TypeDef *u = uartp->usart;
  uint32_t cr1 = u->CR1;

  sr = u->SR;   /* SR reset step 1.*/
  (void)u->DR;  /* SR reset step 2.*/

  if (sr & (USART_SR_LBD | USART_SR_ORE | USART_SR_NE |
            USART_SR_FE  | USART_SR_PE)) {
    u->SR = ~USART_SR_LBD;
    _uart_rx_error_isr_code(uartp, translate_errors(sr));
  }

  if ((sr & USART_SR_TC) && (cr1 & USART_CR1_TCIE)) {
    /* TC interrupt cleared and disabled.*/
    u->SR = ~USART_SR_TC;
    u->CR1 = cr1 & ~USART_CR1_TCIE;

    /* End of transmission, a callback is generated.*/
    _uart_tx2_isr_code(uartp);
  }
}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

#if STM32_UART_USE_USART1 || defined(__DOXYGEN__)
#if !defined(STM32_USART1_HANDLER)
#error "STM32_USART1_HANDLER not defined"
#endif
/**
 * @brief   USART1 IRQ handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(STM32_USART1_HANDLER) {

  OSAL_IRQ_PROLOGUE();

  serve_usart_irq(&UARTD1);

  OSAL_IRQ_EPILOGUE();
}
#endif /* STM32_UART_USE_USART1 */

#if STM32_UART_USE_USART2 || defined(__DOXYGEN__)
#if !defined(STM32_USART2_HANDLER)
#error "STM32_USART2_HANDLER not defined"
#endif
/**
 * @brief   USART2 IRQ handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(STM32_USART2_HANDLER) {

  OSAL_IRQ_PROLOGUE();

  serve_usart_irq(&UARTD2);

  OSAL_IRQ_EPILOGUE();
}
#endif /* STM32_UART_USE_USART2 */

#if STM32_UART_USE_USART3 || defined(__DOXYGEN__)
#if !defined(STM32_USART3_HANDLER)
#error "STM32_USART3_HANDLER not defined"
#endif
/**
 * @brief   USART3 IRQ handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(STM32_USART3_HANDLER) {

  OSAL_IRQ_PROLOGUE();

  serve_usart_irq(&UARTD3);

  OSAL_IRQ_EPILOGUE();
}
#endif /* STM32_UART_USE_USART3 */

#if STM32_UART_USE_UART4 || defined(__DOXYGEN__)
#if !defined(STM32_UART4_HANDLER)
#error "STM32_UART4_HANDLER not defined"
#endif
/**
 * @brief   UART4 IRQ handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(STM32_UART4_HANDLER) {

  OSAL_IRQ_PROLOGUE();

  serve_usart_irq(&UARTD4);

  OSAL_IRQ_EPILOGUE();
}
#endif /* STM32_UART_USE_UART4 */

#if STM32_UART_USE_UART5 || defined(__DOXYGEN__)
#if !defined(STM32_UART5_HANDLER)
#error "STM32_UART5_HANDLER not defined"
#endif
/**
 * @brief   UART5 IRQ handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(STM32_UART5_HANDLER) {

  OSAL_IRQ_PROLOGUE();

  serve_usart_irq(&UARTD5);

  OSAL_IRQ_EPILOGUE();
}
#endif /* STM32_UART_USE_UART5 */

#if STM32_UART_USE_USART6 || defined(__DOXYGEN__)
#if !defined(STM32_USART6_HANDLER)
#error "STM32_USART6_HANDLER not defined"
#endif
/**
 * @brief   USART6 IRQ handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(STM32_USART6_HANDLER) {

  OSAL_IRQ_PROLOGUE();

  serve_usart_irq(&UARTD6);

  OSAL_IRQ_EPILOGUE();
}
#endif /* STM32_UART_USE_USART6 */

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level UART driver initialization.
 *
 * @notapi
 */
void uart_lld_init(void) {

#if STM32_UART_USE_USART1
  uartObjectInit(&UARTD1);
  UARTD1.usart   = USART1;
  UARTD1.dmamode = STM32_DMA_CR_DMEIE | STM32_DMA_CR_TEIE;
  UARTD1.dmarx   = STM32_DMA_STREAM(STM32_UART_USART1_RX_DMA_STREAM);
  UARTD1.dmatx   = STM32_DMA_STREAM(STM32_UART_USART1_TX_DMA_STREAM);
#endif

#if STM32_UART_USE_USART2
  uartObjectInit(&UARTD2);
  UARTD2.usart   = USART2;
  UARTD2.dmamode = STM32_DMA_CR_DMEIE | STM32_DMA_CR_TEIE;
  UARTD2.dmarx   = STM32_DMA_STREAM(STM32_UART_USART2_RX_DMA_STREAM);
  UARTD2.dmatx   = STM32_DMA_STREAM(STM32_UART_USART2_TX_DMA_STREAM);
#endif

#if STM32_UART_USE_USART3
  uartObjectInit(&UARTD3);
  UARTD3.usart   = USART3;
  UARTD3.dmamode = STM32_DMA_CR_DMEIE | STM32_DMA_CR_TEIE;
  UARTD3.dmarx   = STM32_DMA_STREAM(STM32_UART_USART3_RX_DMA_STREAM);
  UARTD3.dmatx   = STM32_DMA_STREAM(STM32_UART_USART3_TX_DMA_STREAM);
#endif

#if STM32_UART_USE_UART4
  uartObjectInit(&UARTD4);
  UARTD4.usart   = UART4;
  UARTD4.dmamode = STM32_DMA_CR_DMEIE | STM32_DMA_CR_TEIE;
  UARTD4.dmarx   = STM32_DMA_STREAM(STM32_UART_UART4_RX_DMA_STREAM);
  UARTD4.dmatx   = STM32_DMA_STREAM(STM32_UART_UART4_TX_DMA_STREAM);
#endif

#if STM32_UART_USE_UART5
  uartObjectInit(&UARTD5);
  UARTD5.usart   = UART5;
  UARTD5.dmamode = STM32_DMA_CR_DMEIE | STM32_DMA_CR_TEIE;
  UARTD5.dmarx   = STM32_DMA_STREAM(STM32_UART_UART5_RX_DMA_STREAM);
  UARTD5.dmatx   = STM32_DMA_STREAM(STM32_UART_UART5_TX_DMA_STREAM);
#endif

#if STM32_UART_USE_USART6
  uartObjectInit(&UARTD6);
  UARTD6.usart   = USART6;
  UARTD6.dmarx   = STM32_DMA_STREAM(STM32_UART_USART6_RX_DMA_STREAM);
  UARTD6.dmatx   = STM32_DMA_STREAM(STM32_UART_USART6_TX_DMA_STREAM);
#endif
}

/**
 * @brief   Configures and activates the UART peripheral.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 *
 * @notapi
 */
void uart_lld_start(UARTDriver *uartp) {

  if (uartp->state == UART_STOP) {
#if STM32_UART_USE_USART1
    if (&UARTD1 == uartp) {
      bool b;
      b = dmaStreamAllocate(uartp->dmarx,
                            STM32_UART_USART1_IRQ_PRIORITY,
                            (stm32_dmaisr_t)uart_lld_serve_rx_end_irq,
                            (void *)uartp);
      osalDbgAssert(!b, "stream already allocated");
      b = dmaStreamAllocate(uartp->dmatx,
                            STM32_UART_USART1_IRQ_PRIORITY,
                            (stm32_dmaisr_t)uart_lld_serve_tx_end_irq,
                            (void *)uartp);
      osalDbgAssert(!b, "stream already allocated");
      rccEnableUSART1(FALSE);
      nvicEnableVector(STM32_USART1_NUMBER, STM32_UART_USART1_IRQ_PRIORITY);
      uartp->dmamode |= STM32_DMA_CR_CHSEL(USART1_RX_DMA_CHANNEL) |
                        STM32_DMA_CR_PL(STM32_UART_USART1_DMA_PRIORITY);
    }
#endif

#if STM32_UART_USE_USART2
    if (&UARTD2 == uartp) {
      bool b;
      b = dmaStreamAllocate(uartp->dmarx,
                            STM32_UART_USART2_IRQ_PRIORITY,
                            (stm32_dmaisr_t)uart_lld_serve_rx_end_irq,
                            (void *)uartp);
      osalDbgAssert(!b, "stream already allocated");
      b = dmaStreamAllocate(uartp->dmatx,
                            STM32_UART_USART2_IRQ_PRIORITY,
                            (stm32_dmaisr_t)uart_lld_serve_tx_end_irq,
                            (void *)uartp);
      osalDbgAssert(!b, "stream already allocated");
      rccEnableUSART2(FALSE);
      nvicEnableVector(STM32_USART2_NUMBER, STM32_UART_USART2_IRQ_PRIORITY);
      uartp->dmamode |= STM32_DMA_CR_CHSEL(USART2_RX_DMA_CHANNEL) |
                        STM32_DMA_CR_PL(STM32_UART_USART2_DMA_PRIORITY);
    }
#endif

#if STM32_UART_USE_USART3
    if (&UARTD3 == uartp) {
      bool b;
      b = dmaStreamAllocate(uartp->dmarx,
                            STM32_UART_USART3_IRQ_PRIORITY,
                            (stm32_dmaisr_t)uart_lld_serve_rx_end_irq,
                            (void *)uartp);
      osalDbgAssert(!b, "stream already allocated");
      b = dmaStreamAllocate(uartp->dmatx,
                            STM32_UART_USART3_IRQ_PRIORITY,
                            (stm32_dmaisr_t)uart_lld_serve_tx_end_irq,
                            (void *)uartp);
      osalDbgAssert(!b, "stream already allocated");
      rccEnableUSART3(FALSE);
      nvicEnableVector(STM32_USART3_NUMBER, STM32_UART_USART3_IRQ_PRIORITY);
      uartp->dmamode |= STM32_DMA_CR_CHSEL(USART3_RX_DMA_CHANNEL) |
                        STM32_DMA_CR_PL(STM32_UART_USART3_DMA_PRIORITY);
    }
#endif

#if STM32_UART_USE_UART4
    if (&UARTD4 == uartp) {
      bool b;

      chDbgAssert((uartp->config->cr2 & STM32_UART45_CR2_CHECK_MASK) == 0,
                  "specified invalid bits in UART4 CR2 register settings");
      chDbgAssert((uartp->config->cr3 & STM32_UART45_CR3_CHECK_MASK) == 0,
                  "specified invalid bits in UART4 CR3 register settings");

      b = dmaStreamAllocate(uartp->dmarx,
                            STM32_UART_UART4_IRQ_PRIORITY,
                            (stm32_dmaisr_t)uart_lld_serve_rx_end_irq,
                            (void *)uartp);
      osalDbgAssert(!b, "stream already allocated");
      b = dmaStreamAllocate(uartp->dmatx,
                            STM32_UART_UART4_IRQ_PRIORITY,
                            (stm32_dmaisr_t)uart_lld_serve_tx_end_irq,
                            (void *)uartp);
      osalDbgAssert(!b, "stream already allocated");
      rccEnableUART4(FALSE);
      nvicEnableVector(STM32_UART4_NUMBER, STM32_UART_UART4_IRQ_PRIORITY);
      uartp->dmamode |= STM32_DMA_CR_CHSEL(UART4_RX_DMA_CHANNEL) |
                        STM32_DMA_CR_PL(STM32_UART_UART4_DMA_PRIORITY);
    }
#endif

#if STM32_UART_USE_UART5
    if (&UARTD5 == uartp) {
      bool b;

      chDbgAssert((uartp->config->cr2 & STM32_UART45_CR2_CHECK_MASK) == 0,
                  "specified invalid bits in UART5 CR2 register settings");
      chDbgAssert((uartp->config->cr3 & STM32_UART45_CR3_CHECK_MASK) == 0,
                  "specified invalid bits in UART5 CR3 register settings");

      b = dmaStreamAllocate(uartp->dmarx,
                            STM32_UART_UART5_IRQ_PRIORITY,
                            (stm32_dmaisr_t)uart_lld_serve_rx_end_irq,
                            (void *)uartp);
      osalDbgAssert(!b, "stream already allocated");
      b = dmaStreamAllocate(uartp->dmatx,
                            STM32_UART_UART5_IRQ_PRIORITY,
                            (stm32_dmaisr_t)uart_lld_serve_tx_end_irq,
                            (void *)uartp);
      osalDbgAssert(!b, "stream already allocated");
      rccEnableUART5(FALSE);
      nvicEnableVector(STM32_UART5_NUMBER, STM32_UART_UART5_IRQ_PRIORITY);
      uartp->dmamode |= STM32_DMA_CR_CHSEL(UART5_RX_DMA_CHANNEL) |
                        STM32_DMA_CR_PL(STM32_UART_UART5_DMA_PRIORITY);
    }
#endif

#if STM32_UART_USE_USART6
    if (&UARTD6 == uartp) {
      bool b;
      b = dmaStreamAllocate(uartp->dmarx,
                            STM32_UART_USART6_IRQ_PRIORITY,
                            (stm32_dmaisr_t)uart_lld_serve_rx_end_irq,
                            (void *)uartp);
      osalDbgAssert(!b, "stream already allocated");
      b = dmaStreamAllocate(uartp->dmatx,
                            STM32_UART_USART6_IRQ_PRIORITY,
                            (stm32_dmaisr_t)uart_lld_serve_tx_end_irq,
                            (void *)uartp);
      osalDbgAssert(!b, "stream already allocated");
      rccEnableUSART6(FALSE);
      nvicEnableVector(STM32_USART6_NUMBER, STM32_UART_USART6_IRQ_PRIORITY);
      uartp->dmamode |= STM32_DMA_CR_CHSEL(USART6_RX_DMA_CHANNEL) |
                        STM32_DMA_CR_PL(STM32_UART_USART6_DMA_PRIORITY);
    }
#endif

    /* Static DMA setup, the transfer size depends on the USART settings,
       it is 16 bits if M=1 and PCE=0 else it is 8 bits.*/
    if ((uartp->config->cr1 & (USART_CR1_M | USART_CR1_PCE)) == USART_CR1_M)
      uartp->dmamode |= STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD;
    dmaStreamSetPeripheral(uartp->dmarx, &uartp->usart->DR);
    dmaStreamSetPeripheral(uartp->dmatx, &uartp->usart->DR);
    uartp->rxbuf = 0;
  }

  uartp->rxstate = UART_RX_IDLE;
  uartp->txstate = UART_TX_IDLE;
  usart_start(uartp);
}

/**
 * @brief   Deactivates the UART peripheral.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 *
 * @notapi
 */
void uart_lld_stop(UARTDriver *uartp) {

  if (uartp->state == UART_READY) {
    usart_stop(uartp);
    dmaStreamRelease(uartp->dmarx);
    dmaStreamRelease(uartp->dmatx);

#if STM32_UART_USE_USART1
    if (&UARTD1 == uartp) {
      nvicDisableVector(STM32_USART1_NUMBER);
      rccDisableUSART1(FALSE);
      return;
    }
#endif

#if STM32_UART_USE_USART2
    if (&UARTD2 == uartp) {
      nvicDisableVector(STM32_USART2_NUMBER);
      rccDisableUSART2(FALSE);
      return;
    }
#endif

#if STM32_UART_USE_USART3
    if (&UARTD3 == uartp) {
      nvicDisableVector(STM32_USART3_NUMBER);
      rccDisableUSART3(FALSE);
      return;
    }
#endif

#if STM32_UART_USE_UART4
    if (&UARTD4 == uartp) {
      nvicDisableVector(STM32_UART4_NUMBER);
      rccDisableUART4(FALSE);
      return;
    }
#endif

#if STM32_UART_USE_UART5
    if (&UARTD5 == uartp) {
      nvicDisableVector(STM32_UART5_NUMBER);
      rccDisableUART5(FALSE);
      return;
    }
#endif

#if STM32_UART_USE_USART6
    if (&UARTD6 == uartp) {
      nvicDisableVector(STM32_USART6_NUMBER);
      rccDisableUSART6(FALSE);
      return;
    }
#endif
  }
}

/**
 * @brief   Starts a transmission on the UART peripheral.
 * @note    The buffers are organized as uint8_t arrays for data sizes below
 *          or equal to 8 bits else it is organized as uint16_t arrays.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 * @param[in] n         number of data frames to send
 * @param[in] txbuf     the pointer to the transmit buffer
 *
 * @notapi
 */
void uart_lld_start_send(UARTDriver *uartp, size_t n, const void *txbuf) {

  /* TX DMA channel preparation.*/
  dmaStreamSetMemory0(uartp->dmatx, txbuf);
  dmaStreamSetTransactionSize(uartp->dmatx, n);
  dmaStreamSetMode(uartp->dmatx, uartp->dmamode    | STM32_DMA_CR_DIR_M2P |
                                 STM32_DMA_CR_MINC | STM32_DMA_CR_TCIE);

  /* Only enable TC interrupt if there's a callback attached to it.
     Also we need to clear TC flag which could be set before. */
  if (uartp->config->txend2_cb != NULL) {
    uartp->usart->SR = ~USART_SR_TC;
    uartp->usart->CR1 |= USART_CR1_TCIE;
  }

  /* Starting transfer.*/
  dmaStreamEnable(uartp->dmatx);
}

/**
 * @brief   Stops any ongoing transmission.
 * @note    Stopping a transmission also suppresses the transmission callbacks.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 *
 * @return              The number of data frames not transmitted by the
 *                      stopped transmit operation.
 *
 * @notapi
 */
size_t uart_lld_stop_send(UARTDriver *uartp) {

  dmaStreamDisable(uartp->dmatx);

  return dmaStreamGetTransactionSize(uartp->dmatx);
}

/**
 * @brief   Starts a receive operation on the UART peripheral.
 * @note    The buffers are organized as uint8_t arrays for data sizes below
 *          or equal to 8 bits else it is organized as uint16_t arrays.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 * @param[in] n         number of data frames to send
 * @param[out] rxbuf    the pointer to the receive buffer
 *
 * @notapi
 */
void uart_lld_start_receive(UARTDriver *uartp, size_t n, void *rxbuf) {

  /* Stopping previous activity (idle state).*/
  dmaStreamDisable(uartp->dmarx);

  /* RX DMA channel preparation.*/
  dmaStreamSetMemory0(uartp->dmarx, rxbuf);
  dmaStreamSetTransactionSize(uartp->dmarx, n);
  dmaStreamSetMode(uartp->dmarx, uartp->dmamode    | STM32_DMA_CR_DIR_P2M |
                                 STM32_DMA_CR_MINC | STM32_DMA_CR_TCIE);

  /* Starting transfer.*/
  dmaStreamEnable(uartp->dmarx);
}

/**
 * @brief   Stops any ongoing receive operation.
 * @note    Stopping a receive operation also suppresses the receive callbacks.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 *
 * @return              The number of data frames not received by the
 *                      stopped receive operation.
 *
 * @notapi
 */
size_t uart_lld_stop_receive(UARTDriver *uartp) {
  size_t n;

  dmaStreamDisable(uartp->dmarx);
  n = dmaStreamGetTransactionSize(uartp->dmarx);
  uart_enter_rx_idle_loop(uartp);

  return n;
}

#endif /* HAL_USE_UART */

/** @} */
