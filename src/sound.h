#define fps		15

int BufferLength=0,sampleCounter;
WAVEHDR waveHdr[2]={};
WAVEFORMATEX waveformat={WAVE_FORMAT_PCM,1,0,0,sizeof(short),sizeof(short)*8,0};

HWAVEOUT openSound(HWND hwnd){
	HWAVEOUT hWaveOut;
	double samplerate;

	samplerate=wav_samplerate(hwav);
	BufferLength=samplerate/fps;
	waveHdr[0].dwBufferLength=BufferLength*sizeof(short);
	waveHdr[0].lpData=(LPSTR)malloc(BufferLength*sizeof(short));
	memset(waveHdr[0].lpData,0,BufferLength*sizeof(short));
	waveHdr[1].dwBufferLength=BufferLength*sizeof(short);
	waveHdr[1].lpData=(LPSTR)malloc(BufferLength*sizeof(short));
	memset(waveHdr[1].lpData,0,BufferLength*sizeof(short));
	waveformat.nSamplesPerSec=samplerate;
	waveformat.nAvgBytesPerSec=samplerate*sizeof(short);
	if (waveOutOpen(&hWaveOut,WAVE_MAPPER,&waveformat,(DWORD_PTR)hwnd,0,CALLBACK_WINDOW)!=MMSYSERR_NOERROR){
		free(waveHdr[0].lpData);
		free(waveHdr[1].lpData);
		return NULL;
	}
	waveOutPrepareHeader(hWaveOut,&waveHdr[0],sizeof(WAVEHDR));
	waveOutPrepareHeader(hWaveOut,&waveHdr[1],sizeof(WAVEHDR));
	return hWaveOut;
}

void deleteSound(){
	BufferLength=0;
	free(waveHdr[0].lpData);
	free(waveHdr[1].lpData);
}

void fillBuffer(short *buffer){
	int i,wavlength;
	double *data;

	wavlength=wav_length(hwav);
	data=wav_getdata(hwav);
	for (i=0;i<BufferLength;++i){
		if (sampleCounter>=wavlength){
			buffer[i]=0;
		}else{
			buffer[i]=data2short(data[sampleCounter],Volume<<7);
		}
		++sampleCounter;
	}
	for (i=currentRow;i<graphLength;++i){
		if (sampleCounter<graphStart[i]) break;
	}
	currentRow=i-1;
}
