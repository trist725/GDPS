#include<stdio.h>
#include<conio.h>

struct config_type
{
    char language ;
    struct
    {
        char oa,halfmn,u,i,k,p ;
    }
    decimal ;
    char kvisible ;
    char appmode,appnum,appconfirm,appbackmode ;
    char delemode,deleconfirm,delebackmode ;
    struct
    {
        struct
        {
            char interval,numbers ;
        }
        x,y ;
    }
    scale ;
    char pointvisible ;
    char coordinate ;
    struct
    {
        char x ;
        char y ;
    }
    relpos ;
    char datavisible ;
    struct
    {
        char axes,scale,curve,point,value ;
    }
    color ;
}
;
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

main()
{
	struct config_type config,*p_cfg=&config;
	FILE *configfp;
	configfp=openfile("config.ini","r",'E');
	p_cfg->language=getcfgch(configfp);
	/*读取Menu配置*/
	skipnextch(configfp,'=');
	p_cfg->decimal.oa=getcfgch(configfp);
	p_cfg->decimal.halfmn=getcfgch(configfp);
	p_cfg->decimal.u=getcfgch(configfp);
	p_cfg->decimal.i=getcfgch(configfp);
	p_cfg->decimal.k=getcfgch(configfp);
	p_cfg->decimal.p=getcfgch(configfp);
	p_cfg->kvisible=getcfgch(configfp);
	/*读取Display配置*/
	p_cfg->appmode=getcfgch(configfp);
	p_cfg->appnum=getcfgnum(configfp);
	p_cfg->appconfirm=getcfgch(configfp);
	p_cfg->appbackmode=getcfgch(configfp);
	/*读取Append配置*/
	p_cfg->delemode=getcfgch(configfp);
	p_cfg->deleconfirm=getcfgch(configfp);
	p_cfg->delebackmode=getcfgch(configfp);
	/*读取Delete配置*/
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
/*读取配置*/