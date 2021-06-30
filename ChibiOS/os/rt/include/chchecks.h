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
 * @file    chchecks.h
 * @brief   Configuration checks macros and structures.
 *
 * @addtogroup checks
 * @{
 */

#ifndef CHCHECKS_H
#define CHCHECKS_H

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if CH_CUSTOMER_LICENSED_RT == FALSE
#error "ChibiOS/RT not licensed"
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
#undef CH_DBG_STATISTICS
#undef CH_DBG_TRACE_MASK

#define CH_DBG_STATISTICS                   FALSE
#define CH_DBG_TRACE_MASK                   CH_DBG_TRACE_MASK_DISABLED

#endif /* (CH_LICENSE_FEATURES == CH_FEATURES_INTERMEDIATE) ||
          (CH_LICENSE_FEATURES == CH_FEATURES_BASIC) */

/* Restrictions in basic mode.*/
#if CH_LICENSE_FEATURES == CH_FEATURES_BASIC

/* Tick-Less mode restricted.*/
#undef CH_CFG_ST_TIMEDELTA
#define CH_CFG_ST_TIMEDELTA                 0

/* Restricted subsystems.*/
#undef CH_CFG_USE_TM
#undef CH_CFG_USE_MUTEXES
#undef CH_CFG_USE_CONDVARS
#undef CH_CFG_USE_DYNAMIC

#define CH_CFG_USE_TM                       FALSE
#define CH_CFG_USE_MUTEXES                  FALSE
#define CH_CFG_USE_CONDVARS                 FALSE
#define CH_CFG_USE_DYNAMIC                  FALSE

#endif /* CH_LICENSE_FEATURES == CH_FEATURES_BASIC */

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

/*===========================================================================*/
/* Module inline functions.                                                  */
/*===========================================================================*/

#endif /* CHCHECKS_H */

/** @} */
