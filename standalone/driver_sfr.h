#ifndef DRIVER_SFR_H
#define DRIVER_SFR_H
#include "stdint.h"

typedef union {
	struct {
		uint32_t rsvd : 31;
		uint32_t gie : 1;
	} bits;
	uint32_t word;
} gier_t;

typedef union {
	struct {
		uint32_t chnl_1 : 1;
		uint32_t chnl_2 : 2;
		uint32_t rsvd : 30;
	} bits;
	uint32_t word;
} ier_t;

typedef union {
	struct {
		uint32_t chnl_1 : 1;
		uint32_t chnl_2 : 2;
		uint32_t rsvd : 30;
	} bits;
	uint32_t word;
} isr_t;

typedef union {
	struct {
		uint32_t pin_0 : 1;
		uint32_t pin_1 : 1;
		uint32_t pin_2 : 1;
		uint32_t pin_3 : 1;
		uint32_t rsvd : 28;
	} bits;
	uint32_t word;
} pins_t;

typedef struct {
	pins_t chnl1_data;
	pins_t chnl1_tri;
	pins_t cnhl2_data;
	pins_t chnl2_tri;
	uint32_t rsvd[67];
	gier_t 	gier;
	isr_t	isr;
	uint32_t rsvd_0;
	ier_t	ier;
} driver_sfr_t;


#endif
