/*
地球物理数据处理系统
(GPDS，Geophysics Data Process System)
2006年7月12日
*/

#include<stdio.h>
#include<conio.h>
#include<string.h>
#include<malloc.h>
#include<dos.h> 
#include<graphics.h>
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
};
/*地球物理数据结构体定义*/

struct config_type
{
	char datafile[32],resultfile[32];
	char language ;
	struct
	{
		char oa,halfmn,u,i,k,p ;
	}
	decimal ;
	char kvisible ;
	char appmode;
	int appnum;
	char appconfirm,appbackmode ;
	char delemode,deleconfirm,delebackmode ;
	struct
	{
		struct
		{
			int interval,numbers ;
		}
		x,y ;
	}
	scale ;
	char pointvisible ;
	char coordinate ;
	struct
	{
		int x ;
		int y ;
	}
	relpos ;
	char datavisible ;
	struct
	{
		int axes,scale,curve,point,value ;
	}
	color ;
};
/*配置数据结构体定义*/

FILE *openfile(char *filename,char *opentype,char language)
{
	FILE*filepoint;
	if((filepoint=fopen(filename,opentype))==NULL)
	{
		if(language=='C')
			printf("文件错误! 无法打开%s文件!\n按任意键退出... ");
		else
		{
			printf("\nFile Error! Cann\'t open file %s.\n",filename);
			printf("Press anykey to exit... ");
		}
		getch();
		exit(0);
	}
	return(filepoint);
}
/*打开文件*/

void skipnextch(FILE *filepoint,char ch)
{
	while(fgetc(filepoint)!=ch&&!feof(filepoint))
		;
}
/*文件指针跳至下一个ch字符*/

char *getmessage(char *message,char language,int step,int item)
{
	FILE*msgfp;
	char *str;
	int i;
	char spflag;
	if(language=='C')
		msgfp=openfile("message\\chinese.msg","r",language);
	else
		msgfp=openfile("message\\english.msg","r",language);
	/*打开消息文件*/
	for(i=0;i<step;i++)
		skipnextch(msgfp,'[');
	for(i=0;i<item;i++)
		skipnextch(msgfp,'=');
	/*跳至指定消息开头*/
	str=message;
	spflag='N';
	while((*str=fgetc(msgfp))!='\n'&&!feof(msgfp))
		if(spflag=='N')
			if(*str!='\\')
				str++;
			else
				spflag='Y';
		else
		{
			if(*str=='n')
				*str='\n';
			else
				*str='\t';
			spflag='N';
			str++;
		}
	*str='\0';
	/*将消息渎至字符串*/
	fclose(msgfp);
	return(message);
}
/*从消息文件读取指定消息*/

struct data *skiptoend(struct data *gpd_p)
{
	while(gpd_p->next!=NULL)
		gpd_p=gpd_p->next;
	return(gpd_p);
}
/*跳至数据尾部*/

struct data *seekdata(struct data *gpd_p,int n)
{
	int i;
	for(i=1;i<n;i++)
		gpd_p=gpd_p->next;
	return(gpd_p);
}
/*跳至第n数据个数据前*/

struct data *appnode(struct data *gpd_p,float indata[4])
{
	gpd_p->next=(struct data*)malloc(sizeof(struct data));
	gpd_p=gpd_p->next;
	gpd_p->oa=indata[0];
	gpd_p->halfmn=indata[1];
	gpd_p->u=indata[2];
	gpd_p->i=indata[3];
	gpd_p->k=(float)
		(PI*(indata[0]+indata[1])*(indata[0]-indata[1])/indata[1]);
	gpd_p->p=gpd_p->k*gpd_p->u/gpd_p->i;
	return(gpd_p);
}
/*根据原始数据计算结果并存至链表尾部*/

void delenode(struct data *gpd_p,int serial)
{
	gpd_p=seekdata(gpd_p,serial);
	gpd_p->next=gpd_p->next->next;
}
/*删除第serial条数据*/

int checknum(char *str)
{
	int len;
	for(len=0;len!=-1&&*str!='\0';str++)
	{
		if(*str>='0'&&*str<='9')
			len++;
		else
			len=-1;
	}
	return(len);
}
/*检查字符串是否全由数字组成并返回字符串长度*/

