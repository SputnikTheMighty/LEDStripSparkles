/*
 * bug.c
 *
 *  Created on: 7 Nov 2020
 *      Author: rri
 */

#include "bug.h"

void UpdateBug(BUG_T *bug){

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
			if(bug->length != 0 ){
				bug->length--;
			}
		}
	}

	for(uint8_t i=0; i<bug->length; i++){
		uint8_t temp = bug->position;
		temp += i;
		if(temp > NUM_PIXELS-1){
			temp = temp-NUM_PIXELS;
		}
		led_set_RGB(temp, bug->colour.red, bug->colour.grn, bug->colour.blu);
	}
}

void AddBug(BUG_T **localHeadBug, uint8_t length, uint8_t position, RGB_T colour, uint8_t direction,
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
		totalNewBugs = 5+rand()%7;

	RGB_T tempcolour = {0xFF, 0x35, 0x30};
	uint8_t startposition = rand()%(NUM_PIXELS-1);

	for(uint8_t i=0; i< totalNewBugs; i++){
		// length, position, colour, direction, speed0(high is low), lifetime
		if(i%2 == 0)
			AddBug(localHeadBug, 2+rand()%5, startposition, tempcolour, 0, (1+rand()%5), (2+rand()%15));
		else
			AddBug(localHeadBug, 2+rand()%5, startposition, tempcolour, 1, (1+rand()%5), (2+rand()%15));
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



