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

#include <stdlib.h>

#include "ch.h"
#include "hal.h"

float ff1(float par);

float ff2(float par1, float par2, float par3, float par4);

/*===========================================================================*/
/* Configurable settings.                                                    */
/*===========================================================================*/

#ifndef RANDOMIZE
#define RANDOMIZE       FALSE
#endif

#ifndef ITERATIONS
#define ITERATIONS      100
#endif

/*===========================================================================*/
/* Test related code.                                                        */
/*===========================================================================*/

static bool saturated;

/*
 * Test worker thread.
 */
static THD_WORKING_AREA(waWorkerThread, 128);
static THD_FUNCTION(WorkerThread, arg) {

  (void)arg;

  while(1) {
    float f1, f2, f3, f4, f5;

    f1 = ff1(3.0f);
    f2 = ff1(4.0f);
    f3 = ff1(5.0f);
    f5 = f1 + f2 + f3;
    f4 = ff1(6.0f);
    f5 = ff2(f5, f4, f5, f4);
    if (f5 != 324.0f)
      chSysHalt("float corrupion #1");
  }
}

/*
 * Test periodic thread.
 */
static THD_WORKING_AREA(waPeriodicThread, 128);
static THD_FUNCTION(PeriodicThread, arg) {

  (void)arg;

  while(1) {
    float f1, f2, f3, f4, f5;

    f1 = ff1(4.0f);
    f2 = ff1(5.0f);
    f3 = ff1(6.0f);
    f5 = f1 + f2 + f3;
    f4 = ff1(7.0f);
    f5 = ff2(f5, f4, f5, f4);
    if (f5 != 484.0f)
      chSysHalt("float corrupion #2");
    chThdSleepSeconds(1);
  }
}

/*
 * GPT2 callback.
 */
static void gpt4cb(GPTDriver *gptp) {
  float f1, f2, f3, f4, f5;

  (void)gptp;

  f1 = ff1(2.0f);
  f2 = ff1(3.0f);
  f3 = ff1(4.0f);
  f5 = f1 + f2 + f3;
  f4 = ff1(5.0f);
  f5 = ff2(f5, f4, f5, f4);
  if (f5 != 196.0f)
    chSysHalt("float corrupion #3");
}

/*
 * GPT3 callback.
 */
static void gpt3cb(GPTDriver *gptp) {
  float f1, f2, f3, f4, f5;

  (void)gptp;

  f1 = ff1(1.0f);
  f2 = ff1(2.0f);
  f3 = ff1(3.0f);
  f5 = f1 + f2 + f3;
  f4 = ff1(4.0f);
  f5 = ff2(f5, f4, f5, f4);
  if (f5 != 100.0f)
    chSysHalt("float corrupion #4");
}

/*
 * GPT4 configuration.
 */
static const GPTConfig gpt4cfg = {
  1000000,  /* 1MHz timer clock.*/
  gpt4cb,   /* Timer callback.*/
  0,
  0
};

/*
 * GPT3 configuration.
 */
static const GPTConfig gpt3cfg = {
  1000000,  /* 1MHz timer clock.*/
  gpt3cb,   /* Timer callback.*/
  0,
  0
};

CH_FAST_IRQ_HANDLER(STM32_TIM1_UP_HANDLER) {
  float f1, f2, f3, f4, f5;

  TIM1->SR = 0;

  f1 = ff1(3.0f);
  f2 = ff1(4.0f);
  f3 = ff1(5.0f);
  f5 = f1 + f2 + f3;
  f4 = ff1(4.0f);
  f5 = ff2(f5, f4, f5, f4);
  if (f5 != 256.0f) {
    chSysHalt("float corrupion #5");
  }
}

/*===========================================================================*/
/* Generic demo code.                                                        */
/*===========================================================================*/

CH_FAST_IRQ_HANDLER(Vector184) {

   while (1)
     ;
}

static void print(char *p) {

  while (*p) {
    streamPut(&SD2, *p++);
  }
}

static void println(char *p) {

  while (*p) {
    streamPut(&SD2, *p++);
  }
  streamWrite(&SD2, (uint8_t *)"\r\n", 2);
}

