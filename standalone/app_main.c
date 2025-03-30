#include "driver_sfr.h"
#include "xparameters.h"
#include "driver.h"
#include "stdio.h"
#include "xil_printf.h"

_IO driver_sfr_t *gpio0_inst;

int main()
{
	xil_printf("[INFO] Start Application \n\r");
	drv_init(&gpio0_inst, XPAR_AXI_GPIO_0_BASEADDR);
	xil_printf("[INFO] gpio0_inst %p \n\r", gpio0_inst);
	
	pins_t leds;
	leds.bits.pin_0 = 1;
	leds.bits.pin_1 = 1;
	leds.bits.pin_2 = 0;
	leds.bits.pin_3 = 1;

	drv_write(gpio0_inst,  leds);
    xil_printf("[INFO] Press a push button \n\r");
    drv_readb(gpio0_inst, &leds);
    xil_printf("[EVENT] Pressed buttons are 0x%1x \n\r", leds.word);
    drv_write(gpio0_inst,  leds);

	xil_printf("[INFO] End Application \n\r");


return 0;
}
