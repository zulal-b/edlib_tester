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
#include <omp.h>

#include "edlib_run_mt.h"
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
	int error_threshold, int read_length, int undef_flag, int *accepted, int *rejected, int *undef_count, int numThreads)
{
	FILE *reads_file = fopen(reads_file_name, "r");
	FILE *refs_file = fopen(refs_file_name, "r");
	int line_count = 0;


	if (reads_file == NULL || refs_file == NULL){
		fprintf(stderr, "Error: Input file do not exist or cannot be opened.\n");
		exit(1);
	}
	else 
	{
		int start_indices[numThreads];
		int finish_indices[numThreads];
		long int reads_start_pos[numThreads], refs_start_pos[numThreads];
		long int reads_finish_pos[numThreads], refs_finish_pos[numThreads];
		long int file_size = 0, sample_size = 0;
		int portion_size = 0, i;
		int thread = 0;
		char *line = NULL;
		size_t line_size = 0;

		//Results
		short *filter[numThreads];
		int *edits[numThreads];

		// File padding
		if (getline(&line, &line_size, reads_file) != -1){
			sample_size = ftell(reads_file);
		}
		fseek(reads_file, 0, SEEK_END);
		file_size = ftell(reads_file);
		line_count = file_size / sample_size;

		// File Padding : Indices and positions are determined
		portion_size = line_count / numThreads;

		#pragma omp parallel for
		for (thread = 0; thread < numThreads; thread++){
			start_indices[thread] = portion_size * thread;
			finish_indices[thread] = start_indices[thread] + portion_size - 1;

			reads_start_pos[thread] = start_indices[thread] * sample_size;
			reads_finish_pos[thread] = (finish_indices[thread]+1) * sample_size;
		}
		reads_finish_pos[numThreads-1] = file_size;
		finish_indices[numThreads-1] = line_count - 1;

		thread = 1, i = 0;
		fseek(refs_file, 0, SEEK_SET);
		while (getline(&line, &line_size, refs_file) != -1){
			if ((i+1) % portion_size == 0){
				refs_start_pos[thread] = ftell(refs_file);
				refs_finish_pos[thread-1] = ftell(refs_file)-1;
				thread++;
			}
			i++;
		}
		fseek(refs_file, 0, SEEK_END);
		refs_finish_pos[numThreads-1] = ftell(refs_file);
		reads_start_pos[0] = 0;
		refs_start_pos[0] = 0;

		// for (thread = 0; thread < numThreads; thread++){
		// 	printf("file size = %ld, line size %ld, line count = %d\n", file_size, sample_size, line_count); 
		// 	printf("dev = %d, start_ind = %d, finish_ind = %d\n", thread, start_indices[thread], finish_indices[thread]); 
		// 	printf("dev = %d, reads_start_pos = %ld, reads_finish_pos = %ld\n", thread, reads_start_pos[thread], reads_finish_pos[thread]); 
		// 	printf("dev = %d, refs_start_pos = %ld, refs_finish_pos = %ld\n", thread, refs_start_pos[thread], refs_finish_pos[thread]); 
		// 	printf("\n"); 
		// } 
		// exit(1);

		// Prep to Multithreading
		fclose(reads_file);
		fclose(refs_file);

		// Main operation
		#pragma omp parallel for
		for (thread = 0; thread < numThreads; thread++){
			
			size_t temp_read_size, temp_ref_size;
			char *read = (char *)malloc((read_length+1) * sizeof(char));
			char *ref = (char *)malloc((read_length+1) * sizeof(char));
			int line_tracer, read_l, ref_l;
			char *read_line = NULL, *ref_line = NULL;
			size_t read_line_size = 0, ref_line_size = 0;
			int thread_sample_count = 0;

			FILE *thread_reads = fopen(reads_file_name, "r");
			FILE *thread_refs = fopen(refs_file_name, "r"); 
			fseek(thread_reads, reads_start_pos[thread], SEEK_SET);
			fseek(thread_refs, refs_start_pos[thread], SEEK_SET);

			filter[thread] = (short *)malloc((start_indices[thread] + portion_size - 1) * sizeof(short));
			edits[thread] = (int *)malloc((start_indices[thread] + portion_size - 1) * sizeof(int));

			while (getline(&read_line, &read_line_size, thread_reads) != -1 && getline(&ref_line, &ref_line_size, thread_refs) != -1){

				line_tracer = 0, read_l = 0, ref_l = 0;
				while(read_line[line_tracer] != '\n' && read_l < read_length){
					read[read_l] = read_line[line_tracer];
					line_tracer++;
					read_l++;
				}
				temp_read_size = read_l;
				read[read_length] = '\0';

				line_tracer = 0;
				while(ref_line[line_tracer] != '\n' && ref_l < read_length){
					ref[ref_l] = ref_line[line_tracer];
					line_tracer++;
					ref_l++;
				}
				temp_ref_size = ref_l;
				ref[read_length] = '\0';

				// EdlibAlignResult result = edlibAlign(ref, temp_ref_size, read, temp_read_size, edlibNewAlignConfig(error_threshold, EDLIB_MODE_HW, EDLIB_TASK_PATH, NULL, 0));
				EdlibAlignResult result = edlibAlign(ref, temp_ref_size, read, temp_read_size, 
					edlibNewAlignConfig(error_threshold, EDLIB_MODE_NW, EDLIB_TASK_PATH, NULL, 0));
				
				if (result.status == EDLIB_STATUS_OK) {
					if (result.editDistance != -1){
						filter[thread][thread_sample_count] = PASS;
					}
					else{
						filter[thread][thread_sample_count] = REJECT;
					}
					edits[thread][thread_sample_count] = result.editDistance;
				}
				edlibFreeAlignResult(result);
				
				// printf("line = %d done\n", count);
				thread_sample_count++;

			} // while

			free(read);
			free(ref);
			fclose(thread_reads);
			fclose(thread_refs);


		} //omp: thread

		// Reporting
		FILE *edits_output = fopen(edits_filename, "w");
		FILE *filter_output = fopen(filter_filename, "w");

		if (filter_output != NULL && edits_output != NULL){

			for (thread = 0; thread < numThreads; thread++){
				for (i = 0; i < (start_indices[thread] + portion_size - 1); i++){
					if (filter[thread][i] == PASS){
						passer_count[edits[thread][i]]++;
						fprintf(filter_output, "%d\n", PASS);
						(*accepted++);
					}
					else {
						fprintf(filter_output, "%d\n", REJECT);
						(*rejected++);
					}
					fprintf(edits_output, "%d\n", edits[thread][i]);
				}
				free(filter[thread]);
				free(edits[thread]);
			}
			fclose(filter_output);
			fclose(edits_output);
		}
		else{
			fprintf(stderr, "Error: Reporting failed.\n");
			for (thread = 0; thread < numThreads; thread++){
				if (filter[thread] != NULL)
					free(filter[thread]);
				if (edits[thread] != NULL)
					free(edits[thread]);
			}
		}
	}

	return line_count;
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
	if(argc < 7){
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
		int numThreads = atoi(argv[6]);
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
			read_length, undef_flag, &accepted, &rejected, &undef_count, numThreads);
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

