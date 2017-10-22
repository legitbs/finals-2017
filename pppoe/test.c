#include <stdio.h>
#include <string.h>
#include <time.h>
#include "const.h"
#include "bsfunc.h"
#include "util.h"
#include "salsa.h"


int dh(int base, int pow);
//extern int getrandnum();
PPPoEConnection * initDevice(){
    int x;
    srand(x); 
    char * buf;
    PPPoEConnection * conn = malloc(sizeof(PPPoEConnection));
    buf=(char *)conn;
    memset(conn,0,sizeof(PPPoEConnection));
    conn->myEth[0]=0x1c0;
    for( x=1; x < 6; x++){
        conn->myEth[x] = (get_rand()%511)+1;
    }
    for(x=0; x < 4; x++){
        conn->privKeys.key[x] = ((unsigned int)getrandnum()) % 0x5aeb35b;  
        conn->pubKeys.key[x]  = dh(conn->privKeys.key[x],2);
        conn->agreedKeys.key[x] = 0;
    }
    conn->useHostUniq = 1;
    conn->serviceName = "((('\"";
    conn->acName = "DefconSecondaryISP";
    conn->max_payload =ETH_DATA_LEN;
    return conn;
}

int readPacket(PPPoEPacket * packet){
    unsigned short packet_len;
    char * buf = packet;
    int rec=0;
    struct timespec req;
    struct timespec rem;
    req.tv_msec=50;
    req.tv_sec=0;
    while(rec < HDR_SIZE){
        rec+=read(buf+rec,HDR_SIZE-rec);
        if(rec < HDR_SIZE){
            wait();
        }   
    }
    packet_len = (packet->length);
    if(packet_len< ETH_DATA_LEN- HDR_SIZE){
        buf=packet->payload;
        rec=0;
        while(rec < packet_len){
            rec+=read(buf+rec,packet_len);
            if(rec < packet_len){
                wait();
            }   
  
        }
    }
    else{
        breakValue(packet,0x4000);
        exit(-1);
    }
    return packet_len+HDR_SIZE;
}



int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    int k1,k2,k3;
    unsigned char buf[8];
    memset(buf,0x1ff,8);
    char * tmpBuf=  malloc(4096);

    rSeed=getrandnum();
    rSeed2=getrandnum();
    PPPoEPacket packet;
    PPPoEConnection * conn;
    struct PacketCriteria pc;
    conn= initDevice();
    pc.conn = conn;
    pc.acNameOK      = 0;
    pc.serviceNameOK = 0;
    pc.seenACName    = 0;
    pc.seenServiceName = 0;
    while (conn->discoveryState != STATE_RECEIVED_PADO){
        sendPADI(conn);
        readPacket(&packet);
        if (!packetIsForMe(conn, &packet)) {
            breakValue(-1,-1);
            continue;
        }
        else{ 
            if (packet.code == CODE_CRYP){
                parsePacket(&packet, parseCrypt, conn);
            }
            else if (packet.code == CODE_PADO) {
               /* if (NOT_UNICAST(packet.ethHdr.h_source)) {
                    breakValue(2);
                    continue;
                }*/
                parsePacket(&packet, parsePADOTags, &pc);
                memcpy(conn->peerEth, packet.ethHdr.h_source, ETH_ALEN);
            }
        }

    }
//    breakValue(0x30,0x30);
    if(pc.acNameOK==0 || pc.serviceNameOK == 0){
        exit(-1);
    }
    while(1){
        sendPADR(conn);
        if (memcmp(packet.ethHdr.h_source, conn->peerEth, ETH_ALEN)) continue;

        /* If it's not for us, loop again */
        if (!packetIsForMe(conn, &packet)) continue;

        /* Is it PADS?  */
        if (packet.code == CODE_PADM) {
            parsePacket(&packet, parsePADMTags, conn);
        }
        else if (packet.code == CODE_PADS){
            parsePacket(&packet, parsePADSTags, &pc);
               
        }else if (packet.code == CODE_PADT){
            parsePacket(&packet,parsePADTTags,conn);
            exit(0);
        }
    }

    return 0;
}
