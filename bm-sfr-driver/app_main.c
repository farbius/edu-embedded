#include "driver_sfr.h"
#include "xparameters.h"
#include "driver.h"
#include "stdio.h"
#include "xil_printf.h"
#include "sleep.h"

#include "xscugic.h"
#include "xil_exception.h"

#define XPAR_AXI_GPIO_BASEADDR XPAR_AXI_GPIO_0_BASEADDR
#define XPAR_FABRIC_AXI_GPIO_IP2INTC_IRPT_INTR (XPAR_FABRIC_AXI_GPIO_0_INTR + 32U)
#define INTERATIONS 1000

volatile uint32_t *global_timer = (volatile uint32_t *)(XPAR_GLOBAL_TIMER_BASEADDR + 0x00);
volatile uint32_t start_time = 0;
volatile uint32_t end_time = 0;
volatile int irq_handled = 0;

XScuGic Intc;
_IO driver_sfr_t *gpio_inst;

void enable_global_timer()
{
    volatile uint32_t *cntcr = (volatile uint32_t *)(XPAR_GLOBAL_TIMER_BASEADDR + 0x08);
    *cntcr |= 0x1;  // bit 0: ENABLE
}


void GpioHandler()
{
    gpio_inst->isr.bits.chnl_1 = 1;
    irq_handled = 1;
}

int SetupIntrSystem();

int main()
{
	xil_printf("[INFO] Start Application \n\r");
    enable_global_timer();
    drv_init(&gpio_inst, XPAR_AXI_GPIO_BASEADDR);
    xil_printf("[INFO] gpio0_inst %p \n\r", gpio_inst);
    drv_inout(gpio_inst, 0, 0); // channel 0 as output
    drv_inout(gpio_inst, 1, 1); // channel 1 as input
    drv_ieren(gpio_inst, 1); 
    
    uint32_t total = 0;
    pins_t leds;
    leds.word = 0;

    xil_printf("[INFO] Polled Mode \n\r");
    total = 0;
    for(int idx = 0; idx < INTERATIONS; idx = idx + 1)
    {
        leds.word ++;
        drv_write(gpio_inst,  0, leds);
        start_time = *global_timer;
        drv_readb(gpio_inst, 1, &leds);
        end_time = *global_timer;
        total += (end_time - start_time);
        usleep(1);
    }
    printf("[INFO] Polled Mode Time = %.2f ns \n\r", (float)(total*1000)/INTERATIONS/650);

    xil_printf("[INFO] Interrupt Mode \n\r");
    total = 0;
    int Status = SetupIntrSystem();
    if(Status != 0) {
        xil_printf("[ERROR] Interrupt initialization error \n\r");
        return -1;
    }
    for(int idx = 0; idx < INTERATIONS; idx = idx + 1)
    {
        leds.word ++;
        drv_write(gpio_inst,  0, leds);
        start_time = *global_timer;
        while (!irq_handled) {   }
        irq_handled = 0;
        drv_readn(gpio_inst, 1, &leds);
        end_time = *global_timer;
        total += (end_time - start_time);
        usleep(1);
    }
    printf("[INFO] Interrupt Mode Time = %.2f ns \n\r", (float)(total*1000)/INTERATIONS/650);

    drv_exit(gpio_inst);
	xil_printf("[INFO] End Application \n\n\r");


return 0;
}

int SetupIntrSystem()
{
	int Status = 0;
    XScuGic_Config *IntcConfig;

	IntcConfig = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(&Intc, IntcConfig, IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}    

    XScuGic_SetPriorityTriggerType(&Intc, XPAR_FABRIC_AXI_GPIO_IP2INTC_IRPT_INTR,0xA0, 0x3);
	/*
	 * Connect the interrupt handler that will be called when an
	 * interrupt occurs for the device.
	 */
	Status = XScuGic_Connect(&Intc, XPAR_FABRIC_AXI_GPIO_IP2INTC_IRPT_INTR, (Xil_ExceptionHandler)GpioHandler, NULL);
	if (Status != XST_SUCCESS) {
		return Status;
	}



	/* Enable the interrupt for the GPIO device.*/
	XScuGic_Enable(&Intc, XPAR_FABRIC_AXI_GPIO_IP2INTC_IRPT_INTR);

	/*
	 * Initialize the exception table and register the interrupt
	 * controller handler with the exception table
	 */
	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			 (Xil_ExceptionHandler)XScuGic_InterruptHandler, &Intc);

	/* Enable non-critical exceptions */
	Xil_ExceptionEnable();

	return 0;
}
