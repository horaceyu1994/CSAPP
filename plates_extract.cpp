#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <math.h>

#define BM 19778                                         // The ASCII code of BM
#define ROW_C 25                                         // The structural element of closing operton
#define COLUMN_C 55
#define ROW_O 11                                         // The structural element of opening operton
#define COLUMN_O 21

unsigned int OffSet = 0;                                 // OffSet from Header part to Data Part
long width, height;
int r[1500][1500], g[1500][1500], b[1500][1500];
int gry[1500][1500], edg[1500][1500], flt[1500][1500], erd[1500][1500];
int cls1[1500][1500], cls2[1500][1500], opn1[1500][1500], opn2[1500][1500];
int lab[5000][4];

int comp(const void *p1, const void *p2)                 // Used for qsort in main function
{
	return *(int *)p1 - *(int *)p2;
}

void main()
{
	double start, end;
	unsigned char *fp_temp;                              // Save the bitgry header
	FILE *fpbmp, *fpout;
	char filename[30];
	printf("Please input the file name (Filename extension is needed):\n");
	gets(filename);
	fpbmp = fopen(filename, "rb");
	if (fpbmp == NULL)
		printf("Open bmp failed!\n");
	start = clock();
	fpout = fopen("out.bmp", "wb+");
	if (fpout == NULL)
		printf("Open out.bmp failed!\n");
	unsigned short bfType = 0;
	fseek(fpbmp, 0L, SEEK_SET);                          // SEEK_SET is the initial position of the file
	fread(&bfType, sizeof(char), 2, fpbmp);
	if (BM != bfType)
		printf("This file is not a bmp file!\n");
		fseek(fpbmp, 10L, SEEK_SET);
	fread(&OffSet, sizeof(char), 4, fpbmp);
		fseek(fpbmp, 18L, SEEK_SET);
	fread(&width, sizeof(char), 4, fpbmp);
	fseek(fpbmp, 22L, SEEK_SET);
	fread(&height, sizeof(char), 4, fpbmp);
	printf("The width of the image is %ld\n", width);
	printf("The height of the image is %ld\n", height);
	fseek(fpbmp, 0L, SEEK_SET);
	fseek(fpout, 0L, SEEK_SET);
	fp_temp = (unsigned char*)malloc(OffSet);
	fread(fp_temp, 1, OffSet, fpbmp);
	fwrite(fp_temp, 1, OffSet, fpout);                   // The bitgry header of out.bmp is equal to that of source file
	
	/* Save the data of image */
	int i, j, stride;
	unsigned char* pix = NULL;
	fseek(fpbmp, OffSet, SEEK_SET);
	stride = (24 * width + 31) / 8;                      // The number of bytes per line, which has to be the multiples of 4. Supply with 0s if not enough.
	stride = stride / 4 * 4;
	pix = (unsigned char*)malloc(stride);
	for (j = 0; j < height; j++)
	{
		fread(pix, 1, stride, fpbmp);
		for (i = 0; i < width; i++)
		{
			r[height - 1 - j][i] = pix[i * 3 + 2];
			g[height - 1 - j][i] = pix[i * 3 + 1];
			b[height - 1 - j][i] = pix[i * 3];
		}
	}

	/* Obtain the gray-scale image */
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
			gry[i][j] = 0.299 * r[i][j] + 0.587 * g[i][j] + 0.114 * b[i][j];

	/* Median fltering	*/
	int tmp[9], m, n, k;
	for (i = 1; i < height - 1; i++)
		for (j = 1; j < width - 1; j++)
		{
			k = 0;
			for (m = 0; m < 3; m++)
				for (n = 0; n < 3; n++)
					tmp[k++] = gry[i - 1 + m][j - 1 + n];
			qsort(tmp, 9, sizeof(tmp[0]), comp);
			flt[i][j] = tmp[4];
		}

	/* Extracting edge with the Prewitt operator */
	int max = 0;
	for (i = 2; i < height - 2; i++)
		for (j = 2; j < width - 2; j++)
		{
			edg[i][j] = abs(gry[i - 1][j + 1] - gry[i - 1][j - 1] + gry[i][j + 1] - gry[i][j - 1] + gry[i + 1][j + 1] - gry[i + 1][j - 1]) + abs(gry[i + 1][j - 1] - gry[i - 1][j - 1] + gry[i + 1][j] - gry[i - 1][j] + gry[i + 1][j + 1] - gry[i - 1][j + 1]);
			if (edg[i][j] > max)
				max = edg[i][j];
		}

	/* Image binaryzation */
	max *= 0.4;
	for (i = 2; i < (height - 2); i++)
		for (j = 2; j < (width - 2); j++)
			if (edg[i][j] > max)
				edg[i][j] = 1;
			else
				edg[i][j] = 0;
	
	/* Eroding the image with [1; 1; 1; 1; 1] */
	for (i = 4; i < height - 4; i++)
		for (j = 2; j < width - 2; j++)
			if (edg[i][j] == 1 && edg[i - 1][j] == 1 && edg[i + 1][j] == 1 && edg[i - 2][j] == 1 && edg[i + 2][j] == 1)
				erd[i][j] = 1;
			else
				erd[i][j] = 0;
		
	/* Closing operton */
	for (i = 4 + ROW_C / 2; i < height - 4 - ROW_C / 2; i++)
		for (j = 2 + COLUMN_C / 2; j < width - 2 - COLUMN_C / 2; j++)
			if (erd[i][j] == 1)
				for (m = 0; m < ROW_C; m++)
					for (n = 0; n < COLUMN_C; n++)
						cls1[i - ROW_C / 2 + m][j - COLUMN_C / 2 + n] = 1;
	for (i = 4 + ROW_C / 2; i < height - 4 - ROW_C / 2; i++)
		for (j = 2 + COLUMN_C / 2; j < width - 2 - COLUMN_C / 2; j++)
			if (cls1[i][j] == 1)
			{
				cls2[i][j] = 1;
				for (m = 0; m < ROW_C; m++)
					for (n = 0; n < COLUMN_C; n++)
						if (cls1[i - ROW_C / 2 + m][j - COLUMN_C / 2 + n] == 0)
						{
							cls2[i][j] = 0;
							break;
						}
			}
	
	/* Opening operton */
	for (i = 4 + ROW_C / 2; i < height - 4 - ROW_C / 2; i++)
		for (j = 2 + COLUMN_C / 2; j < width - 2 - COLUMN_C / 2; j++)
			if (cls2[i][j] == 1)
			{
				opn1[i][j] = 1;
				for (m = 0; m < ROW_O; m++)
					for (n = 0; n < COLUMN_O; n++)
						if (cls2[i - 12 + m][j - 20 + n] == 0)
						{
							opn1[i][j] = 0;
							break;
						}
			}
	for (i = 4 + ROW_C / 2; i < height - 4 - ROW_C / 2; i++)
		for (j = 2 + COLUMN_C / 2; j < width - 2 - COLUMN_C / 2; j++)
			if (opn1[i][j] == 1)
				for (m = 0; m < ROW_O; m++)
					for (n = 0; n < COLUMN_O; n++)
						opn2[i - ROW_O / 2 + m][j - COLUMN_O / 2 + n] = 1;

	/* Connected domains labeling */
	int cnt = 0, tag = 0, flg, tm;
	for (i = 4 + ROW_C / 2; i < height - 4 - ROW_C / 2; i++)
		for (j = 2 + COLUMN_C / 2; j < width - 2 - COLUMN_C / 2; j++)
			if (opn2[i][j])
			{
				for (k = 1; opn2[i][j + k]; k++);
				lab[cnt][0] = i;
				lab[cnt][1] = j;
				lab[cnt][2] = j + k - 1;
				for (m = 0; lab[m][0] != i - 1; m++)
					if (m == cnt)
						break;
				for (flg = -1; lab[m][0] == i - 1; m++)
					if (j + k - 1 > lab[m][1] - 2 && j < lab[m][2] + 2)
					{
						flg++;
						if (!flg)
							lab[cnt][3] = lab[m][3];
						else
						{
							tm = lab[m][3];
							for (n = 0; n < cnt; n++)
								if (lab[n][3] == tm)
									lab[n][3] = lab[cnt][3];
						}
					}
				if (flg == -1)
					lab[cnt][3] = tag++;
				cnt++;
				j += k;
			}

	/* Choosing the suitable domains */
	int zone[100][4];                                   // Save the location parameters of the domains
	for (i = 0; i < tag; i++)
	{
		zone[i][0] = 3000;
		zone[i][1] = -1;
		zone[i][2] = 3000;
		zone[i][3] = -1;
	}
	for (i = 0; i < cnt; i++)
	{
		m = lab[i][3];
		if (lab[i][0] < zone[m][0])
			zone[m][0] = lab[i][0];
		if (lab[i][0] > zone[m][1])
			zone[m][1] = lab[i][0];
		if (lab[i][1] < zone[m][2])
			zone[m][2] = lab[i][1];
		if (lab[i][2] > zone[m][3])
			zone[m][3] = lab[i][2];
	}
	double wdh, hgt, s, rto;
	cnt = 0;
	for (i = 0; i < tag; i++)
	{
		if (zone[i][1] == -1)
			continue;
		wdh = zone[i][3] - zone[i][2];
		hgt = zone[i][1] - zone[i][0];
		s = wdh * hgt;
		rto = wdh / hgt;
		if (s <= 1000 || rto <= 2.5)
			zone[i][1] = -1;
		else if (s < 10000)
		{
			zone[i][0] -= hgt / 3;
			zone[i][2] -= wdh / 6;
		}
		else if (rto > 5)
		{
			zone[tag + cnt][0] = zone[i][0];
			zone[tag + cnt][1] = zone[i][1];
			zone[tag + cnt][3] = zone[i][3];
			zone[tag + cnt][2] = zone[i][2] + wdh / 2 + 10;
			zone[i][3] -= wdh / 2 + 10;
			cnt++;
		}
	}
	
	/* Labeling the number plates with red rectangular frames */
	for (i = 0; i < tag + cnt; i++)
		if (zone[i][1] != -1)
		{
			for (m = zone[i][0] - 3; m < zone[i][0]; m++)
				for (n = zone[i][2] - 3; n < zone[i][3] + 4; n++)
				{
					r[m][n] = 255;
					g[m][n] = 0;
					b[m][n] = 0;
				}
			for (m = zone[i][1] + 1; m < zone[i][1] + 4; m++)
				for (n = zone[i][2] - 3; n < zone[i][3] + 4; n++)
				{
					r[m][n] = 255;
					g[m][n] = 0;
					b[m][n] = 0;
				}
			for (m = zone[i][0]; m <= zone[i][1]; m++)
				for (n = zone[i][2] - 3; n < zone[i][2]; n++)
				{
					r[m][n] = 255;
					g[m][n] = 0;
					b[m][n] = 0;
				}
			for (m = zone[i][0]; m <= zone[i][1]; m++)
				for (n = zone[i][3] + 1; n < zone[i][3] + 4; n++)
				{
					r[m][n] = 255;
					g[m][n] = 0;
					b[m][n] = 0;
				}
		}

	/* Outputing the data to out.bmp */
	fseek(fpout, OffSet, SEEK_SET);
	for (j = 0; j < height; j++)
	{
		for (i = 0; i < width; i++)
		{
			pix[i * 3 + 2] = r[height - 1 - j][i];
			pix[i * 3 + 1] = g[height - 1 - j][i];
			pix[i * 3] = b[height - 1 - j][i];
		}
		fwrite(pix, 1, stride, fpout);
	}
	fclose(fpbmp);
	fclose(fpout);
	printf("Connected domains and their location parameters:\n");
	printf("No.    up     down   left   right\n");
	for (i = 0, j = 0; i < tag + cnt; i++)
		if (zone[i][1] != -1)
			printf("%-7d%-7d%-7d%-7d%-7d\n", ++j, zone[i][0], zone[i][1], zone[i][2], zone[i][3]);
	end = clock();
	printf("Time consuming: %f s\n", (end - start) / 1000);
	system("out.bmp");
	system("pause");
}