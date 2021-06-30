/*
    ChibiOS - Copyright (C) 2006..2016 Giovanni Di Sirio.

    This file is part of ChibiOS.

    ChibiOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    ch.h
 * @brief   Nil RTOS main header file.
 * @details This header includes all the required kernel headers so it is the
 *          only header you usually need to include in your application.
 *
 * @addtogroup NIL_KERNEL
 * @{
 */

#ifndef CH_H
#define CH_H

#include "chtypes.h"
#include "chconf.h"
#include "chlicense.h"

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

/**
 * @brief   ChibiOS/NIL identification macro.
 */
#define _CHIBIOS_NIL_

/**
 * @brief   Stable release flag.
 */
#define CH_KERNEL_STABLE        0

/**
 * @name    ChibiOS/NIL version identification
 * @{
 */
/**
 * @brief   Kernel version string.
 */
#define CH_KERNEL_VERSION       "2.0.0"

/**
 * @brief   Kernel version major number.
 */
#define CH_KERNEL_MAJOR         2

/**
 * @brief   Kernel version minor number.
 */
#define CH_KERNEL_MINOR         0

/**
 * @brief   Kernel version patch number.
 */
#define CH_KERNEL_PATCH         0
/** @} */

/**
 * @name    Wakeup messages
 * @{
 */
#define MSG_OK                  (msg_t)0    /**< @brief OK wakeup message.  */
#define MSG_TIMEOUT             (msg_t)-1   /**< @brief Wake-up caused by
                                                 a timeout condition.       */
#define MSG_RESET               (msg_t)-2   /**< @brief Wake-up caused by
                                                 a reset condition.         */
/** @} */

/**
 * @name    Special time constants
 * @{
 */
/**
 * @brief   Zero time specification for some functions with a timeout
 *          specification.
 * @note    Not all functions accept @p TIME_IMMEDIATE as timeout parameter,
 *          see the specific function documentation.
 */
#define TIME_IMMEDIATE          ((systime_t)-1)

/**
 * @brief   Infinite time specification for all functions with a timeout
 *          specification.
 */
#define TIME_INFINITE           ((systime_t)0)
/** @} */

/**
 * @name    Thread state related macros
 * @{
 */
#define NIL_STATE_READY         (tstate_t)0 /**< @brief Thread ready or
                                                 executing.                 */
#define NIL_STATE_SLEEPING      (tstate_t)1 /**< @brief Thread sleeping.    */
#define NIL_STATE_SUSP          (tstate_t)2 /**< @brief Thread suspended.   */
#define NIL_STATE_WTSEM         (tstate_t)3 /**< @brief On semaphore.       */
#define NIL_STATE_WTOREVT       (tstate_t)4 /**< @brief Waiting for events. */
#define NIL_THD_IS_READY(tr)    ((tr)->state == NIL_STATE_READY)
#define NIL_THD_IS_SLEEPING(tr) ((tr)->state == NIL_STATE_SLEEPING)
#define NIL_THD_IS_SUSP(tr)     ((tr)->state == NIL_STATE_SUSP)
#define NIL_THD_IS_WTSEM(tr)    ((tr)->state == NIL_STATE_WTSEM)
#define NIL_THD_IS_WTOREVT(tr)  ((tr)->state == NIL_STATE_WTOREVT)
/** @} */

/**
 * @name    Events related macros
 * @{
 */
/**
 * @brief   All events allowed mask.
 */
#define ALL_EVENTS              ((eventmask_t)-1)

/**
 * @brief   Returns an event mask from an event identifier.
 */
#define EVENT_MASK(eid)         ((eventmask_t)(1 << (eid)))
/** @} */

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @brief   Number of user threads in the application.
 * @note    This number is not inclusive of the idle thread which is
 *          implicitly handled.
 */
#if !defined(CH_CFG_NUM_THREADS) || defined(__DOXYGEN__)
#define CH_CFG_NUM_THREADS                  2
#endif

/**
 * @brief   System time counter resolution.
 * @note    Allowed values are 16 or 32 bits.
 */
#if !defined(CH_CFG_ST_RESOLUTION) || defined(__DOXYGEN__)
#define CH_CFG_ST_RESOLUTION                32
#endif

/**
 * @brief   System tick frequency.
 * @note    This value together with the @p CH_CFG_ST_RESOLUTION
 *          option defines the maximum amount of time allowed for
 *          timeouts.
 */
#if !defined(CH_CFG_ST_FREQUENCY) || defined(__DOXYGEN__)
#define CH_CFG_ST_FREQUENCY                 100
#endif

/**
 * @brief   Time delta constant for the tick-less mode.
 * @note    If this value is zero then the system uses the classic
 *          periodic tick. This value represents the minimum number
 *          of ticks that is safe to specify in a timeout directive.
 *          The value one is not valid, timeouts are rounded up to
 *          this value.
 */
#if !defined(CH_CFG_ST_TIMEDELTA) || defined(__DOXYGEN__)
#define CH_CFG_ST_TIMEDELTA                 0
#endif

/**
 * @brief   Semaphores APIs.
 * @details If enabled then the Semaphores APIs are included in the kernel.
 *
 * @note    The default is @p TRUE.
 */
#if !defined(CH_CFG_USE_SEMAPHORES) || defined(__DOXYGEN__)
#define CH_CFG_USE_SEMAPHORES               TRUE
#endif

/**
 * @brief   Mutexes APIs.
 * @details If enabled then the mutexes APIs are included in the kernel.
 *
 * @note    Feature not currently implemented.
 * @note    The default is @p FALSE.
 */
#if !defined(CH_CFG_USE_MUTEXES) || defined(__DOXYGEN__)
#define CH_CFG_USE_MUTEXES                  FALSE
#endif

