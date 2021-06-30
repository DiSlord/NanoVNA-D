*****************************************************************************
*** Files Organization                                                    ***
*****************************************************************************

--{root}                  - ChibiOS directory.
  +--readme.txt           - This file.
  +--documentation.html   - Shortcut to the web documentation page.
  +--license.txt          - GPL license text.
  +--demos/               - Demo projects, one directory per platform.
  +--docs/                - Documentation.
  |  +--common/           - Documentation common build resources.
  |  +--hal/              - Builders for HAL.
  |  |  +--Doxyfile_*     - Doxygen project files (required for rebuild).
  |  |  +--html/          - Local HTML documentation (after rebuild).
  |  |  +--reports/       - Test reports.
  |  |  +--rsc/           - Documentation resource files (required for rebuild).
  |  |  +--src/           - Documentation source files (required for rebuild).
  |  |  +--Doxyfile_*     - Doxygen project files (required for rebuild).
  |  |  +--index.html     - Local documentation access (after rebuild).
  |  +--nil/              - Builders for NIL.
  |  |  +--Doxyfile_*     - Doxygen project files (required for rebuild).
  |  |  +--html/          - Local HTML documentation (after rebuild).
  |  |  +--reports/       - Test reports.
  |  |  +--rsc/           - Documentation resource files (required for rebuild).
  |  |  +--src/           - Documentation source files (required for rebuild).
  |  |  +--Doxyfile_*     - Doxygen project files (required for rebuild).
  |  |  +--index.html     - Local documentation access (after rebuild).
  |  +--rt/               - Builders for RT.
  |  |  +--html/          - Local HTML documentation (after rebuild).
  |  |  +--reports/       - Test reports.
  |  |  +--rsc/           - Documentation resource files (required for rebuild).
  |  |  +--src/           - Documentation source files (required for rebuild).
  |  |  +--Doxyfile_*     - Doxygen project files (required for rebuild).
  |  |  +--index.html     - Local documentation access (after rebuild).
  +--ext/                 - External libraries, not part of ChibiOS/RT.
  +--os/                  - ChibiOS components.
  |  +--common/           - Shared OS modules.
  |  |  +--abstractions/  - API emulator wrappers.
  |  |  |  +--cmsis_os/   - CMSIS OS emulation layer for RT (ARMCMx port only).
  |  |  |  +--nasa_osal/  - NASA Operating System Abstraction Layer for RT.
  |  |  +--ext/           - Vendor files used by the OS.
  |  |  +--oslib/         - RTOS modules usable by both RT and NIL.
  |  |  +--ports/         - RTOS ports usable by both RT and NIL.
  |  |  +--startup/       - Startup support for all compilers and platforms.
  |  +--hal/              - HAL component.
  |  |  +--boards/        - HAL board support files.
  |  |  +--dox/           - HAL documentation resources.
  |  |  +--include/       - HAL high level headers.
  |  |  +--lib/           - HAL libraries.
  |  |  +--osal/          - HAL OSAL implementations.
  |  |  +--src/           - HAL high level source.
  |  |  +--ports/         - HAL ports.
  |  |  +--templates/     - HAL driver template files.
  |  |     +--osal/       - HAL OSAL templates.
  |  +--nil/              - NIL RTOS component.
  |  |  +--dox/           - NIL documentation resources.
  |  |  +--include/       - NIL high level headers.
  |  |  +--src/           - NIL high level source.
  |  |  +--templates/     - NIL configuration template files.
  |  +--rt/               - RT RTOS component.
  |  |  +--dox/           - RT documentation resources.
  |  |  +--include/       - RT high level headers.
  |  |  +--src/           - RT high level source.
  |  |  +--templates/     - RT configuration template files.
  |  +--various/          - Various portable support files.
  +--test/                - Kernel test suite source code.
  |  +--lib/              - Portable test engine.
  |  +--hal/              - HAL test suites.
  |  |  +--testbuild/     - HAL build test and MISRA check.
  |  +--nil/              - NIL test suites.
  |  |  +--testbuild/     - NIL build test and MISRA check.
  |  +--rt/               - RT test suites.
  |  |  +--testbuild/     - RT build test and MISRA check.
  |  |  +--coverage/      - RT code coverage project.
  +--testhal/             - HAL integration test demos.

