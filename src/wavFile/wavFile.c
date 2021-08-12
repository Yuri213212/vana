#include "wavFile.h"

int16_t data2short(double x,int amp){
	int cdata;

	cdata=floor(x*amp);
	if (cdata>32767){
		cdata=32767;
	}
	if (cdata<-32768){
		cdata=-32768;
	}
	return (int16_t)cdata;
}

struct wave *wav_new(double samplerate){
	struct wave *wav;

	if (!(samplerate>0.0)) return NULL;
	wav=(struct wave *)malloc(sizeof(struct wave));
	wav->length=0;
	wav->buflen=sizeof(void *);
	wav->samplerate=samplerate;
	wav->data=(double *)malloc(wav->buflen*sizeof(double));
	return wav;
}

void wav_delete(struct wave *wav){
	if (!wav) return;
	free(wav->data);
	free(wav);
}

struct wave *wav_clone(struct wave *wavi){
	struct wave *wav;

	if (!wavi) return NULL;
	wav=(struct wave *)malloc(sizeof(struct wave));
	wav->length=wavi->length;
	wav->buflen=wavi->buflen;
	wav->samplerate=wavi->samplerate;
	wav->data=(double *)malloc(wav->buflen*sizeof(double));
	memcpy(wav->data,wavi->data,wav->length*sizeof(double));
	return wav;
}

void wav_clear(struct wave *wav){
	if (!wav) return;
	wav->length=0;
}

void wav_addPoint(struct wave *wav,double point){
	double *a;

	if (!wav) return;
	if (wav->length+1>=wav->buflen){
		wav->buflen<<=1;
		a=(double *)malloc(wav->buflen*sizeof(double));
		memcpy(a,wav->data,wav->length*sizeof(double));
		free(wav->data);
		wav->data=a;
	}
	wav->data[wav->length]=point;
	++wav->length;
}

void wav_addData(struct wave *wav,double *datai,int length){
	double *a;

	if (!wav||!datai) return;
	if (wav->length+length>=wav->buflen){
		for (wav->buflen<<=1;wav->length+length>=wav->buflen;wav->buflen<<=1);
		a=(double *)malloc(wav->buflen*sizeof(double));
		memcpy(a,wav->data,wav->length*sizeof(double));
		free(wav->data);
		wav->data=a;
	}
	memcpy(&wav->data[wav->length],datai,length*sizeof(double));
	wav->length+=length;
}

void wav_addWav(struct wave *wav,struct wave *wav2){
	if (!wav||!wav2) return;
	wav_addData(wav,wav2->data,wav2->length);
}

struct wave *wav_concat(struct wave *wav1,struct wave *wav2){
	struct wave *wav;

	if (!wav1||!wav2) return NULL;
	wav=wav_new(wav1->samplerate);
	wav_addData(wav1,wav1->data,wav1->length);
	wav_addData(wav2,wav2->data,wav2->length);
	return wav;
}

double wav_samplerate(struct wave *wav){
	if (!wav) return NAN;
	return wav->samplerate;
}

void wav_setSamplerate(struct wave *wav,double samplerate){
	if (!wav) return;
	wav->samplerate=samplerate;
}

int wav_length(struct wave *wav){
	if (!wav) return -1;
	return wav->length;
}

void wav_setLength(struct wave *wav,int length){
	double *a;
	int i;

	if (!wav) return;
	if (length<0) return;
	if (length<=wav->buflen){
		wav->length=length;
		return;
	}
	for (wav->buflen<<=1;length>=wav->buflen;wav->buflen<<=1);
	a=(double *)malloc(wav->buflen*sizeof(double));
	memcpy(a,wav->data,wav->length*sizeof(double));
	for (i=wav->length;i<length;++i){
		a[i]=0.0;
	}
	free(wav->data);
	wav->data=a;
	wav->length=length;
}

double *wav_getData(struct wave *wav){
	if (!wav) return NULL;
	return wav->data;
}

double *wav_copyData(struct wave *wav){
	double *a;

	if (!wav) return NULL;
	a=(double *)malloc(wav->length*sizeof(double));
	memcpy(a,wav->data,wav->length*sizeof(double));
	return a;
}

struct wave *wav_read(FILE *fp){
	static struct wavHeader head;

	int length,i,j,k,temp;
	uint8_t *data8;
	int16_t *data16;
	int32_t *data32;
	float *datafloat;
	double *a;
	struct wave *wav;

