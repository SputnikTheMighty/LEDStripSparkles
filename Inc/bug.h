#ifndef __BUG_H
#define __BUG_H

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "neopixel.h"
#include "stm32G0xx_hal.h"

typedef struct HSL_T{
	uint8_t hue;
	uint8_t sat;
	uint8_t lit;
}HSL_T;

typedef struct HSL_STEP_T{
	int8_t hue;
	int8_t sat;
	int8_t lit;
}HSL_STEP_T;

typedef struct RGB_T{
	uint8_t red;
	uint8_t grn;
	uint8_t blu;
}RGB_T;

typedef struct BUG_T{
	uint8_t length;
	uint8_t position;
	RGB_T colour;
	uint8_t direction; // 1 = forwards; 0 = backwards
	uint8_t framesPerMovement;
	uint8_t frameCount;
	uint16_t lifetime;
	uint16_t lifetimecount;
	struct BUG_T *next;
}BUG_T;

uint32_t hsl_to_rgb(uint8_t h, uint8_t s, uint8_t l);
void send_uart(char *string);
void send_uart_num(uint16_t num);
void UpdateBug(BUG_T *bug);
void UpdateAllBugs(BUG_T **localHeadBug);
void AddBug(BUG_T **localHeadBug, uint8_t length, uint8_t position, RGB_T colour, uint8_t direction, uint8_t framesPerMovement, uint8_t lifetime);
void AddBugs(BUG_T **localHeadBug, uint8_t totalNewBugs);

extern UART_HandleTypeDef huart2;

#endif
