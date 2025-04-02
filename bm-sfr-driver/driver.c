#include "driver.h"
#include "driver_sfr.h"

void drv_init(_IO driver_sfr_t **gpio_inst, uintptr_t base_addr)
{
	*gpio_inst = (_IO driver_sfr_t *)(base_addr);
	(*gpio_inst)->ier.bits.chnl_1 = 0;
	(*gpio_inst)->ier.bits.chnl_2 = 0;
	(*gpio_inst)->gier.bits.gie = 0;

    (*gpio_inst)->chnl1_tri.word = 0xFFFFFFFF;
    (*gpio_inst)->chnl2_tri.word = 0xFFFFFFFF;
}

void drv_inout(_IO driver_sfr_t  *gpio_inst, uint8_t channel, uint8_t dir)
{
    uint32_t tri_val = (dir) ? 0xFFFFFFFF : 0x00000000; // 1=input (tristate), 0=output

    if (channel == 0)
        gpio_inst->chnl1_tri.word = tri_val;
    else if (channel == 1)
        gpio_inst->chnl2_tri.word = tri_val;
}

void drv_exit(_IO driver_sfr_t *gpio_inst)
{
    gpio_inst->ier.bits.chnl_1 = 0;
	gpio_inst->ier.bits.chnl_2 = 0;
	gpio_inst->gier.bits.gie = 0;
    gpio_inst->chnl1_tri.word = 0xFFFFFFFF;
    gpio_inst->chnl2_tri.word = 0xFFFFFFFF;
}

void drv_ieren(_IO driver_sfr_t  *gpio_inst, uint8_t channel)
{
    if (channel == 0)
    {
        gpio_inst->isr.bits.chnl_1 = 1;
        gpio_inst->ier.bits.chnl_1 = 1;
    }
        
    else if (channel == 1)
    {
        gpio_inst->isr.bits.chnl_2 = 1;
        gpio_inst->ier.bits.chnl_2 = 1;
    }
        

    gpio_inst->gier.bits.gie = 1;
}

void drv_readb(_IO driver_sfr_t *gpio_inst, uint8_t channel, pins_t *data)
{

if(channel == 0)
{
    while(1)
	{
		if(gpio_inst->isr.bits.chnl_1 == 1)
		{
			gpio_inst->isr.bits.chnl_1 = 1; // Acknowlegment
            break;
		}
	}
	*data = gpio_inst->chnl1_data;
}
else if (channel == 1) {
{
    while(1)
	{
		if(gpio_inst->isr.bits.chnl_2 == 1)
		{
			gpio_inst->isr.bits.chnl_2 = 1; // Acknowlegment
            break;
		}
	}
	*data = gpio_inst->chnl2_data;
}
}
	
}

void drv_readn(_IO driver_sfr_t *gpio_inst, uint8_t channel, pins_t *data)
{

if(channel == 0)
	*data = gpio_inst->chnl1_data;
else if (channel == 1) 
	*data = gpio_inst->chnl2_data;
	
}

void drv_write(_IO driver_sfr_t *gpio_inst, uint8_t channel, pins_t data)
{

    if (channel == 0)
        gpio_inst->chnl1_data.word = data.word;
    else if (channel == 1)
        gpio_inst->chnl2_data.word = data.word;
	
}
