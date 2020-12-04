/*
Titel: binary_search
Beschreibung:
Autor: Patrick Wintner
Datum der letzten Bearbeitung: 02.12.2020
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
	int fd, n_Byte;
	void*buf = malloc(SIZE);
	
	fd = open(filename, O_RDONLY);
	if(fd == -1) return -1;
	
	for(int i = 0;;++i)
	{
		n_Byte = read(fd, buf, SIZE);
		if( n_Byte < SIZE)
		{
			free(buf);
			close(fd);
			if(n_Byte == -1)
			{
				return -1;
			}
			return i*SIZE+n_Byte;
		}	
	}
}

void *load_buffer(const char *filename, int n_Byte)
{
	if(n_Byte == -1) return NULL;

	int fd;
	void *buf = malloc(n_Byte);

	fd = open(filename, O_RDONLY);
	if(fd == -1) return NULL;

	if(read(fd, buf, n_Byte) == -1)
	{
		close(fd);
		return NULL;
	}
	close(fd);
	return buf;
}

char *binary_search(const char *str2find, char** min, char** max)
{
	if(min>max) {printf("-- Fatal Error: min > max --\n"); return NULL;}
	char **mid;
	mid = max-(max-min)/2;

	if(strcmp(str2find, *mid) < 0)
	{
		if(max == min) return NULL;
		max = mid-1;
		return binary_search(str2find, min, max);
	}
	else
	{
		if(strcmp(str2find, *mid) > 0)
		{
			if(max == min) return NULL;
			min = mid; // nicht +1, weil ansonsten Gefahr, dass min > max wird!!!
			return binary_search(str2find, min, max);
		}
		else return *mid;
	}
}

char *linear_search(const char *str2find, char **search_index, char **max)
{
	for(; search_index <= max; ++search_index)
	{
		if(strcmp(str2find, *search_index) <= 0)
		{
			if(strcmp(str2find, *search_index) == 0) return *search_index;
			else break;
		}
	}
	return NULL;
	/*
	if(search_index > max) return NULL;
	if(strcmp(str2find, *search_index) > 0)
	{
		return linear_search(str2find, ++search_index, max);
	}
	else
	{
		if(strcmp(str2find, *search_index) < 0) return NULL;
		else return *search_index;
	}
	*/
}

void *search(const char *str2find)
{
	int n_words = 0;
	char *buf;
	char *buf_start;
	char **search_index;
	char **search_index_start;

	int n_Byte = size_of_file("wortbuffer");
	printf("Anzahl Bytes: %i\n", n_Byte);
	if(n_Byte == -1) return NULL;
	buf = load_buffer("wortbuffer", n_Byte);
	if(buf == NULL) return NULL;

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

		gettimeofday(&tv_begin, NULL);
		void *res = search(input);
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