int strtonum(char *str)
{
	int number,len,i,p;
	len=checknum(str);
	if(len<=0)
		return(-1);
	number=0;
	p=1;
	while(*str!='\0')
		str++;
	for(i=0;i<len;i++)
	{
		number+=((*(--str)-'0')*p);
		p*=10;
	}
	return(number);
}
/*判断字符串是否为整数并转换成整形数据*/

int getnum(int min,int max,char language)
{
	int num,absnum;
	char str[255],message[255];
	absnum=-1;
	while(absnum==-1)
	{
		gets(str);
		if(str[0]=='-')
		{
			absnum=strtonum(&str[1]);
			num=-absnum;
		}
		else
		{
			absnum=strtonum(str);
			num=absnum;
		}
		if(absnum==-1)
			printf(getmessage(message,language,10,2));
		else
			if(num<min||num>max)
			{
				printf(getmessage(message,language,10,3),min,max);
				absnum=-1;
			}
	}
	return(num);
}
/*获取用户输入的整型数据，范围在min和max之间*/

char acclimit(char ch,char *limit)
{
	char sign;
	sign=0;
	while(!sign&&*limit!='\0')
	{
		if(ch==*limit)
			sign=1;
		else 
			if(ch>='a'&&ch<='z'&&(ch-('a'-'A'))==*limit)
				sign=1;
			else 
				if(ch>='A'&&ch<='Z'&&(ch+('a'-'A'))==*limit)
					sign=1;
		limit++;
	}
	return(sign);
}
/*检查ch是否为limit字符串中字符*/

void limitfmt(char *limitstr,char *limit)
{
	while(*limit!='\0')
	{
		*limitstr++='(';
		*limitstr++=*limit++;
		*limitstr++=')';
		*limitstr++='/';
	}
	*(--limitstr)='\0';
}
/*将限制字符转化成可输出字符串*/

char getchoice(char *limit,char language)
{
	char choice ;
	char str[255],limitstr[32],message[255];
	gets(str);
	while(str[1]!='\0'||!acclimit(str[0],limit))
	{
		limitfmt(limitstr,limit);
		printf(getmessage(message,language,10,1),limitstr);
		gets(str);
	}
	choice=str[0];
	if(choice>='a'&&choice<='z')
			choice-=('a'-'A');
	return(choice);
}
/*获取用户一个选择*/

void printhead(char language,char handle)
{
	char  message[255];
	clrscr();
	printf(getmessage(message,language,1,1));
	printf(getmessage(message,language,1,handle));
}
/*输出屏幕头部*/

void printbool(char boolean,char language,int item)
{
	char message[255],str[5];
	getmessage(message,language,3,item);
	if(boolean=='N')
		getmessage(str,language,11,2);
	else
		getmessage(str,language,11,1);
	printf(message,str);
}
/*输出boolean型配置*/

void getclstr(char *str,int color,char language)
{
	char message[255];
	switch(color)
	{
		case 15:
			strcpy(str,getmessage(message,language,16,1));
			break;
		case 4:
			strcpy(str,getmessage(message,language,16,2));
			break;
		case 1:
			strcpy(str,getmessage(message,language,16,3));
			break;
		case 2:
			strcpy(str,getmessage(message,language,16,4));
			break;
		case 14:
			strcpy(str,getmessage(message,language,16,5));
			break;
		case 6:
			strcpy(str,getmessage(message,language,16,6));
	}
}
/*输出颜色字符串*/

void anykeyback(char language)
{
	char message[255];
	printf(getmessage(message,language,5,1));
	getch();
}
/*按任意键返回主菜单*/

char getcfgch(FILE *configfp)
{
	char cfgchar;
	skipnextch(configfp,'=');
	cfgchar=fgetc(configfp);
	return(cfgchar);
}
/*读取一个字符配置*/

int getcfgnum(FILE *configfp)
{
	int cfgnum;
	skipnextch(configfp,'=');
	fscanf(configfp,"%d",&cfgnum);
	return(cfgnum);
}
/*读取一个数字配置*/

