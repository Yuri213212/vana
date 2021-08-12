#define NoteCount	12

int StartNote=0,RefNote=48,CursorPos=32,DefBaseNote=57,DefTranspose=0,DefAutoTempoMin=128,DefAutoTempoMax=255,DefAutoSet=1,AutoLengthLimit=4096,DefSpeed=8,DefHighlight=16,DefOffset=0,DefVolume=256,LengthLimit=65536;
double DefBaseFreq=440.0,DefTempo=150.0,AutoTrim=0.125,DefAmplifyDB=0.0;
float ThresDB=72.0,Depress=0.5,MinColor=3.0,Gamma=0.5;

int Transpose,Speed,Highlight,Offset,Volume;
double BaseNote,BaseFreq,Tempo,AmplifyDB;

int readnote(char *s){
	int result;

	switch (s[0]){
	case 'C':
		result=0;
		break;
	case 'D':
		result=2;
		break;
	case 'E':
		result=4;
		break;
	case 'F':
		result=5;
		break;
	case 'G':
		result=7;
		break;
	case 'A':
		result=9;
		break;
	case 'B':
		result=11;
		break;
	default:
		return -1;
	}
	switch (s[1]){
	case '-':
		break;
	case '#':
		result+=1;
		break;
	default:
		return -1;
	}
	if (s[2]>='0'&&s[2]<='9'){
		result+=(s[2]-'0')*NoteCount;
	}else{
		return -1;
	}
	return result;
}

int readSettings(FILE *fp){
	HINI hini;
	char *s;
	int temp;

	hini=ini_read(fp);
	if (!hini) return 0;
	s=ini_getValue(hini,"vana","StartNote");
	if (s){
		temp=readnote(s);
		if (temp>=0){
			StartNote=temp;
		}
	}
	s=ini_getValue(hini,"vana","RefNote");
	if (s){
		temp=readnote(s);
		if (temp>=0){
			RefNote=temp;
		}
	}
	s=ini_getValue(hini,"vana","CursorPos");
	if (s){
		sscanf(s,"%d",&CursorPos);
	}
	s=ini_getValue(hini,"vana","DefBaseNote");
	if (s){
		temp=readnote(s);
		if (temp>=0){
			DefBaseNote=temp;
		}
	}
	s=ini_getValue(hini,"vana","DefBaseFreq");
	if (s){
		sscanf(s,"%lf",&DefBaseFreq);
	}
	s=ini_getValue(hini,"vana","DefTranspose");
	if (s){
		sscanf(s,"%d",&DefTranspose);
	}
	s=ini_getValue(hini,"vana","DefTempo");
	if (s){
		sscanf(s,"%lf",&DefTempo);
	}
	s=ini_getValue(hini,"vana","DefAutoTempoMin");
	if (s){
		sscanf(s,"%d",&DefAutoTempoMin);
	}
	s=ini_getValue(hini,"vana","DefAutoTempoMax");
	if (s){
		sscanf(s,"%d",&DefAutoTempoMax);
	}
	s=ini_getValue(hini,"vana","DefAutoSet");
	if (s){
		sscanf(s,"%d",&DefAutoSet);
	}
	s=ini_getValue(hini,"vana","AutoTrim");
	if (s){
		sscanf(s,"%lf",&AutoTrim);
		if (AutoTrim<0.0){
			AutoTrim=0.0;
		}
		if (AutoTrim>0.25){
			AutoTrim=0.25;
		}
	}
	s=ini_getValue(hini,"vana","AutoLengthLimit");
	if (s){
		sscanf(s,"%d",&AutoLengthLimit);
	}
	s=ini_getValue(hini,"vana","DefSpeed");
	if (s){
		sscanf(s,"%d",&DefSpeed);
	}
	s=ini_getValue(hini,"vana","DefHighlight");
	if (s){
		sscanf(s,"%d",&DefHighlight);
	}
	s=ini_getValue(hini,"vana","DefOffset");
	if (s){
		sscanf(s,"%d",&DefOffset);
	}
	s=ini_getValue(hini,"vana","DefAmplifyDB");
	if (s){
		sscanf(s,"%lf",&DefAmplifyDB);
	}
	s=ini_getValue(hini,"vana","ThresDB");
	if (s){
		sscanf(s,"%f",&ThresDB);
		if (ThresDB<0.0f){
			ThresDB=-ThresDB;
		}
	}
	s=ini_getValue(hini,"vana","Depress");
	if (s){
		sscanf(s,"%f",&Depress);
		if (fabsf(Depress)>1.0f){
			Depress=1.0f/Depress;
		}
		if (Depress<0.0f){
			Depress=1.0f-Depress;
		}
	}
	s=ini_getValue(hini,"vana","MinColor");
	if (s){
		sscanf(s,"%f",&MinColor);
		if (MinColor>3.0f){
			MinColor=3.0f;
		}
		if (MinColor<0.0f){
			MinColor=0.0f;
		}
	}
	s=ini_getValue(hini,"vana","Gamma");
	if (s){
		sscanf(s,"%f",&Gamma);
	}
	s=ini_getValue(hini,"vana","DefVolume");
	if (s){
		sscanf(s,"%d",&DefVolume);
		if (DefVolume>256){
			DefVolume=256;
		}
		if (DefVolume<0){
			DefVolume=0;
		}
	}
	s=ini_getValue(hini,"vana","LengthLimit");
	if (s){
		sscanf(s,"%d",&LengthLimit);
		if (LengthLimit<0){
			LengthLimit=0;
		}
	}
	ini_delete(hini);
	return 1;
}
