/* 
 * Input Reader Class
 * Author : Zulal Bingol
 * Version : 1.0
 *           reads a txt file
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "edlib_run.h"
#include "../edlib/include/edlib.h"

#define PASS 1
#define REJECT 0
#define REJECT_EDIT -1
#define UNDEFINED_STR_EDIT -2

#define UNDEF_CHAR 'N'
#define UNDEFINED_STR 'u'
#define DEFINED_STR 'd'

#define DEVOP 3000000

double dtime()
{
	double tseconds = 0.0;
	struct timeval mytime;
	gettimeofday(&mytime,(struct timezone*)0);
	tseconds = (double)(mytime.tv_sec +
		mytime.tv_usec*1.0e-6);
	return( tseconds );
}

/*
 * removes '\n' chars, allocates memory for strings 
 * and encodes the letters 
 */
char* trim_encode(char* text, size_t *size)
{
	size_t t_size = strlen(text);
	size_t cnt = 0;
	size_t ind = 0;
	char* temp_str = realloc(NULL, sizeof(char) *t_size); 
	while (cnt < t_size){
		if (text[cnt] != '\n'){
      //temp_str[ind++] = encode(text[cnt]);
			temp_str[ind++] = text[cnt];
		}
		cnt++;
	}
	*size = ind;

	return realloc(temp_str, sizeof(char) * ind);
}

/*
 * Reads the input file given in the following format: line: read \t ref
 */
int readInput(char *reads_file_name, char *refs_file_name, char *filter_filename, char *edits_filename, int passer_count[], 
	int error_threshold, int read_length, int undef_flag, int *accepted, int *rejected, int *undef_count)
{
	FILE *reads_file = fopen(reads_file_name, "r");
	FILE *refs_file = fopen(refs_file_name, "r");
	FILE *edits_output = fopen(edits_filename, "w");
	FILE *filter_output = fopen(filter_filename, "w");
	int count = 0;

	if (reads_file == NULL || refs_file == NULL || edits_output == NULL || filter_output == NULL){
		fprintf(stderr, "Error: Input file does not exist or cannot be opened.\n");
		fprintf(stderr, "Error: OR output files cannot be generated.\n");
		exit(1);
	}
	else 
	{
		char *read_line = NULL, *ref_line = NULL;
		size_t read_line_size = 0, ref_line_size = 0;
		char str_status;
		size_t temp_read_size, temp_ref_size;
		char *read = (char *)malloc((read_length+1) * sizeof(char));
		char *ref = (char *)malloc((read_length+1) * sizeof(char));
		int line_tracer, read_l, ref_l;

		while (getline(&read_line, &read_line_size, reads_file) != -1 && getline(&ref_line, &ref_line_size, refs_file) != -1){

			str_status = DEFINED_STR;
			line_tracer = 0, read_l = 0, ref_l = 0;

			while(ref_line[line_tracer] != '\n' && ref_l < read_length){
				ref[ref_l] = ref_line[line_tracer];
				line_tracer++;
				ref_l++;
			}
			temp_ref_size = ref_l;
			ref[read_length] = '\0';

			line_tracer = 0;
			while(read_line[line_tracer] != '\n' && read_l < read_length){
				read[read_l] = read_line[line_tracer];
				line_tracer++;
				read_l++;
			}
			temp_read_size = read_l;
			read[read_length] = '\0';


			if (str_status == UNDEFINED_STR){ // if one of the strings is undefined and undef_flag is op't for not executing edlib
			  	fprintf(filter_output, "%d\n", PASS);
				fprintf(edits_output, "%d\n", UNDEFINED_STR_EDIT);
				(*undef_count)++;
				(*accepted)++;
				count++;
				continue;
			}

			// printf("line = %s\n", line);
			// printf("read = \t%s, %ld, %ld\n", read, temp_read_size, strlen(read));
			// printf("ref = \t%s, %ld, %ld\n", ref, temp_ref_size, strlen(ref));
			// exit(1);

			// CHUNK PROCESSING----------------------------------------------------------------------------------------
			int bucket_count = read_length / 100;
			int bucket_tracer = 0;
			int error_count = 0;
			while (bucket_tracer < bucket_count){
				EdlibAlignResult result = edlibAlign(&ref[bucket_tracer * 100], 100, &read[bucket_tracer * 100], 100, 
				edlibNewAlignConfig(-1, EDLIB_MODE_NW, EDLIB_TASK_PATH, NULL, 0));
				if (result.status == EDLIB_STATUS_OK) {
					error_count += result.editDistance;
				}
				edlibFreeAlignResult(result);
				bucket_tracer++;
			}
			fprintf(edits_output, "%d\n", error_count);
			if (error_count <= bucket_count * error_threshold){
				passer_count[error_count / bucket_count]++;
				fprintf(filter_output, "%d\n", PASS);
				(*accepted)++;
			}
			else{
				fprintf(filter_output, "%d\n", REJECT);
				(*rejected)++;
			}
			//-----------------------------------------------------------------------------------------------------------

			// WHOLE STRING PROCESSING----------------------------------------------------------------------------------
			// EdlibAlignResult result = edlibAlign(ref, temp_ref_size, read, temp_read_size, edlibNewAlignConfig(error_threshold, EDLIB_MODE_HW, EDLIB_TASK_PATH, NULL, 0));
			//EdlibAlignResult result = edlibAlign(ref, temp_ref_size, read, temp_read_size, 
			//	edlibNewAlignConfig(error_threshold, EDLIB_MODE_NW, EDLIB_TASK_PATH, NULL, 0));
			
			/*if (result.status == EDLIB_STATUS_OK) {
				fprintf(edits_output, "%d\n", result.editDistance);
				if (result.editDistance != -1){
					passer_count[result.editDistance]++;
					fprintf(filter_output, "%d\n", PASS);
					(*accepted)++;
				}
				else{
					fprintf(filter_output, "%d\n", REJECT);
					(*rejected)++;
				}
			}
			// printf("%d", result.editDistance);
			edlibFreeAlignResult(result);*/
			//--------------------------------------------------------------------------------------------------------------
			
			// printf("line = %d done\n", count);
			count++;

		} // while
      
		free(read);
		free(ref);
	}

	fclose(reads_file);
	fclose(refs_file);
	fclose(filter_output);
	fclose(edits_output);

	return count;
}