/**
 * @brief   Events Flags APIs.
 * @details If enabled then the event flags APIs are included in the kernel.
 *
 * @note    The default is @p TRUE.
 */
#if !defined(CH_CFG_USE_EVENTS) || defined(__DOXYGEN__)
#define CH_CFG_USE_EVENTS                   TRUE
#endif

/**
 * @brief   Mailboxes APIs.
 * @details If enabled then the asynchronous messages (mailboxes) APIs are
 *          included in the kernel.
 *
 * @note    The default is @p TRUE.
 * @note    Requires @p CH_CFG_USE_SEMAPHORES.
 */
#if !defined(CH_CFG_USE_MAILBOXES) || defined(__DOXYGEN__)
#define CH_CFG_USE_MAILBOXES                TRUE
#endif

/**
 * @brief   Core Memory Manager APIs.
 * @details If enabled then the core memory manager APIs are included
 *          in the kernel.
 *
 * @note    The default is @p TRUE.
 */
#if !defined(CH_CFG_USE_MEMCORE) || defined(__DOXYGEN__)
#define CH_CFG_USE_MEMCORE                  TRUE
#endif

/**
 * @brief   Heap Allocator APIs.
 * @details If enabled then the memory heap allocator APIs are included
 *          in the kernel.
 *
 * @note    The default is @p TRUE.
 */
#if !defined(CH_CFG_USE_HEAP) || defined(__DOXYGEN__)
#define CH_CFG_USE_HEAP                     TRUE
#endif

/**
 * @brief   Memory Pools Allocator APIs.
 * @details If enabled then the memory pools allocator APIs are included
 *          in the kernel.
 *
 * @note    The default is @p TRUE.
 */
#if !defined(CH_CFG_USE_MEMPOOLS) || defined(__DOXYGEN__)
#define CH_CFG_USE_MEMPOOLS                 TRUE
#endif

/**
 * @brief   Debug option, kernel statistics.
 *
 * @note    Feature not currently implemented.
 * @note    The default is @p FALSE.
 */
#if !defined(CH_DBG_STATISTICS) || defined(__DOXYGEN__)
#define CH_DBG_STATISTICS                   FALSE
#endif

/**
 * @brief   Debug option, system state check.
 * @note    This is a planned feature, not yet implemented.
 *
 * @note    The default is @p FALSE.
 */
#if !defined(CH_DBG_SYSTEM_STATE_CHECK) || defined(__DOXYGEN__)
#define CH_DBG_SYSTEM_STATE_CHECK           FALSE
#endif

/**
 * @brief   Debug option, parameters checks.
 *
 * @note    The default is @p FALSE.
 */
#if !defined(CH_DBG_ENABLE_CHECKS) || defined(__DOXYGEN__)
#define CH_DBG_ENABLE_CHECKS                FALSE
#endif

/**
 * @brief   System assertions.
 *
 * @note    The default is @p FALSE.
 */
#if !defined(CH_DBG_ENABLE_ASSERTS) || defined(__DOXYGEN__)
#define CH_DBG_ENABLE_ASSERTS               FALSE
#endif

/**
 * @brief   Stack check.
 *
 * @note    The default is @p FALSE.
 */
#if !defined(CH_DBG_ENABLE_STACK_CHECK) || defined(__DOXYGEN__)
#define CH_DBG_ENABLE_STACK_CHECK           FALSE
#endif

/**
 * @brief   System initialization hook.
 */
#if !defined(CH_CFG_SYSTEM_INIT_HOOK) || defined(__DOXYGEN__)
#define CH_CFG_SYSTEM_INIT_HOOK() {}
#endif

/**
 * @brief   Threads descriptor structure extension.
 * @details User fields added to the end of the @p thread_t structure.
 */
#if !defined(CH_CFG_THREAD_EXT_FIELDS) || defined(__DOXYGEN__)
#define CH_CFG_THREAD_EXT_FIELDS
#endif

/**
 * @brief   Threads initialization hook.
 */
#if !defined(CH_CFG_THREAD_EXT_INIT_HOOK) || defined(__DOXYGEN__)
#define CH_CFG_THREAD_EXT_INIT_HOOK(tr) {}
#endif

/**
 * @brief   Idle thread enter hook.
 * @note    This hook is invoked within a critical zone, no OS functions
 *          should be invoked from here.
 * @note    This macro can be used to activate a power saving mode.
 */
#if !defined(CH_CFG_IDLE_ENTER_HOOK) || defined(__DOXYGEN__)
#define CH_CFG_IDLE_ENTER_HOOK() {}
#endif

/**
 * @brief   Idle thread leave hook.
 * @note    This hook is invoked within a critical zone, no OS functions
 *          should be invoked from here.
 * @note    This macro can be used to deactivate a power saving mode.
 */
#if !defined(CH_CFG_IDLE_LEAVE_HOOK) || defined(__DOXYGEN__)
#define CH_CFG_IDLE_LEAVE_HOOK() {}
#endif

/**
 * @brief   System halt hook.
 */
#if !defined(CH_CFG_SYSTEM_HALT_HOOK) || defined(__DOXYGEN__)
#define CH_CFG_SYSTEM_HALT_HOOK(reason) {}
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if CH_CUSTOMER_LICENSED_NIL == FALSE
#error "ChibiOS/NIL not licensed"
#endif

#if (CH_LICENSE_FEATURES != CH_FEATURES_FULL) &&                            \
    (CH_LICENSE_FEATURES != CH_FEATURES_INTERMEDIATE) &&                    \
    (CH_LICENSE_FEATURES == CH_FEATURES_BASIC)
#error "invalid CH_LICENSE_FEATURES setting"
#endif

