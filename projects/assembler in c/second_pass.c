#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "data_structures.h"
#include "conversions.h"
#include "free_linked_list.h"
#include "second_pass.h"

#define OBJECT_FILE_EXTENSION ".ob"
#define EXTERN_FILE_EXTENSION ".ext"
#define ENTRY_FILE_EXTENSION ".ent"

FILE * object_fd;
FILE * externals_fd;
FILE * entries_fd;

/*defines*/
#define MAX_FILE_NAME_SIZE 100
char input_filename[MAX_FILE_NAME_SIZE];
char object_filename[MAX_FILE_NAME_SIZE]; 
char externals_filename[MAX_FILE_NAME_SIZE];
char entries_filename[MAX_FILE_NAME_SIZE];


void second_pass_func(char * filename)
{
    /*take the pointers of lists from first pass into temp pointers in second pass*/
    line_meaning* temp_current_line = p_line_meaning_list->line_head;
    memory_picture_code* temp_current_memory_picture_code = p_memory_picture_code_list->code_memory_head;

    
    int address_in_symbol_table, distance_addresses;
    bits address_label_I[15], address_label_J[25];

    strcpy(input_filename, filename);
    combine_filename_with_new_file_extension(input_filename, object_filename, OBJECT_FILE_EXTENSION);
    combine_filename_with_new_file_extension(input_filename, externals_filename, EXTERN_FILE_EXTENSION);
    combine_filename_with_new_file_extension(input_filename, entries_filename, ENTRY_FILE_EXTENSION);
    if (!open_second_pass_output_files()) 
    {
        return;
    }

    while(temp_current_line != NULL)
    {
        if(temp_current_line->label_as_data[0] != '\0') /*Coding of labels according to symbol table*/
        {
            if(temp_current_line->instruction_type == 'I')
            {
                address_in_symbol_table = label_address(temp_current_line->label_as_data);
                distance_addresses =  address_in_symbol_table - temp_current_memory_picture_code->address;
                convert_dec_to_x_bit_binary(distance_addresses, 16, address_label_I);
	            copy_bits(temp_current_memory_picture_code->bit,address_label_I,16,32);
            }
            if(temp_current_line->instruction_type == 'J')
            {
                address_in_symbol_table = label_address(temp_current_line->label_as_data);
                convert_dec_to_x_bit_binary(address_in_symbol_table, 25, address_label_J);
	            copy_bits(temp_current_memory_picture_code->bit,address_label_J,7,32);
            }
        }
		if(temp_current_line->action_code[0] != '\0')
		{	
        	temp_current_line = temp_current_line->next;
	        temp_current_memory_picture_code = temp_current_memory_picture_code->next;
		}
		else
		{
			temp_current_line = temp_current_line->next;

		}
    }

	print_hex_memory_code_list(p_memory_picture_code_list->code_memory_head,p_memory_picture_data_list->data_memory_head, object_fd);
	print_hex_memory_data_list(p_memory_picture_data_list->data_memory_head, object_fd);
	print_entry_file(p_symbol_list->symbol_head,entries_fd);
	print_extern_file(p_symbol_list->symbol_head, p_line_meaning_list->line_head, externals_fd, p_memory_picture_code_list->code_memory_head);

	free_data_memory_table(p_memory_picture_data_list->data_memory_head);
	free_code_memory_table(p_memory_picture_code_list->code_memory_head);
	free_symbol_table(p_symbol_list->symbol_head);
	free_lines_table(p_line_meaning_list->line_head);

    close_second_pass_output_files();
    
}
/*returns the label's address from symbole table*/
int label_address(char* label_name)
{
	symbol_line *temp = p_symbol_list->symbol_head; 

	while (temp!=NULL)
	{
		if (strcmp(temp->symbol, label_name) == 0)
			return temp->address; 
		else
			temp = temp->next;
	}
	return -1;
}

void combine_filename_with_new_file_extension(char * filename,char* output_filename, char * extension) 
{   
    char * filename_prefix; 
    char new_filename[MAX_FILE_NAME_SIZE];
    char copy_of_filename[MAX_FILE_NAME_SIZE];   
    strcpy(copy_of_filename, filename); 
    filename_prefix = strtok(copy_of_filename, "."); 
    strcpy(new_filename, filename_prefix); 
    strcat(new_filename, extension);     
    strcpy(output_filename, new_filename); 
}

