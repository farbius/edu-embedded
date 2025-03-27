#include "xparameters.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "xil_printf.h"
#include "sleep.h"
#include "stdio.h"

#define USE_INTERRUPT_MODE 0  // 1 = interrupt mode, 0 = polling mode
#define XPAR_FABRIC_AXI_GPIO_0_IP2INTC_IRPT_INTR (XPAR_FABRIC_AXI_GPIO_0_INTR + 32U)


volatile uint32_t *global_timer = (volatile uint32_t *)(XPAR_GLOBAL_TIMER_BASEADDR + 0x00);
volatile uint32_t start_time = 0;
volatile uint32_t end_time = 0;
volatile int irq_handled = 0;

typedef union {
    struct {
        uint32_t reserved : 31;
        uint32_t gie : 1;
    } bits;
uint32_t word;
} gier_t;

typedef union {
    struct {
        uint32_t chnl_1 : 1;
        uint32_t chnl_2 : 1;
        uint32_t reserved : 30;
    } bits;
uint32_t word;
} ier_t;

typedef union {
    struct {
        uint32_t chnl_1 : 1;
        uint32_t chnl_2 : 1;
        uint32_t reserved : 30;
    } bits;
uint32_t word;
} isr_t;

typedef union {
    struct {
        uint32_t pin_0 : 1;
        uint32_t pin_1 : 1;
        uint32_t pin_2 : 1;
        uint32_t pin_3 : 1;
        uint32_t reserved : 28;
    } bits;
uint32_t word;
} pins_t;


typedef struct {
pins_t    gpio_data;    // 4 bits Buttons
pins_t    gpio_tri;
pins_t    gpio2_data;   // 4 bits LED
pins_t    gpio2_tri;
uint32_t  mem[67];
gier_t    gier;
isr_t     isr;  
uint32_t  reserved;
ier_t     ier;
    
} gpio_regs_t;

volatile gpio_regs_t *gpio_inst = (volatile gpio_regs_t *)(XPAR_AXI_GPIO_0_BASEADDR);

XScuGic Intc;


void GpioHandler()
{
    
    static int idx = 0;
	xil_printf("[EVENT] push button interrupt %d \n\r", idx);
    idx = idx + 1;
    start_time = *global_timer;
    gpio_inst->isr.bits.chnl_1 = 1;
    end_time = *global_timer;
    float latency_us = (float)(end_time - start_time) / 650.0;
    printf("[EVENT] Latency = %u cycles = %.2f us \n\r", end_time - start_time, latency_us);
     irq_handled = 1;
}

int SetupIntrSystem();

int main()
{ 
    xil_printf("[INFO] Start Application \n\r");

    gpio_inst->gpio2_data.bits.pin_0 = 1;
    gpio_inst->gpio2_data.bits.pin_1 = 0;
    gpio_inst->gpio2_data.bits.pin_2 = 0;
    gpio_inst->gpio2_data.bits.pin_3 = 1;

#if USE_INTERRUPT_MODE == 0
    xil_printf("[INFO] Polled Mode \n\r");

    gpio_inst->ier.bits.chnl_1 = 1;
    gpio_inst->ier.bits.chnl_2 = 0;
    gpio_inst->gier.bits.gie = 1;

    while(1)
    {
        if (gpio_inst->isr.bits.chnl_1 == 1) {
            gpio_inst->isr.bits.chnl_1 = 1; // Acknowledge
            xil_printf("[POLL] Button press detected!\n\r");
            break;
        }        
    }
#else
    xil_printf("[INFO] Interrupt Mode \n\r");

    int Status = SetupIntrSystem();
    if(Status != 0) {
        xil_printf("[ERROR] Interrupt initialization error \n\r");
        return -1;
    }

    gpio_inst->ier.bits.chnl_1 = 1;
    gpio_inst->ier.bits.chnl_2 = 0;
    gpio_inst->gier.bits.gie = 1;

    while (!irq_handled) {
        usleep(1);
    }
#endif

    gpio_inst->gpio2_data.word = 0;

    xil_printf("[INFO] End Application \n\r");
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

    XScuGic_SetPriorityTriggerType(&Intc, XPAR_FABRIC_AXI_GPIO_0_IP2INTC_IRPT_INTR,0xA0, 0x3);
	/*
	 * Connect the interrupt handler that will be called when an
	 * interrupt occurs for the device.
	 */
	Status = XScuGic_Connect(&Intc, XPAR_FABRIC_AXI_GPIO_0_IP2INTC_IRPT_INTR, (Xil_ExceptionHandler)GpioHandler, NULL);
	if (Status != XST_SUCCESS) {
		return Status;
	}



	/* Enable the interrupt for the GPIO device.*/
	XScuGic_Enable(&Intc, XPAR_FABRIC_AXI_GPIO_0_IP2INTC_IRPT_INTR);

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