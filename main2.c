#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "crc32.h"
#include "duplicate.h"

int main(int argc, char const *argv[]) {

  //Might want to change to malloc
  unsigned char buff[10000];
  //Duplicate the file to prevent from altering original
  char* fileName=duplicate(argv[1]);
  FILE* image=fopen(fileName,"r+b");
  free(fileName);
  if(image==NULL){
    perror("fopen");
    return 1;
  }

  //PNG HEADER on 8 bytes
  int rd=fread(buff,8,1,image);
  if(strstr(buff,"PNG")==NULL){
    printf("Invalid file format, must be PNG\n");
    return 1;
  }

  //IHDR data length and type on 8 bytes
  rd=fread(buff,8,1,image);
  int dataLength=buff[3];

  rd=fread(buff,1,dataLength,image);

  /*
  buff[0..3] correspond à la largeur(width) de l'image (voir dossier explications)
  la width de l'image est sur 4 octets (buff[..]= 1 octet)
  on créé donc un uint (24 bit / 4 octets) en faisant des opérations OR entre les bit de chaque octets
  (buff[0] sont les bits de plus grande valeur donc on les 'décale' à la fin en faisant <<24)
  see : https://dev-notes.eu/2019/07/Convert-an-Array-of-Unsigned-Chars-to-an-int32_t-Value/
  */
  uint32_t width= buff[0] << 24 | buff[1] << 16 | buff[2] << 8 | buff[3];
  uint32_t height= buff[4] << 24 | buff[5] << 16 | buff[6] << 8 | buff[7];
  int bit_depth=buff[8];
  int color_type=buff[9];

  printf("width: %d height: %d bit depth: %d\n",width,height,bit_depth);
  if(color_type!=3){
    printf("This program only work with color type 3 (Palette)\n");
    return 1;
  }

  //CRC
  rd=fread(buff,4,1,image);

  unsigned char pltData[8];

  //Getting PLT data
  rd=fread(pltData,8,1,image);

  dataLength=pltData[3];

  if(dataLength%3!=0){
    printf("Invalid number of entries");
    return 1;
  }

  //Reads all color from Palette
  rd=fread(buff,dataLength,1,image);

  printf("\nColors present in the Palette\n");
  int counter=0;
  for (size_t i = 0; i < dataLength; i+=3) {
    printf("\x1b[38;2;%d;%d;%dm %d: R,G,B(%d,%d,%d)\x1b[0m\n",buff[i],buff[i+1],buff[i+2],counter,buff[i],buff[i+1],buff[i+2]);
    counter++;
  }

  int colorChange=0;
  printf("\nColor to change (representation in terminal might not be accurate):");
  scanf("%d",&colorChange);

  unsigned int values[3];
  printf("\nValues to change with (0-255): ");
  scanf("%u %u %u",&values[0],&values[1],&values[2]);

  //Changing color value *3 because there are three values per colo
  // 0: R,G,B(0,0,0)
  // 1: R,G,B(1,1,1)
  buff[colorChange*3]=values[0];
  buff[colorChange*3+1]=values[1];
  buff[colorChange*3+2]=values[2];

  //On remplace l'octet qui était là à l'origine
  fseek(image,-dataLength+colorChange*3,SEEK_CUR);
  fputc(buff[colorChange*3],image);
  fputc(buff[colorChange*3+1],image);
  fputc(buff[colorChange*3+2],image);

  //To calculate crc we need the last 4 bytes in pltData + the data un buffer
  memcpy(pltData+8,buff,dataLength);
  unsigned int crc=crc32a(&pltData[4],dataLength+4);

  //Returning to end of dataLength (where the CRC is)
  fseek(image,dataLength-(colorChange*3+3),SEEK_CUR);

  /*
  Not optimized but does the following to split an uint into it's bytes
  |10101010101010101010101010101010| -> crc
  |10101010*000000000000000000000000*| -> crc << 24 first value of t
  |10101010101010101010101010101010| -> t=crc
  |*00000000*101010101010101010101010| -> (crc<<8*1)
  |*000000000000000000000000*10101010| -> i+(8*1)
  */
  unsigned char t=crc;
  int cpt=1;
  for (size_t i = 24; i > 0; i=i-8) {
    t=(crc<<8*(cpt-1)) >> i+(8*(cpt-1));
    cpt++;
    fputc(t,image);
    t=crc;
  }
  fputc((crc<<24) >> 24,image);

  fclose(image);
  return 0;
}