/* Restrictions in basic and intermediate modes.*/
#if (CH_LICENSE_FEATURES == CH_FEATURES_INTERMEDIATE) ||                    \
    (CH_LICENSE_FEATURES == CH_FEATURES_BASIC)

/* System tick limited to 1000hz.*/
#if CH_CFG_ST_FREQUENCY > 1000
#undef CH_CFG_ST_FREQUENCY
#define CH_CFG_ST_FREQUENCY                 1000
#endif

/* Restricted subsystems.*/
#undef CH_CFG_USE_MAILBOXES

#define CH_CFG_USE_MAILBOXES                FALSE

#endif /* (CH_LICENSE_FEATURES == CH_FEATURES_INTERMEDIATE) ||
          (CH_LICENSE_FEATURES == CH_FEATURES_BASIC) */

/* Restrictions in basic mode.*/
#if CH_LICENSE_FEATURES == CH_FEATURES_BASIC

/* Tick-Less mode restricted.*/
#undef CH_CFG_ST_TIMEDELTA
#define CH_CFG_ST_TIMEDELTA                 0

/* Restricted subsystems.*/
#undef CH_CFG_USE_MEMCORE
#undef CH_CFG_USE_MEMPOOLS
#undef CH_CFG_USE_HEAP

#define CH_CFG_USE_MEMCORE                  FALSE
#define CH_CFG_USE_MEMPOOLS                 FALSE
#define CH_CFG_USE_HEAP                     FALSE

#endif /* CH_LICENSE_FEATURES == CH_FEATURES_BASIC */

#if !defined(_CHIBIOS_NIL_CONF_)
#error "missing or wrong configuration file"
#endif

#if CH_CFG_NUM_THREADS < 1
#error "at least one thread must be defined"
#endif

#if CH_CFG_NUM_THREADS > 16
#error "ChibiOS/NIL is not recommended for thread-intensive applications,"  \
       "consider ChibiOS/RT instead"
#endif

#if (CH_CFG_ST_RESOLUTION != 16) && (CH_CFG_ST_RESOLUTION != 32)
#error "invalid CH_CFG_ST_RESOLUTION specified, must be 16 or 32"
#endif

#if CH_CFG_ST_FREQUENCY <= 0
#error "invalid CH_CFG_ST_FREQUENCY specified, must be greated than zero"
#endif

#if (CH_CFG_ST_TIMEDELTA < 0) || (CH_CFG_ST_TIMEDELTA == 1)
#error "invalid CH_CFG_ST_TIMEDELTA specified, must "                       \
       "be zero or greater than one"
#endif

#if CH_CFG_USE_MUTEXES == TRUE
#error "mutexes not yet supported"
#endif

#if CH_DBG_STATISTICS == TRUE
#error "statistics not yet supported"
#endif

#if (CH_DBG_SYSTEM_STATE_CHECK == TRUE) ||                                  \
    (CH_DBG_ENABLE_CHECKS == TRUE)      ||                                  \
    (CH_DBG_ENABLE_ASSERTS == TRUE)     ||                                  \
    (CH_DBG_ENABLE_STACK_CHECK == TRUE)
#define NIL_DBG_ENABLED                 TRUE
#else
#define NIL_DBG_ENABLED                 FALSE
#endif

/** Boundaries of the idle thread boundaries, only required if stack checking
    is enabled.*/
#if (CH_DBG_ENABLE_STACK_CHECK == TRUE) || defined(__DOXYGEN__)
#define THD_IDLE_BASE                   (&__main_thread_stack_base__)
#define THD_IDLE_END                    (&__main_thread_stack_end__)
#else
#define THD_IDLE_BASE                   NULL
#define THD_IDLE_END                    NULL
#endif

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   Type of system time.
 */
#if (CH_CFG_ST_RESOLUTION == 32) || defined(__DOXYGEN__)
typedef uint32_t systime_t;
#else
typedef uint16_t systime_t;
#endif

/**
 * @brief   Type of a structure representing a thread.
 * @note    It is required as an early definition.
 */
typedef struct nil_thread thread_t;

#include "chcore.h"

#if (CH_CFG_USE_SEMAPHORES == TRUE) || defined(__DOXYGEN__)
/**
 * @brief   Type of a structure representing a semaphore.
 */
typedef struct nil_semaphore semaphore_t;

/**
 * @brief   Structure representing a counting semaphore.
 */
struct nil_semaphore {
  volatile cnt_t    cnt;        /**< @brief Semaphore counter.              */
};
#endif /* CH_CFG_USE_SEMAPHORES == TRUE */

/**
 * @brief Thread function.
 */
typedef void (*tfunc_t)(void *p);

/**
 * @brief   Type of a structure representing a thread static configuration.
 */
typedef struct nil_thread_cfg thread_config_t;

/**
 * @brief   Structure representing a thread static configuration.
 */
struct nil_thread_cfg {
  stkalign_t        *wbase;     /**< @brief Thread working area base.       */
  stkalign_t        *wend;      /**< @brief Thread working area end.        */
  const char        *namep;     /**< @brief Thread name, for debugging.     */
  tfunc_t           funcp;      /**< @brief Thread function.                */
  void              *arg;       /**< @brief Thread function argument.       */
};

/**
 * @brief   Type of a thread reference.
 */
typedef thread_t * thread_reference_t;

/**
 * @brief   Structure representing a thread.
 */
