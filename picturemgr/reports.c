#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "stockMgmt.h"


extern imageInfoType *imageDB;
extern agencyInfoType *agencyDB;


int imageReport( uint32 imageID ) {
char reportLine[100];
agencyInfoType *cursor;
imageInfoType *theImage;
submissionInfoType *submission;
salesInfoType *saleInfo;

uint32 salesTotal; // in cents
uint32 salesCount;
uint32 agencySalesTotal;
uint32 agencyCountTotal;

char timeStr[27];

	theImage = findImage( imageID );

	if ( theImage == NULL ) {

		printf( "Image %lu not found\n\n", imageID );
		return -1;
	}

	printf("Image Report\n" );

	printImageInfo( theImage );

	// printf("aa\n");


	cursor = agencyDB;

	salesCount = 0;
	salesTotal = 0;

	while ( cursor != NULL ) {

		submission = cursor->submissions;

		while ( submission != NULL && submission->imageID != imageID ) {

			submission = submission->next;
		}

		// if this image was never submitted to this agency, go to the next agency
		if ( submission == NULL ) {

			cursor = cursor->next;
			continue;
		}

		saleInfo = cursor->sales;

		agencyCountTotal = 0;
		agencySalesTotal = 0;

		while ( saleInfo != NULL ) {

			if ( saleInfo->imageID == imageID ) {

				++agencyCountTotal;
				agencySalesTotal += saleInfo->price;

			}

			saleInfo = saleInfo->next;
		}

		sprintf(reportLine, "Agency: %s  Totals: %lu sales, $ %lu\n\n", cursor->agencyName, agencyCountTotal, agencySalesTotal );

		printf("%s", reportLine);

		salesCount += agencyCountTotal;
		salesTotal += agencySalesTotal;

		cursor = cursor->next;

	}

	printf("Total sales: %lu,  $ %lu\n", salesCount, salesTotal );

	return 0;
}


int imageReportByAgency( uint32 imageID, uint32 agencyID ) {

agencyInfoType *cursor;
imageInfoType *theImage;
submissionInfoType *submission;
salesInfoType *saleInfo;

uint32 agencySalesTotal;
uint32 agencyCountTotal;
char timeStr[27];
char reportLine[100];

	cursor = agencyDB;

	while ( cursor != NULL && cursor->agencyID != agencyID ) {

		cursor = cursor->next;
	}

	if ( cursor == NULL ) {

		printf("Agency not found!\n");
		return -1;

	}

	submission = cursor->submissions;

	while ( submission != 0 && submission->imageID != imageID ) {

		submission = submission->next;

	}

	if ( submission == 0 ) {

		printf ( "Image not submitted to this agency\n");
		return -1;
	}

	printf("Agency: %s\n", cursor->agencyName );

	saleInfo = cursor->sales;

	agencyCountTotal = 0;
	agencySalesTotal = 0;

	while ( saleInfo != NULL ) {

		if ( saleInfo->imageID == imageID ) {

			if ( agencyCountTotal == 0 ) {

				theImage = imageDB;

				while ( theImage != NULL && theImage->imageID != imageID ) {

					theImage = theImage->next;
				}

				if ( theImage == NULL ) {

					printf( "Image %lu not found\n\n", imageID );
					// return -1;
				}
				else {

					printImageInfo( theImage );
				}
			}

			++agencyCountTotal;
			agencySalesTotal += saleInfo->price;

			strftime(timeStr, 27, "%D %H:%M:%S", gmtime(&saleInfo->date));

			timeStr[25] = 0;

			sprintf( reportLine, "%s  $ %d\n", timeStr, saleInfo->price );

			printf( "%s", reportLine );

		}

		saleInfo = saleInfo->next;
	}

	printf("Agency Totals: %lu sales, $ %d\n", agencyCountTotal, agencySalesTotal );

	return 0;
}

int salesReportByAgency( uint32 agencyID ) {

agencyInfoType *cursor;
imageInfoType *theImage;
submissionInfoType *submission;
salesInfoType *saleInfo;

uint32 agencySalesTotal;
uint32 agencyCountTotal;

char reportLine[100];
char timeStr[51];
struct tm *mytime;
time_t epochtime;
char tempName[50];

	cursor = agencyDB;

	while ( cursor != NULL && cursor->agencyID != agencyID ) {

		cursor = cursor->next;
	}

	if ( cursor == NULL ) {

		printf("Agency not found!\n");
		return -1;

	}

	printf("Agency: %s\n", cursor->agencyName );

	saleInfo = cursor->sales;

	while ( saleInfo != NULL ) {

		theImage = imageDB;

		while ( theImage != NULL && theImage->imageID != saleInfo->imageID ) {

			theImage = theImage->next;
		}

		if ( theImage != NULL ) {
			

			strftime(timeStr, 27, "%D %H:%M:%S", gmtime(&saleInfo->date));

			timeStr[25] = 0;

			strncpy(tempName, theImage->imageName, sizeof(tempName) );

			tempName[49] = 0;

			sprintf( reportLine, "%s  %7u  %8u  %8s", timeStr, saleInfo->price, theImage->imageID, tempName);
			printf( "%s\n", reportLine );
			fflush(stdout);

		}

		saleInfo = saleInfo->next;
	}

	return 0;
}
