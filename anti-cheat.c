//Lithio (the developer's pseudonym)
//January 31, 2021
//
//Cheaters beware!!!
//
//Usage: select all code files to be evaluated and run it with this program (i.e.
//       all code file paths will be put in the program parameters.)
//       
//       This program asks for two threshold values such that if a comparison between
//       two files exceeds it, a flag will be made. The first threshold is the
//       minimum score for which if the number of semicolons and curly braces match,
//       a flag is made. The second threshold is the minimum score for which all
//       comparisons will be flagged.
//       
//       This program will output a file called "analysis.txt". Be sure to check it!
//       Suggested files that are in conflict with each other will be listed with a
//       "[CAUTION]" and should be checked manually.
//
//Analysis used:
//       Two source files are compared by calculating a convolution between the two
//       files. Instead of multiplication between elements, a comparison is made.
//       The number of semicolons and curly braces are also calculated.
//


//for the unfortunate people using Visual Studio
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h> //for memset()

//returns the length of the file in binary mode (will always be greater than or equal
//to the length of the file in text mode)
//
//returns -1 upon failure to open the file
int get_file_length(const char *filename);

//returns file data read in TEXT MODE as a character array, used for the below functions
char *get_file_data(const char *filename, int file_length);

//returns the number of '\n' characters in the text string (regardless of OS used)
int get_num_lines(const char *data);

//returns the number of ';' character in the text string
int get_num_semicolons(const char *data);

//returns the number of '{' and '}' characters
int get_num_braces(const char *data);

//convolutes two bodies of text by evaluating 1 whether two characters match or 0 when not
//and by summing up the number of matches
int get_convolution_sum_of_matches(const char *data1, const char *data2, int *max_value, float *max_value_per_length);

int main(int argc, char *argv[]){
	char *data1;
	char *data2;
	int conv_sum;
	int max_conv_value;
	float max_value_per_length;
	
	int num_files;
	char **data;
	FILE *fptr;
	
	int i, j;
	int success = 1;
	
	float th1, th2;
	int sc1, sc2;
	int bc1, bc2;
	int comparison_count = 0;
	int total_comparisons;
	int num_caution = 0;

	if(argc < 2){
		printf("Correct usage: <file1> <file2> ...\n");
		return -1;
	}		
	
	data = (char**) malloc(sizeof(char*) * (argc - 1));
	
	//read all file data
	for(i = 1; i < argc; ++i){
		data[i - 1] = get_file_data(argv[i], get_file_length(argv[i]));
		
		if(data[i - 1] == NULL){
			success = 0;
			break;
		}
	}
	
	if(!success){
		printf("error reading one of the files\n");
		return -1;
	}
	
	fptr = fopen("analysis.txt", "wt");
	if(fptr == NULL){
		printf("could not create 'analysis.txt'\n");
		return -1;
	}
	
	printf("Enter the threshold value 1 (0.0-1.0): ");
	scanf("%f", &th1);
	printf("Enter the threshold value 2 (0.0-1.0): ");
	scanf("%f", &th2);
	
	
	total_comparisons = (argc - 2) * (argc - 2 + 1) / 2;
	
	for(i = 1; i < argc; ++i){
		for(j = i + 1; j < argc; ++j){
			data1 = data[i - 1];
			data2 = data[j - 1];
			
			fprintf(fptr, "%s versus %s\n", argv[i], argv[j]);
			fprintf(fptr, "num lines : %d vs %d\n", get_num_lines(data1), get_num_lines(data2));
			sc1 = get_num_semicolons(data1);
			sc2 = get_num_semicolons(data2);
			fprintf(fptr, "num semicolons: %d vs %d\n", sc1, sc2);
			
			bc1 = get_num_braces(data1);
			bc2 = get_num_braces(data2);
			fprintf(fptr, "num braces: %d vs %d\n", bc1, bc2);
	
			conv_sum = get_convolution_sum_of_matches(data1, data2, &max_conv_value, &max_value_per_length);
			
			fprintf(fptr, "conv sum: %d\n", conv_sum);
			fprintf(fptr, "max conv: %d\n", max_conv_value);
			fprintf(fptr, "max conv/length: %f\n", max_value_per_length);
			
			if(max_value_per_length > th1 && sc1 == sc2 && bc1 == bc2){
				fprintf(fptr, "[CAUTION]");
				num_caution++;
			}
			else if(max_value_per_length > th2){
				fprintf(fptr, "[CAUTION]");
				num_caution++;
			}
			
			fprintf(fptr, "\n\n");
			
			comparison_count++;
		}
		printf("\rDone %5d comparisons of %5d, found %d [CAUTION]", comparison_count, total_comparisons, num_caution);
	}
	
	printf("\nSuccessfully ran analysis with %d [CAUTION]. Check analysis.txt.\n", num_caution);
	
	fclose(fptr);
	
	for(i = 1; i < argc; ++i){
		free(data[i - 1]);
	}
	free(data);
	
	return 0;
}