struct nil_thread {
  struct port_context   ctx;        /**< @brief Processor context.          */
  tstate_t              state;      /**< @brief Thread state.               */
  /* Note, the following union contains a pointer while the thread is in a
     sleeping state (!NIL_THD_IS_READY()) else contains the wake-up message.*/
  union {
    msg_t               msg;        /**< @brief Wake-up message.            */
    void                *p;         /**< @brief Generic pointer.            */
    thread_reference_t  *trp;       /**< @brief Pointer to thread reference.*/
#if (CH_CFG_USE_SEMAPHORES == TRUE) || defined(__DOXYGEN__)
    semaphore_t         *semp;      /**< @brief Pointer to semaphore.       */
#endif
#if (CH_CFG_USE_EVENTS == TRUE) || defined(__DOXYGEN__)
    eventmask_t         ewmask;     /**< @brief Enabled events mask.        */
#endif
  } u1;
  volatile systime_t    timeout;    /**< @brief Timeout counter, zero
                                            if disabled.                    */
#if (CH_CFG_USE_EVENTS == TRUE) || defined(__DOXYGEN__)
  eventmask_t           epmask;     /**< @brief Pending events mask.        */
#endif
#if (CH_DBG_ENABLE_STACK_CHECK == TRUE) || defined(__DOXYGEN__)
  stkalign_t            *wabase;    /**< @brief Thread stack boundary.      */
#endif
  /* Optional extra fields.*/
  CH_CFG_THREAD_EXT_FIELDS
};

/**
 * @brief   Type of a structure representing the system.
 */
typedef struct nil_system nil_system_t;

/**
 * @brief   System data structure.
 * @note    This structure contain all the data areas used by the OS except
 *          stacks.
 */
struct nil_system {
  /**
   * @brief   Pointer to the running thread.
   */
  thread_t              *current;
  /**
   * @brief   Pointer to the next thread to be executed.
   * @note    This pointer must point at the same thread pointed by @p current
   *          or to an higher priority thread if a switch is required.
   */
  thread_t              *next;
#if (CH_CFG_ST_TIMEDELTA == 0) || defined(__DOXYGEN__)
  /**
   * @brief   System time.
   */
  volatile systime_t    systime;
#endif
#if (CH_CFG_ST_TIMEDELTA > 0) || defined(__DOXYGEN__)
  /**
   * @brief   System time of the last tick event.
   */
  systime_t             lasttime;
  /**
   * @brief   Time of the next scheduled tick event.
   */
  systime_t             nexttime;
#endif
#if (CH_DBG_SYSTEM_STATE_CHECK == TRUE) || defined(__DOXYGEN__)
  /**
   * @brief   ISR nesting level.
   */
  cnt_t                 isr_cnt;
  /**
   * @brief   Lock nesting level.
   */
  cnt_t                 lock_cnt;
#endif
#if (NIL_DBG_ENABLED == TRUE) || defined(__DOXYGEN__)
  /**
   * @brief   Panic message.
   * @note    This field is only present if some debug options have been
   *          activated.
   * @note    Accesses to this pointer must never be optimized out so the
   *          field itself is declared volatile.
   */
  const char            * volatile dbg_panic_msg;
#endif
  /**
   * @brief   Thread structures for all the defined threads.
   */
  thread_t              threads[CH_CFG_NUM_THREADS + 1];
};

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

#if CH_DBG_SYSTEM_STATE_CHECK == TRUE
#define _dbg_enter_lock() (nil.lock_cnt = (cnt_t)1)
#define _dbg_leave_lock() (nil.lock_cnt = (cnt_t)0)
#endif

/**
 * @brief   Utility to make the parameter a quoted string.
 */
#define __CH_STRINGIFY(a) #a

/**
 * @name    Threads tables definition macros
 * @{
 */
/**
 * @brief   Start of user threads table.
 */
#define THD_TABLE_BEGIN                                                     \
  const thread_config_t nil_thd_configs[CH_CFG_NUM_THREADS + 1] = {

/**
 * @brief   Entry of user threads table
 */
#define THD_TABLE_ENTRY(wap, name, funcp, arg)                              \
  {wap, ((stkalign_t *)(wap)) + (sizeof (wap) / sizeof(stkalign_t)),        \
   name, funcp, arg},

/**
 * @brief   End of user threads table.
 */
#define THD_TABLE_END                                                       \
  {THD_IDLE_BASE, THD_IDLE_END, "idle", NULL, NULL}                         \
};
/** @} */

/**
 * @name    Memory alignment support macros
 */
/**
 * @brief   Alignment mask constant.
 *
 * @param[in] a         alignment, must be a power of two
 */
#define MEM_ALIGN_MASK(a)       ((size_t)(a) - 1U)

/**
 * @brief   Aligns to the previous aligned memory address.
 *
 * @param[in] p         variable to be aligned
 * @param[in] a         alignment, must be a power of two
 */
#define MEM_ALIGN_PREV(p, a)    ((size_t)(p) & ~MEM_ALIGN_MASK(a))

/**
 * @brief   Aligns to the new aligned memory address.
 *
 * @param[in] p         variable to be aligned
 * @param[in] a         alignment, must be a power of two
 */
#define MEM_ALIGN_NEXT(p, a)    MEM_ALIGN_PREV((size_t)(p) +                \
                                               MEM_ALIGN_MASK(a), (a))

/**
 * @brief   Returns whatever a pointer or memory size is aligned.
 *
 * @param[in] p         variable to be aligned
 * @param[in] a         alignment, must be a power of two
 */
#define MEM_IS_ALIGNED(p, a)    (((size_t)(p) & MEM_ALIGN_MASK(a)) == 0U)

/**
 * @brief   Returns whatever a constant is a valid alignment.
 * @details Valid alignments are powers of two.
 *
 * @param[in] a         alignment to be checked, must be a constant
 */
#define MEM_IS_VALID_ALIGNMENT(a)                                           \
  (((size_t)(a) != 0U) && (((size_t)(a) & ((size_t)(a) - 1U)) == 0U))
/** @} */

/**
 * @name    Working Areas
 */
/**
 * @brief   Calculates the total Working Area size.
 *
 * @param[in] n         the stack size to be assigned to the thread
 * @return              The total used memory in bytes.
 *
 * @api
 */
