/*
Titel: binary_search
Beschreibung:
Autor: Patrick Wintner
Datum der letzten Bearbeitung: 05.12.2020
*/

#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdbool.h> 

#include <string.h>
#include <sys/time.h>

int size_of_file(const char *filename)
{
	int SIZE = 1000;
	int RETURN_FAILURE = -1;

	int fd, n_Byte;
	void*buf = malloc(SIZE);
	
	fd = open(filename, O_RDONLY);
	if(fd == -1) return RETURN_FAILURE;
	
	for(int i = 0;;++i)
	{
		n_Byte = read(fd, buf, SIZE);
		if( n_Byte < SIZE)
		{
			free(buf);
			close(fd);
			if(n_Byte == -1)
			{
				return RETURN_FAILURE;
			}
			return i*SIZE+n_Byte;
		}	
	}
}

void *load_buffer(const char *filename, int n_Byte)
{
	void* RETURN_FAILURE = NULL;

	if(n_Byte == -1) return RETURN_FAILURE;

	int fd;
	void *buf = malloc(n_Byte);

	fd = open(filename, O_RDONLY);
	if(fd == -1) return RETURN_FAILURE;

	if(read(fd, buf, n_Byte) == -1)
	{
		close(fd);
		return RETURN_FAILURE;
	}
	close(fd);
	return buf;
}

int count_words(char *buf, int n_Byte)
{
	char *buf_start;
	int n_words = 0;
	for(buf_start = buf; (buf - buf_start) < n_Byte; buf++)
	{
		if (*buf == 0) n_words++;
	}
	return n_words;	
}

char **setup_search_index(char *buf, int n_Byte,  int n_words)
{
	char *buf_start;
	char **search_index;
	char **search_index_start;

	search_index=malloc(n_words*sizeof(char*));
	search_index_start = search_index;
	*search_index = buf;
	for(buf_start = buf; (buf - buf_start) < n_Byte; buf++)
	{
		if (*buf == 0) 
		{
			if(buf+1 == 0) break;
			*(++search_index) = buf+1;
		}
	}
	return search_index_start;
}
char **setup_search(void)
{
	char **RETURN_FAILURE = NULL;

	int n_Byte = size_of_file("wortbuffer");
	if(n_Byte == -1) return RETURN_FAILURE;
	char *buf = load_buffer("wortbuffer", n_Byte);
	if(buf == NULL) return RETURN_FAILURE;

	int n_words = count_words(buf, n_Byte);

	char **search_index = setup_search_index(buf, n_Byte, n_words);
	return search_index;
}

char **find_search_index_end(char **search_index)
{
	char **RETURN_FAILURE = NULL;

	int n_Byte = size_of_file("wortbuffer");
	if(n_Byte == -1) return RETURN_FAILURE;
	char *buf = load_buffer("wortbuffer", n_Byte);
	if(buf == NULL) return RETURN_FAILURE;

	int n_words = count_words(buf, n_Byte);
	char **search_index_end = search_index+n_words-1;
	return search_index_end;
}
char *binary_search(const char *str2find, char** min, char** max)
{
	char *RETURN_FAILURE = NULL;
	if(min>max) {printf("-- Fatal Error: min > max --\n"); return RETURN_FAILURE;}

	char **mid;
	mid = max-(max-min)/2;

	if(strcmp(str2find, *mid) < 0)
	{
		if(max == min) return RETURN_FAILURE;
		max = mid-1;
		return binary_search(str2find, min, max);
	}
	else
	{
		if(strcmp(str2find, *mid) > 0)
		{
			if(max == min) return RETURN_FAILURE;
			min = mid; // nicht +1, weil ansonsten Gefahr, dass min > max wird, wenn max=mid!!!
			return binary_search(str2find, min, max);
		}
		else return *mid;
	}
}

char *linear_search(const char *str2find, char **search_index, char **max)
{
	char *RETURN_FAILURE = NULL;
	for(; search_index <= max; search_index++)
	{
		if(strcmp(str2find, *search_index) <= 0)
		{
			if(strcmp(str2find, *search_index) == 0) return *search_index;
			else break;
		}
	}
	return RETURN_FAILURE;
}

void *search(const char *str2find)
{
	void *RETURN_FAILURE = NULL;

	int n_words = 0;
	char *buf;
	char *buf_start;
	char **search_index;
	char **search_index_start;

	int n_Byte = size_of_file("wortbuffer");
	printf("Anzahl Bytes: %i\n", n_Byte);
	if(n_Byte == -1) return RETURN_FAILURE;
	buf = load_buffer("wortbuffer", n_Byte);
	if(buf == NULL) return RETURN_FAILURE;

	for(buf_start = buf; (buf - buf_start) < n_Byte; buf++)
	{
		if (*buf == 0) n_words++;
	}
	printf("Anzahl Worte: %i\n", n_words);

	search_index=malloc(n_words*sizeof(char*));
	search_index_start = search_index;
	*search_index = buf_start;
	for(buf = buf_start; (buf - buf_start) < n_Byte; buf++)
	{
		if (*buf == 0) 
		{
			if(buf+1 == 0) break;
			*(++search_index) = buf+1;
		}
	}
	printf("Erstes Wort: %s\n", *search_index_start);
	printf("Letztes Wort: %s\n", *(search_index-1));
	char** search_index_end = search_index-1;
	for(search_index = search_index_start; search_index < search_index_start+n_words; search_index++)
	{
		binary_search(*search_index, search_index_start, search_index_end);
		binary_search(*search_index+1, search_index_start, search_index_end);
		binary_search(*search_index-1, search_index_start, search_index_end);
	}
	//return linear_search(str2find, search_index_start, search_index-1);
	return binary_search(str2find, search_index_start, search_index_end);
}

int main()
{
	for (;;)
	{
		char input[100];

		fgets(input, sizeof(input), stdin);
		input[strlen(input)-1] = 0;

		if (!strlen(input)) break;

		struct timeval tv_begin, tv_end, tv_diff;

		char **search_index_start = setup_search();
		char **search_index_end = find_search_index_end(search_index_start);
		gettimeofday(&tv_begin, NULL);
		void *res = binary_search(input, search_index_start, search_index_end);
		//void *res = linear_search(input, search_index_start, search_index_end);
		//void *res = search(input);
		gettimeofday(&tv_end, NULL);

		timersub(&tv_end, &tv_begin, &tv_diff);

		if (res != NULL) {
		printf("found");
		} else {
		printf("not found");
		}
		printf(" in (%ld seconds %ld microseconds)\n", tv_diff.tv_sec, tv_diff.tv_usec);
	}
}
