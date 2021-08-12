#define DisplayCount	120
#define Density		1
#define BarCount	(DisplayCount*Density+2)

struct rlcdt{
	float c1;
	float c2;
	float c3;
	float c4;
	float c5;
	float i;
	float v;
	float out;
};

struct rlcdt bar[BarCount];
int graphLength=0;
int *graphStart=NULL;
int (*graphColor)[DisplayCount];

void rlc_init(double samplerate){
	double t,t1,t2,t3,s,c1,c2;
	int i;

	t=pow(2.0,1.0/(NoteCount*Density));
	t1=t-1.0;
	t2=sqrt(t);
	t3=sqrt(6.0*t-t*t-1.0);
	for (i=0;i<BarCount;++i){
		s=2.0*M_PI*BaseFreq*pow(2.0,(double)(i+StartNote*Density-1)/(NoteCount*Density)-BaseNote)/samplerate;
		c1=exp(-0.5*s*t1/t2)*cos(0.5*s*t3/t2);
		c2=exp(-0.5*s*t1/t2)*sin(0.5*s*t3/t2);
		bar[i].c1=c1;
		bar[i].c2=c2*2.0*t1/t3;
		bar[i].c3=(c1-c2*t3/t1)*0.5;
		bar[i].c4=c1+c2*t1/t3;
		bar[i].c5=t1/t2/s;
		bar[i].i=0.0f;
		bar[i].v=0.0f;
		bar[i].out=0.0f;
	}
}

void rlc_render(double *data,int length,double amp){
	float in,out,i0,v0,b;
	int i,j;

	for (i=0;i<BarCount;++i){
		bar[i].out=0.0f;
	}
	for (j=0;j<length;++j){
		in=data[j]*amp;
		for (i=0;i<BarCount;++i){
			i0=bar[i].i;
			v0=bar[i].v;
			b=in-v0-i0*0.5f;
			bar[i].i=i0*bar[i].c1+b*bar[i].c2;
			bar[i].v=in-i0*bar[i].c3-b*bar[i].c4;
			out=(bar[i].v-v0)*bar[i].c5;
			bar[i].out+=out*out;
		}
	}
	for (i=0;i<BarCount;++i){
		bar[i].out=bar[i].out*2.0f/length;
	}
}

int getColor(float x){
	const float p1=1.0/6.0,p2=2.0/6.0,p3=3.0/6.0,p4=4.0/6.0,p5=5.0/6.0;

	float R,G,B;
	int r,g,b;

	if (x>1.0f){
		x=1.0f;
	}
	if (x<0.0f){
		x=0.0f;
	}
	if (x>=p5){
		R=1.0f;
	}else if (x>=p4){
		R=(x-p4)*6.0f;
	}else if (x<=p2){
		if (x<=p1){
			R=1.0f;
		}else{
			R=(p2-x)*6.0f;
		}
	}else{
		R=0.0f;
	}
	r=(int)roundf(R*powf(x,Gamma)*256.0f);
	if (r>255){
		r=255;
	}
	if (x>=p5){
		G=(1.0f-x)*6.0f;
	}else if (x>=p3){
		G=1.0f;
	}else if (x>=p2){
		G=(x-p2)*6.0f;
	}else{
		G=0.0f;
	}
	g=(int)roundf(G*powf(x,Gamma)*256.0f);
	if (g>255){
		g=255;
	}
	if (x>=p4){
		B=0.0f;
	}else if (x>=p3){
		B=(p4-x)*6.0f;
	}else if (x>=p1){
		B=1.0f;
	}else{
		B=x*6.0f;
	}
	b=(int)roundf(B*powf(x,Gamma)*256.0f);
	if (b>255){
		b=255;
	}
	return RGB(r,g,b);
}

void graph_addline(int row){
	float out;
	int i;

	for (i=0;i<DisplayCount;++i){
		out=bar[i*Density+1].out-(bar[i*Density].out+bar[i*Density+2].out)*Depress;
		if (out>0.0f){
			out=log2f(out)*3.0f/ThresDB+1.0f;
		}
		if (out>0.0f){
			graphColor[row][i]=getColor((out*MinColor+3.0f-MinColor)/3.0f);
		}else{
			graphColor[row][i]=0;
		}
	}
}

void graph_analyze(HWAV hwav,double tempospeed,double amp){
	double samplerate;
	int length,start,row,n,j;
	double *data;

	samplerate=wav_samplerate(hwav);
	length=wav_length(hwav);
	data=wav_getData(hwav);
	graphLength=ceil(length*tempospeed/(60.0*samplerate));
	graphStart=(int *)malloc(graphLength*sizeof(int));
	graphColor=(int (*)[DisplayCount])malloc(graphLength*(DisplayCount*sizeof(int)));
	rlc_init(samplerate);
	start=0;
	row=0;
	for (j=1;j<length;++j){
		n=j*tempospeed/(60.0*samplerate);
		if (n!=row){
			graphStart[row]=start;
			rlc_render(data+start,j-start,amp);
			graph_addline(row);
			row=n;
			start=j;
		}
	}
	graphStart[row]=start;
	rlc_render(data+start,length-start,amp);
	graph_addline(row);
}
