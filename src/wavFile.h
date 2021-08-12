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
void wav_addPoint(HWAV hwav,double point);

//add data to end
void wav_addData(HWAV hwav,double *data,int length);

//add data of wav object to end
void wav_addWav(HWAV hwav,HWAV hwav2);

//concatenate 2 wav object and create new wav object
//call wav_delete to free memory
HWAV wav_concat(HWAV hwav1,HWAV hwav2);

//get sample rate of wav object
double wav_samplerate(HWAV hwav);

//set sample rate of wav object without changing data
void wav_setSamplerate(HWAV hwav,double samplerate);

//get length of wav object
int wav_length(HWAV hwav);

//set length of wav object, add 0.0s or remove last data
void wav_setLength(HWAV hwav,int length);

//get the pointer to internal data
double *wav_getData(HWAV hwav);

//get a copy of current data
//call free to free memory
double *wav_copyData(HWAV hwav);

//read wav file to create wav object
//call wav_delete to free memory
HWAV wav_read(FILE *fp);

//save wav object to wav file
void wav_write(FILE *fp,HWAV hwav);

//delete sample points with low amplitude from end
void wav_trimEnd(HWAV hwav,double threshold);

//compare samples with uncertainty, return 0=same, 1=different, -1=error
int wav_compare(HWAV hwav,HWAV hwav2,double threshold);

//calculate maximum amplitude of wav object
double wav_amplitude(HWAV hwav);

//normalize wav object with specified maximum amplitude, 0.0 for auto
void wav_normalize(HWAV hwav,double max);

//amplify wav object
void wav_amplify(HWAV hwav,double a);

//create new resampled wav object
//call wav_delete to free memory
HWAV wav_resample(HWAV hwav,double samplerate);

//analyze tempo of wav object, with accuracy of 2 decimal points
double wav_analyzeTempo(HWAV hwav,int min,int max,double trim,int limit);

#endif
