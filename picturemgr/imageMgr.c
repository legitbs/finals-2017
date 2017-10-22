#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include "stockMgmt.h"

extern imageInfoType *imageDB;
extern agencyInfoType *agencyDB;

#define BUFFER_LEN 250

int parseAddImageOptions( char *optionString );
int parseAddAgencyOptions( char *optionString );
uint32 getID(char *parmName, char *optionString );
int parseAddSubmissionOptions( int sale, char *optionString );
int salesReport( char * );
int vulnerable_function(char *input);


int main( int argc, char **argv ) {

char buffer[BUFFER_LEN];

uint32 imageID;
uint32 agencyID;
uint32 submissionID;
uint32 salesID;
int retcode;
char *command=0;
int i;
char *option=0;
char *description=0;
char *imageName=0;
char *filePath=0;
char c;

struct timespec req = {0,1};
struct timespec rem;

	srand(myrand());

	while (1) {

		fflush(stdout);

		memset(buffer, 0, BUFFER_LEN);
		i = 0;
		while (1) {

			while (read(&c,1) == 0){

					   millisleep(&req,&rem);
				}
			if ( c == '\n' || i == BUFFER_LEN -1 ) {

				buffer[i] = 0;
				break;
			}
			else {

				buffer[i++] = c;	
			}
		}

		command = buffer;

		while (isspace(*command) )
			++command;

		i = 0;

		while( *(command+i) != ' ' && *(command+i) != '(' && *(command+i) != 0 )
			++i;

		if ( *(command+i) == 0 ) {

			printf("Bad command format\n");
			continue;
		}

		if ( *(command+i) == ' ' ) {

			*(command+i) = 0;

			++i;

			while( *(command+i ) != '(' && *(command+i) != 0 )
				++i;

			if ( *(command+i) == 0 ) {

				printf("Bad command format\n");
				continue;
			}

			++i;
		}
		else {
			*(command+i) = 0;
			++i;
		}

		if ( strcmp( command, "addImage" ) == 0 ) {

			parseAddImageOptions( command+i );

		}
		// else if ( strcmp( command, "test" ) == 0 ) {

		// 	vulnerable_function( command +i );
			
		// }
		else if ( strcmp( command, "updateImage") == 0 ) {

			parseAddImageOptions( command+i );

		}
		else if ( strcmp( command, "addMetadata") == 0 ) {

			parseMetaDataOptions( command+i );

		}
		else if ( strcmp( command, "delImage" ) == 0 ) {

			imageID=getID("imageID", command+i );

			if ( imageID == 0 ) {

				printf("Bad argument for delImage\n");
				continue;
			}

			retcode = deleteImage( imageID );

			if ( retcode == -1 ) {

				printf("Bad imageID or unable to delete image\n");
			}
			else {

				printf("Image %lu deleted\n", imageID );

			}

		}
		else if ( strcmp( command, "addAgency" ) == 0 ) {

			parseAddAgencyOptions( command+i );

		}
		else if ( strcmp( command, "delAgency" ) == 0 ) {

			agencyID=getID("agencyID", command+i );

			if ( agencyID == 0 ) {

				printf("Bad argument for delAgency\n");
				continue;
			}

			retcode = deleteAgency( agencyID );

			if ( retcode == -1 ) {

				printf("Bad agencyID or unable to delete agency\n");
			}
			else {

				printf("Agency %lu deleted\n", agencyID );

			}

		}
		else if ( strcmp( command, "listImages" ) == 0 ) {

			listImages();

		}
		else if ( strcmp( command, "listAgencies" ) == 0 ) {

			listAgencies();

		}
		else if ( strcmp( command, "submitImage" ) == 0 ) {

			parseAddSubmissionOptions( 0, command+i );
		}
		else if ( strcmp( command, "recordSale" ) == 0 ) {

			parseAddSubmissionOptions(1, command+i );
		}
		else if ( strcmp ( command, "infoImage" ) == 0 ) {

			imageID=getID("imageID", command+i );

			if ( imageID == 0 ) {

				printf("Bad argument for infoImage\n");
				continue;
			}

			retcode = imageReport( imageID );

			if ( retcode == -1 ) {

				printf("Bad imageID\n");
			}

		}
		else if ( strcmp ( command, "salesReport") == 0 ) {

			salesReport( command+i );

		}
		else if ( strcmp ( command, "quit" ) == 0 ) {

			exit(0);

		}
		else {

			printf( "Bad command: %s\n", command );
		}
	}
}