#define THD_WORKING_AREA_SIZE(n) MEM_ALIGN_NEXT(PORT_WA_SIZE(n),            \
                                                PORT_STACK_ALIGN)

/**
 * @brief   Static working area allocation.
 * @details This macro is used to allocate a static thread working area
 *          aligned as both position and size.
 *
 * @param[in] s         the name to be assigned to the stack array
 * @param[in] n         the stack size to be assigned to the thread
 *
 * @api
 */
#define THD_WORKING_AREA(s, n) PORT_WORKING_AREA(s, n)
/** @} */

/**
 * @name    Threads abstraction macros
 */
/**
 * @brief   Thread declaration macro.
 * @note    Thread declarations should be performed using this macro because
 *          the port layer could define optimizations for thread functions.
 */
#define THD_FUNCTION(tname, arg) PORT_THD_FUNCTION(tname, arg)
/** @} */

/**
 * @name    ISRs abstraction macros
 */
/**
 * @brief   Priority level validation macro.
 * @details This macro determines if the passed value is a valid priority
 *          level for the underlying architecture.
 *
 * @param[in] prio      the priority level
 * @return              Priority range result.
 * @retval false        if the priority is invalid or if the architecture
 *                      does not support priorities.
 * @retval true         if the priority is valid.
 */
#if defined(PORT_IRQ_IS_VALID_PRIORITY) || defined(__DOXYGEN__)
#define CH_IRQ_IS_VALID_PRIORITY(prio)                                      \
  PORT_IRQ_IS_VALID_PRIORITY(prio)
#else
#define CH_IRQ_IS_VALID_PRIORITY(prio) false
#endif

/**
 * @brief   Priority level validation macro.
 * @details This macro determines if the passed value is a valid priority
 *          level that cannot preempt the kernel critical zone.
 *
 * @param[in] prio      the priority level
 * @return              Priority range result.
 * @retval false        if the priority is invalid or if the architecture
 *                      does not support priorities.
 * @retval true         if the priority is valid.
 */
#if defined(PORT_IRQ_IS_VALID_KERNEL_PRIORITY) || defined(__DOXYGEN__)
#define CH_IRQ_IS_VALID_KERNEL_PRIORITY(prio)                               \
  PORT_IRQ_IS_VALID_KERNEL_PRIORITY(prio)
#else
#define CH_IRQ_IS_VALID_KERNEL_PRIORITY(prio) false
#endif

/**
 * @brief   IRQ handler enter code.
 * @note    Usually IRQ handlers functions are also declared naked.
 * @note    On some architectures this macro can be empty.
 *
 * @special
 */
#define CH_IRQ_PROLOGUE()                                                   \
  PORT_IRQ_PROLOGUE();                                                      \
  _dbg_check_enter_isr()

/**
 * @brief   IRQ handler exit code.
 * @note    Usually IRQ handlers function are also declared naked.
 *
 * @special
 */
#define CH_IRQ_EPILOGUE()                                                   \
  _dbg_check_leave_isr();                                                   \
  PORT_IRQ_EPILOGUE()

/**
 * @brief   Standard normal IRQ handler declaration.
 * @note    @p id can be a function name or a vector number depending on the
 *          port implementation.
 *
 * @special
 */
#define CH_IRQ_HANDLER(id) PORT_IRQ_HANDLER(id)
/** @} */

/**
 * @name    Fast ISRs abstraction macros
 */
/**
 * @brief   Standard fast IRQ handler declaration.
 * @note    @p id can be a function name or a vector number depending on the
 *          port implementation.
 * @note    Not all architectures support fast interrupts.
 *
 * @special
 */
#define CH_FAST_IRQ_HANDLER(id) PORT_FAST_IRQ_HANDLER(id)
/** @} */

/**
 * @name    Time conversion utilities
 * @{
 */
/**
 * @brief   Seconds to system ticks.
 * @details Converts from seconds to system ticks number.
 * @note    The result is rounded upward to the next tick boundary.
 *
 * @param[in] sec       number of seconds
 * @return              The number of ticks.
 *
 * @api
 */
#define S2ST(sec)                                                           \
  ((systime_t)((uint32_t)(sec) * (uint32_t)CH_CFG_ST_FREQUENCY))

/**
 * @brief   Milliseconds to system ticks.
 * @details Converts from milliseconds to system ticks number.
 * @note    The result is rounded upward to the next tick boundary.
 *
 * @param[in] msec      number of milliseconds
 * @return              The number of ticks.
 *
 * @api
 */
#define MS2ST(msec)                                                         \
  ((systime_t)(((((uint32_t)(msec)) *                                       \
                 ((uint32_t)CH_CFG_ST_FREQUENCY)) + 999UL) / 1000UL))

/**
 * @brief   Microseconds to system ticks.
 * @details Converts from microseconds to system ticks number.
 * @note    The result is rounded upward to the next tick boundary.
 *
 * @param[in] usec      number of microseconds
 * @return              The number of ticks.
 *
 * @api
 */
#define US2ST(usec)                                                         \
  ((systime_t)(((((uint32_t)(usec)) *                                       \
                 ((uint32_t)CH_CFG_ST_FREQUENCY)) + 999999UL) / 1000000UL))
/** @} */

/**
 * @name    Time conversion utilities for the realtime counter
 * @{
 */
/**
 * @brief   Seconds to realtime counter.
 * @details Converts from seconds to realtime counter cycles.
 * @note    The macro assumes that @p freq >= @p 1.
 *
 * @param[in] freq      clock frequency, in Hz, of the realtime counter
 * @param[in] sec       number of seconds
 * @return              The number of cycles.
 *
 * @api
 */
#define S2RTC(freq, sec) ((freq) * (sec))

