#ifndef WAVFILE_H
#define WAVFILE_H

#include <stdint.h>
#include <stdio.h>

typedef void* HWAV;

//regulate data for output
int16_t data2short(double x,int amp);

//create empty wav object in specified sample rate
//call wav_delete to free memory
HWAV wav_new(double samplerate);

//destroy wav object
void wav_delete(HWAV hwav);

//clone wav object
//call wav_delete to free memory
HWAV wav_clone(HWAV hwav);

//clear data of wav object
void wav_clear(HWAV hwav);

//add point to end
void wav_addpoint(HWAV hwav,double point);

//add data to end
void wav_adddata(HWAV hwav,double *data,int length);

//add data of wav object to end
void wav_addwav(HWAV hwav,HWAV hwav2);

//concatenate 2 wav object and create new wav object
//call wav_delete to free memory
HWAV wav_concat(HWAV hwav1,HWAV hwav2);

//get sample rate of wav object
double wav_samplerate(HWAV hwav);

//get length of wav object
int wav_length(HWAV hwav);

//get the pointer to internal data
double *wav_getdata(HWAV hwav);

//get a copy of current data
//call free to free memory
double *wav_copydata(HWAV hwav);

//read wav file to create wav object
//call wav_delete to free memory
HWAV wav_read(FILE *fp);

//save wav object to wav file
void wav_write(FILE *fp,HWAV hwav);

//create new resampled wav object
//call wav_delete to free memory
HWAV wav_resample(HWAV hwav,double samplerate);

//analyse tempo of wav object, with accuracy of 2 decimal points
double wav_analysetempo(HWAV hwav,int min,int max);

#endif