void getprtstr(char *prtstr,char decimal,float num)
{
	switch(decimal)
	{
		case '0':
			sprintf(prtstr,"%10.0f",num);
			break;
		case '1':
			sprintf(prtstr,"%10.1f",num);
			break;
		case '2':
			sprintf(prtstr,"%11.2f",num);
			break;
		case '3':
			sprintf(prtstr,"%12.3f",num);
			break;
		case '4':
			sprintf(prtstr,"%13.4f",num);
	}
}
/*根据小数长度生成格式字符串*/

void getconfig(struct config_type *p_cfg)
{
	FILE *configfp;
	configfp=openfile("config.ini","r",'E');
	skipnextch(configfp,'[');
	skipnextch(configfp,'=');
	fscanf(configfp,"%s",p_cfg->datafile);
	skipnextch(configfp,'=');
	fscanf(configfp,"%s",p_cfg->resultfile);
	p_cfg->language=getcfgch(configfp);
	/*读取Main配置*/
	skipnextch(configfp,'[');
	skipnextch(configfp,'=');
	p_cfg->decimal.oa=getcfgch(configfp);
	p_cfg->decimal.halfmn=getcfgch(configfp);
	p_cfg->decimal.u=getcfgch(configfp);
	p_cfg->decimal.i=getcfgch(configfp);
	p_cfg->decimal.k=getcfgch(configfp);
	p_cfg->decimal.p=getcfgch(configfp);
	p_cfg->kvisible=getcfgch(configfp);
	/*读取Display配置*/
	skipnextch(configfp,'[');
	p_cfg->appmode=getcfgch(configfp);
	p_cfg->appnum=getcfgnum(configfp);
	p_cfg->appconfirm=getcfgch(configfp);
	p_cfg->appbackmode=getcfgch(configfp);
	/*读取Append配置*/
	skipnextch(configfp,'[');
	p_cfg->delemode=getcfgch(configfp);
	p_cfg->deleconfirm=getcfgch(configfp);
	p_cfg->delebackmode=getcfgch(configfp);
	/*读取Delete配置*/
	skipnextch(configfp,'[');
	skipnextch(configfp,'=');
	skipnextch(configfp,'=');
	p_cfg->scale.x.interval=getcfgnum(configfp);
	p_cfg->scale.x.numbers=getcfgnum(configfp);
	skipnextch(configfp,'=');
	p_cfg->scale.y.interval=getcfgnum(configfp);
	p_cfg->scale.y.numbers=getcfgnum(configfp);
	p_cfg->pointvisible=getcfgch(configfp);
	p_cfg->coordinate=getcfgch(configfp);
	skipnextch(configfp,'=');
	p_cfg->relpos.x=getcfgnum(configfp);
	p_cfg->relpos.y=getcfgnum(configfp);
	p_cfg->datavisible=getcfgch(configfp);
	skipnextch(configfp,'=');
	p_cfg->color.axes=getcfgnum(configfp);
	p_cfg->color.scale=getcfgnum(configfp);
	p_cfg->color.curve=getcfgnum(configfp);
	p_cfg->color.point=getcfgnum(configfp);
	p_cfg->color.value=getcfgnum(configfp);
	/*读取Diagram配置*/
}
/*从配置文件读取配置*/