/**
 * @brief   Milliseconds to realtime counter.
 * @details Converts from milliseconds to realtime counter cycles.
 * @note    The result is rounded upward to the next millisecond boundary.
 * @note    The macro assumes that @p freq >= @p 1000.
 *
 * @param[in] freq      clock frequency, in Hz, of the realtime counter
 * @param[in] msec      number of milliseconds
 * @return              The number of cycles.
 *
 * @api
 */
#define MS2RTC(freq, msec) (rtcnt_t)((((freq) + 999UL) / 1000UL) * (msec))

/**
 * @brief   Microseconds to realtime counter.
 * @details Converts from microseconds to realtime counter cycles.
 * @note    The result is rounded upward to the next microsecond boundary.
 * @note    The macro assumes that @p freq >= @p 1000000.
 *
 * @param[in] freq      clock frequency, in Hz, of the realtime counter
 * @param[in] usec      number of microseconds
 * @return              The number of cycles.
 *
 * @api
 */
#define US2RTC(freq, usec) (rtcnt_t)((((freq) + 999999UL) / 1000000UL) * (usec))
/** @} */

/**
 * @name    Semaphores macros
 * @{
 */
/**
 * @brief   Data part of a static semaphore initializer.
 * @details This macro should be used when statically initializing a semaphore
 *          that is part of a bigger structure.
 *
 * @param[in] name      the name of the semaphore variable
 * @param[in] n         the counter initial value, this value must be
 *                      non-negative
 */
#define _SEMAPHORE_DATA(name, n) {n}

/**
 * @brief   Static semaphore initializer.
 * @details Statically initialized semaphores require no explicit
 *          initialization using @p chSemInit().
 *
 * @param[in] name      the name of the semaphore variable
 * @param[in] n         the counter initial value, this value must be
 *                      non-negative
 */
#define SEMAPHORE_DECL(name, n) semaphore_t name = _SEMAPHORE_DATA(name, n)
/** @} */

/**
 * @name    Macro Functions
 * @{
 */
/**
 * @brief   Returns the current value of the system real time counter.
 * @note    This function is only available if the port layer supports the
 *          option @p PORT_SUPPORTS_RT.
 *
 * @return              The value of the system realtime counter of
 *                      type rtcnt_t.
 *
 * @xclass
 */
#if (PORT_SUPPORTS_RT == TRUE) || defined(__DOXYGEN__)
#define chSysGetRealtimeCounterX() (rtcnt_t)port_rt_get_counter_value()
#endif

/**
 * @brief   Raises the system interrupt priority mask to the maximum level.
 * @details All the maskable interrupt sources are disabled regardless their
 *          hardware priority.
 * @note    Do not invoke this API from within a kernel lock.
 *
 * @special
 */
#define chSysDisable() {                                                    \
  port_disable();                                                           \
  _dbg_check_disable();                                                     \
}

/**
 * @brief   Raises the system interrupt priority mask to system level.
 * @details The interrupt sources that should not be able to preempt the kernel
 *          are disabled, interrupt sources with higher priority are still
 *          enabled.
 * @note    Do not invoke this API from within a kernel lock.
 * @note    This API is no replacement for @p chSysLock(), the @p chSysLock()
 *          could do more than just disable the interrupts.
 *
 * @special
 */
#define chSysSuspend() {                                                    \
  port_suspend();                                                           \
  _dbg_check_suspend();                                                     \
}

/**
 * @brief   Lowers the system interrupt priority mask to user level.
 * @details All the interrupt sources are enabled.
 * @note    Do not invoke this API from within a kernel lock.
 * @note    This API is no replacement for @p chSysUnlock(), the
 *          @p chSysUnlock() could do more than just enable the interrupts.
 *
 * @special
 */
#define chSysEnable() {                                                     \
  _dbg_check_enable();                                                      \
  port_enable();                                                            \
}

/**
 * @brief   Enters the kernel lock state.
 *
 * @special
 */
#define chSysLock() {                                                       \
  port_lock();                                                              \
  _dbg_check_lock();                                                        \
}

/**
 * @brief   Leaves the kernel lock state.
 *
 * @special
 */
#define chSysUnlock() {                                                     \
  _dbg_check_unlock();                                                      \
  port_unlock();                                                            \
}

/**
 * @brief   Enters the kernel lock state from within an interrupt handler.
 * @note    This API may do nothing on some architectures, it is required
 *          because on ports that support preemptable interrupt handlers
 *          it is required to raise the interrupt mask to the same level of
 *          the system mutual exclusion zone.<br>
 *          It is good practice to invoke this API before invoking any I-class
 *          syscall from an interrupt handler.
 * @note    This API must be invoked exclusively from interrupt handlers.
 *
 * @special
 */
#define chSysLockFromISR() {                                                \
  port_lock_from_isr();                                                     \
  _dbg_check_lock_from_isr();                                               \
}

/**
 * @brief   Leaves the kernel lock state from within an interrupt handler.
 *
 * @note    This API may do nothing on some architectures, it is required
 *          because on ports that support preemptable interrupt handlers
 *          it is required to raise the interrupt mask to the same level of
 *          the system mutual exclusion zone.<br>
 *          It is good practice to invoke this API after invoking any I-class
 *          syscall from an interrupt handler.
 * @note    This API must be invoked exclusively from interrupt handlers.
 *
 * @special
 */
#define chSysUnlockFromISR() {                                              \
  _dbg_check_unlock_from_isr();                                             \
  port_unlock_from_isr();                                                   \
}

/**
 * @brief   Evaluates if a reschedule is required.
 *
 * @retval true         if there is a thread that must go in running state
 *                      immediately.
 * @retval false        if preemption is not required.
 *
 * @iclass
 */
#define chSchIsRescRequiredI() ((bool)(nil.current != nil.next))

