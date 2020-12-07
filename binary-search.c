/*
Titel: binary_search
Beschreibung:
Autor: Patrick Wintner
Datum der letzten Bearbeitung: 07.12.2020
*/

// -- Standardbibliotheken --
#include <stdlib.h>
#include <stdio.h>

// -- Dateiverwaltungsbibliotheken --
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <unistd.h>

// -- andere Bibliotheken --
#include <string.h>
#include <sys/time.h>
#include <stdbool.h> 

// -- Funktion int size_of_file --
// Parameter: const char *filename ... Dateiname des Wortbuffers
// Beschreibung: int size_of_file gibt die Dateigroesse in Bytes aus
// Rueckgabewert: entweder 
//	* Dateigroesse in Bytes oder
//	* -1 bei Auftreten eines Fehlers
int size_of_file(const char *filename)
{
	int SIZE = 1000; // Groesse des Buffers
	int RETURN_FAILURE = -1;

	int fd, n_Byte; //File-Deskriptor, Anzahl Bytes
	void*buf = malloc(SIZE); // Buffer
	
	fd = open(filename, O_RDONLY);
	if(fd == -1) return RETURN_FAILURE;
	
	for(int i = 0;;++i)
	{
		n_Byte = read(fd, buf, SIZE);
		if( n_Byte < SIZE) // Fehler oder am Dateiende angelangt
		{
			free(buf); //Buffer wird nicht mehr benoetigt
			close(fd);
			if(n_Byte == -1)
			{
				return RETURN_FAILURE;
			}
			return i*SIZE+n_Byte;
		}	
	}
}

