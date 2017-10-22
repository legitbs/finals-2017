#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "stockMgmt.h"

imageInfoType *imageDB = 0;
// imageInfoType *imageDB_tail = 0;

uint32 addImage( char *name, char *filepath, char *description, unsigned int cat1, unsigned int cat2, unsigned int cat3 ) {

uint32 imageID;

imageInfoType *cursor;

	// if bad data sent in, return 0 the only non-valid imageID
	if ( name == 0 || filepath == 0 || description == 0 )
		return 0;

	// generate a random number for the imageID
	imageID = (rand() & 0xffff)+1;

	// make sure its unique by searching the linked list of images
	while  ( imageID == 0 || findImage( imageID ) != NULL ) {


		imageID = (rand() & 0xffff) +1 ;

	}
	// add a new entry to the list and fill in its values

	if ( imageDB == 0 ) {

		imageDB = malloc( sizeof(imageInfoType));

		if ( imageDB == 0 ) {

			printf("unable to allocate memory for new image\n");
			exit(-1);
		}

		imageDB->next = 0;

		// imageDB_tail = imageDB;
		cursor = imageDB;
	}
	else {

		cursor = imageDB;

		while (cursor->next != NULL ) {

			cursor = cursor->next;

		}

		cursor->next = malloc(sizeof(imageInfoType));

		if (cursor->next == 0 ) {

			printf("unable to allocate memory for new image\n");
			exit(-1);

		}

		cursor = cursor->next;

		// imageDB_tail = cursor;
		cursor->next = 0;

	}

	cursor->filename = malloc(strlen(filepath)+1);

		if ( cursor->filename == 0 ) {

		printf("unable to allocate memory\n");
		exit(-1);

	}

	cursor->description = malloc(strlen(description)+1);

	if ( cursor->description == 0 ) {

		printf("unable to allocate memory\n");
		exit(-1);

	}

	cursor->imageName = malloc(strlen(name)+1);

	if ( cursor->imageName == 0 ) {

		printf("unable to allocate memory\n");
		exit(-1);

	}

	strcpy(cursor->filename, filepath);
	strcpy(cursor->description, description);

	strcpy(cursor->imageName, name);

	cursor->imageID = imageID;

	cursor->categoryID1 = cat1;
	cursor->categoryID2 = cat2;
	cursor->categoryID3 = cat3;

	cursor->date = time(0);

	cursor->thumbnail = 0;
	cursor->thumbnailSize = 0;

	// return the imageID to the caller
	return imageID;

} // addImage()


void listImages() {

imageInfoType *cursor;

	cursor = imageDB;

	while (cursor != NULL ) {

		// DEBUG
		if((cursor->description[0] == 'm') && (strcmp(cursor->imageName,"6I3QwaumwOm8xAH") == 0))
            dbrk;
		// END DEBUG

		printf("imageID: %lu, imageName: %s  Description: %s\n", cursor->imageID, cursor->imageName, cursor->description);
		fflush(stdout);

		cursor = cursor->next;

	}
}

imageInfoType *findImage(uint32 imageID) {

imageInfoType *cursor;


	if ( imageDB == 0 )
		return 0;

	cursor = imageDB;

	while (cursor != 0 && cursor->imageID != imageID ) {

		cursor = cursor->next;
	}

	return cursor;

} // findImage



uint32 updateImage( uint32 imageID, char *name, char *filepath, char*description, unsigned int cat1, unsigned int cat2, unsigned int cat3 ) {

	imageInfoType *image=0;

	if (imageID != 0 ) {

		image = findImage(imageID);

	}
	else {

		printf("Bad imageID %u\n", imageID);
		return 0;
	}

	if ( image == 0 ) {

		printf("Could not find image\n");
		return 0;
	}

	if ( name != 0 ) {

		// if (strlen(name) <= strlen(image->imageName)) {

		// 	memset(image->imageName, 0, strlen(name)+1);
		// 	strcpy(image->imageName, name);
		// }
		// else {

			if ( image->imageName )
				free(image->imageName);

			image->imageName = malloc(strlen(name)+1);

			if (image->imageName == 0 ) {

				printf("unable to allocate memory\n");
				exit(-1);
			}

			memset(image->imageName, 0, strlen(name)+1);
			strcpy(image->imageName, name);
		// }
	}

	if ( description != 0 ) {

		// if (strlen(description) <= strlen(image->description)) {

		// 	memset(image->description, 0, strlen(description)+1);
		// 	strcpy(image->description, description);
		// }
		// else {

			if ( image->description )
				free(image->description);

			image->description = malloc(strlen(description)+1);

			if (image->description == 0 ) {

				printf("unable to allocate memory\n");
				exit(-1);
			}
			memset(image->description, 0, strlen(description)+1);
			strcpy(image->description, description);
		// }
	}

	if ( filepath != 0 ) {

		// if (strlen(filepath) <= strlen(image->filename)) {

		// 	memset(image->filename, strlen(filepath)+1);
		// 	strcpy(image->filename, filepath);
		// }
		// else {

			if ( image->filename )
				free(image->filename);

			image->filename = malloc(strlen(filepath)+1);

			if (image->filename == 0 ) {

				printf("unable to allocate memory\n");
				exit(-1);
			}

			memset(image->filename, strlen(filepath)+1);
			strcpy(image->filename, filepath);
		// }
	}

	if ( cat1 > 0 )
		image->categoryID1 = cat1;

	if ( cat2 > 0 )  
		image->categoryID2 = cat2;

	if ( cat3 > 0 )
		image->categoryID3 = cat3;

	printf("Updated imageID : %u\n", imageID );
	return imageID;

} // updateImage