/**
 * @brief   Returns a pointer to the current @p thread_t.
 *
 * @xclass
 */
#define chThdGetSelfX() nil.current

/**
 * @brief   Delays the invoking thread for the specified number of seconds.
 * @note    The specified time is rounded up to a value allowed by the real
 *          system clock.
 * @note    The maximum specified value is implementation dependent.
 *
 * @param[in] sec       time in seconds, must be different from zero
 *
 * @api
 */
#define chThdSleepSeconds(sec) chThdSleep(S2ST(sec))

/**
 * @brief   Delays the invoking thread for the specified number of
 *          milliseconds.
 * @note    The specified time is rounded up to a value allowed by the real
 *          system clock.
 * @note    The maximum specified value is implementation dependent.
 *
 * @param[in] msec      time in milliseconds, must be different from zero
 *
 * @api
 */
#define chThdSleepMilliseconds(msec) chThdSleep(MS2ST(msec))

/**
 * @brief   Delays the invoking thread for the specified number of
 *          microseconds.
 * @note    The specified time is rounded up to a value allowed by the real
 *          system clock.
 * @note    The maximum specified value is implementation dependent.
 *
 * @param[in] usec      time in microseconds, must be different from zero
 *
 * @api
 */
#define chThdSleepMicroseconds(usec) chThdSleep(US2ST(usec))

/**
 * @brief   Suspends the invoking thread for the specified time.
 *
 * @param[in] timeout   the delay in system ticks
 *
 * @sclass
 */
#define chThdSleepS(timeout) (void) chSchGoSleepTimeoutS(NIL_STATE_SLEEPING, timeout)

/**
 * @brief   Suspends the invoking thread until the system time arrives to the
 *          specified value.
 *
 * @param[in] abstime   absolute system time
 *
 * @sclass
 */
#define chThdSleepUntilS(abstime)                                           \
    (void) chSchGoSleepTimeoutS(NIL_STATE_SLEEPING, (abstime) -             \
                                chVTGetSystemTimeX())

#if (CH_CFG_USE_SEMAPHORES == TRUE) || defined(__DOXYGEN__)
/**
 * @brief   Initializes a semaphore with the specified counter value.
 *
 * @param[out] sp       pointer to a @p semaphore_t structure
 * @param[in] n         initial value of the semaphore counter. Must be
 *                      non-negative.
 *
 * @init
 */
#define chSemObjectInit(sp, n) ((sp)->cnt = n)

/**
 * @brief   Performs a wait operation on a semaphore.
 *
 * @param[in] sp        pointer to a @p semaphore_t structure
 * @return              A message specifying how the invoking thread has been
 *                      released from the semaphore.
 * @retval CH_MSG_OK   if the thread has not stopped on the semaphore or the
 *                      semaphore has been signaled.
 * @retval CH_MSG_RST  if the semaphore has been reset using @p chSemReset().
 *
 * @api
 */
#define chSemWait(sp) chSemWaitTimeout(sp, TIME_INFINITE)

/**
 * @brief   Performs a wait operation on a semaphore.
 *
 * @param[in] sp        pointer to a @p semaphore_t structure
 * @return              A message specifying how the invoking thread has been
 *                      released from the semaphore.
 * @retval CH_MSG_OK   if the thread has not stopped on the semaphore or the
 *                      semaphore has been signaled.
 * @retval CH_MSG_RST  if the semaphore has been reset using @p chSemReset().
 *
 * @sclass
 */
#define chSemWaitS(sp) chSemWaitTimeoutS(sp, TIME_INFINITE)

/**
 * @brief   Decreases the semaphore counter.
 * @details This macro can be used when the counter is known to be positive.
 *
 * @param[in] sp        pointer to a @p semaphore_t structure
 *
 * @iclass
 */
#define chSemFastWaitI(sp) ((sp)->cnt--)

/**
 * @brief   Increases the semaphore counter.
 * @details This macro can be used when the counter is known to be not
 *          negative.
 *
 * @param[in] sp        pointer to a @p semaphore_t structure
 *
 * @iclass
 */
#define chSemFastSignalI(sp) ((sp)->cnt++)

/**
 * @brief   Returns the semaphore counter current value.
 *
 * @iclass
 */
#define chSemGetCounterI(sp) ((sp)->cnt)
#endif /* CH_CFG_USE_SEMAPHORES == TRUE */

/**
 * @brief   Current system time.
 * @details Returns the number of system ticks since the @p chSysInit()
 *          invocation.
 * @note    The counter can reach its maximum and then restart from zero.
 * @note    This function can be called from any context but its atomicity
 *          is not guaranteed on architectures whose word size is less than
 *          @p systime_t size.
 *
 * @return              The system time in ticks.
 *
 * @xclass
 */
#if (CH_CFG_ST_TIMEDELTA == 0) || defined(__DOXYGEN__)
#define chVTGetSystemTimeX() (nil.systime)
#else
#define chVTGetSystemTimeX() port_timer_get_time()
#endif

/**
 * @brief   Returns the elapsed time since the specified start time.
 *
 * @param[in] start     start time
 * @return              The elapsed time.
 *
 * @xclass
 */
#define chVTTimeElapsedSinceX(start)                                        \
  ((systime_t)(chVTGetSystemTimeX() - (start)))

/**
 * @brief   Checks if the specified time is within the specified time window.
 * @note    When start==end then the function returns always true because the
 *          whole time range is specified.
 * @note    This function can be called from any context.
 *
 * @param[in] time      the time to be verified
 * @param[in] start     the start of the time window (inclusive)
 * @param[in] end       the end of the time window (non inclusive)
 * @retval true         current time within the specified time window.
 * @retval false        current time not within the specified time window.
 *
 * @xclass
 */
