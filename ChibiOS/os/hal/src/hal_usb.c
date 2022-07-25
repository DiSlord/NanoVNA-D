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
 * @file    hal_usb.c
 * @brief   USB Driver code.
 *
 * @addtogroup USB
 * @{
 */

#include <string.h>

#include "hal.h"

#if (HAL_USE_USB == TRUE) || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

static const uint8_t zero_status[] = {0x00, 0x00};
static const uint8_t active_status[] ={0x00, 0x00};
static const uint8_t halted_status[] = {0x01, 0x00};

/*  Error response, the state machine goes into an error state, the low
 *  level layer will have to reset it to USB_EP0_WAITING_SETUP after
 *  receiving a SETUP packet.
 */
static void _usb_ep0_error(USBDriver *usbp) {
  usb_lld_stall_in(usbp, 0);
  usb_lld_stall_out(usbp, 0);
  _usb_isr_invoke_event_cb(usbp, USB_EVENT_STALLED);
  usbp->ep0state = USB_EP0_ERROR;
}

#if USB_USE_WAIT == TRUE
static void _usb_reset_all_threads(USBDriver *usbp) {
  unsigned i;
  osalSysLockFromISR();
  for (i = 0; i <= (unsigned)USB_MAX_ENDPOINTS; i++) {
    if (usbp->epc[i] != NULL) {
      if (usbp->epc[i]->in_state != NULL) {
        osalThreadResumeI(&usbp->epc[i]->in_state->thread, MSG_RESET);
      }
      if (usbp->epc[i]->out_state != NULL) {
        osalThreadResumeI(&usbp->epc[i]->out_state->thread, MSG_RESET);
      }
    }
  }
  osalSysUnlockFromISR();
}
#endif

/**
 * @brief  SET ADDRESS transaction callback.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 */
static void set_address(USBDriver *usbp) {
  usbp->address = usbp->setup.wValue;
  usb_lld_set_address(usbp);
  _usb_isr_invoke_event_cb(usbp, USB_EVENT_ADDRESS);
  usbp->state = USB_SELECTED;
}

/**
 * @brief   Standard requests handlers.
 * @details This is the standard requests default handler, most standard
 *          requests are handled here, the user can override the standard
 *          handling using the @p requests_hook_cb hook in the
 *          @p USBConfig structure.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @return              The request handling exit code.
 * @retval false        Request not recognized by the handler or error.
 * @retval true         Request handled.
 */

static bool device_handler(USBDriver *usbp, uint32_t request) {
  switch (request) {
    case USB_REQ_GET_STATUS: /* Just returns the current status word.*/
      usbSetupTransfer(usbp, (uint8_t *)&usbp->status, 2, NULL);
      return true;
    case USB_REQ_CLEAR_FEATURE:
    case USB_REQ_SET_FEATURE:
      /* Only the DEVICE_REMOTE_WAKEUP is handled here, any other feature number is handled as an error.*/
      if (usbp->setup.wValue == USB_FEATURE_DEVICE_REMOTE_WAKEUP) {
        usbp->status&=~2U;
        if (request == USB_REQ_SET_FEATURE)
          usbp->status|= 2U;
        usbSetupTransfer(usbp, NULL, 0, NULL);
        return true;
      }
      return false;
    case USB_REQ_SET_ADDRESS:
      /* The SET_ADDRESS handling can be performed here or postponed after
         the status packed depending on the USB_SET_ADDRESS_MODE low driver setting.*/
#if USB_SET_ADDRESS_MODE == USB_EARLY_SET_ADDRESS
      set_address(usbp);
      usbSetupTransfer(usbp, NULL, 0, NULL);
#else
      usbSetupTransfer(usbp, NULL, 0, set_address);
#endif
      return true;
    case USB_REQ_GET_DESCRIPTOR:
      {
        /* Handling descriptor requests from the host.*/
        const USBDescriptor *dp = usbp->config->get_descriptor_cb(usbp,
                                        (usbp->setup.wValue>>8)&0xFF,
                                        (usbp->setup.wValue>>0)&0xFF,
                                         usbp->setup.wIndex);
        if (dp == NULL) return false;
        usbSetupTransfer(usbp, (uint8_t *)dp->ud_string, dp->ud_size, NULL);
        return true;
      }
//    case USB_REQ_SET_DESCRIPTOR:
//    break;
    case USB_REQ_GET_CONFIGURATION:
      /* Returning the last selected configuration.*/
      usbSetupTransfer(usbp, &usbp->configuration, 1, NULL);
      return true;
    case USB_REQ_SET_CONFIGURATION:
      /* Handling configuration selection from the host.*/
      usbp->configuration = usbp->setup.wValue;
      usbp->state = usbp->configuration != 0 ? USB_ACTIVE : USB_SELECTED;
      _usb_isr_invoke_event_cb(usbp, USB_EVENT_CONFIGURED);
      usbSetupTransfer(usbp, NULL, 0, NULL);
      return true;
  }
  return false;
}

static bool interface_handler(USBDriver *usbp, uint32_t request) {
  switch (request) {
  case USB_REQ_GET_STATUS:
    /* Just sending two zero bytes, the application can change the behavior using a hook..*/
    usbSetupTransfer(usbp, (uint8_t *)zero_status, 2, NULL);
    return true;
//  case USB_REQ_CLEAR_FEATURE: break;
//  case USB_REQ_SET_FEATURE: break;
//  case USB_REQ_GET_INTERFACE: break;
//  case USB_REQ_SET_INTERFACE: break;
  }
  return false;
}

static bool endpoint_handler(USBDriver *usbp, uint32_t request) {
  uint16_t ep  = usbp->setup.wIndex & 0x0F;
  bool in_mode = usbp->setup.wIndex & 0x80;
  switch (request) {
  case USB_REQ_SYNCH_FRAME:
    /* Just sending two zero bytes, the application can change the behavior using a hook..*/
    usbSetupTransfer(usbp, (uint8_t *)zero_status, 2, NULL);
    return true;
  case USB_REQ_GET_STATUS:
    /* Sending the EP status.*/
    switch (in_mode ? usb_lld_get_status_in(usbp, ep) : usb_lld_get_status_out(usbp, ep))
    {
      case EP_STATUS_STALLED:usbSetupTransfer(usbp, (uint8_t *)halted_status, 2, NULL);
        return true;
      case EP_STATUS_ACTIVE: usbSetupTransfer(usbp, (uint8_t *)active_status, 2, NULL);
        return true;
      case EP_STATUS_DISABLED:
      default: break;
    }
    return false;
  case USB_REQ_CLEAR_FEATURE:
  case USB_REQ_SET_FEATURE:
    /* Only ENDPOINT_HALT is handled as feature.*/
    if (usbp->setup.wValue != USB_FEATURE_ENDPOINT_HALT)
      return false;
    /* Clearing the EP status, not valid for EP0, it is ignored in that case.*/
    if (ep == 0)
      return false;
    if (request == USB_REQ_CLEAR_FEATURE) {if (in_mode) usb_lld_clear_in(usbp, ep); else usb_lld_clear_out(usbp, ep);}
    else                                  {if (in_mode) usb_lld_stall_in(usbp, ep); else usb_lld_stall_out(usbp, ep);}

    usbSetupTransfer(usbp, NULL, 0, NULL);
    return true;
  }
  return false;
}

static bool default_handler(USBDriver *usbp) {
  uint32_t rtype = usbp->setup.bmRequestType;
  /* Handle only STD type setup */
  if ((rtype & USB_RTYPE_TYPE_MASK) == USB_RTYPE_TYPE_STD) {
    /* Get state and decoding the request.*/
    uint16_t request = usbp->setup.bRequest;
    switch (rtype & USB_RTYPE_RECIPIENT_MASK) {
      case USB_RTYPE_RECIPIENT_DEVICE:    return device_handler(usbp, request);
      case USB_RTYPE_RECIPIENT_INTERFACE: return interface_handler(usbp, request);
      case USB_RTYPE_RECIPIENT_ENDPOINT:  return endpoint_handler(usbp, request);
    }
  }
 /* All the above requests are not handled here, if you need them then
       use the hook mechanism and provide handling.*/
    return false;
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   USB Driver initialization.
 * @note    This function is implicitly invoked by @p halInit(), there is
 *          no need to explicitly initialize the driver.
 *
 * @init
 */
void usbInit(void) {

  usb_lld_init();
}

/**
 * @brief   Initializes the standard part of a @p USBDriver structure.
 *
 * @param[out] usbp     pointer to the @p USBDriver object
 *
 * @init
 */
void usbObjectInit(USBDriver *usbp) {
  unsigned i;

  usbp->state        = USB_STOP;
  usbp->config       = NULL;
  for (i = 0; i < (unsigned)USB_MAX_ENDPOINTS; i++) {
    usbp->in_params[i]  = NULL;
    usbp->out_params[i] = NULL;
  }
  usbp->transmitting = 0;
  usbp->receiving    = 0;
}

/**
 * @brief   Configures and activates the USB peripheral.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] config    pointer to the @p USBConfig object
 *
 * @api
 */
void usbStart(USBDriver *usbp, const USBConfig *config) {
  unsigned i;

  osalDbgCheck((usbp != NULL) && (config != NULL));

  osalSysLock();
  osalDbgAssert((usbp->state == USB_STOP) || (usbp->state == USB_READY),
                "invalid state");
  usbp->config = config;
  for (i = 0; i <= (unsigned)USB_MAX_ENDPOINTS; i++) {
    usbp->epc[i] = NULL;
  }
  usb_lld_start(usbp);
  usbp->state = USB_READY;
  osalSysUnlock();
}

/**
 * @brief   Deactivates the USB peripheral.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 *
 * @api
 */
void usbStop(USBDriver *usbp) {
  unsigned i;

  osalDbgCheck(usbp != NULL);

  osalSysLock();
  osalDbgAssert((usbp->state == USB_STOP) || (usbp->state == USB_READY) ||
                (usbp->state == USB_SELECTED) || (usbp->state == USB_ACTIVE) ||
                (usbp->state == USB_SUSPENDED),
                "invalid state");
  usb_lld_stop(usbp);
  usbp->state = USB_STOP;

  /* Resetting all ongoing synchronous operations.*/
  for (i = 0; i <= (unsigned)USB_MAX_ENDPOINTS; i++) {
#if USB_USE_WAIT == TRUE
    if (usbp->epc[i] != NULL) {
      if (usbp->epc[i]->in_state != NULL) {
        osalThreadResumeI(&usbp->epc[i]->in_state->thread, MSG_RESET);
      }
      if (usbp->epc[i]->out_state != NULL) {
        osalThreadResumeI(&usbp->epc[i]->out_state->thread, MSG_RESET);
      }
    }
#endif
    usbp->epc[i] = NULL;
  }
  osalOsRescheduleS();
  osalSysUnlock();
}

/**
 * @brief   Enables an endpoint.
 * @details This function enables an endpoint, both IN and/or OUT directions
 *          depending on the configuration structure.
 * @note    This function must be invoked in response of a SET_CONFIGURATION
 *          or SET_INTERFACE message.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 * @param[in] epcp      the endpoint configuration
 *
 * @iclass
 */
void usbInitEndpointI(USBDriver *usbp, usbep_t ep,
                      const USBEndpointConfig *epcp) {

  osalDbgCheckClassI();
  osalDbgCheck((usbp != NULL) && (epcp != NULL));
  osalDbgAssert(usbp->state == USB_ACTIVE,
                "invalid state");
  osalDbgAssert(usbp->epc[ep] == NULL, "already initialized");

  /* Logically enabling the endpoint in the USBDriver structure.*/
  usbp->epc[ep] = epcp;

  /* Clearing the state structures, custom fields as well.*/
  if (epcp->in_state != NULL) {
    memset(epcp->in_state, 0, sizeof(USBInEndpointState));
  }
  if (epcp->out_state != NULL) {
    memset(epcp->out_state, 0, sizeof(USBOutEndpointState));
  }

  /* Low level endpoint activation.*/
  usb_lld_init_endpoint(usbp, ep);
}

/**
 * @brief   Disables all the active endpoints.
 * @details This function disables all the active endpoints except the
 *          endpoint zero.
 * @note    This function must be invoked in response of a SET_CONFIGURATION
 *          message with configuration number zero.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 *
 * @iclass
 */
void usbDisableEndpointsI(USBDriver *usbp) {
  unsigned i;

  osalDbgCheckClassI();
  osalDbgCheck(usbp != NULL);
  osalDbgAssert(usbp->state == USB_SELECTED, "invalid state");

  usbp->transmitting &= ~1U;
  usbp->receiving    &= ~1U;
#if USB_USE_WAIT == TRUE
  _usb_reset_all_threads(usbp);
#endif
  for (i = 1; i <= (unsigned)USB_MAX_ENDPOINTS; i++) {
    usbp->epc[i] = NULL;
  }

  /* Low level endpoints deactivation.*/
  usb_lld_disable_endpoints(usbp);
}

/**
 * @brief   Starts a receive transaction on an OUT endpoint.
 * @note    This function is meant to be called from ISR context outside
 *          critical zones because there is a potentially slow operation
 *          inside.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 * @param[out] buf      buffer where to copy the received data
 * @param[in] n         transaction size. It is recommended a multiple of
 *                      the packet size because the excess is discarded.
 *
 * @iclass
 */
void usbStartReceiveI(USBDriver *usbp, usbep_t ep,
                      uint8_t *buf, size_t n) {
  USBOutEndpointState *osp;

  osalDbgCheckClassI();
  osalDbgCheck((usbp != NULL) && (ep <= (usbep_t)USB_MAX_ENDPOINTS));
  //osalDbgAssert(!usbGetReceiveStatusI(usbp, ep), "already receiving");

  /* Marking the endpoint as active.*/
  usbp->receiving |= (uint16_t)((unsigned)1U << (unsigned)ep);

  /* Setting up the transfer.*/
  /*lint -save -e661 [18.1] pclint is confused by the check on ep.*/
  osp = usbp->epc[ep]->out_state;
  /*lint -restore*/
  osp->rxbuf  = buf;
  osp->rxsize = n;
  osp->rxcnt  = 0;
#if USB_USE_WAIT == TRUE
  osp->thread = NULL;
#endif

  /* Starting transfer.*/
  usb_lld_start_out(usbp, ep);
}

/**
 * @brief   Starts a transmit transaction on an IN endpoint.
 * @note    This function is meant to be called from ISR context outside
 *          critical zones because there is a potentially slow operation
 *          inside.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 * @param[in] buf       buffer where to fetch the data to be transmitted
 * @param[in] n         transaction size
 *
 * @iclass
 */
void usbStartTransmitI(USBDriver *usbp, usbep_t ep,
                       const uint8_t *buf, size_t n) {
  USBInEndpointState *isp;

  osalDbgCheckClassI();
  osalDbgCheck((usbp != NULL) && (ep <= (usbep_t)USB_MAX_ENDPOINTS));
  //osalDbgAssert(!usbGetTransmitStatusI(usbp, ep), "already transmitting");

  /* Marking the endpoint as active.*/
  usbp->transmitting |= (uint16_t)((unsigned)1U << (unsigned)ep);

  /* Setting up the transfer.*/
  /*lint -save -e661 [18.1] pclint is confused by the check on ep.*/
  isp = usbp->epc[ep]->in_state;
  /*lint -restore*/
  isp->txbuf  = buf;
  isp->txsize = n;
  isp->txcnt  = 0;
#if USB_USE_WAIT == TRUE
  isp->thread = NULL;
#endif

  /* Starting transfer.*/
  usb_lld_start_in(usbp, ep);
}

#if (USB_USE_WAIT == TRUE) || defined(__DOXYGEN__)
/**
 * @brief   Performs a receive transaction on an OUT endpoint.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 * @param[out] buf      buffer where to copy the received data
 * @param[in] n         transaction size. It is recommended a multiple of
 *                      the packet size because the excess is discarded.
 *
 * @return              The received effective data size, it can be less than
 *                      the amount specified.
 * @retval MSG_RESET    driver not in @p USB_ACTIVE state or the operation
 *                      has been aborted by an USB reset or a transition to
 *                      the @p USB_SUSPENDED state.
 *
 * @api
 */
msg_t usbReceive(USBDriver *usbp, usbep_t ep, uint8_t *buf, size_t n) {
  msg_t msg;

  osalSysLock();

  if (usbGetDriverStateI(usbp) != USB_ACTIVE) {
    osalSysUnlock();
    return MSG_RESET;
  }

  usbStartReceiveI(usbp, ep, buf, n);
  msg = osalThreadSuspendS(&usbp->epc[ep]->out_state->thread);
  osalSysUnlock();

  return msg;
}

/**
 * @brief   Performs a transmit transaction on an IN endpoint.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 * @param[in] buf       buffer where to fetch the data to be transmitted
 * @param[in] n         transaction size
 *
 * @return              The operation status.
 * @retval MSG_OK       operation performed successfully.
 * @retval MSG_RESET    driver not in @p USB_ACTIVE state or the operation
 *                      has been aborted by an USB reset or a transition to
 *                      the @p USB_SUSPENDED state.
 *
 * @api
 */
msg_t usbTransmit(USBDriver *usbp, usbep_t ep, const uint8_t *buf, size_t n) {
  msg_t msg;

  osalSysLock();

  if (usbGetDriverStateI(usbp) != USB_ACTIVE) {
    osalSysUnlock();
    return MSG_RESET;
  }

  usbStartTransmitI(usbp, ep, buf, n);
  msg = osalThreadSuspendS(&usbp->epc[ep]->in_state->thread);
  osalSysUnlock();

  return msg;
}
#endif /* USB_USE_WAIT == TRUE */

/**
 * @brief   Stalls an OUT endpoint.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 *
 * @return              The operation status.
 * @retval false        Endpoint stalled.
 * @retval true         Endpoint busy, not stalled.
 *
 * @iclass
 */
bool usbStallReceiveI(USBDriver *usbp, usbep_t ep) {
  osalDbgCheckClassI();
  osalDbgCheck(usbp != NULL);

  if (usbGetReceiveStatusI(usbp, ep)) {
    return true;
  }

  usb_lld_stall_out(usbp, ep);
  return false;
}

/**
 * @brief   Stalls an IN endpoint.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 *
 * @return              The operation status.
 * @retval false        Endpoint stalled.
 * @retval true         Endpoint busy, not stalled.
 *
 * @iclass
 */
bool usbStallTransmitI(USBDriver *usbp, usbep_t ep) {

  osalDbgCheckClassI();
  osalDbgCheck(usbp != NULL);

  if (usbGetTransmitStatusI(usbp, ep)) {
    return true;
  }

  usb_lld_stall_in(usbp, ep);
  return false;
}

/**
 * @brief   USB reset routine.
 * @details This function must be invoked when an USB bus reset condition is
 *          detected.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 *
 * @notapi
 */
void _usb_reset(USBDriver *usbp) {
  unsigned i;
  /* State transition.*/
  usbp->state         = USB_READY;
  /* Resetting internal state.*/
  usbp->status        = 0;
  usbp->address       = 0;
  usbp->configuration = 0;
  usbp->transmitting  = 0;
  usbp->receiving     = 0;
  /* Invalidates all endpoints into the USBDriver structure.*/
#if USB_USE_WAIT == TRUE
  _usb_reset_all_threads(usbp);
#endif
  for (i = 0; i <= (unsigned)USB_MAX_ENDPOINTS; i++) {
    usbp->epc[i] = NULL;
  }
  /* EP0 state machine initialization.*/
  usbp->ep0state = USB_EP0_WAITING_SETUP;
  /* Low level reset.*/
  usb_lld_reset(usbp);
  /* Notification of reset event.*/
  _usb_isr_invoke_event_cb(usbp, USB_EVENT_RESET);
}

/**
 * @brief   USB suspend routine.
 * @details This function must be invoked when an USB bus suspend condition is
 *          detected.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 *
 * @notapi
 */
void _usb_suspend(USBDriver *usbp) {
  /* State transition.*/
  usbp->state = USB_SUSPENDED;
  /* Notification of suspend event.*/
  _usb_isr_invoke_event_cb(usbp, USB_EVENT_SUSPEND);
  /* Signaling the event to threads waiting on endpoints.*/
#if USB_USE_WAIT == TRUE
  _usb_reset_all_threads(usbp);
#endif
}

/**
 * @brief   USB wake-up routine.
 * @details This function must be invoked when an USB bus wake-up condition is
 *          detected.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 *
 * @notapi
 */
void _usb_wakeup(USBDriver *usbp) {
  /* State transition.*/
  usbp->state = USB_ACTIVE;
  /* Notification of suspend event.*/
  _usb_isr_invoke_event_cb(usbp, USB_EVENT_WAKEUP);
}

/**
 * @brief   Default EP0 SETUP callback.
 * @details This function is used by the low level driver as default handler
 *          for EP0 SETUP events.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number, always zero
 *
 * @notapi
 */
void _usb_ep0setup(USBDriver *usbp, usbep_t ep) {
  usbp->ep0state = USB_EP0_WAITING_SETUP;
  /* Read received setup packet from out endpoint */
  usbReadSetup(usbp, ep, (uint8_t *)&usbp->setup);
  /* Reset callback for usbSetupTransfer macro */
  usbp->ep0endcb = NULL;
  /* First verify if the application has an handler installed for this request.*/
  if ((usbp->config->requests_hook_cb == NULL) || !(usbp->config->requests_hook_cb(usbp))) {
    /* Invoking the default handler, if this fails then stalls the endpoint zero as error.*/
    if (!default_handler(usbp)) {
      _usb_ep0_error(usbp);
      return;
    }
  }
#if (USB_SET_ADDRESS_ACK_HANDLING == USB_SET_ADDRESS_ACK_HW)
  if (usbp->setup.bRequest == USB_REQ_SET_ADDRESS) {
    /* Zero-length packet sent by hardware */
    return;
  }
#endif
  /* Transfer preparation. The request handler must have populated
     correctly the fields ep0next, ep0n and ep0endcb using the macro
     usbSetupTransfer().*/
  /* The transfer size cannot exceed the specified amount.*/
  if (usbp->ep0n > usbp->setup.wLength)
    usbp->ep0n = usbp->setup.wLength;

  /* Process prepared ep0 ask / answer, disable ISR event for this time*/
  osalSysLockFromISR();
  if ((usbp->setup.bmRequestType & USB_RTYPE_DIR_MASK) == USB_RTYPE_DIR_DEV2HOST) {
    /* IN phase.*/
    if (usbp->ep0n != 0U) {
      /* Starts the transmit phase.*/
      usbStartTransmitI(usbp, 0, usbp->ep0next, usbp->ep0n);
      usbp->ep0state = USB_EP0_TX;
    }
    else {
      /* No transmission phase, directly receiving the zero sized status packet.*/
#if (USB_EP0_STATUS_STAGE == USB_EP0_STATUS_STAGE_SW)
      usbStartReceiveI(usbp, 0, NULL, 0);
#else
      usb_lld_end_setup(usbp, ep);
#endif
      usbp->ep0state = USB_EP0_WAITING_STS;
    }
  } else { /* OUT phase.*/
    if (usbp->ep0n != 0U) {
      /* Starts the receive phase.*/
      usbStartReceiveI(usbp, 0, usbp->ep0next, usbp->ep0n);
      usbp->ep0state = USB_EP0_RX;
    } else {
      /* No receive phase, directly sending the zero sized status packet.*/
#if (USB_EP0_STATUS_STAGE == USB_EP0_STATUS_STAGE_SW)
      usbStartTransmitI(usbp, 0, NULL, 0);
#else
      usb_lld_end_setup(usbp, ep);
#endif
      usbp->ep0state = USB_EP0_SENDING_STS;
    }
  }
  osalSysUnlockFromISR();
}

/**
 * @brief   Default EP0 IN callback.
 * @details This function is used by the low level driver as default handler
 *          for EP0 IN events.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number, always zero
 *
 * @notapi
 */
void _usb_ep0in(USBDriver *usbp, usbep_t ep) {
  (void)ep;
  switch (usbp->ep0state) {
  case USB_EP0_TX:
    /* If the transmitted size is less than the requested size and it is a
       multiple of the maximum packet size then a zero size packet must be
       transmitted.*/
    if ((usbp->ep0n < usbp->setup.wLength) &&
        ((usbp->ep0n % usbp->epc[0]->in_maxsize) == 0U)) {
      osalSysLockFromISR();
      usbStartTransmitI(usbp, 0, NULL, 0);
      osalSysUnlockFromISR();
      usbp->ep0state = USB_EP0_WAITING_TX0;
      return;
    }
    __attribute__ ((fallthrough));
  case USB_EP0_WAITING_TX0:
    /* Transmit phase over, receiving the zero sized status packet.*/
#if (USB_EP0_STATUS_STAGE == USB_EP0_STATUS_STAGE_SW)
    osalSysLockFromISR();
    usbStartReceiveI(usbp, 0, NULL, 0);
    osalSysUnlockFromISR();
#else
    /* Use hardware end phase send */
    usb_lld_end_setup(usbp, ep);
#endif
    usbp->ep0state = USB_EP0_WAITING_STS;
    return;
  case USB_EP0_SENDING_STS:
    /* Status packet sent, invoking the callback if defined.*/
    if (usbp->ep0endcb != NULL)
      usbp->ep0endcb(usbp);
    usbp->ep0state = USB_EP0_WAITING_SETUP;
    return;
  case USB_EP0_WAITING_SETUP:
  case USB_EP0_WAITING_STS:
  case USB_EP0_RX:
    /* All the above are invalid states in the IN phase.*/
    osalDbgAssert(false, "EP0 state machine error");
    __attribute__ ((fallthrough));
  case USB_EP0_ERROR:
    _usb_ep0_error(usbp);
    return;
  default:
    osalDbgAssert(false, "EP0 state machine invalid state");
  }
}

/**
 * @brief   Default EP0 OUT callback.
 * @details This function is used by the low level driver as default handler
 *          for EP0 OUT events.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number, always zero
 *
 * @notapi
 */
void _usb_ep0out(USBDriver *usbp, usbep_t ep) {
  (void)ep;
  switch (usbp->ep0state) {
  case USB_EP0_RX:
    /* Receive phase over, sending the zero sized status packet.*/
#if (USB_EP0_STATUS_STAGE == USB_EP0_STATUS_STAGE_SW)
    osalSysLockFromISR();
    usbStartTransmitI(usbp, 0, NULL, 0);
    osalSysUnlockFromISR();
#else
    /* Use hardware end phase send */
    usb_lld_end_setup(usbp, ep);
#endif
    usbp->ep0state = USB_EP0_SENDING_STS;
    return;
  case USB_EP0_WAITING_STS:
    /* Status packet received, it must be zero sized, invoking the callback if defined.*/
#if (USB_EP0_STATUS_STAGE == USB_EP0_STATUS_STAGE_SW)
    if (usbGetReceiveTransactionSizeX(usbp, 0) != 0U) {
      break;
    }
#endif
    if (usbp->ep0endcb != NULL)
      usbp->ep0endcb(usbp);
    usbp->ep0state = USB_EP0_WAITING_SETUP;
    return;
  case USB_EP0_WAITING_SETUP:
  case USB_EP0_TX:
  case USB_EP0_WAITING_TX0:
  case USB_EP0_SENDING_STS:
    /* All the above are invalid states in the IN phase.*/
    osalDbgAssert(false, "EP0 state machine error");
    __attribute__ ((fallthrough));
  case USB_EP0_ERROR:
    _usb_ep0_error(usbp);
    return;
  default:
    osalDbgAssert(false, "EP0 state machine invalid state");
  }
}

#endif /* HAL_USE_USB == TRUE */

/** @} */
