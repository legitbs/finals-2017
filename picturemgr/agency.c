#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "stockMgmt.h"

agencyInfoType *agencyDB = 0;

int addStockAgency(char *name, char *url) {

uint32 agencyID;
agencyInfoType *cursor;

	if (name == 0 || url == 0 ) {

		return 0;
	}

	agencyID = (rand() & 0xffff) + 1;

	while ( agencyID == 0 || (findAgency(agencyID) != NULL))
		agencyID = (rand() & 0xffff) + 1;


	if ( agencyDB == 0 ) {

		agencyDB = malloc(sizeof(agencyInfoType));

		if (agencyDB == 0 ) {

			printf("unable to allocate memory\n");
			exit(-1);

		}

		cursor = agencyDB;
	}
	else {

		cursor = agencyDB;

		while (cursor->next != NULL ) {

			cursor = cursor->next;
		}

		cursor->next = malloc(sizeof(agencyInfoType));

		if (cursor->next == 0 ) {

			printf("unable to allocate memory\n");
			exit(-1);
		}

		cursor = cursor->next;
	}

	cursor->agencyID = agencyID;
	cursor->next = 0;

	cursor->agencyName = malloc( strlen(name)+1 );

	cursor->agencyURL = malloc( strlen(url)+1 );

	if ( cursor->agencyURL == 0 ) {

		printf("unable to allocate memory\n");
		exit(-1);

	}

	strncpy(cursor->agencyName, name, strlen(name)+1);
	strncpy(cursor->agencyURL, url, strlen(url)+1 );

	cursor->submissions = NULL;
	cursor->sales = NULL;

	return cursor->agencyID;

} // addAgency()

uint32 updateAgency(uint32 agencyID, char *name, char *url) {

agencyInfoType *agency;


	agency = findAgency(agencyID);

	if ( agency == 0 ) {

		return 0;
	}

	if ( name ) {

		if (strlen(name) < strlen(agency->agencyName)) {

			strcpy(agency->agencyName, name );

		}
		else {

			free(agency->agencyName);

			agency->agencyName = malloc(strlen(name)+1);

			if (agency->agencyName == 0 ) {

				printf("unable to allocate memory\n");
				exit(-1);
			}

			strcpy(agency->agencyName, name);
		}
	}

	if ( url ) {

		if ( strlen(url) > strlen(agency->agencyURL)) {

			free(agency->agencyURL);

			agency->agencyURL = malloc(strlen(url)+1);

			if (agency->agencyURL == 0 ) {

				printf("unable to allocate memory\n");
				exit(-1);

			}

			strcpy(agency->agencyURL, url );
		}
	}

	return agencyID;

} // updateAgency()

int listAgencies() {

	agencyInfoType *cursor;

	if ( agencyDB == NULL ) {

		return -1;
	}

	cursor = agencyDB;

	while (cursor != NULL ) {

		printf("Agency: %s  ID: %lu  URL: %s\n", cursor->agencyName, cursor->agencyID, cursor->agencyURL );
		cursor = cursor->next;

	}
	return 0;

} // listAgencies()

agencyInfoType *findAgency( uint32 agencyID ) {

agencyInfoType *cursor;

	cursor = agencyDB;

	while (cursor != NULL && cursor->agencyID != agencyID ) {

		cursor = cursor->next;
	}

	return cursor;

} // findAgency()

int deleteAgency( uint32 agencyID ) {

agencyInfoType *prevAgency, *curAgency;
submissionInfoType *submission, *submission_prev;
salesInfoType *sales, *sales_prev;

	if ( agencyDB == NULL ) {

		printf("no Agencies defined\n");
		return -1;
	}

	curAgency = agencyDB;

	while ( curAgency->next != NULL && curAgency->agencyID != agencyID ) {

		prevAgency = curAgency;
		curAgency = curAgency->next;
	}

	if ( curAgency->agencyID == agencyID ) {

		if ( curAgency == agencyDB ) {

			agencyDB = curAgency->next;
		}
		else {

			prevAgency->next = curAgency->next;

		}

		sales = curAgency->sales;
		submission = curAgency->submissions;

		while ( sales != NULL ) {

			sales_prev = sales;
			sales = sales->next;
			free( sales_prev );
		}

		while ( submission != NULL ) {

			submission_prev = submission;
			submission = submission->next;
			free( submission_prev );
		}

		free( curAgency );

		return 0;
	}

	return -1;

} // deleteAgency()



uint32 addSubmission(uint32 agencyID, uint32 imageID) {

agencyInfoType *agency;
submissionInfoType *submission;
submissionInfoType *cursor;
imageInfoType *image;

	agency = findAgency(agencyID);

	if ( agency == NULL ) {

		return 0;
	}

	image = findImage(imageID);

	if ( image == NULL ) {

		return 0;
	}

	submission = malloc(sizeof(submissionInfoType));

	if (submission == 0 ) {

		printf("unable to allocate memory\n");
		exit(-1);
	}

	submission->imageID = imageID;
	submission->status = 0;
	submission->date = time(0);
	submission->next = 0;

	if ( agency->submissions == NULL ) {

		agency->submissions = submission;
	}
	else {

		cursor = agency->submissions;

		while( cursor->next != NULL && cursor->imageID != imageID ) {

			cursor = cursor->next;
		}

		if ( cursor->imageID == imageID ) {

			free(submission);
			return 0;
		}
		else {
			cursor->next = submission;
		}
	}

	return 1;

} // addSubmission()


int updateSubmissionStatus( uint32 agencyID, uint32 imageID, uint8 status ) {

agencyInfoType *agency;
submissionInfoType *submission;

	agency = findAgency(agencyID);

	if ( agency == NULL ) {

		return 0;
	}

	submission = agency->submissions;

	while(submission != NULL && submission->imageID != imageID ) {

		submission = submission->next;

	}

	if ( submission == NULL ) {

		return 0;
	}

	submission->status = status;

	return 1;

}

int addSale ( uint32 agencyID, uint32 imageID, uint32 price ) {

agencyInfoType *agency;
submissionInfoType *submission;
salesInfoType *sales;

	agency = findAgency(agencyID);

	if ( agency == NULL ) {

		return 0;
	}

	submission = agency->submissions;

	while(submission != NULL && submission->imageID != imageID ) {

		submission = submission->next;

	}

	if ( submission == NULL ) {

		return 0;
	}


	if ( agency->sales == NULL ) {

		agency->sales = malloc(sizeof(salesInfoType));

		if (agency->sales == NULL ) {

			printf("unable to allocate memory\n");
			exit(-1);
		}
		else { 
			sales = agency->sales;
		}
	}
	else {

		sales = agency->sales;

		while (sales->next != NULL ) {

			sales = sales->next;
		}

		sales->next = malloc(sizeof(salesInfoType));

		if (sales->next == NULL ) {

			printf("unable to allocate memory\n");
			exit(-1);

		}
		else {

			sales = sales->next;
		}
	}

	sales->next = NULL;

	sales->price = price;

	sales->imageID = imageID;

	sales->date = time(0);

	return 1;

}
