#include "driver.h"
#include "driver_sfr.h"

void drv_init(_IO driver_sfr_t **gpio_inst, uintptr_t base_addr)
{
	*gpio_inst = (_IO driver_sfr_t *)(base_addr);
	(*gpio_inst)->ier.bits.chnl_1 = 0;
	(*gpio_inst)->ier.bits.chnl_2 = 1;
	(*gpio_inst)->gier.bits.gie = 1;
}

void drv_exit(_IO driver_sfr_t *gpio_inst)
{
	gpio_inst->chnl1_data.word = 0;
}

void drv_readb(_IO driver_sfr_t *gpio_inst, pins_t *data)
{
	while(1)
	{
		if(gpio_inst->isr.bits.chnl_2 == 1)
		{
			gpio_inst->isr.bits.chnl_2 = 1; // Acknowlegment
            break;
		}
	}
	*data = gpio_inst->cnhl2_data;
}

void drv_write(_IO driver_sfr_t *gpio_inst, pins_t data)
{
	gpio_inst->chnl1_data.word = data.word;
}
