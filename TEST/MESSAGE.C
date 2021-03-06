#include<stdio.h>
#include<conio.h>
#include<string.h>
#include<malloc.h>
#include<dos.h>

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

char*getmessage(char*message,char language,int step,int item)
{
	FILE*msgfp;
	char *str;
	int i;
	char spflag;
	if(language=='C')
		msgfp=openfile("message\\chinese.msg","r",language);
	else
		msgfp=openfile("message\\english.msg","r",language);
	/*打开消息文�
	�*/
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

main()
{
	char message[255];
	printf(getmessage(message,'C',7,5));
}