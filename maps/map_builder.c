#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Baut eine JSON-Map aus der Ausgabe, die sie erzeugen würde, auf.
 * Es werden nur die Tile-Typen unterschieden, Items und Spawns müssen
 * von Hand hinzugefügt werden.
 * Die Berechnung der Breite funktioniert irgendwie nicht.
 * Nimmt eine Datei als Kommandozeilenargument
 */
int main(int argc, char *argv[]){
	FILE *file;
	file=fopen(argv[1],"r");
	char c;
	char *buf=(char *)malloc(2*sizeof(char));
	while((c=getc(file))!=EOF){
		char *tmp=(char *)realloc(buf,(strlen(buf)+2)*sizeof(char));
		strcpy(tmp,buf);
		tmp[strlen(tmp)]=c;
		buf=tmp;
	}
	fclose(file);
	/*Dimensionen*/
	int width=0;
	int height=1;
	for(int i=0;i<strlen(buf)-1;i++){
		if(buf[i]=='\n')
			height++;
	}
	int w_tmp=0;
	for(int i=0;i<strlen(buf);i++){
		if(buf[i]!='\n')
			w_tmp++;
		else
			w_tmp=0;
		if(w_tmp>width)
			width=w_tmp;
	}
	int w_len, h_len;
	char *tmp=malloc(12);
	sprintf(tmp, "%d", width);
	w_len=strlen(tmp);
	sprintf(tmp, "%d", height);
	h_len=strlen(tmp);
	free(tmp);
	int indent=0;
	char *out=(char *)calloc(47+w_len+h_len,sizeof(char));
	sprintf(out, "{\n\t\"name\":\"name\",\n\t\"x\":%d,\n\t\"y\":%d,\n\t\"tiles\":[\n\t\t[",width,height);
	indent=2;
	for(int i=0;i<strlen(buf);i++){
		char *tmp;
		c=buf[i];
		switch(c){
		case '#':
			tmp=(char *)calloc(strlen(out)+14,sizeof(char));
			strcpy(tmp,out);
			free(out);
			out=tmp;
			sprintf(out,"%s{\"type\":\"w\"},",out);
			break;
		case '.':
		case '*':
		case '@':
			tmp=(char *)calloc(strlen(out)+14,sizeof(char));
			strcpy(tmp,out);
			free(out);
			out=tmp;
			sprintf(out,"%s{\"type\":\"f\"},",out);
			break;
		case ' ':
			tmp=(char *)calloc(strlen(out)+4,sizeof(char));
			strcpy(tmp,out);
			free(out);
			out=tmp;
			sprintf(out,"%s{},",out);
			break;
		case '?':
			tmp=(char *)calloc(strlen(out)+17,sizeof(char));
			strcpy(tmp,out);
			free(out);
			out=tmp;
			sprintf(out,"%s{\"type\":\"hint\"},",out);
			break;
		case '+':
			tmp=(char *)calloc(strlen(out)+16,sizeof(char));
			strcpy(tmp,out);
			free(out);
			out=tmp;
			sprintf(out,"%s{\"type\":\"btn\"},",out);
			break;
		case '~':
			tmp=(char *)calloc(strlen(out)+16,sizeof(char));
			strcpy(tmp,out);
			free(out);
			out=tmp;
			sprintf(out,"%s{\"type\":\"h2o\"},",out);
			break;
		case '!':
			tmp=(char *)calloc(strlen(out)+17,sizeof(char));
			strcpy(tmp,out);
			free(out);
			out=tmp;
			sprintf(out,"%s{\"type\":\"exit\"},",out);
			break;
		case '|':
		case '-':
			tmp=(char *)calloc(strlen(out)+17,sizeof(char));
			strcpy(tmp,out);
			free(out);
			out=tmp;
			sprintf(out,"%s{\"type\":\"door\"},",out);
			break;
		case '\n':
			if(i<strlen(buf)-1){
				tmp=(char *)calloc(strlen(out)+6,sizeof(char));
				out[strlen(out)-1]=']';
				strcpy(tmp,out);
				free(out);
				out=tmp;
				sprintf(out,"%s,\n\t\t[",out);
			}
			else{
				tmp=(char *)calloc(strlen(out)+4,sizeof(char));
				out[strlen(out)-1]=']';
				strcpy(tmp,out);
				free(out);
				out=tmp;
				sprintf(out,"%s\n\t]",out);
			}
			break;
		}
	}
	tmp=calloc(strlen(out)+17,sizeof(char));
	strcpy(tmp,out);
	free(out);
	out=tmp;
	sprintf(out,"%s,\n\t\"spawns\":[]\n}",out);
	printf("%s\n",out);
	char *filename=(char *)calloc(strlen(argv[1])+6,sizeof(char));
	sprintf(filename,"%s.json",argv[1]);
	file=fopen(filename,"w");
	fprintf(file,"%s",out);
	fclose(file);
	return EXIT_SUCCESS;
}