int parseAddImageOptions( char *optionString ) {

char *option;
char *value;
int optionLen;
char *description=0;
char *name=0;
char *filepath=0;
uint8 cat1, cat2, cat3=0;
uint32 imageID = 0;
imageInfoType *image;
int finished = 0;

	while ( 1 ) {

		while ( isspace( *optionString ) && *optionString != 0 ) 
			++optionString;

		if ( *optionString == 0 ) {

			printf("Bad command format\n");
			return 0;
		}

		option = optionString;
		optionLen = 0;

		while ( *optionString != 0 && *optionString != '=' ) {

			++optionLen;
			++optionString;
		}

		if ( *optionString == 0 ) {

			printf("Bad command format\n");
			return 0;
		}

		if ( *optionString == '=') {

			*optionString = 0;

			++optionString;
		}

		while ( isspace( *optionString ) && *optionString != 0 ) 
			++optionString;

		if ( *optionString == 0 ) {

			printf("Bad command format\n");
			return 0;
		}

		value = optionString;

		while ( *optionString != 0 && *optionString != ',' && *optionString != ')' ) {

			++optionString;
		}

		if ( *optionString == 0 ) {

			printf("Bad command format\n");
			return 0;
		}

		if (*optionString == ',' || *optionString == ')' ) {

			if (*optionString == ')' ) {

				finished = 1;
			}

			*optionString  = 0;
			++optionString;
		}

		if ( strcmp( option, "imageID" ) == 0 ) {

			imageID = atoi(value);
		}
		else if ( strcmp( option, "descr" ) == 0 ) {

			// printf( "description = %s\n", value );
			description = value;

		}
		else if ( strcmp( option, "name" ) == 0 ) {

			// printf( "name = %s\n", value );
			name = value;

		}
		else if ( strcmp( option, "path" ) == 0 ) {

			// printf( "path = %s\n", value );

			filepath = value;
		}
		else if ( strcmp( option, "cat1" ) == 0 ) {

			// printf( "cat1 = %s\n", value );

			cat1 = atoi(value);

		}
		else if ( strcmp( option, "cat2" ) == 0 ) {

			// printf( "cat2 = %s\n", value );

			cat2 = atoi(value);
		}
		else if ( strcmp( option, "cat3" ) == 0 ) {

			// printf( "cat3 = %s\n", value );

			cat3 = atoi(value);
		}

		else {

			printf( "Bad option: %s\n", option );

			return -1;
		}

		if ( finished )
			break;	

	}

	if ( imageID != 0 ) {

		if (updateImage(imageID, name, filepath, description, cat1, cat2, cat3) == 0 ){

			// printf("Error updating image\n");
			return -1;

		}
		else {

			return 0;
		}

	}

	imageID = addImage(name, filepath, description, cat1, cat2, cat3);

	if ( imageID != 0 ) {

		printf( "ImageID=%lu\n", imageID );
		return 0;
	}
	else {

		printf("Error adding image\n");

		return -1;
	}

}

int parseAddAgencyOptions( char *optionString ) {

char *option;
char *value;
int optionLen;
char *name=0;
char *url=0;
uint32 agencyID;
int finished = 0;

	while ( *optionString != 0 ) {
	
		while ( isspace( *optionString ) ) 
			++optionString;

		option = optionString;
		optionLen = 0;

		while ( *optionString != 0 && *optionString != '=' ) {

			++optionLen;
			++optionString;
		}

		if ( *optionString == '=') {

			*optionString = 0;

			++optionString;
		}

		while ( isspace( *optionString ) ) 
			++optionString;

		value = optionString;

		while ( *optionString != 0 && *optionString != ',' && *optionString != ')' ) {

			++optionString;
		}

		if (*optionString == ',' || *optionString == ')' ) {

			if ( *optionString == ')' )
				finished = 1;
			*optionString  = 0;
			++optionString;
		}

		if ( strcmp( option, "url" ) == 0 ) {

			// printf( "url = %s\n", value );
			url = value;

		}
		else if ( strcmp( option, "name" ) == 0 ) {

			// printf( "name = %s\n", value );
			name = value;

		}
		else {

			printf( "Bad option: %s\n", option );

			return -1;
		}	

	}

	if ( finished == 0 ) {

		printf("Bad command format\n");
		return 0;
	}

	agencyID = addStockAgency( name, url );

	if ( agencyID != 0 ) {

		printf( "AgencyID=%lu\n", agencyID );
		return 0;
	}
	else {
		printf("error adding agency\n");
		return -1;
	}


}

uint32 getID(char *parmName, char *optionString ) {

char *option;
char *value;

	if ( optionString == 0 || *optionString == 0 ) {

		return 0;
	}

	while ( isspace( *optionString ) && *optionString != 0 ) 
		++optionString;	


	if ( *optionString == 0 )

		return 0;

	option = optionString;

	while ( *optionString != 0 && *optionString != '=' ) {

		++optionString;
	}

	if ( *optionString == '=' ) {

		*optionString = 0;

		++optionString;
	}
	else {

		return 0;
	}

	if ( strncmp( parmName, option, strlen(parmName)) != 0 ) {

		return 0;
	}

	while ( isspace( *optionString ) ) 
		++optionString;		

	value = optionString;

	while ( *optionString != 0 && *optionString != ')' )
		++optionString;

	if ( *optionString != ')' )
		return 0;

	return ( atoi( value ) );

}