int get_file_length(const char *filename){
	FILE *fptr = NULL;
	int len = 0;
	char buf;
	
	fptr = fopen(filename, "rb");
	
	if(fptr == NULL){
		return -1;
	}
	
	//count the length of the file, byte by byte...
	//sorry if a more efficient method exists!
	while(fread(&buf, 1, 1, fptr) == 1){
		len++;
	}
	
	fclose(fptr);
	
	return len;
}

char *get_file_data(const char *filename, int file_length){
	FILE *fptr;
	char *line_buf;
	char *file_buf;
	
	//open the file in TEXT MODE this time
	fptr = fopen(filename, "rt");
	
	if(fptr == NULL){
		return NULL;
	}
	
	line_buf = (char*) malloc(file_length + 1);
	file_buf = (char*) malloc(file_length + 1);
	
	//set the bytes of each string to 0
	memset(line_buf, 0, file_length + 1);
	memset(file_buf, 0, file_length + 1);
	
	//read line by line and concatenate
	while(fgets(line_buf, file_length + 1, fptr)){
		strcat(file_buf, line_buf);
	}
	
	free(line_buf);
	fclose(fptr);
	
	return file_buf;
}

//returns the number of '\n' characters in the text string (regardless of OS used)
int get_num_lines(const char *data){
	int i;
	int len;
	int count = 0;
	
	len = strlen(data) + 1;
	
	for(i = 0; i < len; ++i){
		if(data[i] == '\n'){
			count++;
		}
	}
	
	return count;
}

//returns the number of ';' character in the text string
int get_num_semicolons(const char *data){
	int i;
	int len;
	int count = 0;
	
	len = strlen(data) + 1;
	
	for(i = 0; i < len; ++i){
		if(data[i] == ';'){
			count++;
		}
	}

	return count;
}

//returns the number of '{' and '}' characters
int get_num_braces(const char *data){
	int i;
	int len;
	int count = 0;
	
	len = strlen(data) + 1;
	
	for(i = 0; i < len; ++i){
		if(data[i] == '{' || data[i] == '}'){
			count++;
		}
	}

	return count;
}

//convolutes two bodies of text by evaluating 1 whether two characters match or 0 when not
//and by summing up the number of matches
int get_convolution_sum_of_matches(const char *data1, const char *data2, int *max_value, float *max_value_per_length){
	int i, j;
	int len1, len2;
	int padded_length;
	char *padded_string;
	
	int count;
	int sum = 0;
	
	*max_value = 0;
	*max_value_per_length = 0.0;
	
	//unpadded string length
	len1 = strlen(data1) + 1;
	len2 = strlen(data2) + 1;
	
	//create padded string using data2
	padded_length = 2*len1 + len2 - 2;
	padded_string = (char*) malloc(padded_length);
	
	//the padded bytes will have a value that doesn't exist in text files
	memset(padded_string, 255, padded_length);
	
	memcpy(padded_string + len1 - 1, data2, len2);
	
	//convolute data1 and the data2 (the padded string)
	for(i = 0; i < len1 + len2 - 2; ++i){
		count = 0;
		
		for(j = 0; j < len1; ++j){
			if(data1[j] == padded_string[i + j]){
				count++;
			}
		}
		
		if(count > *max_value){
			*max_value = count;
			
			if(len1 > len2){
				*max_value_per_length = (float)(count)/(float)(len2);
			}
			else{
				*max_value_per_length = (float)(count)/(float)(len1);
			}
		}
		
		sum += count;
	}
	
	free(padded_string);
	
	return sum;
}