	if (!fp) return NULL;
	if (!fread(&head,sizeof(struct wavHeader),1,fp)) return NULL;
	if (head.RIFF!=0x46464952) return NULL;
	if (head.size_add36<head.size+36) return NULL;
	if (head.WAVE!=0x45564157) return NULL;
	if (head.fmt!=0x20746d66) return NULL;
	if (head.size_eq16!=16) return NULL;
	if (head.format!=1&&head.format!=3) return NULL;
	if (head.channels<1) return NULL;
	if (head.samplerate<1) return NULL;
	if (head.byterate!=head.samplerate*head.align) return NULL;
	if (head.align!=head.channels*((head.samplebits+7)>>3)) return NULL;
	if (head.samplebits<1) return NULL;
	if (head.data!=0x61746164) return NULL;
	if (head.size<0) return NULL;
	length=head.size/head.align;
	wav=wav_new((double)head.samplerate);
	a=(double *)malloc(length*sizeof(double));
	switch (head.samplebits|head.format){
	case 0x09:	//8-bit unsigned
		data8=(uint8_t *)malloc(head.size);
		fread(data8,1,head.size,fp);
		for (i=0;i<length;++i){
			a[i]=0.0;
			for (j=0;j<head.channels;++j){
				a[i]+=(data8[i*head.channels+j]-128.0)/128.0;
			}
			a[i]/=head.channels;
		}
		free(data8);
		break;
	case 0x11:	//16-bit signed
		data16=(int16_t *)malloc(head.size);
		fread(data16,1,head.size,fp);
		for (i=0;i<length;++i){
			a[i]=0.0;
			for (j=0;j<head.channels;++j){
				a[i]+=data16[i*head.channels+j]/32768.0;
			}
			a[i]/=head.channels;
		}
		free(data16);
		break;
	case 0x15:	//20-bit signed
	case 0x19:	//24-bit signed
		data8=(uint8_t *)malloc(head.size);
		fread(data8,1,head.size,fp);
		temp=0;
		for (i=0;i<length;++i){
			a[i]=0.0;
			for (j=0;j<head.channels;++j){
				for (k=0;k<3;++k){
					*((char *)&temp+k+1)=data8[(i*head.channels+j)*3+k];
				}
				a[i]+=temp/2147483648.0;
			}
			a[i]/=head.channels;
		}
		free(data8);
		break;
	case 0x21:	//32-bit signed
		data32=(int32_t *)malloc(head.size);
		fread(data32,1,head.size,fp);
		for (i=0;i<length;++i){
			a[i]=0.0;
			for (j=0;j<head.channels;++j){
				a[i]+=data32[i*head.channels+j]/2147483648.0;
			}
			a[i]/=head.channels;
		}
		free(data32);
		break;
	case 0x23:	//32-bit float
		datafloat=(float *)malloc(head.size);
		fread(datafloat,1,head.size,fp);
		for (i=0;i<length;++i){
			a[i]=0.0;
			for (j=0;j<head.channels;++j){
				a[i]+=datafloat[i*head.channels+j];
			}
			a[i]/=head.channels;
		}
		free(datafloat);
		break;
	default:
		wav_delete(wav);
		free(a);
		return NULL;
	}
	wav_addData(wav,a,length);
	free(a);
	return wav;
}

void wav_write(FILE *fp,struct wave *wav){
	static struct wavHeader head;

	int length,i;
	int16_t *data16;

	if (!fp) return;
	if (!wav) return;
	length=wav->length;
	data16=(int16_t *)malloc(length*sizeof(int16_t));
	for (i=0;i<length;++i){
		data16[i]=data2short(wav->data[i],32768);
	}
	head.RIFF=0x46464952;
	head.size_add36=length*2+36;
	head.WAVE=0x45564157;
	head.fmt=0x20746d66;
	head.size_eq16=16;
	head.format=1;
	head.channels=1;
	head.samplerate=(int32_t)wav->samplerate;
	head.byterate=head.samplerate*2;
	head.align=2;
	head.samplebits=16;
	head.data=0x61746164;
	head.size=length*2;
	fwrite(&head,sizeof(struct wavHeader),1,fp);
	fwrite(data16,sizeof(int16_t),length,fp);
	free(data16);
}

void wav_trimEnd(struct wave *wav,double threshold){
	int i;

	if (!wav) return;
	if (DOUBLE2LONG(threshold)==DOUBLE2LONG(0.0)){
		for (i=wav->length-1;i>=0;--i){
			if (DOUBLE2LONG(wav->data[i])!=DOUBLE2LONG(0.0)) return;
			--wav->length;
		}
	}else{
		for (i=wav->length-1;i>=0;--i){
			if (fabs(wav->data[i])>threshold) return;
			--wav->length;
		}
	}
}

