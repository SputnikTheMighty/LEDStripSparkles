/*
 * bug.c
 *
 *  Created on: 7 Nov 2020
 *      Author: rri
 */

#include "bug.h"

HSL_T hslColourPalette[5] = {GREENYELLOWCRAYOLA, GRANNYSMITHAPPLE, ORCHIDCRAYOLA, CERISE, PRINCETONORANGE};

void UpdateBug(BUG_T *bug){

	if(bug->length <= 4)
		bug->colour.lit = bug->colour.lit*0.99;

	bug->frameCount++;
	if(bug->frameCount > bug->framesPerMovement){
		bug->frameCount = 0;
		if(bug->direction == 1){
			bug->position++;
			if(bug->position > NUM_PIXELS-1){
				bug->position = 0;
			}
		}
		else{
			bug->position--;
			if(bug->position == 0xFF){
				bug->position = NUM_PIXELS;
			}
		}
		bug->lifetimecount++;
		if(bug->lifetimecount > bug->lifetime){
			bug->lifetimecount = bug->lifetime-1;

			//bug->colour.lit = bug->colour.lit*0.6;

			if(bug->length != 0 ){
				bug->length--;
			}
		}
	}

	uint8_t temp_hsl_lit = bug->colour.lit;
	for(uint8_t i=0; i<bug->length; i++){
		uint8_t temp = bug->position;
		if(bug->direction == 0){
			temp += i;
			if(temp > NUM_PIXELS-1){
				temp = temp-NUM_PIXELS;
			}
		}
		else{
			temp -=i;
			if(temp > bug->position){
				temp = NUM_PIXELS-temp;
			}
		}
		SetSingleLED_RGB(temp, ConvertHSL_T(bug->colour.hue, bug->colour.sat, temp_hsl_lit));
		temp_hsl_lit *= 0.5;
	}
}

void AddBug(BUG_T **localHeadBug, uint8_t length, uint8_t position, HSL_T colour, uint8_t direction,
		uint8_t framesPerMovement, uint8_t lifetime){

	BUG_T* newBug = (BUG_T*)malloc(sizeof(BUG_T));
	if(newBug != NULL){
		newBug->length = length;
		newBug->position = position;
		newBug->colour = colour;
		newBug->direction = direction;
		newBug->framesPerMovement = framesPerMovement;
		newBug->frameCount = 0;
		newBug->lifetime = lifetime;
		newBug->lifetimecount = 0;
		newBug->next = *localHeadBug;

		*localHeadBug = newBug;
	}
}

void AddBugs(BUG_T **localHeadBug, uint8_t totalNewBugs){

	if(totalNewBugs == 0)
		totalNewBugs = 5+rand()%5;

	uint8_t startposition = rand()%(NUM_PIXELS-1);

	for(uint8_t i=0; i< totalNewBugs; i++){

		uint8_t direction = 0;
		if(i%2 == 0)
			direction = 1;
		// length, position, colour, direction, speed0(high is low), lifetime
		AddBug(localHeadBug, 2+rand()%5, startposition, hslColourPalette[rand()%5], direction, (1+rand()%10), (2+rand()%(totalNewBugs)));
	}
	send_uart("Bugs added: ");
	send_uart_num(totalNewBugs);
}

void UpdateAllBugs(BUG_T **localHeadBug){
	BUG_T *currentBug = NULL;
	BUG_T* previousBug = NULL;
	currentBug = *localHeadBug;

	while(currentBug != NULL){
		UpdateBug(currentBug);

		// if the bug is dead, delete bug and move to the next bug
		if(currentBug->length == 0){

			if(currentBug == *localHeadBug){		// first bug
				//send_uart("Deleting first bug...\r\n");
				if((*localHeadBug)->next != NULL){
					*localHeadBug = (*localHeadBug)->next;
					free(currentBug);
					currentBug = *localHeadBug;
				}
				else{	// only bug
					//send_uart("Deleting only bug...\r\n");
					*localHeadBug = NULL;
					free(currentBug);
					currentBug = NULL;
				}
				//send_uart("Deleted first/only bug.\r\n");
			}
			else{
				if(currentBug->next != NULL){ // middle bugs
					previousBug->next = currentBug->next;
					free(currentBug);
					currentBug = previousBug->next;
				}
				else{
					previousBug->next = NULL; // last bug
					free(currentBug);
					currentBug = NULL;
				}
			}
		}

		// if the bug is not dead, move to the next bug.
		else{
			if(currentBug->next != NULL){	// check for last bug in list
				previousBug = currentBug;
				currentBug = currentBug->next;
			}
			else{
				currentBug = NULL;
			}
		}
	}
}

