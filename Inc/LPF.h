/*
 * LPF.h
 *
 *  Created on: 11 Nov 2020
 *      Author: rri
 */

#ifndef LPF_H_
#define LPF_H_

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 40000 Hz

fixed point precision: 16 bits

* 0 Hz - 400 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = n/a

* 500 Hz - 20000 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = n/a

*/

#define LPF_TAP_NUM 501

typedef struct {
  int history[LPF_TAP_NUM];
  unsigned int last_index;
} LPF_T;

void LPF_init(LPF_T* f);
void LPF_put(LPF_T* f, int input);
int LPF_get(LPF_T* f);

#endif /* LPF_H_ */