int wav_compare(struct wave *wav,struct wave *wav2,double threshold){
	int length,i;

	if (!wav) return -1;
	if (!wav2) return -1;
	if (wav==wav2) return 0;
	length=wav->length;
	if (length!=wav2->length) return 1;
	if (DOUBLE2LONG(threshold)==DOUBLE2LONG(0.0)){
		if (memcmp(wav->data,wav2->data,length*sizeof(double))) return 1;
	}else{
		for (i=0;i<length;++i){
			if (fabs(wav->data[i]-wav2->data[i])>threshold) return 1;
		}
	}
	return 0;
}

double wav_amplitude(struct wave *wav){
	int length,i;
	double max=-1.0,min=1.0;

	if (!wav) return NAN;
	length=wav->length;
	if (!length) return NAN;
	for (i=0;i<length;++i){
		if (max<wav->data[i]){
			max=wav->data[i];
		}
		if (min>wav->data[i]){
			min=wav->data[i];
		}
	}
	return max>=-min?max:-min;
}

void wav_normalize(struct wave *wav,double max){
	int length,i;

	if (!wav) return;
	if (DOUBLE2LONG(max)==DOUBLE2LONG(0.0)){
		max=wav_amplitude(wav);
	}
	length=wav->length;
	for (i=0;i<length;++i){
		wav->data[i]/=max;
	}
}

void wav_amplify(struct wave *wav,double a){
	int length,i;

	if (!wav) return;
	length=wav->length;
	for (i=0;i<length;++i){
		wav->data[i]*=a;
	}
}

struct wave *wav_resample(struct wave *wavi,double samplerate){
	int length,fs,cs,fe,i,j;
	double speed,start,end,out;
	double *a;
	struct wave *wav;

	if (!wavi) return NULL;
	if (!(samplerate>0.0)) return NULL;
	if (DOUBLE2LONG(samplerate)==DOUBLE2LONG(wavi->samplerate)) return wav_clone(wavi);
	wav=wav_new(samplerate);
	length=ceil(wavi->length*samplerate/wavi->samplerate);
	a=(double *)malloc(length*sizeof(double));
	speed=wavi->samplerate/samplerate;
	for (i=0;i<length;++i){
		start=i*speed;
		end=(i+1)*speed;
		fs=floor(start);
		fe=floor(end);
		if (fs==fe){
			out=wavi->data[fs];
		}else{
			cs=ceil(start);
			out=(cs-start)*wavi->data[fs]+(end-fe)*wavi->data[fe];
			for (j=cs;j<fe;++j){
				out+=wavi->data[j];
			}
			out/=speed;
		}
		a[i]=out;
	}
	wav_addData(wav,a,length);
	free(a);
	return wav;
}

double analyzeTempo(struct wave *wavpwr,double left,double right,int density){
	double w0,cs,sn,a,max;
	int channelcount,result,i,j;
	double *channel;

	if (!wavpwr) return NAN;
	if (DOUBLE2LONG(left)==DOUBLE2LONG(right)) return left;
	channelcount=(right-left)*density+1;
	channel=(double *)malloc(channelcount*sizeof(double));
	max=0.0;
	result=0;
	for (j=0;j<channelcount;++j){
		w0=(M_PI/30.0)*((double)j/density+left)/wavpwr->samplerate;
		cs=0.0;
		sn=0.0;
		for (i=0;i<wavpwr->length;++i){
			cs+=wavpwr->data[i]*cos(i*w0);
			sn+=wavpwr->data[i]*sin(i*w0);
		}
		a=sqrt(cs*cs+sn*sn);
		if (a>max){
			max=a;
			result=j;
		}
	}
	free(channel);
	return (double)result/density+left;
}

double wav_analyzeTempo(struct wave *wav,int min,int max,double trim,int limit){
	double sum,result;
	int left,right,window,i,j,n;
	struct wave *wavpwr;

	if (!wav) return NAN;
	if (min>max) return NAN;
	if (trim<0.0||trim>=0.5) return NAN;
	if (min==max) return (double)min;
	left=wav->length*trim;
	right=wav->length*(1.0-trim);
	if (left==right) return NAN;
	if (limit<=0){
		window=1;
	}else{
		window=ceil((double)(right-left)/limit);
	}
	wavpwr=wav_new(wav->samplerate/window);
	for (j=0;;++j){
		sum=0.0;
		for (i=0;i<window;++i){
			n=j*window+i+left;
			if (n>=right) goto next;
			sum+=wav->data[n]*wav->data[n];
		}
		wav_addPoint(wavpwr,sum);
	}
next:
	result=analyzeTempo(wavpwr,(double)min,(double)max,1);
	result=analyzeTempo(wavpwr,result-1.0,result+1.0,100);
	wav_delete(wavpwr);
	return result;
}
