#include<stdio.h>
#include<conio.h>
#include<malloc.h>
#define PI 3.14159

struct data
{
    float oa ;/*OA*/
    float halfmn ; /*MN/2*/
    float u ; /*U*/
    float i ; /*I*/
    float k ; /*k*/
    float p ; /*p*/
    struct data*next ; /*��һ�ڵ��ַ*/
}
;
/*�����������ݽṹ�嶨��*/

FILE *openfile(char *filename,char *opentype)
{
   FILE*filepoint;
   if((filepoint=fopen(filename,opentype))==NULL)
   {
        printf("Cann\'t open this file.\n");
        printf("Press anykey to exit...");
        getch();
        exit(0);
   }
   return(filepoint);
}
/*���ļ�*/

void writeresult(struct data *gpd_p)
{
	FILE *resultfp;
	resultfp=openfile("result.txt","w");
	fprintf(resultfp,"OA\tp\t\n");
	while(gpd_p->next!=NULL)
	{
		gpd_p=gpd_p->next;
		fprintf(resultfp,"%.4f\t",gpd_p->oa);
		fprintf(resultfp,"%.4f\n",gpd_p->p);		
	}
}
/*��������д��������ļ�*/

main()
{
	FILE*ordatafp;
	struct data*gpd_head,*gpd_p;
	float oa,halfmn,u,i,k,p;
	gpd_head=(struct data*)malloc(sizeof(struct data));
	gpd_head->oa=0;
	gpd_p=gpd_head;
	/*���������׽ڵ�*/
	ordatafp=openfile("original.txt","r");
	while(fgetc(ordatafp)!='\n'&&!feof(ordatafp))
		;
	while(!feof(ordatafp))
	{
		fscanf(ordatafp,"%f\t%f\t%f\t%f\n",&oa,&halfmn,&u,&i);
		k=PI*(oa+halfmn)*(oa-halfmn)/halfmn;
		p=k*u/i;
		gpd_p->next=(struct data*)malloc(sizeof(struct data));
		gpd_p=gpd_p->next;
		gpd_p->oa=oa;
		gpd_p->halfmn=halfmn;
		gpd_p->u=u;
		gpd_p->i=i;
		gpd_p->k=k;
		gpd_p->p=p;
	}
	gpd_p->next=NULL;
	/*��ԭʼ�����ļ��������ݲ�����p*/
	writeresult(gpd_head);
	/*����ԭʼ����ˢ�¼������ļ�*/
}
/*��ʼ��*/