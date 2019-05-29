#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


// PAGE STRUCTURE
struct page {
	unsigned value;
	struct page* next;
	int dirty;
};


void enqueue(unsigned val, int dirty, struct page **ptr, struct page **prev);
void saveToList(unsigned val, struct page **ptr);
struct page* dequeue(struct page** ptr, struct page** prev);
int removeFromList(unsigned val, struct page **ptr);

void LRU(FILE * file, int nframes, int debug);
void LRUinsert(unsigned val, int dirty, int nframes, int debug);
int LRUfind(unsigned val, int dirty, int debug);

void FIFO(FILE * file, int nframes, int debug);
void FIFOinsert(unsigned val, int dirty, int nframes, int debug);
int FIFOfind(unsigned val, int dirty, int debug);

void VMS(FILE * file, int nframes, int debug);
void VMSprocessA(unsigned val, int dirty, int nframes, int debug);
void VMSprocessB(unsigned val, int dirty, int nframes, int debug);
void removeClean(unsigned val);
void removeDirty(unsigned val);


struct page* ptr = NULL;
struct page* prev = NULL;

struct page* ptrDisk = NULL;

struct page* ptrPA = NULL;
struct page* prevPA = NULL;

struct page* ptrPB = NULL;
struct page* prevPB = NULL;

struct page* ptrClean = NULL;
struct page* prevClean = NULL;

struct page* ptrDirty = NULL;
struct page* prevDirty = NULL;

struct page* ptrMem = NULL;

clock_t start, end;
double cpu_time_used;

int frms = 0;
int frmsPA = 0;
int frmsPB = 0;

int ntraces = 0;
int nreads = 0;
int nwrites = 0;
int nhits = 0;
int nmisses = 0;


void enqueue(unsigned val, int dirty, struct page **ptr, struct page **prev) 
{
	struct page* new_node = (struct page*)malloc(sizeof(struct page));
	new_node->value = val;
	new_node->next = NULL;
	new_node->dirty = dirty;

	if(*ptr == NULL && *prev == NULL)
	{
		*ptr = *prev = new_node;
		return;
	}

	(*prev)->next = new_node;
	*prev = new_node;
}


void saveToList(unsigned val, struct page **ptr)
{
	struct page* new_node = (struct page*)malloc(sizeof(struct page));
	new_node->value = val;
	new_node->next = NULL;

	if(*ptr == NULL)
		*ptr = new_node;
	else
	{
		new_node->next = *ptr;
		*ptr = new_node;
	}
}


struct page* dequeue(struct page** ptr, struct page** prev) 
{
	struct page* new_node = *ptr;

	if(*ptr == NULL) 
		return NULL;
	
	if(*ptr == *prev) 
		*ptr = *prev = NULL;
	else 
		*ptr = (*ptr)->next;
	
	return new_node;
}


int removeFromList(unsigned val, struct page **ptr)
{
	struct page* aux = *ptr;
	struct page* prev = NULL;
	
	while(aux != NULL)
	{
		if(aux->value == val)
		{
			if(prev != NULL)
				prev->next = aux->next;
			else
				*ptr = aux->next;
			
			free(aux);
			return 1;
		}
		prev = aux;
		aux = aux->next;
	}
	return 0;
}


void removeClean(unsigned val)
{
	struct page* aux = ptrClean;
	struct page* prev = NULL;

	while(aux != NULL)
	{
		if(aux->value == val)
		{
			if(aux == ptrClean)
			{
                if(aux == prevClean)
				{
                    ptrClean = NULL;
                    prevClean = NULL;
                }
				else
                    ptrClean = ptrClean->next;
                
			}
			else if(aux == prevClean)
			{
			    if(prev == ptrClean)
                    prevClean = ptrClean;
			    else
			        prevClean = prev;
			    
			}
			else
			    prev->next = aux->next;
            
			free(aux);

			return;
		}
		prev = aux;
		aux = aux->next;
	}
}

