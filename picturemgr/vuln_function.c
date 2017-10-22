
int vulnerable_function(char *input) {
	
	char buffer[10];


	strcpy( buffer, input );

	printf ("I have now overwritten whatever you sent in enough data to overwrite.  Have fun.");

	return 0;

}