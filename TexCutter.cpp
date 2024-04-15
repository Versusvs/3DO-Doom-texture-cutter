#include "stdafx.h"
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <cmath>
//#include <fstream>
//#include <io.h>
//#include <fcntl.h>
//#include <iomanip>
//#include <sstream>
typedef unsigned int Word;
//using System.IO;
using namespace std;

static void ConvertTexture(Word Config);
static void ConvertFlat(Word Config);
static void LoadTexScript(FILE* fp);
static void LoadFlatScript(FILE* fp);

static char Delimiters[] = " \t\n";        /* Token delimiters */
static char NumDelimiters[] = " ,\t\n";    /* Value delimiters */
static Word LineNum;               /* Line being executed from the script */
static char InputLine[256];        /* Input line from script */
static char *Commands[] = {
	"TYPE","LOAD","ENDIAN","LOADNEXT","LOADFIXED","LOADHANDLE"
};
#define CommandCount 6      /* Number of commands */

#define Swap2_2Bytes(val) val = ( (((val) >> 8) & 0x00FF) | (((val) << 8) & 0xFF00) )
#define Swap4Bytes(val) val = ( (((val) >> 24) & 0x000000FF) | (((val) >> 8) & 0x0000FF00) | (((val) <<  8) & 0x00FF0000) | (((val) << 24) & 0xFF000000) )
//#define SwapULong(val) val = ( (((val) >> 24) & 0x000000FF) | (((val) >> 8) & 0x0000FF00) | (((val) <<  8) & 0x00FF0000) | (((val) << 24) & 0xFF000000) )
//#define SwapULong(val) (val << 24 | (val << 8 & 0xFF0000) | (val >> 8 & 0xFF00) | val >> 24 & 0xFF)
#define Swap8Bytes(val) ( (((val) >> 56) & 0x00000000000000FF) | (((val) >> 40) & 0x000000000000FF00) | (((val) >> 24) & 0x0000000000FF0000) | (((val) >>  8) & 0x00000000FF000000) | (((val) <<  8) & 0x000000FF00000000) | (((val) << 24) & 0x0000FF0000000000) | (((val) << 40) & 0x00FF000000000000) | (((val) << 56) & 0xFF00000000000000) )
#define Swap2Bytes(val) ( (((val) >> 8) & 0x00FF) | (((val) << 8) & 0xFF00) )

int countFlat, countTex;

static void PrintError(char *Error)
{
	printf("# Error in Line %d, %s\n",LineNum,Error);
}





static void LoadTexScript(FILE* fp)
{
	char *TextPtr;  /* Pointer to string token */
	Word i;         /* Index */

	while (fgets(InputLine,sizeof(InputLine),fp)) { /* Get a string */
		++LineNum;          /* Adjust the line # */
		TextPtr = strtok(InputLine,Delimiters); /* Get the first token */
		if (!TextPtr) {
			continue;
		}
		i = 0;      /* Check for the first command */
		if (isalnum(TextPtr[0])) {  /* Comment? */
			do {
				if (!strcmp(TextPtr,Commands[i])) { /* Match? */
					switch (i) {        /* Execute the command */
					case 0:
//						SetType();   /* Target machine */
						break;
					case 1:
						ConvertTexture(0); 
						break;
					}
					break;      /* Don't parse anymore! */
				}
			} while (++i<CommandCount); /* Keep checking */
		}
		if (i==CommandCount) {      /* Didn't find it? */
			printf("# Command %s not implemented\n",TextPtr);
		}
	}
}

