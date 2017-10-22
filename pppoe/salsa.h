#ifndef SALSA_LOAD
#define SALSA_LOAD
#include "const.h"

#define STATE_SIZE 512
#define KEYSIZE 12

typedef struct{
  unsigned char state[STATE_SIZE];
}SalsaState;

SalsaState crypt;
int iterator;



typedef struct {
  unsigned int p0;
  unsigned int p1;
  unsigned int p2;
  unsigned int p3;
  unsigned int p4;
  unsigned int p5;
  unsigned int p6;
  unsigned int p7;
} hashstate;

hashstate * hs;
int streamPosition;
int streamValue;


int genNextValue(){
  hs->p1=hs->p1 ^ hs->p5;
  hs->p2=hs->p2 * hs->p7;
  hs->p3=hs->p3+hs->p1;
  hs->p7=rotl(hs->p7,(hs->p1%20));
  hs->p4=hs->p4*rotr(hs->p6,4);
  hs->p4=hs->p4^hs->p2;
  hs->p5=hs->p5^hs->p4;
  hs->p1= (hs->p1>>2);
  hs->p7=hs->p7*hs->p5;
  hs->p1=hs->p0^hs->p1;
  hs->p6=hs->p6^rotr(hs->p3,3);
  hs->p5= hs->p5+hs->p0;
  hs->p0=hs->p1^hs->p4+1;
  hs->p5 = hs->p5 ^0x1fffff;

  return hs->p1^ hs->p2^ hs->p3^ hs->p4^ hs->p5^ hs->p6;
}
void initHash(dhKey * key){
  hs= malloc(sizeof(hashstate));
  hs->p0=1;
  hs->p1=key->key[0];
  hs->p2=0x88227;
  hs->p3=key->key[1];
  hs->p4=0x292929;
  hs->p5=0x33213;
  hs->p6=key->key[1];
  hs->p6=0x11f1f1f;
  hs->p7=key->key[3];
  streamValue=3;
  genNextValue();
}
void doCrypt(unsigned char * buffer,
                 unsigned int len)
{
  int i=0;
  int j=0;
  unsigned int * vals;
  while(i < len){
    if(streamPosition>=3){
      streamValue=genNextValue();
    //  breakValue(streamValue,0x9090);
      streamPosition=0;
    }
   // breakValue(buffer[i],1);
    buffer[i]=buffer[i]^(streamValue&0x1ff);
  //  breakValue(streamValue&0x1ff,1);
 //   breakValue(buffer[i],2);
    streamValue=streamValue>>9;
    i++;
    streamPosition++;
  }

}

#endif