void viewoption(struct config_type *p_cfg)
{
	char message[255],str[5][10];
	char oa,halfmn,u,i,k,p;
	int num[4];
	printhead(p_cfg->language,7);
	getmessage(message,p_cfg->language,3,1);
	printf(message,p_cfg->datafile);
	getmessage(message,p_cfg->language,3,2);
	printf(message,p_cfg->resultfile);
	getmessage(message,p_cfg->language,3,3);
	if(p_cfg->language=='C')
		getmessage(str[0],p_cfg->language,12,2);
	else
		getmessage(str[0],p_cfg->language,12,1);
	printf(message,str[0]);
	/*显示Main配置*/
	getmessage(message,p_cfg->language,3,4);
	oa=p_cfg->decimal.oa;
	halfmn=p_cfg->decimal.halfmn;
	u=p_cfg->decimal.u;
	i=p_cfg->decimal.i;
	k=p_cfg->decimal.k;
	p=p_cfg->decimal.p;
	printf(message,oa,halfmn,u,i,k,p);
	printbool(p_cfg->kvisible,p_cfg->language,5);
	/*显示Display配置*/
	getmessage(message,p_cfg->language,3,6);
	if(p_cfg->appmode=='A')
		getmessage(str[0],p_cfg->language,13,3);
	else
		if(p_cfg->appmode=='N')
			getmessage(str[0],p_cfg->language,13,2);
		else
			getmessage(str[0],p_cfg->language,13,1);
	printf(message,str[0]);
	getmessage(message,p_cfg->language,3,7);
	printf(message,p_cfg->appnum);
	printbool(p_cfg->appconfirm,p_cfg->language,8);
	getmessage(message,p_cfg->language,3,9);
	if(p_cfg->appbackmode=='D')
		getmessage(str[0],p_cfg->language,15,3);
	else
		if(p_cfg->appbackmode=='W')
			getmessage(str[0],p_cfg->language,15,2);
		else
			getmessage(str[0],p_cfg->language,15,1);
	printf(message,str[0]);
	/*显示Append配置*/
	getmessage(message,p_cfg->language,3,10);
	if(p_cfg->delemode=='A')
		getmessage(str[0],p_cfg->language,14,3);
	else
		if(p_cfg->delemode=='Z')
			getmessage(str[0],p_cfg->language,14,2);
		else
			getmessage(str[0],p_cfg->language,14,1);
	printf(message,str[0]);
	printbool(p_cfg->deleconfirm,p_cfg->language,11);
	getmessage(message,p_cfg->language,3,12);
	if(p_cfg->delebackmode=='D')
		getmessage(str[0],p_cfg->language,15,3);
	else
		if(p_cfg->delebackmode=='W')
			getmessage(str[0],p_cfg->language,15,2);
		else
			getmessage(str[0],p_cfg->language,15,1);
	printf(message,str[0]);
	/*显示Delete配置*/
	getmessage(message,p_cfg->language,3,13);	
	num[0]=p_cfg->scale.x.interval;
	num[1]=p_cfg->scale.x.numbers;
	num[2]=p_cfg->scale.y.interval;
	num[3]=p_cfg->scale.y.numbers;
	printf(message,num[0],num[1],num[2],num[3]);
	printbool(p_cfg->pointvisible,p_cfg->language,14);
	printbool(p_cfg->coordinate,p_cfg->language,15);
	getmessage(message,p_cfg->language,3,16);	
	num[0]=p_cfg->relpos.x;
	num[1]=p_cfg->relpos.y;
	printf(message,num[0],num[1]);
	printbool(p_cfg->datavisible,p_cfg->language,17);
	getmessage(message,p_cfg->language,3,18);
	getclstr(str[0],p_cfg->color.axes,p_cfg->language);
	getclstr(str[1],p_cfg->color.scale,p_cfg->language);
	getclstr(str[2],p_cfg->color.curve,p_cfg->language);
	getclstr(str[3],p_cfg->color.point,p_cfg->language);
	getclstr(str[4],p_cfg->color.value,p_cfg->language);
	printf(message,str[0],str[1],str[2],str[3],str[4]);
	/*显示Diagram配置*/
	/*
	printf(getmessage(message,p_cfg->language,3,19));
	*/
	/*由于修改配置函数未完成，输出暂时屏蔽*/
	/*显示修改选项*/	
}
/*部分功能暂时被屏蔽*/
/*显示配置*/

void sortdata(struct data *gpd_head)
{
	struct data *gpd_p,*ptemp,*ptemp1,*ptemp2;
	gpd_p=gpd_head;
	while(gpd_p->next!=NULL)
	{
		if(gpd_p->oa<gpd_p->next->oa)
			gpd_p=gpd_p->next;
		else
		{
			ptemp1=gpd_p->next;
			if(ptemp1->next!=NULL)
				gpd_p->next=ptemp1->next;
			else
				gpd_p->next=NULL;
			ptemp2=gpd_head;
			while(ptemp2->next!=NULL&&ptemp1->oa>ptemp2->next->oa)
				ptemp2=ptemp2->next;
			ptemp=ptemp2->next;
			ptemp2->next=ptemp1;
			ptemp1->next=ptemp;	
		}
	}
}
/*将数据按升序排列*/

