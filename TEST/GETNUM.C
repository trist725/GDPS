#include<stdio.h>
#include<conio.h>

FILE *openfile(char *filename,char *opentype,char language)
{
	FILE*filepoint;
	if((filepoint=fopen(filename,opentype))==NULL)
	{
		if(language=='C')
			printf("�ļ�����! �޷���%s�ļ�!\n��������˳�... ");
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
/*���ļ�*/

void skipnextch(FILE *filepoint,char ch)
{
	while(fgetc(filepoint)!=ch&&!feof(filepoint))
		;
}
/*�ļ�ָ��������һ��ch�ַ�*/

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
	/*����Ϣ�ļ�*/
	for(i=0;i<step;i++)
		skipnextch(msgfp,'[');
	for(i=0;i<item;i++)
		skipnextch(msgfp,'=');
	/*����ָ����Ϣ��ͷ*/
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
	/*����Ϣ�����ַ���*/
	fclose(msgfp);
	return(message);
}
/*����Ϣ�ļ���ȡָ����Ϣ*/

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
/*����ַ����Ƿ�ȫ��������ɲ������ַ�������*/

int strtonum(char *str)
{
	int number,len,i;
	len=checknum(str);
	if(len<=0)
		return(-1);
	number=0;
	for(i=10*(len-1);i>0&&*str!='\0';i/=10)
	{
		number+=((*str-'0')*i);
		str++;
	}
	return(number);
}
/*�ж��ַ����Ƿ�Ϊ������ת������������*/

main()
{
	int min=1,max=18;
	int num,absnum;
	char language='E';
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
	printf("%d",num);
}