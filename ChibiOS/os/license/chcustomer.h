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
 * @file    chcustomer.h
 * @brief   Customer-related info.
 *
 * @addtogroup customer
 * @{
 */

#ifndef CHCUSTOMER_H
#define CHCUSTOMER_H

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

/**
 * @brief   Customer readable identifier.
 */
#define CH_CUSTOMER_ID_STRING               "Santa, North Pole"

/**
 * @brief   Customer code.
 */
#define CH_CUSTOMER_ID_CODE                 "xxxx-yyyy"

/**
 * @brief   Current license.
 * @note    This setting is reserved to the copyright owner.
 * @note    Changing this setting invalidates the license.
 * @note    The license statement in the source headers is valid, applicable
 *          and binding regardless this setting.
 */
#define CH_LICENSE                          CH_LICENSE_GPL

/**
 * @name    Licensed Products
 * @{
 */
#define CH_CUSTOMER_LICENSED_RT             TRUE
#define CH_CUSTOMER_LICENSED_NIL            TRUE
#define CH_CUSTOMER_LICENSED_EX             TRUE
#define CH_CUSTOMER_LICENSED_PORT_CM0       TRUE
#define CH_CUSTOMER_LICENSED_PORT_CM3       TRUE
#define CH_CUSTOMER_LICENSED_PORT_CM4       TRUE
#define CH_CUSTOMER_LICENSED_PORT_CM7       TRUE
#define CH_CUSTOMER_LICENSED_PORT_ARM79     TRUE
#define CH_CUSTOMER_LICENSED_PORT_E200Z0    TRUE
#define CH_CUSTOMER_LICENSED_PORT_E200Z2    TRUE
#define CH_CUSTOMER_LICENSED_PORT_E200Z3    TRUE
#define CH_CUSTOMER_LICENSED_PORT_E200Z4    TRUE
/** @} */

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

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

#endif /* CHCUSTOMER_H */

/** @} */