int parseAddSubmissionOptions( int sale, char *optionString ) {

uint32 imageID=0;
uint32 agencyID=0;
int price = 0;
char *option;
char *value;
int retcode;

	while ( *optionString != 0 ) {

		while ( isspace( *optionString ) ) 
			++optionString;

		option = optionString;

		while ( *optionString != 0 && *optionString != '=' ) {

			++optionString;
		}

		if ( *optionString == '=') {

			*optionString = 0;

			++optionString;
		}

		while ( isspace( *optionString ) ) 
			++optionString;

		value = optionString;

		while ( *optionString != 0 && *optionString != ',' && *optionString != ')' ) {

			++optionString;
		}

		if (*optionString == ',' || *optionString == ')' ) {

			*optionString  = 0;
			++optionString;
		}

		if ( strcmp( option, "agencyID" ) == 0 ) {

			// printf( "agencyID = %s\n", value );

			agencyID = atoi( value );

		}
		else if ( strcmp( option, "imageID" ) == 0 ) {

			// printf( "imageID = %s\n", value );

			imageID = atoi( value );
		}
		else if ( strcmp( option, "price" ) == 0 ) {

			price = atoi( value );

		}
		else {

			printf( "Bad option: %s\n", option );

			return -1;
		}	

	}

	if ( agencyID == 0 || imageID == 0 ) {

		printf("missing parameters for command\n");
		return -1;
	}

	if ( sale == 0 ) {

		retcode = addSubmission(agencyID, imageID );

		if ( retcode == 1 ) {

			printf( "ImageID %lu submitted\n", imageID );
			return 0;
		}
		else {

			printf("ImageID %lu not submitted\n", imageID );
			return -1;
		}
	}
	else {

		if ( price == 0 ) {

			printf("Sale price needed to record sale\n");
			return -1;
		}

		retcode = addSale( agencyID, imageID, (uint32)price );

		if ( retcode == 1 ) {

			printf("Sale recorded\n");
			return 0;
		}
		else {

			printf("Unable to record sale of image %05lu\n", imageID );
			return -1;
		}
	}

}


int parseMetaDataOptions(char *optionString ) {

uint32 imageID=0;
uint32 dataSize=0;
char *option;
char *value;
int retcode;

	while ( *optionString != 0 ) {

		while ( isspace( *optionString ) ) 
			++optionString;

		option = optionString;

		while ( *optionString != 0 && *optionString != '=' ) {

			++optionString;
		}

		if ( *optionString == '=') {

			*optionString = 0;

			++optionString;
		}

		while ( isspace( *optionString ) ) 
			++optionString;

		value = optionString;

		while ( *optionString != 0 && *optionString != ',' && *optionString != ')' ) {

			++optionString;
		}

		if (*optionString == ',' || *optionString == ')' ) {

			*optionString  = 0;
			++optionString;
		}

		if ( strcmp( option, "dataSize" ) == 0 ) {

			dataSize = atoi( value );

		}
		else if ( strcmp( option, "imageID" ) == 0 ) {

			imageID = atoi( value );
		}
		else {

			printf( "Bad option: %s\n", option );

			return -1;
		}	

	}

	if ( imageID == 0 || dataSize == 0 ) {

		printf("missing parameters for command\n");
		return -1;
	}


	receiveMetadata( imageID, dataSize );

}

int salesReport( char *optionString ) {

uint32 imageID=0;
uint32 agencyID=0;
char *option;
char *value;
int retcode;
int finished = 0;

	while ( *optionString != 0 ) {

		while ( isspace( *optionString ) ) 
			++optionString;

		option = optionString;

		while ( *optionString != 0 && *optionString != '=' ) {

			++optionString;
		}

		if ( *optionString == '=') {

			*optionString = 0;

			++optionString;
		}

		while ( isspace( *optionString ) ) 
			++optionString;

		value = optionString;

		while ( *optionString != 0 && *optionString != ',' && *optionString != ')' ) {

			++optionString;
		}

		if (*optionString == ',' || *optionString == ')' ) {

			if ( *optionString == ')' )
				finished = 1;

			*optionString  = 0;
			++optionString;
		}

		if ( strncmp( option, "agencyID", strlen("agencyID") ) == 0 ) {

			agencyID = atoi( value );

		}
		else if ( strncmp( option, "imageID", strlen("imageID") ) == 0 ) {

			// printf( "imageID = %s\n", value );

			imageID = atoi( value );
		}
		else{

			printf( "Bad option: %s\n", option );

			return -1;
		}	

		if ( finished )
			break;

	}

	if ( finished != 1 ) 

		return -1;

	if ( imageID == 0 && agencyID != 0 ) {

		retcode = salesReportByAgency( agencyID );

	}
	else if ( imageID && agencyID ) {

		retcode = imageReportByAgency( imageID, agencyID );

	}
	else {

		printf("invalid command options\n");
		return -1;

	}

	return 0;

}