void removeDirty(unsigned val)
{
	struct page* aux = ptrDirty;
	struct page* prev = NULL;

	while(aux != NULL)
	{
		if(aux->value == val)
		{
			if(aux == ptrDirty)
			{
                if(aux == prevDirty)
				{
                    ptrDirty = NULL;
                    prevDirty = NULL;
                }
				else
                    ptrDirty = ptrDirty->next;
			}
			else if(aux == prevDirty)
			{
			    if(prev == ptrDirty)
                    prevDirty = ptrDirty;
			    else
			        prevDirty = prev;
			    
			}
			else
				prev->next = aux->next;
            
			free(aux);
			return;
		}
		prev = aux;
		aux = aux->next;
	}
}


//////////////////////////////////////////////////////////////////////////////
///////////////////////////////// LRU ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void LRU(FILE * file, int nframes, int debug)
{
	unsigned addr;
	char rw;

	while(fscanf(file, "%x %c", &addr, &rw) > 0)
	{
		addr = addr >> 12;
		int dirty = 0;
		
		if(rw=='W') 
			dirty = 1;

		LRUinsert(addr, dirty, nframes, debug);
		ntraces++;
	}
	printf("[*] Total Memory Frames: %d\n", nframes);
	printf("[*] Events in Trace: %d\n", ntraces);
	printf("[*] Total Disk Reads: %d\n", nreads);
	printf("[*] Total Disk Writes: %d\n", nwrites);
	if (debug == 1)
	{
		printf("[*] Number of Hits: %d\n", nhits);
		printf("[*] Number of Misses: %d\n", nmisses);
		double hitrate = (double)nhits / ((double)(nhits+nmisses));
		printf("[*] Hit Rate: %.1f\n", hitrate);

	}
}

//Method to insert a given page to the memory with the LRU algorithm
void LRUinsert(unsigned val, int dirty, int nframes, int debug)
{
    if(LRUfind(val, dirty, debug))
		nhits++;
    else
	{
        if (debug == 1) 
			printf("Miss!\n");
		
		nmisses++;
        
		if(frms == nframes)
		{
            struct page* aux = dequeue(&ptr, &prev);

			if(aux->dirty == 1)
			{
				saveToList(aux->value, &ptrDisk);
				nwrites++;
			}
			free(aux);

			if(removeFromList(val, &ptrDisk) == 1)
				nreads++;
            
			enqueue(val, dirty, &ptr, &prev);
        }
		else
		{
            enqueue(val, dirty, &ptr, &prev);
			frms++;
        }
    }
}