void send_uart (char *string){
	uint8_t len = strlen (string);
	HAL_UART_Transmit(&huart2, (uint8_t *) string, len, HAL_MAX_DELAY);  // transmit in blocking mode
}

void send_uart_num (uint16_t num){
	char string[5] = {"\0"};
	sprintf(string, "%d\r\n", num);
	uint8_t len = strlen (string);
	HAL_UART_Transmit(&huart2, (uint8_t *) string, len, HAL_MAX_DELAY);  // transmit in blocking mode
}

// Fast hsl2rgb algorithm: https://stackoverflow.com/questions/13105185/fast-algorithm-for-rgb-hsl-conversion
uint32_t hsl_to_rgb(uint8_t h, uint8_t s, uint8_t l) {
	if(l == 0) return 0;

	volatile uint8_t  r, g, b, lo, c, x, m;
	volatile uint16_t h1, l1, H;
	l1 = l + 1;
	if (l < 128)    c = ((l1 << 1) * s) >> 8;
	else            c = (512 - (l1 << 1)) * s >> 8;

	H = h * 6;              // 0 to 1535 (actually 1530)
	lo = H & 255;           // Low byte  = primary/secondary color mix
	h1 = lo + 1;

	if ((H & 256) == 0)   x = h1 * c >> 8;          // even sextant, like red to yellow
	else                  x = (256 - h1) * c >> 8;  // odd sextant, like yellow to green

	m = l - (c >> 1);
	switch(H >> 8) {       // High byte = sextant of colorwheel
	 case 0 : r = c; g = x; b = 0; break; // R to Y
	 case 1 : r = x; g = c; b = 0; break; // Y to G
	 case 2 : r = 0; g = c; b = x; break; // G to C
	 case 3 : r = 0; g = x; b = c; break; // C to B
	 case 4 : r = x; g = 0; b = c; break; // B to M
	 default: r = c; g = 0; b = x; break; // M to R
	}

	return (((uint32_t)r + m) << 16) | (((uint32_t)g + m) << 8) | ((uint32_t)b + m);
}

RGB_T ConvertHSL_T(uint8_t h, uint8_t s, uint8_t l) {

	RGB_T result = {0,0,0};
	uint8_t  r, g, b, lo, c, x, m;
	uint16_t h1, l1, H;

	if(l == 0)
		return result;
	if(s == 0){
		result.red = l;
		result.grn = l;
		result.blu = l;
		return result;
	}

	l1 = l + 1;
	if (l < 128)    c = ((l1 << 1) * s) >> 8;
	else            c = (512 - (l1 << 1)) * s >> 8;

	H = h * 6;              // 0 to 1535 (actually 1530)
	lo = H & 255;           // Low byte  = primary/secondary color mix
	h1 = lo + 1;

	if ((H & 256) == 0)   x = h1 * c >> 8;          // even sextant, like red to yellow
	else                  x = (256 - h1) * c >> 8;  // odd sextant, like yellow to green

	m = l - (c >> 1);
	switch(H >> 8) {       // High byte = sextant of colorwheel
	 case 0 : r = c; g = x; b = 0; break; // R to Y
	 case 1 : r = x; g = c; b = 0; break; // Y to G
	 case 2 : r = 0; g = c; b = x; break; // G to C
	 case 3 : r = 0; g = x; b = c; break; // C to B
	 case 4 : r = x; g = 0; b = c; break; // B to M
	 default: r = c; g = 0; b = x; break; // M to R
	}


	result.red = r + m;
	result.grn = g + m;
	result.blu = b + m;

	return result;
}

void SetSingleLED_RGB(uint8_t LED, RGB_T colour){
	led_set_RGB(LED, colour.red, colour.grn, colour.blu);
}

void SetSingleLED_HSL(uint8_t LED, RGB_T colour){
	led_set_RGB(LED, colour.red, colour.grn, colour.blu);
}
