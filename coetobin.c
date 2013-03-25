#include <malloc.h>
#include <stdio.h>
#include <string.h>

inline unsigned int byteSwap(unsigned int word)
{
	return (word >> 24) + (word << 24) + ((word << 8) & 0x00FF0000) + ((word >> 8) & 0x0000FF00);
}


int main(int argc, char **argv)
{
	int argOffset = 0;
	unsigned char bigEndian = 1;
	if(argc!=3)
	{
		if(argc==4 && !strcmp(argv[1],"-littleendian"))
		{
			argOffset = 1;
			bigEndian = 0;
		} else {
			printf("Usage: %s [-littleendian] [file.coe] [file.bin]\n",argv[0]);
			return 0;
		}
	}
	FILE *fp = fopen(argv[1+argOffset],"r");
	if(!fp)
	{
		fprintf(stderr,"Error opening file: %s\n",argv[1+argOffset]);
		return 0;
	}

	FILE *fpw = fopen(argv[2+argOffset],"wb");
	if(!fpw)
	{
		fprintf(stderr,"Error writing file: %s\n",argv[2+argOffset]);
		return 0;
	}
	
	int instr;
	char temp[80];
	fscanf(fp, "%s\n%s\n",temp,temp);
	while(fscanf(fp, "%x %c\n", &instr, temp) != EOF)
	{
		//printf("%x\n",instr);
		if(bigEndian) instr = byteSwap(instr);
		fwrite(&instr, sizeof(int), 1, fpw);
	}
	fclose(fp);
	fclose(fpw);
}
