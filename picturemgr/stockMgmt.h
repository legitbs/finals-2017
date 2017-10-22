
#include <sys/types.h>

#define time rand
typedef unsigned long uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef struct imageInfo {
	
	uint32 imageID;
	time_t date;
	char *filename;
	char *description;
	char *imageName;
	uint8 categoryID1;
	uint8 categoryID2;
	uint8 categoryID3;

	uint32 thumbnailSize;
	void *thumbnail;

	struct imageInfo *next;

} imageInfoType;


typedef struct submissionInfo {

	uint32 imageID;
	time_t date;
	uint8 status;

	struct submissionInfo *next;

} submissionInfoType;

typedef struct salesInfo {

	uint32 imageID;
	time_t date;  // epoch date
	uint16 price; // in cents

	struct salesInfo *next;

} salesInfoType;

typedef struct agencyInfo {

	uint32 agencyID;
	char *agencyName;
	char *agencyURL;

	submissionInfoType *submissions;
	salesInfoType *sales;

	struct agencyInfo *next;

} agencyInfoType;



uint32 addImage( char *name, char *filepath, char *description, uint8 cat1, uint8 cat2, uint8 cat3 );
imageInfoType *findImage(uint32 imageID);
int printImageInfo( imageInfoType *image );
void listImages();
int deleteImage( uint32 imageID );
uint32 updateImage( uint32 imageID, char *name, char *filepath, char*description, uint8 cat1, uint8 cat2, uint8 cat3 );
int receiveMetadata( uint32 imageID, uint32 dataSize );
int addStockAgency(char *name, char *url);
agencyInfoType *findAgency( uint32 agencyID );
int listAgencies();
int deleteAgency( uint32 agencyID );
uint32 addSubmission(uint32 agencyID, uint32 imageID);
int updateSubmissionStatus( uint32 agencyID, uint32 imageID, uint8 status );
uint32 updateAgency(uint32 agencyID, char *name, char *url);
int addSale ( uint32 agencyID, uint32 imageID, uint32 price );
int imageReport( uint32 imageID );
int imageReportByAgency( uint32 imageID, uint32 agencyID );
int salesReportByAgency( uint32 agencyID );

