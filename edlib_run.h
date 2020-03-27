#ifndef __EDLIB_RUN
#define __EDLIB_RUN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 500
#define BUFFER_SIZE 16


char* trim_encode(char* text, size_t *size);
int readInput(char *inputfile, char *filter_filename, char *edits_filename, int passer_count[], 
	int error_threshold, int read_length, int undef_flag, int *accepted, int *rejected, int *undef_count);


#endif