// -- Funktion void *create_buffer --
// Parameter:
//	* const char *filename ... Dateiname des Wortbuffers
//	* int n_Byte ... Dateigroesse
// Beschreibung: create_buffer liest den Inhalt der Datei filename aus und schreibt diesen in
// in einen Buffer. Da dieser nicht freigegeben wird, steht dieser nach Beendigung der Funktion
// zur Verfügung.
// Rueckgabewert: entweder
//	* Startadresse des Buffers oder
//	* NULL bei Auftreten eines Fehlers
void *create_buffer(const char *filename, int n_Byte)
{
	void* RETURN_FAILURE = NULL;

	int fd; // File-Deskriptor
	void *buf = malloc(n_Byte); // Buffer zum Speichern der eingelesenen Daten
	if(buf == NULL) return RETURN_FAILURE;
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

// -- Funktion int count_words --
// Parameter:
//	* char *buf ... Buffer mit Worten, welche durch ein "0-Byte" getrennt werden
//	* int n_Byte ... Buffergroesse
// Beschreibung: count_words gibt die Anzahl an "0-Bytes" und damit die Anzahl an im 
// Buffer befindlichen Worten zurück, sofern auf jedes Wort ein solches "0-Byte" folgt.
// Rueckgabewert: entweder
//	* die Anzahl an Worten im Buffer oder
//	* -1 bei Auftreten eines Fehlers
int count_words(char *buf, int n_Byte)
{
	int RETURN_FAILURE = -1;

	if(buf == NULL) return RETURN_FAILURE;
	if(n_Byte < 1) return RETURN_FAILURE;

	char *buf_start; // markiert Anfang des Buffers
	int n_words = 0; // Anzahl Worte
	for(buf_start = buf; (buf - buf_start) < n_Byte; buf++)
	// Die Differenz buf - buf_start gibt die um eins verringerte Anzahl der gelesenen
	// Bytes an und kann daher als Abbruchbedingung verwendet werden
	{
		if (*buf == 0) n_words++; // 0en markieren das Ende von Worten und koennen daher
		// zum Zaehlen der Worte verwendet werden
	}
	return n_words;	
}

// -- Funktion char **setup_search_index --
// Parameter:
//	* char *buf ... Buffer mit Worten, welche durch ein "0-Byte" getrennt werden
//	* int n_Byte ... Buffergroesse
//	* int n_words ... Anzahl Worte im Buffer
// Beschreibung: setup_search_index gibt einen Pointer auf eine Folge von Pointern, welche
// auf die einzelnen Worte des Bufferszeigen, zurueck. Durch das Erhöhen oder Verniedrigen des
// Pointers kann somit auf die verschiedenen Worte gezeigt werden. 
// Rueckgabewert: entweder
//	* ein Pointer auf jenen Pointer, welcher auf das erste Wort zeigt, oder
//	* NULL bei Auftreten eines Fehlers
char **setup_search_index(char *buf, int n_Byte,  int n_words)
{
	char **RETURN_FAILURE = NULL;

	if(buf == NULL) return RETURN_FAILURE;
	if(n_Byte < 1) return RETURN_FAILURE;

	char *buf_start; // markiert Start des Buffers
	char **search_index; // Pointer auf Pointerfolge
	char **search_index_start; //speichert Anfangsadresse der Pointerfolge

	search_index=malloc(n_words*sizeof(char*)); // Allozieren des Speichers entsprechend
	//der Wortanzahl
	if(search_index == NULL) return RETURN_FAILURE;
	search_index_start = search_index; // Speichern des Startwertes
	*search_index = buf; //Erster Pointer der Pointerfolge zeigt nun auf erstes Wort
	for(buf_start = buf; (buf - buf_start) < n_Byte; buf++)
	// Schleife laeuft bis zum Ende des Buffers
	{
		if (*buf == 0) 
		{
			if(buf+1 == 0) break; // Ende des Buffers erreicht
			*(++search_index) = buf+1; // naechster Pointer der Pointerfolge wird
			// Adresse des naechsten Wortes zugewiesen
		}
	}
	return search_index_start; //Rueckgabe der Adresse des ersten Pointers der Pointerfolge
}

// -- Funktion char *binary_search --
// Parameter:
//	* const char *str2find ... zu suchende Zeichenfolge
//	* char** min ... "kleinstmoegliches" Wort
//	* char** max ... "groesstmoeglichstes" Wort
// Beschreibung: binary_search sucht die Zeichenfolge str2find im Buffer gemaess dem Prinzip
// der binaeren Suche, d. h. dass sie str2find mit jenem Wort vergleicht, welches sich mittig
// zwischen min und max befindet. Sollte str2find gleich diesem sein, so wird diese als
// Rueckgabewert ausgegeben, ansonsten wird die Funktion rekursiv mit geaenderten Grenzwerten
// (z. B. wuerde der min-Wert angepasst werden, sollte str2find "groesser" als das mittlere
// Wort sein) aufgerufen. Dies wird so lange fortgesetzt, bis entweder die Zeichenfolge
// gefunden wird oder aber mit Sicherheit feststeht, dass das Wort im Wortbuffer nicht vorkommt.
// Rueckgabewert: entweder
//	* die gefundene Zeichenfolge oder
//	* NULL, sollte die Zeichenfolge nicht gefunden worden sein
char *binary_search(const char *str2find, char** min, char** max)
{
	char *RETURN_FAILURE = NULL;
	if(min>max) {printf("-- Fatal Error: min > max --\n"); return RETURN_FAILURE;}

	char **mid;
	mid = max-(max-min)/2; // Berechnung der Adresse des mittleren Wortes zw. min und max;
	// Achtung: mid kann - wenn max-min <= 1 - gleich max werden!

	if(strcmp(str2find, *mid) < 0)
	{
		if(max == min) return RETURN_FAILURE; // Wort existiert im Wortbuffer nicht
		max = mid-1; // Maximalwert wird angepasst
		// -1, weil feststeht, dass str2find kleiner ist als mid und daher nicht mid
		// sein kann
		return binary_search(str2find, min, max); // rekursiver Funktionsaufruf
	}
	else
	{
		if(strcmp(str2find, *mid) > 0)
		{
			if(max == min) return RETURN_FAILURE; // Wort existiert im Wortbuffer nicht
			min = mid; // Minimalwert wird angepasst
			// nicht +1, weil ansonsten Gefahr, dass min > max wird, wenn max=mid!!!
			return binary_search(str2find, min, max); // rekursiver Funktionsaufruf
		}
		else return *mid; // String gefunden
	}
}

// -- Funktion char *linear_search --
// Parameter:
//	* const char *str2find ... zu suchende Zeichenfolge
//	* char** min ... "kleinstmoegliches" Wort
//	* char** max ... "groesstmoeglichstes" Wort
// Beschreibung: linear_search sucht str2find gemaess dem Prinzip der linearen Suche, d. h.,
// dass str2find zunaechst mit dem ersten Wort verglichen wird; bei Uebereinstimmung wird die
// entsprechende Zeichenfolge ausgeben, ansonsten wird str2find mit dem naechsten Wort verglichen,
// usw.
// Rueckgabewert: entweder
//	* die gefundene Zeichenfolge oder
//	* NULL, sollte die Zeichenfolge nicht gefunden worden sein
char *linear_search(const char *str2find, char **search_index, char **max)
{
	char *RETURN_FAILURE = NULL;

	for(; search_index <= max; search_index++)
	{
		if(strcmp(str2find, *search_index) <= 0)
		{
			if(strcmp(str2find, *search_index) == 0) return *search_index; // Zeichenfolge gefunden
			else break; // Zeichenfolge existiert nicht
		}
	}
	return RETURN_FAILURE;
}

// -- Funktion int setup_search --
// Parameter:
//	* const char *filename ... Dateiname des Wortbuffers
//	* char*** search_index_start ... Adresse des Pointers **search_index_start
//	* char*** search_index_end ... Adresse des Pointers **search_index_end
// Beschreibung: setup_search erstellt unter Verwendung der oben beschriebenen Funktkionen den
// Buffer, in welchem die Daten der Wortbuffer-Datei eingelesen werden, baut einen Suchindex
// auf und schreibt den Anfang als auch das Ende des Suchindexes in durch die uebergebenen
// Parameter search_index_start und search_index_end bestimmten Speicherorte
// Rueckgabewert:
//	* 0: kein Fehler aufgetreten
//	* -1: Fehler aufgetreten
int setup_search(const char *filename, char ***search_index_start, char ***search_index_end)
{
	int RETURN_FAILURE = -1;
	int RETURN_SUCCESS = 0;

	int n_Byte = size_of_file("wortbuffer");
	char *buf = create_buffer("wortbuffer", n_Byte);
	int n_words = count_words(buf, n_Byte);
	*search_index_start = setup_search_index(buf, n_Byte, n_words);
	if(search_index_start == NULL) return RETURN_FAILURE;
	*search_index_end = *search_index_start + n_words - 1;
	return RETURN_SUCCESS;
}

int main()
{
	int RETURN_FAILURE = -1;

	char **search_index_start;
	char **search_index_end;

	if(setup_search("wortbuffer", &search_index_start, &search_index_end) != 0) return RETURN_FAILURE;
	for (;;)
	{
		char input[100];

		fgets(input, sizeof(input), stdin);
		input[strlen(input)-1] = 0;

		if (!strlen(input)) break;

		struct timeval tv_begin, tv_end, tv_diff;

		gettimeofday(&tv_begin, NULL);
		void *res = binary_search(input, search_index_start, search_index_end);
		gettimeofday(&tv_end, NULL);
		//void *res = linear_search(input, search_index_start, search_index_end);
		timersub(&tv_end, &tv_begin, &tv_diff);

		if (res != NULL) {
		printf("found");
		} else {
		printf("not found");
		}
		printf(" in (%ld seconds %ld microseconds)\n", tv_diff.tv_sec, tv_diff.tv_usec);
	}
}
