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

char *binary_search(const char *str2find, char *search_index[100], int min, int max)
{
	if(*search_index[(max+min)/2]<*str2find)
	{
		if(max=min) return NULL;
		max=(max+min)/2;
		binary_search(str2find, search_index, min, max);
		
	}
	else
	{
		if(*search_index[(max+min)/2]>*str2find)
		{
			if(max=min) return NULL;
			min=(max+min)/2;
			binary_search(str2find, search_index, min, max);
		}
		else
		{
			return search_index[max+min/2];
		}
	}
}

void *search(const char *str2find)
{
	int n = 0;
	char *buf;
	char *buf_temp;
	char **search_index;
	char **search_index_start;

	int n_Byte = size_of_file("wortbuffer");
	printf("Anzahl Bytes: %i\n", n_Byte);
	if(n_Byte == -1) return NULL;
	buf = load_buffer("wortbuffer", n_Byte);
	if(buf == NULL) return NULL;

	for(buf_temp = buf; (buf_temp - buf) < n_Byte; buf_temp++)
	{
		if (*buf_temp == 0) n++;
		//printf("%c", *buf_temp);
	}
	printf("Anzahl Worte: %i\n", n);

	search_index=malloc(n*sizeof(char*));
	search_index_start = search_index;
	*search_index = buf;
	search_index++;
	for(buf_temp = buf; (buf_temp - buf) < n_Byte; buf_temp++)
	{
		if (*buf_temp == 0) 
		{
			*search_index = buf_temp+1;
			search_index++;
		}
	}
	printf("%s\n", *search_index_start);
	return NULL;
	
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