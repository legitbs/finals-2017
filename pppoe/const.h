#ifndef CONST_LOAD
#define CONST_LOAD
#include <stdlib.h>


typedef unsigned short UINT16_t;

void shitcpy(char *a, char *b, int len);

typedef struct {
	unsigned int key[4];
} dhKey;


#define GETVER(X)	(X&31)
#define GETTYPE(X)	((X&496)>>5)
#define MAKEVER(X)	(X)
#define MAKETYPE(X)	(X<<5)
#define NOT_UNICAST(e) ((e[0] & 0x01) != 0)
#define BROADCAST(e) ((e[0] & e[1] & e[2] & e[3] & e[4] & e[5]) == 0x1FF)
#define NOT_BROADCAST(e) ((e[0] & e[1] & e[2] & e[3] & e[4] & e[5]) != 0x1FF)



/* Ethernet frame types according to RFC 2516 */
#define ETH_PPPOE_DISCOVERY 0x8863
#define ETH_PPPOE_SESSION   0x8864


/* PPPoE codes */
#define CODE_PADI           0x09
#define CODE_PADO           0x07
#define CODE_PADR           0x19
#define CODE_PADS           0x65
#define CODE_PADT           0xA7
#define CODE_PADM           0xD3
#define CODE_SESS           0x00
#define CODE_CRYP           0x99
#define ETH_ALEN	6		/* Octets in one ethernet addr	 */
#define ETH_HLEN	14		/* Total octets in header.	 */
#define ETH_ZLEN	60		/* Min. octets in frame sans FCS */
#define ETH_DATA_LEN	1500		/* Max. octets in payload	 */
#define ETH_FRAME_LEN	1514		/* Max. octets in frame sans FCS */



/* PPPoE Tags */
#define TAG_END_OF_LIST    		0x0000
#define TAG_SERVICE_NAME 		0x0101
#define TAG_AC_NAME  			0x0102
#define TAG_HOST_UNIQ 			0x0103
#define TAG_AC_COOKIE  			0x0104
#define TAG_VENDOR_SPECIFIC 	0x0105
#define TAG_RELAY_SESSION_ID 	0x0110
#define TAG_HURL				0x0111
#define TAG_MOTM				0x0112
#define TAG_SET_SESSION			0x0113
#define TAG_MAX_PAYLOAD			0x0120
#define TAG_KEX					0x0125
#define TAG_SERVICE_NAME_ERROR	0x0201
#define TAG_AC_SYSTEM_ERROR  	0x0202
#define TAG_GENERIC_ERROR		0x0203
#define ENCRYPTED				0x1000


/* Discovery phase states */
#define STATE_SENT_PADI     0
#define STATE_RECEIVED_PADO 1
#define STATE_SENT_PADR     2
#define STATE_SESSION       3
#define STATE_TERMINATED    4

/* How many PADI/PADS attempts? */
#define MAX_PADI_ATTEMPTS 3

/* Initial timeout for PADO/PADS */
#define PADI_TIMEOUT 5

/* States for scanning PPP frames */
#define STATE_WAITFOR_FRAME_ADDR 0
#define STATE_DROP_PROTO         1
#define STATE_BUILDING_PACKET    2

/* Special PPP frame characters */
#define FRAME_ESC    0x7D
#define FRAME_FLAG   0x7E
#define FRAME_ADDR   0xFF
#define FRAME_CTRL   0x03
#define FRAME_ENC    0x20

#define IPV4ALEN     4
#define SMALLBUF   256


struct ethhdr 
{
	unsigned char	h_dest[ETH_ALEN];	/* destination eth addr	*/
	unsigned char	h_source[ETH_ALEN];	/* source ether addr	*/
	unsigned short	h_proto;		/* packet type ID field	*/
};

// SIZE IS 6+6+2
// 72 bit


/* A PPPoE Packet, including Ethernet headers */
typedef struct PPPoEPacketStruct {
    struct ethhdr ethHdr;	/* Ethernet header */
    unsigned short length; /* Payload length */
	char vertype;
    char code;	/* PPPoE code */
    short session;	/* PPPoE session */
    unsigned char payload[ETH_DATA_LEN]; /* A bit of room to spare */
} PPPoEPacket;

/* Header size of a PPPoE packet */
#define PPPOE_OVERHEAD 6  /* type, code, session, length */
#define HDR_SIZE (sizeof(struct ethhdr) + PPPOE_OVERHEAD)
#define MAX_PPPOE_PAYLOAD (ETH_DATA_LEN - PPPOE_OVERHEAD)
#define MAX_PPPOE_MTU (MAX_PPPOE_PAYLOAD - 2)

/* PPPoE Tag */

typedef struct PPPoETagStruct {
	unsigned short type;
	unsigned short length;
    unsigned char payload[ETH_DATA_LEN]; /* A LOT of room to spare */
} PPPoETag;
 


typedef struct PPPoEConnectionStruct {
    int discoveryState;		/* Where we are in discovery */
    unsigned char myEth[ETH_ALEN]; /* My MAC address */
    unsigned char peerEth[ETH_ALEN]; /* Peer's MAC address */
    unsigned short session;		/* Session ID */
    char *ifName;		/* Interface name */
    char *serviceName;		/* Desired service name, if any */
    char *acName;		/* Desired AC name, if any */
    unsigned short max_payload;
    dhKey privKeys;
    dhKey pubKeys;
    dhKey agreedKeys;
    int useHostUniq;		/* Use Host-Uniq tag */
    int numPADOs;		/* Number of PADO packets received */
    PPPoETag cookie;		/* We have to send this if we get it */
    PPPoETag relayId;		/* Ditto */
    PPPoETag hurl;
    PPPoETag motm;
} PPPoEConnection;

/* Header size of a PPPoE tag */
#define TAG_HDR_SIZE 4

/* Chunk to read from stdin */
#define READ_CHUNK 4096

struct PacketCriteria {
    PPPoEConnection *conn;
    int acNameOK;
    int serviceNameOK;
    int seenACName;
    int seenServiceName;
};

#endif