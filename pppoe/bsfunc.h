#ifndef BSFUNC_LOAD
#define BSFUNC_LOAD

#include <stdlib.h>
#include "util.h"


#define ntohs(X) ((X>>9)|((X<<9)&261632))


PPPoETag * formatError(PPPoEConnection *conn){
	char buf[ETH_FRAME_LEN];
	PPPoETag *svc = (PPPoETag *)malloc(sizeof(PPPoETag));
	int blah;
	memset(buf,0,1500);
	conn->motm.payload[conn->motm.length]=0;
	conn->hurl.payload[conn->hurl.length]=0;
	blah=sprintf(buf,1500,"Message update %s for URL's and users at %s\n",
		&(conn->motm.payload),&(conn->hurl.payload));
	svc->type = TAG_MOTM;
	svc->length = strlen(buf);
	shitcpy(svc->payload,buf,blah+1);
	return svc;
}
void
sendPADT(PPPoEConnection *conn, unsigned int reason)
{
	PPPoEPacket packet;
	unsigned char *cursor = packet.payload;
	PPPoETag *svc = (PPPoETag *) (&packet.payload);
	unsigned short namelen = 0;
	unsigned short plen;
	unsigned short pid;

	memset(packet.ethHdr.h_dest, 0x1FF, ETH_ALEN);
	memcpy(packet.ethHdr.h_source, conn->myEth, ETH_ALEN);

	packet.ethHdr.h_proto = (ETH_PPPOE_SESSION);
	packet.vertype = MAKEVER(1)| MAKETYPE(1);
	packet.code = CODE_PADT;
	packet.session = 0;

    /* If we're using Host-Uniq, copy it over */
	if (conn->useHostUniq) {
		PPPoETag hostUniq;
		pid = getPid();
		hostUniq.type = (TAG_HOST_UNIQ);
		hostUniq.length = (sizeof(pid));
		memcpy(hostUniq.payload, &pid, sizeof(pid));
		memcpy(cursor, &hostUniq, sizeof(pid) + TAG_HDR_SIZE);
		cursor += sizeof(pid) + TAG_HDR_SIZE;
		plen += sizeof(pid) + TAG_HDR_SIZE;
	}
	switch(reason){
		case TAG_SERVICE_NAME_ERROR:
			if ((packet.payload-cursor + 5+TAG_HDR_SIZE)<conn->max_payload ) {
				svc=cursor;
				svc->type=TAG_SERVICE_NAME;
				svc->length=5;
				cursor+=TAG_HDR_SIZE;
				shitcpy(cursor, &(conn->serviceName), 
					5 + TAG_HDR_SIZE);
				cursor += 5;
				plen += 5+TAG_HDR_SIZE;
			}
			break;
		case TAG_AC_SYSTEM_ERROR:
			if ((packet.payload-cursor + 5+TAG_HDR_SIZE)<conn->max_payload ) {
				svc=cursor;
				svc->type=TAG_SERVICE_NAME;
				svc->length=5;
				cursor+=TAG_HDR_SIZE;
				shitcpy(cursor, &(conn->serviceName), 
					5 + TAG_HDR_SIZE);
				cursor += 5;
				plen += 5+TAG_HDR_SIZE;
			}
			break;
		case TAG_GENERIC_ERROR:
			break;
		case TAG_MOTM:
			svc=formatError(conn);
			if(svc->length < conn->max_payload){
				shitcpy(cursor,svc,svc->length+TAG_HDR_SIZE);
			}
		default:
			breakValue(reason,0x1000);
	}

	packet.length = (plen);

	write(packet,plen+HDR_SIZE);

}



