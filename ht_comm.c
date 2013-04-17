/* ===========================================================================
 * HT COMM
 *
 * Author : Barbarossa
 * Description : Main module
 *		Tests the functionality of hash table.
 *
 * ===========================================================================
 */
#include "ht.h"
#include "hash.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*	String-keyed hash func implementations */
unsigned int ex_of_string_hash_fn(void*, unsigned int);
int 	string_key_cmp(void* key1, void* key2);	
char* 	string_to_string(void* key);


/*	I/O	Funcs		*/
#define BUF_LEN		256
void parse_execute_command(char* comm);
char* read_command_from_file(char* commandBuffer, int bufLength);
#define PRINT_CONSOLE()	printf("ht-com> ")

/* 	Global variables	*/
char*   inputFileName = NULL;
FILE*	pInputFile  = NULL;

char*   outputFileName_default = NULL;
char*   outputFileName = NULL;
FILE*	pOutputFile_default  = NULL;
FILE*	pOutputFile  = NULL;

hashTable_t* g_ht = NULL;


/* ------------------------------------------------------------------------ */
/*	Main	*/

int main(int argc, char **argv)
{
    char commandBuffer[BUF_LEN];
	int size;	

	/*	Incomplete nr of args	*/
    if(argc < 2) {
		printf("Usage : ht-comm   hash_table_size [input_file] [output_file]\n");
		exit(1);
	}

	/*	First arg = hash table size*/
	size = atoi(argv[1]);

	/*	(optional) Second arg = input file name	*/
    if(argc >= 3) {
        inputFileName = argv[2];
		pInputFile = fopen (inputFileName,"r");
    } 
	else {
		pInputFile = stdin ;
	}

	/*	(optional) Third arg = output file name	*/    
	if(argc == 4) {
        outputFileName_default = argv[3];
		pOutputFile_default = fopen (outputFileName_default,"w");
    } else {
		pOutputFile_default = stdout;
	}

	printf("Creating hash table (size %d)... \n", size);
	g_ht = ht_new( size, ex_of_string_hash_fn, 
						string_key_cmp,
						string_to_string,
						string_to_string); 
    
	printf("Reading and executing commands...\n");
	printf("To list available commands type 'help'\n");
	PRINT_CONSOLE();
	while(read_command_from_file(commandBuffer, BUF_LEN) != NULL) {
		parse_execute_command(commandBuffer);
		PRINT_CONSOLE();
	}
	
	if(pInputFile != stdin)	
		fclose(pInputFile);
	if(pOutputFile_default != stdout)	
		fclose(pOutputFile_default);

	return 0;
}


void parse_execute_command(char* comm) 
{
    char* commAux = strdup(comm);

    char* command = strtok (commAux, "\n");
	if(command == NULL)
		return;	
	
	char* commEl = strtok (command, " ");
	
    /* Add  */
	if(strcmp(commEl,"add") == 0){
        commEl = strtok (NULL," \n");
        while (commEl != NULL) {
            ht_entry_add(g_ht, strdup(commEl), NULL);
            commEl = strtok (NULL," \n");
        }
	}

    /* Remove  */
	else if(strcmp(commEl,"remove") == 0) {
        commEl = strtok (NULL," \n");
        while (commEl != NULL) {
            ht_entry_remove(g_ht, commEl);
            commEl = strtok (NULL," \n");
        }
	}

    /* Find  */
	else if(strcmp(commEl,"find") == 0) {
		char searchedKey[BUF_LEN] ;		
        bool_t found;
		
		/*	searched key	*/
		commEl = strtok (NULL," \n");
        if (commEl != NULL) {
			strcpy(searchedKey, commEl);
			if(!ht_entry_find(g_ht, commEl, NULL))
				found = FALSE;
			else 
				found = TRUE;
        }
		
		/*	output file	 */
		commEl = strtok (NULL," \n");
        if (commEl != NULL) {	
		    outputFileName = commEl;
			pOutputFile = fopen (outputFileName,"a");
		} else {
		    pOutputFile = pOutputFile_default;
		}
		
		fprintf(pOutputFile,"Search : %s = %s\n", searchedKey, (found == TRUE) ? "TRUE" : "FALSE");
		if(pOutputFile != pOutputFile_default)
			fclose(pOutputFile);
	}
	
	/* Clear */
	else if(strcmp(commEl,"clear") == 0) {
		ht_clear(g_ht);    		
	}

	/* Resize */
	else if(strcmp(commEl,"resize") == 0) {
		commEl = strtok (NULL," \n");
		/*  Half    */
		if(strcmp(commEl,"half") == 0) {
        	ht_resize(g_ht, (g_ht->size) / 2 );	
		}
		/*  Double  */ 
		else if (strcmp(commEl,"double") == 0) {
    		ht_resize(g_ht, (g_ht->size) * 2 );
		}
	}
	
    /* Print */
	else if(strcmp(commEl,"print") == 0) {
		commEl = strtok (NULL," \n");
		if(commEl != NULL) {
		    outputFileName = commEl;
			pOutputFile = fopen (outputFileName,"a");
		} else {
		    pOutputFile = pOutputFile_default;
		}
		ht_print(g_ht, pOutputFile);
	
		if(pOutputFile != pOutputFile_default)
			fclose(pOutputFile);
	}

    /* Print Bucket */
	else if(strcmp(commEl,"print_bucket") == 0) {
		int bIndex;
		
		/* Bucket index */
		commEl = strtok (NULL," \n");
		if(commEl == NULL) 
			printf("Usage : print_bucket $bucket_nr	[ouput_file]\n");
		else 
			bIndex = atoi(commEl);
		 
		/* File */
		commEl = strtok (NULL," \n");
		if(commEl != NULL) {
		    outputFileName = commEl;
			pOutputFile = fopen (outputFileName,"a");
		} else {
		    pOutputFile = pOutputFile_default;
		}
		ht_print_bucket(g_ht, bIndex, FALSE, pOutputFile);
	
		if(pOutputFile != pOutputFile_default)
			fclose(pOutputFile);
	}
    
	/* Exit */
	else if((strcmp(commEl,"help") == 0) || (strcmp(commEl,"?") == 0)){
		printf("\nAvailable commands : ");
		printf("\n add {keys-list}");
		printf("\n remove {keys-list}");
		printf("\n find {key} [output-file]");
		printf("\n clear ");
		printf("\n resize {half/double}");
		printf("\n print [output-file]");
		printf("\n print_bucket {bucket_nr} [output-file]");
		printf("\n exit");
		printf("\n");
	}

	/* Exit */
	else if(strcmp(commEl,"exit") == 0) {
		exit(0);
	}

	free(commAux);
}


char* 
read_command_from_file(char* commandBuffer, int bufLength)
{
    return fgets(commandBuffer, bufLength, pInputFile);
}


/* ------------------------------------------------------------------------ */
/*	String hash table funcs	*/

unsigned int ex_of_string_hash_fn(void* data, unsigned int size)
{
	//	defined in hash.h
	return hash((char*)data, size);
}


int string_key_cmp(void* key1, void* key2)
{
	return strcmp((char*)key1, (char*)key2);
}

char* 	string_to_string(void* key)
{
	return (char*)key;
}