static void printn(uint32_t n) {
  char buf[16], *p;

  if (!n)
    streamPut(&SD2, '0');
  else {
    p = buf;
    while (n)
      *p++ = (n % 10) + '0', n /= 10;
    while (p > buf)
      streamPut(&SD2, *--p);
  }
}

/*
 * Application entry point.
 */
int main(void) {
  unsigned i;
  gptcnt_t interval, threshold, worst;

  /* Enables FPU exceptions.*/
  nvicEnableVector(FPU_IRQn, 1);

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Prepares the Serial driver 2 and GPT drivers 4 and 3.
   */
  sdStart(&SD2, NULL);          /* Default is 38400-8-N-1.*/
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));
  gptStart(&GPTD4, &gpt4cfg);
  gptStart(&GPTD3, &gpt3cfg);

  /*
   * Enabling TIM1 as a fast interrupts source.
   */
  rccEnableTIM1(false);
  nvicEnableVector(STM32_TIM1_UP_NUMBER, 0);
  TIM1->ARR  = 10000;
  TIM1->PSC  = 0;
  TIM1->CNT  = 0;
  TIM1->DIER = TIM_DIER_UIE;
  TIM1->CR1  = TIM_CR1_CEN;

  /*
   * Initializes the worker threads.
   */
  chThdCreateStatic(waWorkerThread, sizeof waWorkerThread,
                    NORMALPRIO - 20, WorkerThread, NULL);
  chThdCreateStatic(waPeriodicThread, sizeof waPeriodicThread,
                    NORMALPRIO - 10, PeriodicThread, NULL);

  /*
   * Test procedure.
   */
  println("");
  println("*** ChibiOS/RT IRQ-STORM-FPU long duration test");
  println("***");
  print("*** Kernel:       ");
  println(CH_KERNEL_VERSION);
  print("*** Compiled:     ");
  println(__DATE__ " - " __TIME__);
#ifdef PORT_COMPILER_NAME
  print("*** Compiler:     ");
  println(PORT_COMPILER_NAME);
#endif
  print("*** Architecture: ");
  println(PORT_ARCHITECTURE_NAME);
#ifdef PORT_CORE_VARIANT_NAME
  print("*** Core Variant: ");
  println(PORT_CORE_VARIANT_NAME);
#endif
#ifdef PORT_INFO
  print("*** Port Info:    ");
  println(PORT_INFO);
#endif
#ifdef PLATFORM_NAME
  print("*** Platform:     ");
  println(PLATFORM_NAME);
#endif
#ifdef BOARD_NAME
  print("*** Test Board:   ");
  println(BOARD_NAME);
#endif
  println("***");
  print("*** System Clock: ");
  printn(STM32_SYSCLK);
  println("");
  print("*** Iterations:   ");
  printn(ITERATIONS);
  println("");
  print("*** Randomize:    ");
  printn(RANDOMIZE);
  println("");

  println("");
  worst = 0;
  for (i = 1; i <= ITERATIONS; i++){
    print("Iteration ");
    printn(i);
    println("");
    saturated = FALSE;
    threshold = 0;
    for (interval = 2000; interval >= 10; interval -= interval / 10) {
      gptStartContinuous(&GPTD4, interval - 1); /* Slightly out of phase.*/
      gptStartContinuous(&GPTD3, interval + 1); /* Slightly out of phase.*/
      chThdSleepMilliseconds(1000);
      gptStopTimer(&GPTD4);
      gptStopTimer(&GPTD3);
      if (!saturated)
        print(".");
      else {
        print("#");
        if (threshold == 0)
          threshold = interval;
      }
    }
    /* Gives the worker threads a chance to empty the mailboxes before next
       cycle.*/
    chThdSleepMilliseconds(20);
    println("");
    print("Saturated at ");
    printn(threshold);
    println(" uS");
    println("");
    if (threshold > worst)
      worst = threshold;
  }
  gptStopTimer(&GPTD4);
  gptStopTimer(&GPTD3);

  print("Worst case at ");
  printn(worst);
  println(" uS");
  println("");
  println("Test Complete");

  /*
   * Normal main() thread activity, nothing in this test.
   */
  while (true) {
    chThdSleepMilliseconds(5000);
  }
}