static void LoadFlatScript(FILE* fp)
{
	char *TextPtr;  /* Pointer to string token */
	Word i;         /* Index */

	while (fgets(InputLine,sizeof(InputLine),fp)) { /* Get a string */
		++LineNum;          /* Adjust the line # */
		TextPtr = strtok(InputLine,Delimiters); /* Get the first token */
		if (!TextPtr) {
			continue;
		}
		i = 0;      /* Check for the first command */
		if (isalnum(TextPtr[0])) {  /* Comment? */
			do {
				if (!strcmp(TextPtr,Commands[i])) { /* Match? */
					switch (i) {        /* Execute the command */
					case 0:
//						SetType();   /* Target machine */
						break;
					case 1:
						ConvertFlat(0); 
						break;
					}
					break;      /* Don't parse anymore! */
				}
			} while (++i<CommandCount); /* Keep checking */
		}
		if (i==CommandCount) {      /* Didn't find it? */
			printf("# Command %s not implemented\n",TextPtr);
		}
	}
}



static void ConvertTexture(Word Config) {

	FILE *infile, *outfile;
	char *TextPtr;
	char *filename;
//	string outfilename[50];


		TextPtr = strtok(0,NumDelimiters);
	if (!TextPtr) {
		PrintError("Not enough parms for LOAD");
		return;
	}

	TextPtr = strtok(0,NumDelimiters);
	if (!TextPtr) {
		PrintError("Not enough parms for LOAD");
		return;
	}

	int FileSize, i, j, k, temp, temp2, countPLUT, offset, PLUT_offset, PDAT_offset, count, cnt; 
	int	repeatCount, ccbWidth_div2;
	Word PLUT_size;
	char String[160];
//	float repeatCount, ccbWidth_div2;
//	int repeatCount;
//	double	ccbWidth_div2;

struct {
		Word temp;
	} PLUT;

struct {
		Word temp;
	} PDAT;

struct {
		char temp;
	} String1;

offset = 0;

infile = fopen(TextPtr, "rb");
countTex++;

//outfile = fopen("bigdoor2", "wb");
// Сделать новое имя файла, в котором будет сначала "r", потом имя файла без расширения.
char begin[2] = "r";
filename = TextPtr;
//rename(oldname, newname);
char *FinName = strcat(begin, filename);

//char FinName2;
//FinName2 = System.IO.Path.GetFileNameWithoutExtension(FinName);

outfile = fopen(FinName, "wb");

fseek(infile,0,SEEK_END);
FileSize = ftell(infile);
rewind(infile);

cout << "Texture FileSize=" << FileSize << "\n";
//cout << "\n";

for (i=0; i<FileSize/4; i++) {
//for (i=0; i<2071; i++) {
		fread(&PLUT.temp,4,1,infile);
		temp = PLUT.temp;
//		fwrite(&temp,4,1,outfile);

		// Читаем PLUT 50 4C 55 54 как 54 55 - 4С 50
		if (temp == 0x54554C50) {

			PLUT_offset = offset;
			cout << "PLUT offset=" << offset << "\n";

			// Какой размер у секции PLUT?
			fread(&PLUT.temp,4,1,infile);
			PLUT_size = Swap4Bytes(PLUT.temp);
			cout << "PLUT size=" << PLUT_size << "\n";
//			fwrite(&PLUT_size,4,1,outfile);

			fread(&PLUT.temp,4,1,infile);
			fread(&PLUT.temp,4,1,infile);
			temp = PLUT.temp;
			fwrite(&temp,4,1,outfile);
			for (j=0; j<PLUT_size/4; j++) {
				fread(&PLUT.temp,4,1,infile);
				temp = PLUT.temp;
				if (temp == 0x00000000) {
					break;
					}
				fwrite(&temp,4,1,outfile);
				}
			}
		offset += 4;
	}

fclose(infile);

// Обнуляем offset, чтобы этой же переменной вычислить смещение PDAT.
offset = 0;
infile = fopen(TextPtr, "rb");
//infile = fopen("bigdoor2.cel", "rb");

for (i=0; i<FileSize/4; i++) {
		fread(&PDAT.temp,4,1,infile);
		temp2 = PDAT.temp;

		// Читаем PDAT 50 44 41 54 как 54 41 - 44 50
		if (temp2 == 0x54414450) {

			PDAT_offset = offset;
			cout << "PDAT offset=" << offset << "\n\n";

			fread(&PDAT.temp,4,1,infile); // Просто смещаем позицию на 4 байта.

			for (j=0; j<PLUT_offset/4-PDAT_offset/4-2; j++) {	// Читаем и пишем весь PDAT. -2 потому что заголовок PDAT длиной 8 байт (2 раза) мы уже отсчитали.
					fread(&PDAT.temp,4,1,infile);
					temp2 = PDAT.temp;
					fwrite(&temp2,4,1,outfile);
				}

			}
		offset += 4;
	}
fclose(infile);
//////fclose(outfile);
// К текстурам неба паразитные 160 байт не добавляем!
if ((!strcmp(TextPtr, "SKY1.cel")) || (!strcmp(TextPtr, "SKY2.cel")) || (!strcmp(TextPtr, "SKY3.cel")) ||
	(!strcmp(TextPtr, "SKY4.cel")) || (!strcmp(TextPtr, "SKY5.cel")) || (!strcmp(TextPtr, "SKY6.cel")) ||
	(!strcmp(TextPtr, "SKY7.cel")) || (!strcmp(TextPtr, "SKY8.cel")) || (!strcmp(TextPtr, "SKY9.cel")))	{
goto contin2;
} 

//if ((TextPtr=="SKY1.cel") || (TextPtr=="SKY2.cel") || (TextPtr=="sky3.cel") || (TextPtr=="sky4.cel") || (TextPtr=="sky5.cel") || (TextPtr=="sky6.cel")) {
//goto contin2;
//}

#if 1
// Здесь вычислим длину повторяющейся строки и запишем ее в конец файла outfile.
infile = fopen(TextPtr, "rb");
//////outfile = fopen(FinName, "ab");

fseek(infile,72,SEEK_SET); // Нужный нам параметр здесь
fread(&PDAT.temp,4,1,infile);
temp = Swap4Bytes(PDAT.temp);
// Длина повторяющейся строки
ccbWidth_div2 = temp/2;
cout << "String length to duplicate=" << ccbWidth_div2 << "\n";

// Теперь найдем эту строку и допишем ее дубли в конец файла outfile, но общаяя длина не должна быть больше 160 байт.
rewind(infile); // перемотаем на начало
// Перемотаем на начало PLUT - длину нашей строки. Читать ее будем с ее начала.
fseek(infile,PLUT_offset-ccbWidth_div2,SEEK_SET);
count = 0;

//repeatCount = ceil(160/ccbWidth_div2);
repeatCount = 160/ccbWidth_div2;
cout << "repeatCount=" << repeatCount << "\n";

//char *StringReal = new char[ccbWidth_div2];
again:
do {

	// Переделать на кратность ccbWidth_div2
///	if ((count == ccbWidth_div2/4)||(count == ccbWidth_div2/4*2)||(count == ccbWidth_div2/4*3)||(count == ccbWidth_div2))

#if 1
	if ((count == ccbWidth_div2/4)||(count == ccbWidth_div2/4*2)||(count == ccbWidth_div2/4*3)||(count == ccbWidth_div2)||
		(count == ccbWidth_div2/4*5)||(count == ccbWidth_div2/4*6)||(count == ccbWidth_div2/4*7)||(count == ccbWidth_div2/4*8)||
		(count == ccbWidth_div2/4*9)||(count == ccbWidth_div2/4*10)||(count == ccbWidth_div2/4*11)||(count == ccbWidth_div2/4*12)||
		(count == ccbWidth_div2/4*13)||(count == ccbWidth_div2/4*14)||(count == ccbWidth_div2/4*15)||(count == ccbWidth_div2/4*16)||
		(count == ccbWidth_div2/4*17)||(count == ccbWidth_div2/4*18)||(count == ccbWidth_div2/4*19)||(count == ccbWidth_div2/4*20)||
		(count == ccbWidth_div2/4*21)||(count == ccbWidth_div2/4*22)||(count == ccbWidth_div2/4*23)||(count == ccbWidth_div2/4*24)||
		(count == ccbWidth_div2/4*25)||(count == ccbWidth_div2/4*26)||(count == ccbWidth_div2/4*27)||(count == ccbWidth_div2/4*28)||
		(count == ccbWidth_div2/4*29)||(count == ccbWidth_div2/4*30)||(count == ccbWidth_div2/4*31)||(count == ccbWidth_div2/4*32)||
		(count == ccbWidth_div2/4*33)||(count == ccbWidth_div2/4*34)||(count == ccbWidth_div2/4*35)||(count == ccbWidth_div2/4*36)||
		(count == ccbWidth_div2/4*37)||(count == ccbWidth_div2/4*38)||(count == ccbWidth_div2/4*39)||(count == ccbWidth_div2/4*40)||
		(count == ccbWidth_div2/4*41)||(count == ccbWidth_div2/4*42)||(count == ccbWidth_div2/4*43)||(count == ccbWidth_div2/4*44)||
		(count == ccbWidth_div2/4*45)||(count == ccbWidth_div2/4*46)||(count == ccbWidth_div2/4*47)||(count == ccbWidth_div2/4*48)||
		(count == ccbWidth_div2/4*49)||(count == ccbWidth_div2/4*50)||(count == ccbWidth_div2/4*51)||(count == ccbWidth_div2/4*52)||
		(count == ccbWidth_div2/4*53)||(count == ccbWidth_div2/4*54)||(count == ccbWidth_div2/4*55)||(count == ccbWidth_div2/4*56)||
		(count == ccbWidth_div2/4*57)||(count == ccbWidth_div2/4*58)||(count == ccbWidth_div2/4*59)||(count == ccbWidth_div2/4*60)||
		(count == ccbWidth_div2/4*61)||(count == ccbWidth_div2/4*62)||(count == ccbWidth_div2/4*63)||(count == ccbWidth_div2/4*64)||
		(count == ccbWidth_div2/4*65)||(count == ccbWidth_div2/4*66)||(count == ccbWidth_div2/4*67)||(count == ccbWidth_div2/4*68)||
		(count == ccbWidth_div2/4*69)||(count == ccbWidth_div2/4*70)||(count == ccbWidth_div2/4*71)||(count == ccbWidth_div2/4*72)||
		(count == ccbWidth_div2/4*73)||(count == ccbWidth_div2/4*74)||(count == ccbWidth_div2/4*75)||(count == ccbWidth_div2/4*76)||
		(count == ccbWidth_div2/4*77)||(count == ccbWidth_div2/4*78)||(count == ccbWidth_div2/4*79)||(count == ccbWidth_div2/4*80)) 
#endif;
	{
//	if (count == ccbWidth_div2/4) {
//	if (count % ccbWidth_div2/4) {
//			if (repeatCount==0) {
//				goto contin;
//				}
				fseek(infile,PLUT_offset-ccbWidth_div2,SEEK_SET);
//				repeatCount--;
//				count = ccbWidth_div2/4;
				}
			fread(&PDAT.temp,4,1,infile);
			temp = PDAT.temp;
			fwrite(&temp,4,1,outfile);
			count++;
		
	}  while (count<160/4);
//repeatCount--;
contin:

fclose(infile);
contin2:
fclose(outfile);
#endif;

}