/*
 */
void destruct_2D(char **buffer, int size){
	if(!buffer){
		fprintf(stderr, "Reader: Memory free problem, exiting...\n");
		exit(1);
	}
	else {
		struct rusage r_usage;
		getrusage(RUSAGE_SELF,&r_usage);
		printf("\tReader Memory usage before = %ld kB\n",r_usage.ru_maxrss);
		int i;
		for (i = 0; i < size; i++){
			free(buffer[i]);
		}
		free(buffer);
		buffer = NULL;
	}
}


int main(int argc, char* argv[])
{
	if(argc < 6){
		printf("MISSING ARGUMENT\n");
		exit(0);
	}

	else 
	{
		char *reads_file_name = argv[1];
		char *refs_file_name = argv[2];
		int undef_flag = atoi(argv[3]);
		int read_length = atoi(argv[4]);
		int error_threshold = atoi(argv[5]);
		double tstart, tstop, ttime;
		int num_reads = 0;
		int i = 0;
		int accepted = 0, rejected = 0, undef_count = 0;

		char edit_dist[sizeof(int) * 4 + 1];
     	sprintf(edit_dist, "%d", error_threshold);

		char filter_file_name[30];
		strcpy(filter_file_name, "edlib_filter_output_err");
		strcat(filter_file_name, edit_dist);
		strcat(filter_file_name, ".txt");

		char edit_file_name[30];
		strcpy(edit_file_name, "edlib_edits_output_err");
		strcat(edit_file_name, edit_dist);
		strcat(edit_file_name, ".txt");

		int passer_count[error_threshold+1];
		while (i <= error_threshold){
			passer_count[i] = 0;
			i++;
		}

		printf("Edlib starts with read_length = %d, error_threshold = %d\n", read_length, error_threshold);
		if (undef_flag == 1){
			printf("Directly passing undefined sequences...\n\n");
		}
		else{
			printf("Processing undefined sequences, too...\n\n");
		}

		tstart = dtime();
		num_reads = readInput(reads_file_name, refs_file_name, filter_file_name, edit_file_name, passer_count, error_threshold, 
			read_length, undef_flag, &accepted, &rejected, &undef_count);
		tstop = dtime();
		i = 0;
		while (i <= error_threshold){
			if (passer_count[i] != 0){
				printf("passer_count[%d] = %d\n", i, passer_count[i]);
			}
			i++;
		}

		ttime = tstop - tstart;
		printf(" Accepted = %d(undefined included), rejected = %d, + %d undefined\n", accepted, rejected, undef_count);
		printf("Execution took %10.3lf sec.\n", ttime);
		printf("Total comparisons : %d\n", num_reads);
	}

	return 0;
}

