#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#define NHASH 29989
#define MULT 31

struct node
{
	char word[77];
	int num;
	node *next;
};

int comp(const void *pn1, const void *pn2)
{
	node **p1 = (node **)pn1;                                         //强制类型转换，将qsort默认的无类型指针转换为结构体指针
	node **p2 = (node **)pn2;
	if ((*p1)->num > (*p2)->num || ((*p1)->num == (*p2)->num && strcmp((*p1)->word, (*p2)->word) < 0))
		return -1;
	return 1;
}

void main()
{
	double start, end;
	start = clock();
	node *p, *bin[NHASH], *pile[NHASH];                               //hash表所用指针数组；整理后的指针数组
	char w[77], *q;                                                   //读取字符时存储单词的临时数组
	int i, j, ch, cnt = 0;
	FILE *fp, *f;
	unsigned int h;                                                   //确保h为正	
	for (i = 0; i < NHASH; i++)                                       //指针数组初始化
		bin[i] = NULL;
	i = 0;
	fp = fopen("UnsortedFile.txt", "r");
	f = fopen("SortedFile.txt", "w");
	while ((ch = fgetc(fp)) != EOF)                                   //逐个读取文本中的字符
	{
		if (ch > 64 && ch < 91)                                       //大写字母变小写
			ch = ch + 32;
		if (ch > 96 && ch < 123 || ch == 39 || ch == 45)              //遇到字母或单引号或连接符
		{
			if (ch > 96 && ch < 123)                                  //遇到字母直接存储
				w[i++] = ch;
			else if (ch == 39 && w[i - 1] > 96 && w[i - 1] < 123)     //遇到单引号，如果它的前一个字符是字母，则将其存储
				w[i++] = 39;
		}
		else if (i)                                                   //遇到不想要的字符
		{
			if (w[i - 1] != 39)                                       //将w变为完整的字符串，并消除单词末尾的单引号
				w[i] = 0;
			else
				w[i - 1] = 0;
			h = 0;
			for (q = w; *q; q++)
				h = MULT * h + *q;
			h = h % NHASH;                                            //将字符串映射为小于NHASH的正整数
			for (p = bin[h]; p != NULL; p = p -> next)
				if (strcmp(w, p->word) == 0)                          //单词先前出现过，数目加1即可
				{
					(p->num)++;
					break;
				}
			if (p == NULL)                                            //遇到新单词，初始化
			{
				cnt++;
				p = (node*)malloc(sizeof node);
				strcpy(p->word, w);
				p -> num = 1;
				p->next = bin[h];
				bin[h] = p;
			}
			i = 0;
		}
	}
	fclose(fp);
	for (i = 0, j = 0; i < NHASH; i++)                                //重新排列hash表，便于排序
		for (p = bin[i]; p != NULL; p = p->next)
			pile[j++] = p;
	qsort(pile, cnt, sizeof(pile[0]), comp);
	end = clock();
	fprintf(f, "Time-consuming: %f s\n", (end - start) / 1000);
	fprintf(f, "No.\tword\t\t\t\t\t\t\t\t\t\t\t\t\tnumber\n");
	for (i = 0; i < cnt; i++)
		fprintf(f, "%-5d\t%-100s\t%d\n", i + 1, pile[i]->word, pile[i]->num);
	fclose(f);
	system("SortedFile.txt");
}