int LRUfind(unsigned val, int dirty, int debug)
{
    struct page* aux = ptr;
    struct page* prev = NULL;

    while(aux != NULL)
	{
        if(aux->value == val)
		{
			if (debug == 1) 
				printf("Hit!\n");

			if(aux->dirty == 1) 
				dirty = aux->dirty;

			if(ptr == prev) 
				return 1;
				
			if(aux == ptr)
			{
				ptr = aux->next;
				free(aux);
				enqueue(val, dirty, &ptr, &prev);
			}
			
			if(aux != prev)
			{
				prev->next = aux->next;
				free(aux);
				enqueue(val, dirty, &ptr, &prev);
			}

            return 1;
        }
        prev = aux;
        aux = aux->next;
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////// FIFO ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void FIFO(FILE * file, int nframes, int debug)
{
	unsigned addr;
	char rw;
	start = clock();

	while(fscanf(file, "%x %c", &addr, &rw) > 0)
	{
		addr = addr >> 12;
		int dirty = 0;
		if(rw == 'W') 
			dirty = 1;

		FIFOinsert(addr, dirty, nframes, debug);
		ntraces++;
	}
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

	printf("[*] Total Memory Frames: %d\n", nframes);
	printf("[*] Events in Trace: %d\n", ntraces);
	printf("[*] Total Disk Reads: %d\n", nreads);
	printf("[*] Total Disk Writes: %d\n", nwrites);
	if (debug == 1)
	{
		printf("[*] Number of Hits: %d\n", nhits);
		printf("[*] Number of Misses: %d\n", nmisses);
		double hitrate = (double)nhits / ((double)(nhits+nmisses));
		printf("[*] Hit Rate: %.1f\n", hitrate);
		printf("[*] Time: %.2f\n", cpu_time_used);

	}
}


void FIFOinsert(unsigned val, int dirty, int nframes, int debug)
{
    if(FIFOfind(val, dirty, debug))
		nhits++;
    else
	{
		if (debug == 1) 
			printf("Miss!\n");

		nmisses++;

        if(frms==nframes)
		{
            struct page* aux = dequeue(&ptr, &prev);

			if(aux->dirty == 1)
			{
				saveToList(aux->value, &ptrDisk);
				nwrites++;
			}
			free(aux);
			
			if(removeFromList(val, &ptrDisk) == 1)
				nreads++;

            enqueue(val, dirty, &ptr, &prev);
        }
		else
		{
            enqueue(val, dirty, &ptr, &prev);
			frms++;
        }
    }
}


int FIFOfind(unsigned val, int dirty, int debug)
{
    struct page* aux = ptr;

    while(aux != NULL)
	{
        if(aux->value == val)
		{
			if (debug == 1)
				printf("Hit!\n");

			if(aux->dirty == 0)
				if(dirty == 1) 
					aux->dirty = dirty;

            return 1;
        }
        aux = aux->next;
    }
    return 0;
}


//////////////////////////////////////////////////////////////////////////////
///////////////////////////////// VMS ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void VMS(FILE * file, int nframes, int debug)
{
	unsigned addr;
	char rw;
	start = clock();
	while(fscanf(file, "%x %c", &addr, &rw) > 0)
	{
		addr = addr >> 12;
		int dirty = 0;
		if(rw=='W')
			dirty = 1;

		unsigned first = addr >> 16;
		
		if(first == 3)
			VMSprocessA(addr, dirty, nframes, debug);
		else
			VMSprocessB(addr, dirty, nframes, debug);

		ntraces++;
	}
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("[*] Total Memory Frames: %d\n", nframes);
	printf("[*] Events in Trace: %d\n", ntraces);
	printf("[*] Total Disk Reads: %d\n", nreads);
	printf("[*] Total Disk Writes: %d\n", nwrites);
	if (debug == 1)
	{
		printf("[*] Number of Hits: %d\n", nhits);
		printf("[*] Number of Misses: %d\n", nmisses);
		double hitrate = (double)nhits / ((double)(nhits+nmisses));
		printf("[*] Hit Rate: %.1f\n", hitrate);
		printf("[*] Time: %.2f\n", cpu_time_used);

	}
}


int VMSfind(unsigned val, int dirty, struct page **ptr)
{
	struct page* aux = *ptr;

    while(aux != NULL)
	{
        if(aux->value == val)
		{
			if(aux->dirty == 0)
				if(dirty == 1) 
					aux->dirty = dirty;
			return 1;
        }
        aux = aux->next;
    }
    return 0;
}


void VMSprocessA(unsigned val, int dirty, int nframes, int debug)
{
	if(VMSfind(val, dirty, &ptrPA) == 1)
		nhits++;
	else
	{
		if (debug == 1) 
			printf("Miss! Process A\n");

		if(removeFromList(val, &ptrDisk) == 1)
			nreads++;
		
		if(frmsPA == nframes / 2)
		{
			struct page* aux = dequeue(&ptrPA, &prevPA);

			if(aux->dirty == 1)
				enqueue(aux->value, aux->dirty, &ptrDirty, &prevDirty);
			else
				enqueue(aux->value, aux->dirty, &ptrClean, &prevClean);
			
			enqueue(val, dirty, &ptrPA, &prevPA);
			free(aux);
		}
		else
		{
			enqueue(val, dirty, &ptrPA, &prevPA);
			frmsPA++;
		}
		if(VMSfind(val, dirty, &ptrMem))
		{
			if (debug == 1) 
				printf("Hit! Process A\n");

			removeClean(val);
			removeDirty(val);
			nhits++;
		}
		else
		{
			nmisses++;
			if (debug == 1)
				printf("Miss! Process A \n");

			if(frms == nframes)
			{
				struct page* aux = dequeue(&ptrClean, &prevClean);
				
				if(aux == NULL)
				{
					aux = dequeue(&ptrDirty, &prevDirty);
					nwrites++;
					saveToList(aux->value, &ptrDisk);
				}

				removeFromList(aux->value, &ptrMem);
				free(aux);
				saveToList(val, &ptrMem);
			}
			else
			{
				saveToList(val, &ptrMem);
				frms++;
			}
		}
	}
}


void VMSprocessB(unsigned val, int dirty, int nframes, int debug)
{
	if(VMSfind(val, dirty, &ptrPB))
	{
		if (debug == 1)
            printf("Hit! Process B\n");
		nhits++;
	}
	else
	{
		if (debug == 1)
            printf("Miss! Process B\n");

		if(removeFromList(val, &ptrDisk) == 1)
			nreads++;
		
		if(frmsPB == nframes/2)
		{
			struct page* aux = dequeue(&ptrPB, &prevPB);

			if(aux->dirty == 1)
				enqueue(aux->value, aux->dirty, &ptrDirty, &prevDirty);
			else
				enqueue(aux->value, aux->dirty, &ptrClean, &prevClean);
			
			enqueue(val, dirty, &ptrPB, &prevPB);
			free(aux);
		}
		else
		{
			enqueue(val, dirty, &ptrPB, &prevPB);
			frmsPB++;
		}
		if(VMSfind(val, dirty, &ptrMem))
		{
			if (debug == 1)
				printf("Hit! Process B\n");

			removeClean(val);
			removeDirty(val);
			nhits++;
		}
		else
		{
			nmisses++;
			if (debug == 1) 
				printf("Miss! Process B\n");

			if(frms == nframes)
			{
				struct page* aux = dequeue(&ptrClean, &prevClean);
				if(aux == NULL)
				{
					aux = dequeue(&ptrDirty, &prevDirty);
					nwrites++;

					saveToList(aux->value, &ptrDisk);
				}

				removeFromList(aux->value, &ptrMem);
				free(aux);
				saveToList(val, &ptrMem);

			}
			else
			{
				saveToList(val, &ptrMem);
				frms++;
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
///////////////////////////////// MAIN ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	int option = -1;
	int nframes = 0;
	char algorithm[100];
	char opt[100];
	int debug = 0;
	FILE * file;



	file = fopen(argv[1], "r");
	nframes = atoi(argv[2]);
	strcpy(algorithm, argv[3]);
	strcpy(opt, argv[4]);

	char aux0[] = "quiet";
	if(strcmp(opt, "quiet") == 0)
		debug = 0;
	else
		debug = 1;


	char aux1[] = "lru";
	if (strcmp(algorithm, aux1) == 0)
		option = 1;
	char aux2[] = "fifo";
	if (strcmp(algorithm, aux2) == 0)
		option = 2;
	char aux3[] = "vms";
	if (strcmp(algorithm, aux3) == 0)
		option = 3;
	



	switch(option)
	{
		case 1 :
			printf("--- LRU: Last Recently Used --\n");
			LRU(file, nframes, debug);
			break;
		case 2 :
			printf("--- FIFO: First in First Out --\n");
			FIFO(file, nframes, debug);
			break;
		case 3 :
			printf("--- VMS --\n");
			VMS(file, nframes, debug);
			break;

		default:
			printf("Invalid algorithm.\n");


	}

return 0;
}