*****************************************************************************
*** Releases and Change Log                                               ***
*****************************************************************************

*** Next ***
- HAL: All high level file names have been renamed and prefixed with "hal_"
       in order to minimize the risk of name conflicts when integrating
       external libraries. 
- LIB: Added Guarded Memory Pools to RT and NIL.
- RT:  Updated the RT test suite by generating the code using the new system.
- NIL: Updated the NIL test suite by generating the code using the new system.
- RT:  Removed I/O Queues and Streams interface, now those exists (much
       improved) inside the HAL.
- HAL: Improvements to the I/O queues now timeouts are absolute for
       iqReadTimeout() and oqWriteTimeout() functions.
- RT:  Added a NASA-OSAL API emulator over the RT kernel.
- RT:  Added RT-STM32L476-DISCOVERY demo.
- HAL: Added more STM32L4xx testhal demos.
- HAL: Updated all STM32F476 mcuconf.h files.
- ALL: Startup files relicensed under Apache 2.0.
- ALL: Enhanced GCC .ld files with multiple flash regions and capability to
       insert additional sections within the standard loading rules.
- VAR: The shell now accepts quoted arguments.
- VAR: Centralized all usual shell commands into a single shell_cmd.c
       file. This will allow to update all demos with a single change.
       Each single command can be disabled using preprocessor switches.
       Shell files are now located under ./os/various/shell and have a
       dedicated shell.mk file.
