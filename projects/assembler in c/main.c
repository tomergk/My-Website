#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "analyze_line.h"
#include "conversions.h"
#include "data_structures.h"
#include "free_linked_list.h"
#include "second_pass.h" 

#define FILE_EXTENSION ".as"

int main(int argc, char * argv[]) 
{
    FILE *file_ptr;
    int i;

    /* verify if there are files as arguments in command line */ 
    if (argc == 1) /*if (argc == 1) is true, that means that no argument has passad in command line besides the name of exe file.*/
    {
        fprintf(stderr, "ERROR: You must enter at least 1 assembly (%s) input file name in the command line \n", FILE_EXTENSION);
        exit(1); 
    }

    /* verify files exists, files extension, file length and permissions */
    for (i = 1; i < argc; i++) 
    {
        char *filename = argv[i];

        /* check if there is a filename by checking length */
        if (strlen(filename) < 4) 
        {
            fprintf(stderr, "ERROR: File %s must have at least 4 chars.\n", filename);
            exit(0);
        }
        
        else /* check file extension if it is .as */
        { 
            char *extension = &filename[strlen(filename)-3];
            if (strcmp(extension, FILE_EXTENSION) != 0) 
            {
                fprintf(stderr, "ERROR: File %s has the wrong extension\n", filename);
                exit(0);
            }
        }
        
        /* checking if files exists and if permission is granted */
        if (!(file_ptr = fopen(filename, "r"))) 
        {
            fprintf(stderr, "ERROR: No such file: %s , or you don't have a reading permission for it\n", filename);
            exit(0);
        }
		fclose(file_ptr);
    }
    /*run on input files*/
    for (i = 1; i < argc; i++) 
    {
        char* filename = argv[i];
        file_ptr = fopen(argv[i], "r");
        printf("First pass: Assembler is running on file: %s\n", filename);
		if (first_pass(file_ptr))
        {
            printf("First pass: successfully completed running on file %s\n", filename);
            printf("Second pass: Assembler is running on file: %s\n", filename);

            second_pass_func(filename);
            printf("Second pass: successfully completed running on file %s\n\n", filename);
        }
        else 
        {
            printf("File ERROR! there are errors on first pass, therefore Second pass won't run.\n");
        }
        fclose(file_ptr);
    }
    return 0; 
}