void
sendPADI(PPPoEConnection *conn)
{
	PPPoEPacket packet;
	unsigned char *cursor = packet.payload;
	PPPoETag *svc = (PPPoETag *) (&packet.payload);
	UINT16_t namelen = 0;
	int x=0;
	UINT16_t plen;
	unsigned short pid;
	namelen = 5;
	plen = TAG_HDR_SIZE + namelen;

    /* Set destination to Ethernet broadcast address */
	memset(packet.ethHdr.h_dest, 0x1FF, ETH_ALEN);
	for(x=0;x < ETH_ALEN;x++){
		packet.ethHdr.h_source[x] = conn->myEth[x];
	}
//	shitcpy(packet.ethHdr.h_source, conn->myEth, ETH_ALEN);

	packet.ethHdr.h_proto = (ETH_PPPOE_DISCOVERY);
	packet.vertype = MAKEVER(1)| MAKETYPE(1);
	packet.code = CODE_PADI;
	packet.session = 0;

	svc->type = TAG_SERVICE_NAME;
	svc->length = (namelen);

	if (conn->serviceName) {
		memcpy(svc->payload, conn->serviceName, namelen);
	}
	cursor += namelen + TAG_HDR_SIZE;

    /* If we're using Host-Uniq, copy it over */
	if (conn->useHostUniq) {
		PPPoETag hostUniq;
		pid = getPid();
		hostUniq.type = (TAG_HOST_UNIQ);
		hostUniq.length = (sizeof(pid));
		memcpy(hostUniq.payload, &pid, sizeof(pid));
		memcpy(cursor, &hostUniq, sizeof(pid) + TAG_HDR_SIZE);
		cursor += sizeof(pid) + TAG_HDR_SIZE;
		plen += sizeof(pid) + TAG_HDR_SIZE;
	}

	packet.length = (plen);
	write(&packet,plen+HDR_SIZE);

}


void
sendPADR(PPPoEConnection *conn)
{
	PPPoEPacket packet;
	PPPoETag *svc = (PPPoETag *) packet.payload;
	unsigned char *cursor = packet.payload;
	unsigned short pid;

	UINT16_t namelen = 5;
	UINT16_t plen;

	plen = TAG_HDR_SIZE + namelen;

	shitcpy(packet.ethHdr.h_dest, conn->peerEth, ETH_ALEN);
	shitcpy(packet.ethHdr.h_source, conn->myEth, ETH_ALEN);

	packet.ethHdr.h_proto = (ETH_PPPOE_DISCOVERY);
	packet.vertype = MAKEVER(1)| MAKETYPE(1);
	packet.code = CODE_PADR;
	packet.session = 0;

	svc->type = TAG_SERVICE_NAME;
	svc->length = (namelen);
	if (conn->serviceName) {
		shitcpy(svc->payload, conn->serviceName, namelen);
	}
	cursor += namelen + TAG_HDR_SIZE;
	/* If we're using Host-Uniq, copy it over */
	if (conn->useHostUniq) {
		PPPoETag hostUniq;
		pid = getPid();
		hostUniq.type = (TAG_HOST_UNIQ);
		hostUniq.length = (sizeof(pid));
		shitcpy(hostUniq.payload, &pid, sizeof(pid));
		shitcpy(cursor, &hostUniq, sizeof(pid) + TAG_HDR_SIZE);
		cursor += sizeof(pid) + TAG_HDR_SIZE;
		plen += sizeof(pid) + TAG_HDR_SIZE;
	}

    /* Copy cookie and relay-ID if needed */
   if (conn->cookie.type && 
		(cursor-packet.payload + conn->cookie.length+TAG_HDR_SIZE)<conn->max_payload ) {
		shitcpy(cursor, &(conn->cookie), 
			(conn->cookie.length) + TAG_HDR_SIZE);
		cursor += (conn->cookie.length) + TAG_HDR_SIZE;
		plen += (conn->cookie.length) + TAG_HDR_SIZE;
	}

	if (conn->relayId.type 
		&& 
		(cursor-packet.payload + conn->relayId.length+TAG_HDR_SIZE)
			< conn->max_payload ) {
		shitcpy(cursor, &(conn->relayId), 
			(conn->relayId.length) + TAG_HDR_SIZE);
		cursor += (conn->relayId.length) + TAG_HDR_SIZE;
		plen += (conn->relayId.length) + TAG_HDR_SIZE;
	}

	packet.length = (plen);
	write(&packet, (int) (plen + HDR_SIZE));
}


