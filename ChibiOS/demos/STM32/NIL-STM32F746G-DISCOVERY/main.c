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

#include "hal.h"
#include "ch.h"
#include "ch_test.h"

/*
 * Blinker thread #1.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;

  /*
   * GPIOI1 is programmed as output (board LED).
   */
  palClearLine(LINE_ARD_D13);
  palSetLineMode(LINE_ARD_D13, PAL_MODE_OUTPUT_PUSHPULL);

  while (true) {
    palSetLine(LINE_ARD_D13);
    chThdSleepMilliseconds(500);
    palClearLine(LINE_ARD_D13);
    chThdSleepMilliseconds(500);
  }
}

/*
 * Tester thread.
 */
THD_WORKING_AREA(waThread2, 128);
THD_FUNCTION(Thread2, arg) {

  (void)arg;

  /*
   * Activates the serial driver 1 using the driver default configuration.
   */
  sdStart(&SD1, NULL);

  /* Welcome message.*/
  chnWrite(&SD1, (const uint8_t *)"Hello World!\r\n", 14);

  /* Waiting for button push and activation of the test suite.*/
  while (true) {
    if (palReadLine(LINE_BUTTON_USER))
      test_execute((BaseSequentialStream *)&SD1);
    chThdSleepMilliseconds(500);
  }
}

/*
 * Threads static table, one entry per thread. The number of entries must
 * match CH_CFG_NUM_THREADS.
 */
THD_TABLE_BEGIN
  THD_TABLE_ENTRY(waThread1, "blinker1", Thread1, NULL)
  THD_TABLE_ENTRY(wa_test_support, "test_support", test_support, (void *)&nil.threads[2])
  THD_TABLE_ENTRY(waThread2, "tester", Thread2, NULL)
THD_TABLE_END

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /* This is now the idle thread loop, you may perform here a low priority
     task but you must never try to sleep or wait in this loop. Note that
     this tasks runs at the lowest priority level so any instruction added
     here will be executed after all other tasks have been started.*/
  while (true) {
  }
}
