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
    struct data*next ; /*下一节点地址*/
}
;
/*地球物理数据结构体定义*/

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
/*打开文件*/

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
/*将计算结果写入计算结果文件*/

main()
{
	FILE*ordatafp;
	struct data*gpd_head,*gpd_p;
	float oa,halfmn,u,i,k,p;
	gpd_head=(struct data*)malloc(sizeof(struct data));
	gpd_head->oa=0;
	gpd_p=gpd_head;
	/*建立链表首节点*/
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
	/*从原始数据文件读入数据并计算p*/
	writeresult(gpd_head);
	/*根据原始数据刷新计算结果文件*/
}
/*初始化*/