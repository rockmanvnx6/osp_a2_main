#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <unistd.h>
#include <math.h>
#define MAXCHAR 1000

typedef struct _Alloc {
	char * wpt;
	int wsize;
} Alloc;

int main(int argc, char* argv[])
{
    if(argc != 2){
		printf("The input file is not specified. Message from Ke!\n");
		return EXIT_FAILURE;
    }
    char const* const filename = argv[1]; /* should check that argc > 1 */

    FILE *fp;
    char str[MAXCHAR];

	fp = fopen(filename, "r");
	if (fp == NULL){
		printf("Could not open file %s",filename);
		return 1;
	}
	
	char * token;
	
	int arraySize = 10;
	Alloc** alloc = malloc(sizeof(Alloc*) * arraySize);
	
    if (!alloc) {
        perror("malloc");
        return 1;
    }
	
	int i = 0;
	while (fgets(str, MAXCHAR, fp) != NULL){
		token = strtok(str, "\n");
		size_t wsize= strlen(token)+1;
 
		void *request;
		request = sbrk(wsize);
		strcpy((char *)request,token);
		
		alloc[i] = (Alloc *)malloc(sizeof(Alloc));
        alloc[i]->wpt = request;
        alloc[i]->wsize  = wsize;
		
		i++;
		
		if(i==arraySize){
			break;
		}
	}
	
	fclose(fp);	
	
	int j;
	for(j=0; j<arraySize; j++){
		printf("starting address=%p, size=%d, p=%s\n",alloc[j]->wpt,alloc[j]->wsize, (char *)alloc[j]->wpt);
	}
	
	for (j = 0; j < arraySize; ++j) {
        free(alloc[j]);
    }
	free(alloc);
	
	return 0;
}
		