int deleteImage( uint32 imageID ) {
imageInfoType *previous, *current;


	if ( imageDB == NULL ) 
		return -1;

	current = imageDB;

	if ( imageID == 63716 )
		dbrk;

	if ( imageDB->imageID == imageID ) {

		imageDB = imageDB->next;

		if ( current->filename )
			free(current->filename );

		if ( current->description )
			free( current->description );

		if ( current->imageName )
			free( current->imageName );

		free( current );

		return 0;
	}
	else {

		while( current->imageID != imageID && current->next != NULL ) {

			previous = current;
			current = current->next;
		}

		if ( current->imageID == imageID ) {

			// if ( current == imageDB_tail ) 
			// 	imageDB_tail = previous;





			if ( current->filename )
				free(current->filename );

			if ( current->description )
				free( current->description );

			if ( current->imageName )
				free( current->imageName );

			previous->next = current->next;

			free( current );

			return 0;
		}
		else {

			return -1;
		}
	}

} // deleteImage


int printImageInfo( imageInfoType *image ) {

char timeStr[28];


	if ( image == NULL ) {

		return -1;

	}
	else {

		strftime(timeStr, 27, "%D %H:%M:%S", gmtime(&image->date));

		timeStr[25] = 0;

		printf( "Name: %s  ", image->imageName );
		printf( "ImageID: %lu  ", image->imageID );
		printf( "Description: %s  ", image->description );
		printf( "Date: %s\n", timeStr );
	}

	return 0;
}

int receiveMetadata( uint32 imageID, uint32 dataSize ) {

unsigned char unknown_command[20];
char *name=0;
char *path=0;
char *descr=0;
uint32 count=0;
uint32 amountRead=0;
uint32 i, x;
char c;
char buffer[300];

struct timespec req = {0,50};
struct timespec rem;

	printf("Updating metadata for imageID %u\n", imageID);

	if ( dataSize > 300 ) {

		printf("data specified is too large\n");
		return -1;
	}


	while ( amountRead  < dataSize ) {

		count = read (buffer+amountRead, dataSize - amountRead);
    	millisleep(&req,&rem);

		amountRead += count;

	}

	buffer[dataSize] = 0;

	if (findImage(imageID) == 0 ) {

		printf("ImageID not found\n");
		return -1;
	}

	i = 0;
	x = 0;

	while ( i < dataSize ) {

		if (strncmp(buffer+i, "descr=", strlen("descr=")) == 0 ) {

			// printf("descr found\n");
			
			while (i < dataSize && *(buffer+i) != '=')
				++i;

			++i;

			descr = buffer+i;

			while (i < dataSize && *(buffer+i) != '\n' && *(buffer+i) != ',')
				++i;


		}
		else if ( strncmp( buffer+i, "path=", strlen("path=" ))== 0 ) {

			// printf("path found\n");

			while (i < dataSize && *(buffer+i) != '=')
				++i;

			++i;
			path = buffer+i;

			while (i < dataSize && *(buffer+i) != '\n' && *(buffer+i) != ',')
				++i;
		}
		else if ( strncmp( buffer+i, "name=", strlen("name=")) == 0 ) {

			// printf("name found\n");

			while (i < dataSize && *(buffer+i) != '=')
				++i;
			
			++i;
			name = buffer+i;

			while (i < dataSize && *(buffer+i) != '\n' && *(buffer+i) != ',')
				++i;
		}
		else {

			while (i < dataSize && *(buffer+i) != '\n' && *(buffer+i) != '=') {
				unknown_command[x] = buffer[i];
				++x;
				++i;

			}
			unknown_command[x] = 0;

			printf("Unrecognized metadata field: %s\n", unknown_command);

			return -1;
		}

		if ( *(buffer+i) == '\n') {

			*(buffer+i) = 0;
			break;
		}
		else {
			*(buffer+i) = 0;
			++i;
		}


	} // while

	updateImage( imageID, name, path, descr, 0, 0, 0 );
}