void writeordata(char *datafile,struct data *gpd_p,char language)
{
	FILE *ordatafp;
	ordatafp=openfile(datafile,"w",language);
	fprintf(ordatafp,"\tOA\t\tMN/2\t\tU\t\tI");
	while(gpd_p->next!=NULL)
	{
		gpd_p=gpd_p->next;
		fprintf(ordatafp,"\n%13.4f\t",gpd_p->oa);
		fprintf(ordatafp,"%13.4f\t",gpd_p->halfmn);
		fprintf(ordatafp,"%13.4f\t",gpd_p->u);
		fprintf(ordatafp,"%13.4f",gpd_p->i);
	}
	fclose(ordatafp);
}
/*将原始数据写入原始数据文件*/

void writeresult(char *resultfile,struct data *gpd_p,char language)
{
	FILE *resultfp;
	resultfp=openfile(resultfile,"w",language);
	fprintf(resultfp,"\tOA\t\tp");
	while(gpd_p->next!=NULL)
	{
		gpd_p=gpd_p->next;
		fprintf(resultfp,"\n%13.4f\t",gpd_p->oa);
		fprintf(resultfp,"%13.4f",gpd_p->p);
	}
	fclose(resultfp);
}
/*将计算结果写入计算结果文件*/

struct data *initialize(struct config_type *p_cfg)
{
	FILE*ordatafp;
	struct data*gpd_head,*gpd_p; 
	int i;
	float temp[4];
	gpd_head=(struct data*)malloc(sizeof(struct data));
	gpd_head->oa=-1;
	gpd_p=gpd_head;
	/*建立链表首节点*/
	ordatafp=openfile(p_cfg->datafile,"r",p_cfg->language);
	skipnextch(ordatafp,'\n');
	while(!feof(ordatafp))
	{
		for(i=0;i<4;i++)
			fscanf(ordatafp,"%f",&temp[i]);
		fscanf(ordatafp,"\n");
		gpd_p=appnode(gpd_p,temp);
	}	
	gpd_p->next=NULL;
	fclose(ordatafp);
	/*从原始数据文件读入数据并计算p*/
	sortdata(gpd_head);
	writeordata(p_cfg->datafile,gpd_head,p_cfg->language);
	/*将原始数据排序并存入原始数据文件*/
	writeresult(p_cfg->resultfile,gpd_head,p_cfg->language);
	/*根据原始数据刷新计算结果文件*/
	return(gpd_head);
}
/*读取数据文件进行初始化*/

char mainmenu(char language)
{
	char message[255];
	char choice ;
	printhead(language,2);
	printf(getmessage(message,language,2,1));
	printf(getmessage(message,language,4,1));
	choice=getchoice("123456",language);
	return(choice);
}
/*显示主菜单，获取用户选择*/

int display(struct data *gpd_p, struct config_type *p_cfg,int dptype)
{
	char message[255],prtstr[6][16];
	int nums,i;
	if(dptype=='A')
		printhead(p_cfg->language,3);
	printf("\t OA\t");
	if(dptype!='R')
	{
		printf("   MN/2\t       U\t  I\t");
		if(p_cfg->kvisible=='Y')
			printf("    k\t   ");
	}
	printf("    p\n");
	/*输出表头*/
	nums=0;
	while(gpd_p->next!=NULL)
	{
		nums++;
		printf("%3d.",nums);
		gpd_p=gpd_p->next;
		getprtstr(prtstr[0],p_cfg->decimal.oa,gpd_p->oa);
		printf(&prtstr[0][2]);
		if(dptype!='R')
		{
			getprtstr(prtstr[1],p_cfg->decimal.halfmn,gpd_p->halfmn);
			getprtstr(prtstr[2],p_cfg->decimal.u,gpd_p->u);
			getprtstr(prtstr[3],p_cfg->decimal.i,gpd_p->i);
			for(i=1;i<4;i++)
				printf(prtstr[i]);		
			if(p_cfg->kvisible=='Y')
			{
				getprtstr(prtstr[4],p_cfg->decimal.k,gpd_p->k);
				printf(prtstr[4]);
			}
		}
		getprtstr(prtstr[5],p_cfg->decimal.p,gpd_p->p);
		printf(prtstr[5]);
		printf("\n");
	}
	if(dptype!='R')
		printf(getmessage(message,p_cfg->language,6,1),nums);
	/*生成输出格式字符串并输出数据*/
	if(dptype=='A')
		anykeyback(p_cfg->language);
	return(nums);
}
/*根据配置及浏览种类显示数据并返回数据条数*/