int open_second_pass_output_files() 
{
    object_fd = fopen(object_filename, "w");
    externals_fd = fopen(externals_filename, "w");
    entries_fd = fopen(entries_filename, "w");
    if ((!object_fd) && (!externals_fd) && (!entries_fd)) 
    {
        fprintf(stderr, "ERROR: Could not open for write one or more of the assembler output files\n");
        return 0;
    }
    return 1;
}
/*printing memory code list in hex*/
void print_hex_memory_code_list(memory_picture_code* head, memory_picture_data* data_head, FILE* object_fd)
{
	memory_picture_code* temp = head;
	bits bitbox[BYTE];
	int count, start, end; 
	char hexNum[2];
	fprintf(object_fd,"\t %d \t", data_head->address-100);
	fprintf(object_fd,"%d\n", DC);
	while(temp)
	{
		start = 24;
		end = 32;
		fprintf(object_fd, "%d%d", 0,temp->address);
		fprintf(object_fd, "\t");
		for(count =0; count < 4; count++)
		{
			copy_bits_two(temp->bit, bitbox, start, end);
			convert_to_hex(bitbox, hexNum);
			fprintf(object_fd,"%s",hexNum);
			start-=BYTE;
			end  -=BYTE;
			fprintf(object_fd, "\t");
		}
		temp = temp->next;
		fprintf(object_fd,"\n");
	}
}
/*printing memory data list in hex*/
void print_hex_memory_data_list(memory_picture_data* head, FILE* object_fd)
{
	int i;
	memory_picture_data* temp = head;
	bits bitbox[BYTE];
	int count=0, address = temp->address;
	char hexNum[2];
	bits_of_data_memory* temp_binary_list;
	while(temp)
	{ 
		temp_binary_list = temp->head;
		while (temp_binary_list)
		{
			if(count == 0)
			{
				fprintf(object_fd, "%d%d", 0,address);
				fprintf(object_fd, "\t");
			}
			while(count<4)
			{
				copy_bits_three(temp_binary_list, bitbox);
				convert_to_hex(bitbox, hexNum);
				fprintf(object_fd,"%s",hexNum);
				fprintf(object_fd, "\t");
				count++;
				for(i=0; i<8; i++)
				{
					temp_binary_list = temp_binary_list->next;
				}
				if(!temp_binary_list)
					break;
			}
			if(!temp_binary_list)
					break;
			fprintf(object_fd,"\n");
			address+=4;
			count = 0;
		}
		temp = temp->next;	
	}
}

void print_entry_file(symbol_line* symbol_head,FILE* entries_fd)
{
	symbol_line* temp = symbol_head;
	while(temp)
	{
		if(temp->symbol_attributes[0] == entry || temp->symbol_attributes[1] == entry)
		{
			fprintf(entries_fd, "%s %d%d \n", temp->symbol, 0, temp->address);
		}
		temp = temp->next;
	}
}

void print_extern_file(symbol_line* symbol_head, line_meaning * line_head, FILE* externals_fd, memory_picture_code* memory_head)
{
	symbol_line* temp_symbol = symbol_head;
	line_meaning* temp_line;
	memory_picture_code* temp_memory;
	while(temp_symbol)
	{
		if(temp_symbol->symbol_attributes[0] == external)
		{	
			temp_line = line_head;
			temp_memory = memory_head;

			while(temp_line)
			{
				if((temp_line->instruction_type == 'J') && (strcmp(temp_line->label_as_data,temp_symbol->symbol)==0))
					fprintf(externals_fd, "%s %d%d \n", temp_symbol->symbol, 0, temp_memory->address);
				if(temp_line->action_code[0] != '\0')
				{
					temp_line = temp_line->next;
					temp_memory = temp_memory->next;
				}
				else
					temp_line = temp_line->next;
			}
		}
		temp_symbol = temp_symbol->next;
	}
}

void close_second_pass_output_files() 
{
    fclose(object_fd);
    fclose(externals_fd);
    fclose(entries_fd);
}