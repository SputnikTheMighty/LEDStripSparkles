#ifndef _LED_DRIVER_NEOPIXEL
#define _LED_DRIVER_NEOPIXEL

#define PWM_HI (38)
#define PWM_LO (19)

// LED parameters
#define NUM_BPP (3) // WS2812B
//#define NUM_BPP (4) // SK6812
#define NUM_PIXELS (8)
#define NUM_BYTES (NUM_BPP * NUM_PIXELS)

#include <stdint.h>

void led_set_RGB(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
void led_set_RGBW(uint8_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void led_set_all_RGB(uint8_t r, uint8_t g, uint8_t b);
void led_set_all_RGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void led_render();

#endif
