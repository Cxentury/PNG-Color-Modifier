#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* duplicate(char* pathname){
  unsigned char buff[50];
  FILE* src=fopen(pathname,"r");
  if(src == NULL){
    perror("fopen");
    exit(-1);
  }
  char* fileName=malloc(sizeof(char)*(strlen(pathname)+20));
  strcpy(fileName,pathname);
  strcat(fileName,"_Modified.png");
  fclose(fopen(fileName,"w"));
  FILE* dst=fopen(fileName,"a");

  if(src == NULL){
    perror("fopen duplicate");
    exit(1);
  }

  while(fread(buff,1,1,src)>0){
    fwrite(buff,1,1,dst);
  }

  fclose(src);
  fclose(dst);

  return fileName;
}
