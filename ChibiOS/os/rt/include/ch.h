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
 * @brief   ChibiOS/RT main include file.
 * @details This header includes all the required kernel headers so it is the
 *          only kernel header you usually want to include in your application.
 *
 * @addtogroup kernel_info
 * @details Kernel related info.
 * @{
 */

#ifndef CH_H
#define CH_H

/**
 * @brief   ChibiOS/RT identification macro.
 */
#define _CHIBIOS_RT_

/**
 * @brief   Stable release flag.
 */
#define CH_KERNEL_STABLE        0

/**
 * @name    ChibiOS/RT version identification
 * @{
 */
/**
 * @brief   Kernel version string.
 */
#define CH_KERNEL_VERSION       "4.0.0"

/**
 * @brief   Kernel version major number.
 */
#define CH_KERNEL_MAJOR         4

/**
 * @brief   Kernel version minor number.
 */
#define CH_KERNEL_MINOR         0

/**
 * @brief   Kernel version patch number.
 */
#define CH_KERNEL_PATCH         0
/** @} */

/* Core headers.*/
#include "chtypes.h"
#include "chconf.h"

#if !defined(_CHIBIOS_RT_CONF_)
#error "invalid configuration file"
#endif

#include "chlicense.h"
#include "chchecks.h"
#include "chsystypes.h"
#include "chalign.h"
#include "chcore.h"
#include "chdebug.h"
#include "chtrace.h"
#include "chtm.h"
#include "chstats.h"
#include "chschd.h"
#include "chsys.h"
#include "chvt.h"
#include "chthreads.h"

/* Optional subsystems headers.*/
#include "chregistry.h"
#include "chsem.h"
#include "chbsem.h"
#include "chmtx.h"
#include "chcond.h"
#include "chevents.h"
#include "chmsg.h"
#include "chmboxes.h"
#include "chmemcore.h"
#include "chheap.h"
#include "chmempools.h"
#include "chdynamic.h"

#if !defined(_CHIBIOS_RT_CONF_)
#error "missing or wrong configuration file"
#endif

#endif /* CH_H */

/** @} */