#define chVTIsTimeWithinX(time, start, end)                                 \
  ((bool)((systime_t)((time) - (start)) < (systime_t)((end) - (start))))

/**
 * @brief   Function parameters check.
 * @details If the condition check fails then the kernel panics and halts.
 * @note    The condition is tested only if the @p CH_DBG_ENABLE_CHECKS switch
 *          is specified in @p chconf.h else the macro does nothing.
 *
 * @param[in] c         the condition to be verified to be true
 *
 * @api
 */
#if !defined(chDbgCheck)
#define chDbgCheck(c) do {                                                  \
  /*lint -save -e506 -e774 [2.1, 14.3] Can be a constant by design.*/       \
  if (CH_DBG_ENABLE_CHECKS != FALSE) {                                      \
    if (!(c)) {                                                             \
  /*lint -restore*/                                                         \
      chSysHalt(__func__);                                                  \
    }                                                                       \
  }                                                                         \
} while (false)
#endif /* !defined(chDbgCheck) */

/**
 * @brief   Condition assertion.
 * @details If the condition check fails then the kernel panics with a
 *          message and halts.
 * @note    The condition is tested only if the @p CH_DBG_ENABLE_ASSERTS
 *          switch is specified in @p chconf.h else the macro does nothing.
 * @note    The remark string is not currently used except for putting a
 *          comment in the code about the assertion.
 *
 * @param[in] c         the condition to be verified to be true
 * @param[in] r         a remark string
 *
 * @api
 */
#if !defined(chDbgAssert)
#define chDbgAssert(c, r) do {                                              \
  /*lint -save -e506 -e774 [2.1, 14.3] Can be a constant by design.*/       \
  if (CH_DBG_ENABLE_ASSERTS != FALSE) {                                     \
    if (!(c)) {                                                             \
  /*lint -restore*/                                                         \
      chSysHalt(__func__);                                                  \
    }                                                                       \
  }                                                                         \
} while (false)
#endif /* !defined(chDbgAssert) */
/** @} */

/* Empty macros if the state checker is not enabled.*/
#if CH_DBG_SYSTEM_STATE_CHECK == FALSE
#define _dbg_enter_lock()
#define _dbg_leave_lock()
#define _dbg_check_disable()
#define _dbg_check_suspend()
#define _dbg_check_enable()
#define _dbg_check_lock()
#define _dbg_check_unlock()
#define _dbg_check_lock_from_isr()
#define _dbg_check_unlock_from_isr()
#define _dbg_check_enter_isr()
#define _dbg_check_leave_isr()
#define chDbgCheckClassI()
#define chDbgCheckClassS()
#endif

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if !defined(__DOXYGEN__)
#if (CH_DBG_ENABLE_STACK_CHECK == TRUE) || defined(__DOXYGEN__)
extern stkalign_t __main_thread_stack_base__, __main_thread_stack_end__;
#endif
extern nil_system_t nil;
extern const thread_config_t nil_thd_configs[CH_CFG_NUM_THREADS + 1];
#endif

#ifdef __cplusplus
extern "C" {
#endif
  void chSysInit(void);
  void chSysHalt(const char *reason);
  void chSysTimerHandlerI(void);
  void chSysUnconditionalLock(void);
  void chSysUnconditionalUnlock(void);
  syssts_t chSysGetStatusAndLockX(void);
  bool chSysIsCounterWithinX(rtcnt_t cnt, rtcnt_t start, rtcnt_t end);
  void chSysPolledDelayX(rtcnt_t cycles);
  void chSysRestoreStatusX(syssts_t sts);
  thread_t *chSchReadyI(thread_t *tp, msg_t msg);
  bool chSchIsPreemptionRequired(void);
  void chSchDoReschedule(void);
  void chSchRescheduleS(void);
  msg_t chSchGoSleepTimeoutS(tstate_t newstate, systime_t timeout);
  msg_t chThdSuspendTimeoutS(thread_reference_t *trp, systime_t timeout);
  void chThdResumeI(thread_reference_t *trp, msg_t msg);
  void chThdSleep(systime_t timeout);
  void chThdSleepUntil(systime_t abstime);
#if CH_CFG_USE_SEMAPHORES == TRUE
  msg_t chSemWaitTimeout(semaphore_t *sp, systime_t timeout);
  msg_t chSemWaitTimeoutS(semaphore_t *sp, systime_t timeout);
  void chSemSignal(semaphore_t *sp);
  void chSemSignalI(semaphore_t *sp);
  void chSemReset(semaphore_t *sp, cnt_t n);
  void chSemResetI(semaphore_t *sp, cnt_t n);
#endif /* CH_CFG_USE_SEMAPHORES == TRUE */
#if CH_CFG_USE_EVENTS == TRUE
  void chEvtSignal(thread_t *tp, eventmask_t mask);
  void chEvtSignalI(thread_t *tp, eventmask_t mask);
  eventmask_t chEvtWaitAnyTimeout(eventmask_t mask, systime_t timeout);
#endif
#if CH_DBG_SYSTEM_STATE_CHECK == TRUE
  void _dbg_check_disable(void);
  void _dbg_check_suspend(void);
  void _dbg_check_enable(void);
  void _dbg_check_lock(void);
  void _dbg_check_unlock(void);
  void _dbg_check_lock_from_isr(void);
  void _dbg_check_unlock_from_isr(void);
  void _dbg_check_enter_isr(void);
  void _dbg_check_leave_isr(void);
  void chDbgCheckClassI(void);
  void chDbgCheckClassS(void);
#endif
#ifdef __cplusplus
}
#endif

/* Optional subsystems.*/
#include "chmboxes.h"
#include "chmemcore.h"
#include "chmempools.h"
#include "chheap.h"

#endif /* CH_H */

/** @} */
