#ifndef DRIVER_H
#define DRIVER_H

#include "driver_sfr.h"
#define _IO volatile

void drv_init( _IO driver_sfr_t **gpio_inst, uintptr_t base_addr);
void drv_exit( _IO driver_sfr_t  *gpio_inst);

void drv_ieren(_IO driver_sfr_t  *gpio_inst, uint8_t channel);
void drv_inout(_IO driver_sfr_t  *gpio_inst, uint8_t channel, uint8_t dir);
void drv_readb(_IO driver_sfr_t  *gpio_inst, uint8_t channel, pins_t *data);
void drv_readn(_IO driver_sfr_t  *gpio_inst, uint8_t channel, pins_t *data);
void drv_write(_IO driver_sfr_t  *gpio_inst, uint8_t channel, pins_t  data);


#endif
