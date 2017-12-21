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
	node **p1 = (node **)pn1;                                         //ǿ������ת������qsortĬ�ϵ�������ָ��ת��Ϊ�ṹ��ָ��
	node **p2 = (node **)pn2;
	if ((*p1)->num > (*p2)->num || ((*p1)->num == (*p2)->num && strcmp((*p1)->word, (*p2)->word) < 0))
		return -1;
	return 1;
}

void main()
{
	double start, end;
	start = clock();
	node *p, *bin[NHASH], *pile[NHASH];                               //hash������ָ�����飻������ָ������
	char w[77], *q;                                                   //��ȡ�ַ�ʱ�洢���ʵ���ʱ����
	int i, j, ch, cnt = 0;
	FILE *fp, *f;
	unsigned int h;                                                   //ȷ��hΪ��	
	for (i = 0; i < NHASH; i++)                                       //ָ�������ʼ��
		bin[i] = NULL;
	i = 0;
	fp = fopen("UnsortedFile.txt", "r");
	f = fopen("SortedFile.txt", "w");
	while ((ch = fgetc(fp)) != EOF)                                   //�����ȡ�ı��е��ַ�
	{
		if (ch > 64 && ch < 91)                                       //��д��ĸ��Сд
			ch = ch + 32;
		if (ch > 96 && ch < 123 || ch == 39 || ch == 45)              //������ĸ�����Ż����ӷ�
		{
			if (ch > 96 && ch < 123)                                  //������ĸֱ�Ӵ洢
				w[i++] = ch;
			else if (ch == 39 && w[i - 1] > 96 && w[i - 1] < 123)     //���������ţ��������ǰһ���ַ�����ĸ������洢
				w[i++] = 39;
		}
		else if (i)                                                   //��������Ҫ���ַ�
		{
			if (w[i - 1] != 39)                                       //��w��Ϊ�������ַ���������������ĩβ�ĵ�����
				w[i] = 0;
			else
				w[i - 1] = 0;
			h = 0;
			for (q = w; *q; q++)
				h = MULT * h + *q;
			h = h % NHASH;                                            //���ַ���ӳ��ΪС��NHASH��������
			for (p = bin[h]; p != NULL; p = p -> next)
				if (strcmp(w, p->word) == 0)                          //������ǰ���ֹ�����Ŀ��1����
				{
					(p->num)++;
					break;
				}
			if (p == NULL)                                            //�����µ��ʣ���ʼ��
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
	for (i = 0, j = 0; i < NHASH; i++)                                //��������hash����������
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