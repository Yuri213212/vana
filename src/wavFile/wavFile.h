#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../doubleUtil.h"

struct wave{
	int length;
	int buflen;
	double samplerate;
	double *data;
};

struct wavHeader{
	int32_t RIFF;		//"RIFF",0x46464952
	int32_t size_add36;	//typical size+36, or more for extra information
	int32_t WAVE;		//"WAVE",0x45564157
	int32_t fmt;		//"fmt ",0x20746d66
	int32_t size_eq16;	//16
	int16_t format;		//1 for integer, 3 for float
	int16_t channels;	//1 for mono, 2 for stereo
	int32_t samplerate;
	int32_t byterate;	//samplerate*align
	int16_t align;		//channels*((samplebits+7)>>3)
	int16_t samplebits;	//8 for unsigned char, 16 for short, 32 for int/float
	int32_t data;		//"data",0x61746164
	int32_t size;		//length*align
};