void append(struct data *gpd_head,struct config_type *p_cfg)
{
	struct data*gpd_p,*apphead;
	char  message[255];
	int num,i,j;
	char appoper,negflag;
	float temp[4];
	if(p_cfg->appmode=='A') 
	{
		printf(getmessage(message,p_cfg->language,7,1));
		num=getnum(1,10,p_cfg->language);			
	}		
	else
		if(p_cfg->appmode=='N')
			num=0;
		else
			num=p_cfg->appnum;
	/*读取添加方式配置*/
	appoper='F';
	apphead=skiptoend(gpd_head);	
	do
	{
		negflag='N';
		i=0;
		gpd_p=apphead;
		printhead(p_cfg->language,4);
		if(appoper=='C')
			printf(getmessage(message,p_cfg->language,7,6));
		if(appoper=='A')		
			printf(getmessage(message,p_cfg->language,7,7));
		if(num>0)
			printf(getmessage(message,p_cfg->language,7,2),num);
		else
			printf(getmessage(message,p_cfg->language,7,3));
		printf("OA MN/2 U I\n");
		/*输出提示*/
		while(i<num||num==0)
		{
			for(j=0;j<4;j++)
			{
				if(negflag=='N')
				{
					scanf("%f",&temp[j]);
					if(temp[j]<0)
						negflag='Y';
				}
			}	
			if(negflag=='N')
				gpd_p=appnode(gpd_p,temp);
			else
				printf(getmessage(message,p_cfg->language,10,4));
			i++;
		}
		gpd_p->next=NULL;
		/*输入数据并存至链表尾部*/
		if(p_cfg->appconfirm=='Y')
		{
			printf(getmessage(message,p_cfg->language,7,4),i);
			display(apphead,p_cfg,'T');
		}
		printf(getmessage(message,p_cfg->language,7,5));
		scanf("\n");
		appoper=getchoice("YNCA",p_cfg->language);
		/*显示输入,获取用户确认*/
		if(appoper=='Y'||appoper=='C')
		{
			sortdata(gpd_head);
			writeordata(p_cfg->datafile,gpd_head,p_cfg->language);
			writeresult(p_cfg->resultfile,gpd_head,p_cfg->language);
			apphead=gpd_p;
		}
		/*排序并存至文件*/
		else
			apphead->next=NULL;
	}while(appoper=='C'||appoper=='A');
	if(appoper=='Y')
		printf(getmessage(message,p_cfg->language,7,6));
	if(appoper=='N')		
		printf(getmessage(message,p_cfg->language,7,7));
	if(p_cfg->appbackmode=='T')
		anykeyback(p_cfg->language);
	else 
		if(p_cfg->appbackmode=='W')
			delay(1000);
	/*退回主菜单*/
}
/*与用户对话以添加数据*/