- ALL: Reorganized source tree, now ports are shared between RT and NIL.
- RT:  Merged RT4.
- NIL: Merged NIL2.
- NIL: Added STM32F7 demo.
- HAL: Fixed PAL lines support not working for STM32 GPIOv1 (bug #730)
       (backported to 16.1.4).
- RT:  Fixed bug in chSchPreemption() function (bug #728)(backported to 2.6.10,
       3.0.6 and 16.1.5).
- HAL: Fixed prescaler not initialized in STM32 ADCv1 (bug #725)
       (backported to 16.1.5).
- HAL: Fixed missing DAC section in STM32F072 mcuconf.h files (bug #724)
       (backported to 16.1.5).
- VAR: Fixed palSetMode glitching outputs (bug #723)(backported to 3.0.6
       and 16.1.4).
- VAR: Fixed error in STM32 PWM driver regarding channels 4 and 5 (bug #722)
       (backported to 3.0.6 and 16.1.4).
- VAR: Fixed wrong flash and ram size in linker script for maple mini
       (bug #719).
- VAR: Fixed GCC 5.2 crashes while compiling ChibiOS (bug #718)(backported
       to 3.0.6 and 16.1.4).
- HAL: Fixed wrong definition in STM32L4 ext_lld_isr.h (bug #717)
       (backported to 16.1.4).
- HAL: Fixed wrong definitions in STM32F746 mcuconf.h files (bug #716)
       (backported to 16.1.4).
- RT:  Fixed wrong SysTick initialization in generic demos (bug #715)
       (backported to 16.1.4).
- NIL: Fixed wrong SysTick initialization in generic demos (bug #715)
       (backported to 16.1.4).
- HAL: Fixed usbStop does not resume threads suspended in synchronous calls
       to usbTransmit (bug #714)(backported to 16.1.4).
- VAR: Fixed state check in lwIP when SYS_LIGHTWEIGHT_PROT is disabled
       (bug #713)(backported to 2.6.10, 3.0.6 and 16.1.4). 
- RT:  Removed the p_msg field from the thread_t structure saving a
       msg_t-sized field from the structure. Messages now use a new field
       into the p_u union. Now synchronous messages are even faster.
- HAL: Fixed IAR warnings in ext_lld_isr.c (bug #711)(backported to 16.1.4).
- HAL: Fixed build error caused by STM32 SPIv1 driver (bug #710)(backported
       to 3.0.6 and 16.1.4).
- HAL: Fixed shift of signed constant causes warnings with IAR compiler
       (bug #709)(backported to 2.6.10, 3.0.6 and 16.1.4).
- HAL: Fixed wrong RTCv2 settings for STM32L4 (bug #708)(backported
       to 16.1.4).
- HAL: Fixed missing OTGv1 support for STM32L4 (bug #707)(backported
       to 16.1.4).
- NIL: Fixed ARM errata 752419 (bug #706)(backported to 2.6.10,
       3.0.6 and 16.1.4).
- RT:  Fixed ARM errata 752419 (bug #706)(backported to 2.6.10,
       3.0.6 and 16.1.4).
- HAL: Fixed unused variable in STM32 SPIv2 driver (bug #705)(backported
       to 16.1.3).
- HAL: Fixed chDbgAssert() still called from STM32 SPIv1 driver (bug #704)
       (backported to 3.0.6 and 16.1.3).
- HAL: Fixed broken demo for STM32F429 (bug #703)(backported to 16.1.3).
- HAL: Fixed wrong macro definition for palWriteLine (bug #702)(backported
       to 16.1.3).
- HAL: Fixed error is buffer queues (bug #701)(backported to 16.1.3).
- HAL: Fixed F105_F107 CANv1 build failure (bug #699).
- HAL: Fixed typos in STM32F0 RCC enable/disable macros (bug #698)(backported
       to 16.1.3).
- RT:  Fixed useless call to chTMStartMeasurementX() in _thread_init()
       (bug #697)(backported to 3.0.6 and 16.1.3).
- VAR: Fixed missing time conversion in lwIP arch module (bug #696)
       (backported to 2.6.10, 3.0.5 and 16.1.2).
- HAL: Fixed incorrect handling of TIME_IMMEDIATE in the HAL buffer queues
       (bug #695)(backported to 16.1.2).
- NIL: Fixed NIL_CFG_USE_EVENTS not properly checked in NIL (bug #694)
       (backported to 3.0.5 and 16.1.1).
- RT:  Fixed ISR statistics are not updated from a critical zone in RT
       (bug #693)(backported to 3.0.5 and 16.1.1).
- NIL: Fixed NIL test suite calls I and S functions outside critical zone
       (bug #692)(backported to 3.0.5 and 16.1.1).
- NIL: Fixed protocol violation in NIL OSAL (bug #691)(backported to
       3.0.5 and 16.1.1).
- HAL: Fixed error in HAL buffer queues (bug #689)(backported to 16.1.1).
- RT:  Fixed tm_stop - best case bug (bug #688)(backported to 16.1.1
       and 3.0.5).
- ALL: Several minor documentation/formatting-related fixes.

*** 16.1.0 ***
- RT:  Added CodeWarrior compiler support to the e200 port.
- HAL: Added support for STM32F446.
- HAL: Introduced preliminary support for STM32F7xx devices.
- HAL: Introduced preliminary support for STM32L4xx devices.
- HAL: Introduced preliminary support for STM32L0xx devices.
- HAL: Increased performance of USBv1 and OTGv1 driver thanks to better
       data copying code.
- HAL: Enhanced Serial-USB driver using the new buffers queues object.
- HAL: Simplified USB driver, queued API has been removed.
- HAL: Enhanced the CAN driver with I-class functions. Now it is possible
       to exchange frames from ISRs.
- HAL: Added watchdog driver model (WDG) and STM32 implementation on IWDG.
- HAL: Added synchronous API and mutual exclusion to the UART driver.
- HAL: Added PAL driver for STM32L4xx GPIOv3 peripheral.
- HAL: Added I2S driver for STM32 SPIv2 peripheral.
- HAL: Added demos an- d board files for ST's Nucleo32 boards (F031, F042, F303).
- HAL: Added "lines" handling to PAL driver, lines are identifiers of both
       ports and pins encoded in a single value. Added a set of macros
       operating on lines.
- HAL: Merged the latest STM32F3xx CMSIS headers.
- HAL: Merged the latest STM32F2xx CMSIS headers and fixed the support
       broken in 3.0.x.
- RT:  Added new function chVTGetTimersStateI() returning the state of the
       timers list.
- HAL: Now STM32 USARTv2 driver initializes the ISR vectors statically on
       initialization. Disabling them was not necessary and added to
       the code size.
- HAL: Added DMA channel selection on STM32F030xC devices.
- HAL: Added serial driver support for USART 3..6 on STM32F030xC devices.
- HAL: Merged the newest ST header files for STM32F1xx.
- HAL: Added support for differential mode to the STM32F3xx ADC driver.
- HAL: Experimental isochronous capability added to STM32 OTGv1 driver.
- HAL: Modified the serial-USB driver to reject write/read attempts if the
       underlying USB is not in active state. In case of disconnection the
       SDU driver broadcasts a CHN_DISCONNECTED event.
- HAL: Modified the USB driver to have a separate USB_SUSPENDED state, this
       allows the application to detect if the USB is communicating or if
       it is disconnected or powered down.
- HAL: Added wake-up and suspend events to the STM32 OTGv1 driver.
- HAL: STM32 USB/OTG buffers and queues do not more require to be aligned in
       position and size.
- VAR: Improved GCC rules.ld, now it is possible to assign the heap to any
       of the available RAM regions.
- HAL: STM32 GPT, ICU and PWM driver enhancements. Now it is possible to
       suppress default ISRs by defining STM32_TIMx_SUPPRESS_ISR.
       The application is now able to define custom handlers if required
       or simply save space if the driver callbacks are not used.
       Now the functions xxx_lld_serve_interrupts() have global scope, this
       way custom ISRs can call them from outside the driver module.
- HAL: Added TIM units use cross-check in STM32 GPT, ICU, PWM and ST drivers,
       now use collisions are explicitly reported.
- NIL: Added polled delays required to fix bug #629.
- HAL: Added support for I2C3 and I2C4 to the STM32 I2Cv2 I2C driver.
- HAL: Added support for SPI4...SPI6 to the STM32 SPIv2 SPI driver.
- HAL: Added support for UART4...UART8 to the STM32 UARTv2 UART driver.
- HAL: Added support for UART7 and UART8,LPUART1 to the STM32 UARTv2 serial
       driver.
- HAL: STM32F3xx and STM32L4xx devices now share the same ADCv3 driver.
- HAL: STM32F2xx, STM32F4xx and STM32F7xx devices now share the same ADCv2
       and DMAv2 drivers.
- HAL: STM32F0xx and STM32L0xx devices now share the same ADCv1 driver.
- HAL: STM32F0xx, STM32F1xx, STM32F3xx, STM32F37x, STM32L0xx and STM32L1xx
       devices now share the same DMAv1 driver.
- HAL: New STM32 shared DMAv2 driver supporting channel selection and
       data cache invalidation (F2, F4, F7).
- HAL: New STM32 shared DMAv1 driver supporting channel selection and fixing
       the behavior with shared IRQs (F0, L0).
- HAL: New STM32 ADCv3 driver supporting middle STM32 devices (F3, L4).
- HAL: New STM32 ADCv2 driver supporting large STM32 devices (F2, F4, F7).
- HAL: New STM32 ADCv1 driver supporting small STM32 devices (F0, L0).
- HAL: Introduced support for TIM21 and TIM22 in STM32 ST driver.
- HAL: Updated STM32F0xx headers to STM32CubeF0 version 1.3.0. Added support
       for STM32F030xC, STM32F070x6, STM32F070xB, STM32F091xC,
       STM32F098xx devices.
- RT:  Fixed ARM port enforcing THUMB mode (bug #687)(backported to 3.0.5).
- HAL: Fixed HAL drivers still calling RT functions (bug #686)(backported
       to 3.0.5).
- HAL: Fixed chprintf() still calling RT functions (bug #684)(backported
       to 3.0.5).
- HAL: Fixed STM32 ICU driver uses chSysLock and chSysUnlock (bug #681)
       (backported to 3.0.4).
- HAL: Fixed wrong DMA priority assigned to STM32F3 ADC3&4 (bug #680)
       (backported to 3.0.4 and 2.6.10).
- HAL: Fixed invalid DMA settings in STM32 DACv1 driver in dual mode
       (bug #677)(backported to 3.0.4).
- HAL: Fixed usbStop() hangs in STM32 OTGv1 driver (bug #674)(backported
       to 3.0.4 and 2.6.10).
- HAL: Fixed STM32 I2Cv2 driver fails on transfers greater than 255 bytes
       (bug #673)(backported to 3.0.4).
- HAL: Fixed STM32 I2Cv2 DMA conflict (bug #671)(backported to 3.0.4).
- HAL: Fixed I2S clock selection not working in STM32F4xx HAL (bug #667)
       (backported to 3.0.4 and 2.6.10).
- HAL: Fixed differences in STM32F3 ADC macro definitions (bug #665)
       (backported to 3.0.3).
- HAL: Fixed RTC module loses day of week when converting (bug #664)
       (backported to 3.0.3).
- HAL: Fixed STM32 USBv1 broken isochronous endpoints (bug #662)
       (backported to 3.0.4).
- HAL: Fixed STM32 USBv1 wrong multiplier when calculating descriptor address
       in BTABLE (bug #661)(backported to 3.0.4 and 2.6.10).
- HAL: Fixed STM32 USBv1 does not make use of BTABLE_ADDR define (bug #660)
       (backported to 3.0.4 and 2.6.10).
- HAL: Fixed invalid class type for sdPutWouldBlock() and sdGetWouldBlock()
       functions (bug #659)(backported to 3.0.3 and 2.6.10).
- HAL: Fixed STM32F0xx HAL missing MCOPRE support (bug #658).
- HAL: Fixed STM32L1xx HAL errors in comments (bug #657)(backported
       to 3.0.3 and 2.6.10).
- HAL: Fixed STM32 USBv1 wrong buffer alignment (bug #656)(backported
       to 3.0.3 and 2.6.10).
- HAL: Fixed wrong vector name for STM32F3xx EXTI33 (bug #655)(backported
       to 3.0.3 and 2.6.10).
- HAL: Fixed nvicEnableVector broken for Cortex-M0 (bug #654)(backported
       to 3.0.3).
- HAL: Fixed no demo for nucleo STM32F072RB board (bug #652).
- HAL: Fixed missing RCC and ISR definitions for STM32F0xx timers (bug #651)
       (backported to 3.0.3 and 2.6.10).
- HAL: Fixed incorrect compiler check in STM32 RTCv1 driver (bug #650)
       (backported to 3.0.3).
- HAL: Fixed incorrect case in path (bug #649).
- HAL: Fixed STM32F3xx HAL checking for non-existing macros (bug #648)
       (backported to 3.0.3 and 2.6.10).
- HAL: Fixed error in STM32F030 EXT driver (bug #647)(backported to 3.0.3).
- RT:  Fixed problem with chVTIsTimeWithinX() (bug #646)(backported to
       3.0.3 and 2.6.10).
- VAR: Fixed _sbrk_r with incr == 0 should be valid (bug #645)(backported to
       3.0.3 and 2.6.10).
- RT:  Fixed issues in CMSIS RTOS interface (bug #644)(backported to 3.0.3).
- HAL: Fixed RT dependency in STM32 SDCv1 driver (bug #643)(backported
       to 3.0.2).
- VAR: Fixed incorrect working area size in LwIP creation in demos (bug #642)
       (backported to 3.0.2 and 2.6.10).
- HAL: Fixed error in hal_lld_f100.h checks (bug #641)(backported to 3.0.2
       and 2.6.10).
- HAL: Fixed volatile variable issue in I/O queues, both RT and HAL (bug #640)
       (backported to 3.0.2).
- HAL: Fixed wrong DMA assignment for I2C1 in STM32F302xC registry (bug #637)
       (backported to 3.0.2).
- HAL: Fixed missing timers 5, 6, 7, 10 & 11 from STM32L1 HAL port (bug #636)
       (backported to 3.0.2).
- VAR: Fixed CRT0_CALL_DESTRUCTORS not utilized in crt0_v7m.s (bug #635)
       (backported to 3.0.2).
- HAL: Fixed wrong ld file in STM32F072xB USB CDC demo (bug #634)(backported
       to 3.0.2).
- NIL: Fixed Wrong assertion in NIL chSemResetI() and NIL OSAL
       osalThreadDequeueAllI() (bug #633)(backported to 3.0.2).
- RT:  Fixed problem with RT mutexes involving priority inheritance (bug #632)
       (backported to 3.0.2 and 2.6.10).
- HAL: Fixed HAL to RT dependency in STM32 DAC driver (bug #631)(backported
       to 3.0.2).
- HAL: Fixed problem with STM32 I2S driver restart (bug #630)(backported
       to 3.0.2).
- HAL: Fixed STM32F3xx ADC driver uses US2RTC directly (bug #629)(backported
       to 3.0.2).
- HAL: Fixed CEC clock cannot be disabled on STM32F0xx (bug #628)
       (backported to 3.0.1).
- VAR: Fixed lwIP arch code breaks with a 16-bit systick timer (bug #627)
       (backported to 3.0.1).
- HAL: Fixed broken MAC driver for STM32F107 (bug #626)(backported to 3.0.1).
- NIL: Fixed missing configuration options from NIL PPC port (bug #625)
       (backported to 3.0.1).
- HAL: Fixed wrong offset in STM32 DAC driver (bug #624)(backported to 3.0.1).
- HAL: Fixed crash on STM32F030x4/6 devices (bug #623)(backported to 3.0.1).
- HAL: Fixed duplicated doxygen tag in STM32F4xx hal_lld.h file (bug #621)
       (backported to 3.0.1 and 2.6.9).
- HAL: Fixed STM32F042 registry error (bug #620)(backported to 3.0.1).
- HAL: Fixed wrong check in canReceive() (bug #619)(backported to 3.0.1
       and 2.6.9).
- HAL: Fixed wrong EXTI[18] vector number on STM32F373 (bug #618)(backported
       to 3.0.1 and 2.6.9).
- HAL: Fixed wrong check on STM32_LSE_ENABLED definition in STM32L1xx HAL port
       (bug #617)(backported to 3.0.1 and 2.6.9).
- HAL: Fixed rtcConvertDateTimeToFAT() incorrect conversion (bug #615)
       (backported to 3.0.1).
- HAL: Fixed missing UART7 and UART8 support on STM32F4xx family (bug #612).
- HAL: Fixed outdated CMSIS headers for STM32F1xx devices (bug #609).
- HAL: Fixed CAN errors (bug #387).
- HAL: Fixed USB HS ULPI Support (except board files because patch originally
       targeted version 2.6.x)(bug #377).

*** 3.0.0 ***
- NEW: Added an initialization function to the lwIP bindings, now it is
       sufficient to call lwipInit(NULL); in order to start the subsystem.
       Demo updated.
- RT:  Fixed compilation error in RT when registry is disabled (bug #614).
- NIL: Fixed OSAL_ST_MODE not defined in AVR port (bug #613).
- NIL: Fixed nilrtos redefinition of systime_t (bug #611).
- HAL: Fixed TIM2 wrongly classified as 32bits in STM32F1xx devices
       (bug #610).

*** 3.0.0p6 ***
- HAL: Removed call to localtime_r() function for non-GNU compilers in
       STM32F1xx RTC driver.
- DEM: Fixed the FatFS demo timeout, now it is expressed in milliseconds.
- DEM: Added -Wundef to all the demos and test programs in order to find
       common error cases.
- NIL: Added INTC priorities check to the e200z port.
- RT:  Added INTC priorities check to the e200z port.
- HAL: Added support for CAN in STM32F042/72 devices.
- HAL: Added support for extra DMA channels in STM32F072 devices.
- HAL: Modified the STM32 CAN driver to support unified IRQs.
- RT:  SPE-related issue in e200z ports (bug #607).
- NIL: SPE-related issue in e200z ports (bug #607).
- HAL: Fixed dependency between STM32 MAC driver and RT (bug #606).
- HAL: Fixed wrong macro names in STM32F0xx HAL driver (bug #605).
- HAL: Fixed wrong check on ADC3 in STM32F3xx ADC driver (bug #604).
- HAL: Fixed wrong macro names in STM32F3xx HAL driver (bug #603).
- HAL: Fixed errors in STM32 OTGv1 driver (bug #601).
- DEM: Fixed missing paths in e200z demos (bug #600).
- HAL: Fixed error in platform_f105_f107.mk file (bug #599).
- HAL: Fixed issue in DMA drivers when channels share ISRs (bug #597).

*** 3.0.0p5 ***
- HAL: Added no-DMA mode to the STM32 I2Cv2 driver.
- HAL: Added DAC support to all STM32 sub-platforms, added another demo for
       the STM32F3xx.
- HAL: Fixed STM32 USARTv1: incorrect txend2_cb callback behavior (bug #596).
- DEM: Fixed wrong comment in ARMCM4-STM32F401RE-NUCLEO demo (bug #595).
- HAL: Fixed STM32 SDC LLD driver initialization with Asserts disabled
       (bug #594).

*** 3.0.0p4 ***
- NEW: Added no-DMA mode to STM32 I2Cv2 driver.
- BLD: New "smart build" mode added to makefiles, now only used files are
       compiled.
- HAL: Change to the Serial_USB driver, now the INT endpoint is no more
       mandatory.
- HAL: New DAC driver implementation for STM32F4xx.
- HAL: Fixed SDC STM32 driver broken in 50MHz mode (bug #592).
- HAL: Fixed STM32 RTC SSR Register Counts Down (bug #591).
- HAL: Fixed STM32 RTC PRER Register not being set in init (bug #590).
- HAL: Fixed STM32F334 does not have an EXT18 interrupt (bug #588).
- HAL: Fixed STM32L1xx USB is missing disconnect/connect macros (bug #587).
- HAL: Fixed wrong vector number for STM32L1xx USB (bug #586).
- HAL: Fixed spurious TC interrupt in STM32 UART (v1 and v2) driver (bug #584).
- HAL: Fixed invalid checks on STM32L1xx LSI and LSE clocks (bug #583).
- HAL: Fixed RCC CAN2 macros missing in STM32F1xx platform (bug #582).
- HAL: Fixed STM32 I2Cv2 driver issue (bug 581).
- BLD: Fixed ules.mk: adding "PRE_MAKE_ALL_RULE_HOOK" (bug #580).
- BLD: Fixed rules.mk should not explicitly depend on $(BUILDDIR) (bug #579).

*** 3.0.0p3 ***
- RT:  Fixed tickless mode instability in RT (bug 577).

*** 3.0.0p2 ***
- HAL: Fixed instances of RT API in HAL drivers (bug 574).
- RT:  Fixed system time overflow issue in tickless mode (bug 573).
- RT:  Improvements to the IRQ_STORM applications.

*** 3.0.0p1 ***
- First 3.0.0 release, see release note 3.0.0.
