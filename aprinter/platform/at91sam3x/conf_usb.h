#ifndef CONF_USB_H
#define CONF_USB_H

#include <stdint.h>
#include <sam/utils/compiler.h>

#define USB_DEVICE_VENDOR_ID 0x2341
#define USB_DEVICE_PRODUCT_ID 0x003e
#define USB_DEVICE_MAJOR_VERSION 1
#define USB_DEVICE_MINOR_VERSION 0
#define USB_DEVICE_POWER 100
#define USB_DEVICE_ATTR USB_CONFIG_ATTR_BUS_POWERED

#define UDI_CDC_ENABLE_EXT(port) true
#define UDI_CDC_DISABLE_EXT(port)
#define UDI_CDC_SET_DTR_EXT(port, set)
#define UDI_CDC_SET_RTS_EXT(port, set)
#define UDI_CDC_SET_CODING_EXT(port, cfg)
#define UDI_CDC_RX_NOTIFY(port)

#define UDI_CDC_LOW_RATE
#define UDI_CDC_DEFAULT_RATE 1000000
#define UDI_CDC_DEFAULT_STOPBITS CDC_STOP_BITS_1
#define UDI_CDC_DEFAULT_PARITY CDC_PAR_NONE
#define UDI_CDC_DEFAULT_DATABITS 8

#include "udi_cdc_conf.h"

#endif