void deledata(struct data *gpd_head,struct config_type *p_cfg)
{
	struct data*gpd_temp,*gpd_dele,*gpd_dh;
	char  message[255];
	int sernum,deleser[20],nums,i,n;
	char delestate,deleoper;
	if(p_cfg->delemode=='A') 
	{
		printf(getmessage(message,p_cfg->language,8,1));
		delestate=getchoice("OZ",p_cfg->language);
	}		
	else
		if(p_cfg->delemode=='Z')
			delestate='Z';
		else
			delestate='O';
	/*读取删除方式配置*/
	deleoper='F';
	gpd_dh=(struct data*)malloc(sizeof(struct data));
	do
	{
		printhead(p_cfg->language,5);
		if(deleoper=='C')
			printf(getmessage(message,p_cfg->language,8,6));
		if(deleoper=='A')		
			printf(getmessage(message,p_cfg->language,8,7));
		nums=display(gpd_head,p_cfg,'T');
		if(delestate=='O')
			printf(getmessage(message,p_cfg->language,8,2));
		else
			printf(getmessage(message,p_cfg->language,8,3));
		/*输出提示*/
		gpd_dele=gpd_dh;
		n=0;
		sernum=1;
		while(sernum>0)
		{
			sernum=getnum(0,nums,p_cfg->language);
			if(sernum>0)
			{
				deleser[n]=sernum;
				gpd_dele->next
					=(struct data*)malloc(sizeof(struct data));
				gpd_dele=gpd_dele->next;
				*gpd_dele=*seekdata(gpd_head,sernum)->next;
				n++;
			}
			if(delestate=='O')
				sernum=-1;
		}
		gpd_dele->next=NULL;
		/*获取删除项*/
		printf(getmessage(message,p_cfg->language,8,4));
		display(gpd_dh,p_cfg,'T');
		printf(getmessage(message,p_cfg->language,8,5));
		scanf("\n");
		deleoper=getchoice("YNCA",p_cfg->language);
		/*显示删除项,获取用户确认*/
		if(deleoper=='Y'||deleoper=='C')
		{
			for(i=0;i<n;i++)
				delenode(gpd_head,deleser[i]);
			writeordata(p_cfg->datafile,gpd_head,p_cfg->language);
			writeresult(p_cfg->resultfile,gpd_head,p_cfg->language);
		}
		/*删除并存至文件*/
	}while(deleoper=='C'||deleoper=='A');
	if(deleoper=='Y')
		printf(getmessage(message,p_cfg->language,8,6));
	if(deleoper=='N')		
		printf(getmessage(message,p_cfg->language,8,7));
	if(p_cfg->delebackmode=='A')
		anykeyback(p_cfg->language);
	else 
		if(p_cfg->delebackmode=='W')
			delay(1000);
	/*退回主菜单*/		
}
/*与用户对话以删除数据*/

void drawcurve(struct data *gpd_p, struct config_type *p_cfg)
{
	int width;
	float xpixpval,ypixpval;	
	int x1,x2,y1,y2;
	char str[16];
	if(p_cfg->datavisible=='Y')
		width=340;
	else
		width=500;
	xpixpval=(float)width/
		(p_cfg->scale.x.interval*p_cfg->scale.x.numbers);
	ypixpval=(float)320/
		(p_cfg->scale.y.interval*p_cfg->scale.y.numbers);
	if(gpd_p->next!=NULL)
	{
		gpd_p=gpd_p->next;
		x1=(int)(50+gpd_p->oa*xpixpval);
		y1=(int)(350-gpd_p->p*ypixpval);
		if(p_cfg->pointvisible!='N')
		{
			setcolor(p_cfg->color.point);
			circle(x1,y1,1);
		}
		/*描点*/
		if(p_cfg->coordinate=='Y')
		{
			setcolor(p_cfg->color.value);
			sprintf(str,"(%.1f,%.1f)",gpd_p->oa,gpd_p->p);
			outtextxy(x1+p_cfg->relpos.x,y1+p_cfg->relpos.y,str);
		}
		/*输出坐标值*/
	}
	/*读取并画出第一点*/
	while(gpd_p->next!=NULL)
	{	
		gpd_p=gpd_p->next;
		x2=(int)(50+gpd_p->oa*xpixpval);
		y2=(int)(350-gpd_p->p*ypixpval);
		setcolor(p_cfg->color.curve);
		line(x1,y1,x2,y2);
		/*连线*/
		if(p_cfg->pointvisible!='N')
		{
			setcolor(p_cfg->color.point);
			circle(x2,y2,1);
		}
		/*描点*/
		if(p_cfg->coordinate=='Y')
		{
			setcolor(p_cfg->color.value);
			sprintf(str,"(%.1f,%.1f)",gpd_p->oa,gpd_p->p);
			outtextxy(x2+p_cfg->relpos.x,y2+p_cfg->relpos.y,str);
		}
		/*输出坐标值*/
		x1=x2;
		y1=y2;
	}
	/*读取其它点并连线*/
}
/*绘制图像曲线*/

