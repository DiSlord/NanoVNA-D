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

#include "ch.h"

#if !defined(SYSTEM_CLOCK)
#define SYSTEM_CLOCK 8000000U
#endif

/*
 * @brief   System Timer handler.
 */
CH_IRQ_HANDLER(SysTick_Handler) {

  CH_IRQ_PROLOGUE();

  chSysLockFromISR();
  chSysTimerHandlerI();
  chSysUnlockFromISR();

  CH_IRQ_EPILOGUE();
}

static uint32_t seconds_counter;
static uint32_t minutes_counter;

/*
 * Seconds counter thread.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;

  chRegSetThreadName("counter");

  while (true) {
    chThdSleepMilliseconds(1000);
    seconds_counter++;
  }
}

/*
 * Application entry point.
 */
int main(void) {

  /*
   * Hardware initialization, in this simple demo just the systick timer is
   * initialized.
   */
  SysTick->LOAD = SYSTEM_CLOCK / CH_CFG_ST_FREQUENCY - (systime_t)1;
  SysTick->VAL = (uint32_t)0;
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk |
                  SysTick_CTRL_TICKINT_Msk;

  /*
   * System initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  chSysInit();

  /*
   * Creates the example thread.
   */
  (void) chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * increasing the minutes counter.
   */
  while (true) {
    chThdSleepSeconds(60);
    minutes_counter++;
  }
}