#if 1
static void ConvertFlat(Word Config) {

	FILE *infile, *outfile;
	char *TextPtr;
	char *filename;

	int FileSize, i, j, k, temp, temp2, countPLUT, offset, PLUT_offset, PDAT_offset, count, cnt; 
	int	repeatCount, ccbWidth_div2;
	Word PLUT_size;
	char String[160];
//	float repeatCount, ccbWidth_div2;
//	int repeatCount;
//	double	ccbWidth_div2;


	TextPtr = strtok(0,NumDelimiters);
	if (!TextPtr) {
		PrintError("Not enough parms for LOAD");
		return;
	}

	TextPtr = strtok(0,NumDelimiters);
	if (!TextPtr) {
		PrintError("Not enough parms for LOAD");
		return;
	}


struct {
		Word temp;
	} PLUT;

struct {
		Word temp;
	} PDAT;

struct {
		char temp;
	} String1;

offset = 0;

//infile = fopen("bigdoor2.cel", "rb");
infile = fopen(TextPtr, "rb");

char begin[2] = "r";
filename = TextPtr;
char *FinName = strcat(begin, filename);
outfile = fopen(FinName, "wb");
//outfile = fopen("rFLAT23", "wb");

fseek(infile,0,SEEK_END);
FileSize = ftell(infile);
rewind(infile);

cout << "Flat FileSize=" << FileSize << "\n";
//cout << "\n";

for (i=0; i<FileSize/4; i++) {
//for (i=0; i<2071; i++) {
		fread(&PLUT.temp,4,1,infile);
		temp = PLUT.temp;
//		fwrite(&temp,4,1,outfile);

		// Читаем PLUT 50 4C 55 54 как 54 55 - 4С 50
		if (temp == 0x54554C50) {

			PLUT_offset = offset;
			cout << "PLUT offset=" << offset << "\n";

			// Какой размер у секции PLUT?
			fread(&PLUT.temp,4,1,infile);
			PLUT_size = Swap4Bytes(PLUT.temp);
			cout << "PLUT size=" << PLUT_size << "\n";
//			fwrite(&PLUT_size,4,1,outfile);

			fread(&PLUT.temp,4,1,infile);
			fread(&PLUT.temp,4,1,infile);
			temp = PLUT.temp;
			fwrite(&temp,4,1,outfile);
			for (j=0; j<PLUT_size/4-4; j++) {
				fread(&PLUT.temp,4,1,infile);
				temp = PLUT.temp;
				if (temp == 0x00000000) {
					break;
					}
				fwrite(&temp,4,1,outfile);
				}
			}
		offset += 4;
	}

fclose(infile);

// Обнуляем offset, чтобы этой же переменной вычислить смещение PDAT.
offset = 0;
infile = fopen(TextPtr, "rb");
//infile = fopen("bigdoor2.cel", "rb");

for (i=0; i<FileSize/4; i++) {
		fread(&PDAT.temp,4,1,infile);
		temp2 = PDAT.temp;

		// Читаем PDAT 50 44 41 54 как 54 41 - 44 50
		if (temp2 == 0x54414450) {

			PDAT_offset = offset;
			cout << "PDAT offset=" << offset << "\n\n";

			fread(&PDAT.temp,4,1,infile); // Просто смещаем позицию на 4 байта.

			for (j=0; j<PLUT_offset/4-PDAT_offset/4-2; j++) {	// Читаем и пишем весь PDAT. -2 потому что заголовок PDAT длиной 8 байт (2 раза) мы уже отсчитали.
					fread(&PDAT.temp,4,1,infile);
					temp2 = PDAT.temp;
					fwrite(&temp2,4,1,outfile);
				}

			}
		offset += 4;
	}
fclose(infile);
fclose(outfile);
}
#endif;




int _tmain(int argc, _TCHAR* argv[])
{
FILE *fp;

countTex = 0;
countFlat = 0;

fp = fopen("LoadTextures.txt","r");    /* Read the ASCII script */
	if (!fp) {
		printf("# Can't open Textures script file %s.\n",argv[1]);    /* Oh oh */
		return 1;
	}

LoadTexScript(fp);

fclose(fp);     /* Close the file */

LineNum = 0;
fp = fopen("LoadFlats.txt","r");    /* Read the ASCII script */
	if (!fp) {
		printf("# Can't open Flats script file %s.\n",argv[1]);    /* Oh oh */
		return 1;
	}

LoadFlatScript(fp);

fclose(fp);     /* Close the file */



	cout << "Press the enter key to continue ...";
    cin.get();

	return 0;
}