void
parsePADOTags(unsigned int tagType, unsigned int len, unsigned char *data,
              void *extra)
{
	unsigned short type = tagType;
    struct PacketCriteria *pc = (struct PacketCriteria *) extra;
    PPPoEConnection *conn = pc->conn;
    int i;
    short tag_len;
    switch(type) {
	    case TAG_AC_NAME:
	        pc->seenACName = 1;
	        if (conn->acName && 
	            !strncmp((char *) data, conn->acName, 18)) {
	            pc->acNameOK = 1;
	        }
	        break;
	    case TAG_SERVICE_NAME:
	        pc->seenServiceName = 1;
	        if (conn->serviceName &&
	            !strncmp((char *) data, conn->serviceName, 5)) {
	            pc->serviceNameOK = 1;
	        }
	        break;
	    case TAG_AC_COOKIE:
	        conn->cookie.type = (type);
	        conn->cookie.length = (len);
	        shitcpy(conn->cookie.payload, data, len);
	        break;
	    case TAG_RELAY_SESSION_ID:
	        conn->relayId.type = (type);
	        conn->relayId.length = (len);
	        shitcpy(conn->relayId.payload, data, len);
	        break;
	    case TAG_SERVICE_NAME_ERROR:
	    	sendPADT(conn,TAG_SERVICE_NAME_ERROR);
	    	breakValue(-1,-1);
	        break;
	    case TAG_AC_SYSTEM_ERROR:
	        sendPADT(conn,TAG_AC_SYSTEM_ERROR);
	        breakValue(-1,-1);
	        break;
	    case TAG_MAX_PAYLOAD:
	    	shitcpy(&tag_len,data,sizeof(unsigned int));// BUG 1
			if(tag_len < ETH_FRAME_LEN){
				conn->max_payload = tag_len;
			}
	        break;
	    case TAG_SET_SESSION:
	    	shitcpy(&tag_len,data,sizeof(unsigned int));// BUG 1
			conn->session = tag_len;
	        break;
	    case TAG_END_OF_LIST:
	        conn->discoveryState = STATE_RECEIVED_PADO;	    	
	        break;
    }
}
void
parsePADSTags(unsigned int type, unsigned int len, unsigned char *data,
              void *extra)
{
    PPPoEConnection *conn = (PPPoEConnection *) extra;
    unsigned short tag_len;
    switch(type) {
    case TAG_SERVICE_NAME:
    	sendPADT(conn,TAG_SERVICE_NAME_ERROR);
    	breakValue(-1,-1);
    case TAG_SERVICE_NAME_ERROR:
    	sendPADT(conn,TAG_SERVICE_NAME_ERROR);
        breakValue(-1,-1);
    case TAG_AC_SYSTEM_ERROR:
    	sendPADT(conn,TAG_AC_SYSTEM_ERROR);
        breakValue(-1,-1);
    case TAG_GENERIC_ERROR:
        breakValue(-1,-1);
    case TAG_RELAY_SESSION_ID:
        conn->relayId.type = (type);
        conn->relayId.length = (len);
        shitcpy(conn->relayId.payload, data, len);
        break;
    }
}
void
parsePADTTags(unsigned int type, unsigned int len, unsigned char *data,
              void *extra)
{
    PPPoEConnection *conn = (PPPoEConnection *) extra;
    switch(type) {
    case TAG_MOTM:
        break;
    case TAG_SERVICE_NAME_ERROR:
    	sendPADT(conn,TAG_SERVICE_NAME_ERROR);
        breakValue(-1,-1);
    case TAG_AC_SYSTEM_ERROR:
    	sendPADT(conn,TAG_AC_SYSTEM_ERROR);
        breakValue(-1,-1);
    case TAG_GENERIC_ERROR:
    	sendPADT(conn,TAG_GENERIC_ERROR);
        breakValue(-1,-1);
    case TAG_RELAY_SESSION_ID:
    	sendPADT(conn,TAG_RELAY_SESSION_ID);
        break;

    }
    exit(0);
}

