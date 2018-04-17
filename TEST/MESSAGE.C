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
			printf("ÎÄ¼ş´íÎó! ÎŞ·¨´ò¿ª%sÎÄ¼ş!\n°´ÈÎÒâ¼üÍË³ö... ");
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
/*´ò¿ªÎÄ¼ş*/

void skipnextch(FILE *filepoint,char ch)
{
	while(fgetc(filepoint)!=ch&&!feof(filepoint))
		;
}
/*ÎÄ¼şÖ¸ÕëÌøÖÁÏÂÒ»¸öch×Ö·û*/

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
	/*´ò¿ªÏûÏ¢ÎÄ¼
	ş*/
	for(i=0;i<step;i++)
		skipnextch(msgfp,'[');
	for(i=0;i<item;i++)
		skipnextch(msgfp,'=');
	/*ÌøÖÁÖ¸¶¨ÏûÏ¢¿ªÍ·*/
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
	/*½«ÏûÏ¢äÂÖÁ×Ö·û´®*/
	fclose(msgfp);
	return(message);
}
/*´ÓÏûÏ¢ÎÄ¼ş¶ÁÈ¡Ö¸¶¨ÏûÏ¢*/

main()
{
	char message[255];
	printf(getmessage(message,'C',7,5));
}