void drawaxes(struct config_type *p_cfg)
{	
	int width,interval,numbers;
	int i,n;
	float pixpitv;
	char str[6];
	if(p_cfg->datavisible=='Y')
		width=340;
	else
		width=500;
	setcolor(p_cfg->color.axes);
	line(50,350,width+70,350);
	line(width+70,350,width+65,345);
	line(width+70,350,width+65,355);
	/*绘制X轴*/
	line(50,350,50,15);
	line(50,15,55,20);
	line(50,15,45,20);
	/*绘制Y轴*/
	setcolor(p_cfg->color.scale);
	outtextxy(40,355,"0");
	interval=p_cfg->scale.x.interval;
	numbers=p_cfg->scale.x.numbers;
	pixpitv=(float)width/numbers;
	for(i=1;i<=numbers;i++)
	{
		line((int)(i*pixpitv)+50,350,(int)(i*pixpitv)+50,345);
		sprintf(str,"%d",i*interval);
		outtextxy((int)(i*pixpitv)+45,355,str);
	}
	outtextxy(width+65,360,"OA");
	/*标X轴刻度*/	
	interval=p_cfg->scale.y.interval;
	numbers=p_cfg->scale.y.numbers;
	pixpitv=(float)320/numbers;
	for(i=1;i<=numbers;i++)
	{
		line(50,350-(int)(i*pixpitv),55,350-(int)(i*pixpitv));
		sprintf(str,"%4d",i*interval);
		outtextxy(15,348-(int)(i*pixpitv),str);
	}
	outtextxy(35,12,"p");
	/*标Y轴刻度*/
}
/*绘制坐标轴及刻度*/

void diagram(struct data *gpd_head, struct config_type *p_cfg)
{
	int driver=DETECT,mode ;
	printhead(p_cfg->language,6);
	registerbgidriver(EGAVGA_driver);
	initgraph(&driver,&mode,"");
	/*图形界面初始化*/
	if(p_cfg->datavisible=='Y')
		setviewport(180,60,620,460,1);
	else
		setviewport(20,60,620,460,1);
	drawcurve(gpd_head,p_cfg);
	drawaxes(p_cfg);
	if(p_cfg->datavisible=='Y')
	{
		printf("\n\n\n\n\n");
		display(gpd_head,p_cfg,'R');
	}
	getch();
	closegraph();
}
/*显示函数图像*/

void option(struct config_type *p_cfg)
{
	int itemchoice;
	printhead(p_cfg->language,6);
	do
	{
		viewoption(p_cfg);
		/*显示配置*/
		/*以下功能暂时未完成*/
		/*获取用户要修改的选项*/
		/*获取用户的修改*/
		/*根据用户的选择进行修改*/
		itemchoice=21;/*由于函数未完成，临时赋值*/
	}while(itemchoice<20);
	/*当选择不为退出时显示修改菜单，等待用户修改*/
	anykeyback(p_cfg->language);
	/*由于函数未完成，浏览配置后按任意键退出*/
}
/*修改功能未完成,当前只能显示配置*/
/*与用户对话以更改配置*/

void prequit(struct config_type *p_cfg)
{
	char message[255];
	printf(getmessage(message,p_cfg->language,5,2));
	delay(1000);
	clrscr();
}
/*输出结束信息，准备退出*/

void process(char choice,struct data *gdp_head,struct config_type *p_cfg)
{
	switch(choice)
	{
		case '1' :
		display(gdp_head,p_cfg,'A');
		break ;
		/*浏览*/
		case '2' :
		append(gdp_head,p_cfg);
		break ;
		/*添加*/
		case '3' :
		deledata(gdp_head,p_cfg);
		break ;
		/*删除*/
		case '4' :
		diagram(gdp_head,p_cfg);
		break ;
		/*图表*/
		case '5' :
		option(p_cfg);
		break ;
		/*设置*/
		case '6' :
		prequit(p_cfg);
		/*退出*/
	}
}
/*处理用户选择的操作*/

int main()
{
	struct data *gpd_head;
	struct config_type config,*p_cfg=&config;
	char choice;
	getconfig(p_cfg);
	gpd_head=initialize(p_cfg);
	/*从文件读取数据*/
	do
	{
		choice=mainmenu(p_cfg->language);
		/*显示主菜单，选择要进行的操作*/
		process(choice,gpd_head,p_cfg);
		/*处理用户选择的操作*/
	}
	while(choice!='6');
	/*当选择'6'时退出*/
	return(0);
}
/*主函数*/