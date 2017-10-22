#ifndef UTIL_LOAD
#define UTIL_LOAD 
#include <stdlib.h>
#include "const.h"

static unsigned short mypid =0 ;
unsigned int rSeed = 0;
unsigned int rSeed2 = 0;

typedef void ParseFunc(UINT16_t type,
               unsigned int len,
               unsigned char *data,
               void *extra);


#define DEBUG 1
int parsePacket(PPPoEPacket *packet, ParseFunc *func, void *extra);
int breakValue(int x, int y){
#ifdef DEBUG
    dbrk;
#endif
    if(x==-1 && y==-1){
    	exit(-1);
    }
    return x;
}


unsigned short get_rand(){
	int ret=0;
	rSeed  *= 127851;
	rSeed2 *= (2<<17)-1;
	rSeed2 += 199;
	ret= rSeed^rSeed2;
	rSeed+=(rSeed2>>22);
	return ret;
}

unsigned short getPid(){
	if(mypid==0){
		mypid=get_rand();
	}
	return mypid;
}

unsigned short ntohs(unsigned short n){
	return ((n&0x1ff)<<9)|(n>>9);
}
unsigned short htons(unsigned short n){
	return ntohs(n);
}



void
parseForHostUniq(unsigned int type, unsigned int  len, unsigned char *data,
	void *extra)
{
	int *val = (int *) extra;
	if (type == TAG_HOST_UNIQ && len == sizeof(unsigned short)) {
		unsigned short tmp;
		shitcpy(&tmp, data, len);
		if (tmp == getPid()) {
			*val = 1;
		}
	}
}

int
packetIsForMe(PPPoEConnection *conn, PPPoEPacket *packet)
{
	int forMe = 0;
	int x=0;
    for(x=0; x < ETH_ALEN; x++){
		if(packet->ethHdr.h_dest[x]!=conn->myEth[x]){
			return 0;
		}    	
    }
	if (!conn->useHostUniq) return 1;
	parsePacket(packet, parseForHostUniq, &forMe);
	return forMe;
}


int parsePacket(PPPoEPacket *packet, ParseFunc *func, void *extra)
{
    UINT16_t len = packet->length;
    unsigned char * curTag;
    unsigned short * tags;
    UINT16_t tagType, tagLen;
    if (GETVER(packet->vertype) != 1) {
        return -1;
    }
    if (GETTYPE(packet->vertype) != 1) {
        return -1;
    }
    /* Do some sanity checks on packet */
    if (len > ETH_DATA_LEN - 6) { /* 6-byte overhead for PPPoE header */
    	return -1;
    }
    /* Step through the tags */
    curTag = packet->payload;
    while(curTag - packet->payload < len) {
        /* Alignment is not guaranteed, so do this by hand... */
        tags=curTag;
        tagType =  tags[0];
        tagLen = tags[1];

     
        if ((curTag - packet->payload) + tagLen + TAG_HDR_SIZE > len) {
            return -1;
        }
        if((tagType&ENCRYPTED) !=0 ){
            breakValue(tagType,-8);
            doCrypt(curTag+TAG_HDR_SIZE,tagLen);
        }
        func(tagType, tagLen, curTag+TAG_HDR_SIZE, extra);
        curTag = curTag + TAG_HDR_SIZE + tagLen;
    }
    return 0;
}

#endif