void
parsePADMTags(unsigned int type, unsigned int len, unsigned char *data,
              void *extra)
{
    PPPoEConnection *conn = (PPPoEConnection *) extra;
    unsigned short tag_len;
    switch(type) {
	case TAG_MOTM:
        conn->motm.type = (type);
        conn->motm.length = (len);
        shitcpy(conn->relayId.payload, data, len);

		break;
	case TAG_HURL:
        conn->hurl.type = (type);
        conn->hurl.length = (len);
        shitcpy(conn->relayId.payload, data, len);
		break;


    }
}


void
parseCrypt(unsigned int type, unsigned int len, unsigned char *data,
              void *extra)
{
    PPPoEConnection *conn = (PPPoEConnection *) extra;
    unsigned int i =0;
    unsigned int blah;
    switch(type) {
		case TAG_KEX:
			sendCrypt(conn);
			if(len==sizeof(dhKey)){
				shitcpy(&(conn->agreedKeys),data,sizeof(dhKey));
			}
			else{
			//	breakValue(0x100,0x100);
				exit(0);
			}
			i=0;
			for(;i < 4; i++){
				blah=dh(conn->privKeys.key[i],
					conn->agreedKeys.key[i]);
				conn->agreedKeys.key[i] = blah;
//				breakValue(conn->agreedKeys.key[i],0x110);//
			}
			initHash(&conn->agreedKeys);
			break;
	}
	return;

}


void
sendCrypt(PPPoEConnection *conn)
{
	PPPoEPacket packet;
	unsigned char *cursor = packet.payload;
	UINT16_t namelen = 0;
    PPPoETag cryptTag;
	int x=0;
	UINT16_t plen=0;
	unsigned short pid;

	memset(packet.ethHdr.h_dest, 0x1FF, ETH_ALEN);
	for(x=0;x < ETH_ALEN;x++){
		packet.ethHdr.h_source[x] = conn->myEth[x];
	}
	packet.ethHdr.h_proto = (ETH_PPPOE_DISCOVERY);
	packet.vertype = MAKEVER(1)| MAKETYPE(1);
	packet.code = CODE_CRYP;
	packet.session = 0;

    /* If we're using Host-Uniq, copy it over */
	if (conn->useHostUniq) {
		PPPoETag hostUniq;
		pid = getPid();
		hostUniq.type = (TAG_HOST_UNIQ);
		hostUniq.length = (sizeof(pid));
		shitcpy(hostUniq.payload, &pid, sizeof(pid));
		shitcpy(cursor, &hostUniq, sizeof(pid) + TAG_HDR_SIZE);
		cursor += sizeof(pid) + TAG_HDR_SIZE;
		plen += sizeof(pid) + TAG_HDR_SIZE;
	}
    shitcpy(cryptTag.payload,&(conn->pubKeys),sizeof(dhKey));
    cryptTag.length = sizeof(dhKey) + TAG_HDR_SIZE;
    cryptTag.type = TAG_KEX;
	shitcpy(cursor,&cryptTag, sizeof(dhKey) + TAG_HDR_SIZE);
	plen += sizeof(dhKey) + TAG_HDR_SIZE;
	
	packet.length = (plen);
	write(&packet,plen+HDR_SIZE);
    struct timespec req;
    struct timespec rem;
    req.tv_msec=50;
    req.tv_sec=0;
    millisleep(&req,&rem);

}


#endif