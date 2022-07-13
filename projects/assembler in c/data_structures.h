#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef data_structures
#define data_structures

#define MAX_SYMBOL_SIZE 32
#define DH_COUNT_OF_BITS 16
#define DW_COUNT_OF_BITS 32
#define MAX_BITS 32
#define BYTE 8


enum attributes {data=1, code=2, entry=3, external=4};

typedef struct bits
{
    unsigned int value: 1;
}bits;

typedef struct bits_of_data_memory
{
    unsigned int value: 1;
	struct bits_of_data_memory* next;
	struct bits_of_data_memory *prev;

}bits_of_data_memory;

/* struct that holds opecode, funct, type of action and name of action */
typedef struct Table
{
	char* name;
	char type_of_action;
	int opcode_num;
	int funct_num;
	
}Table;

typedef struct symbol_line
{
	char symbol[MAX_SYMBOL_SIZE];
	int address;
	bool is_external;
	int symbol_attributes[2]; /* from enum: data, code, entry */
	struct symbol_line *next;
} symbol_line;

typedef struct symbol_list
{
	symbol_line *symbol_tail;
	symbol_line *symbol_head;	
}symbol_list;


/* struct for holding the IC \ DC (code memory word address \ data memory word address), 
an array of 10 bits (in chars), and a pointer to the next struct of the same type */
typedef struct memory_picture_code 
{ 
	int address;
	bits bit[MAX_BITS]; 
    struct memory_picture_code *next;
} memory_picture_code;

typedef struct memory_picture_code_list
{
	memory_picture_code *code_memory_tail;
	memory_picture_code *code_memory_head;
}memory_picture_code_list;

typedef struct memory_picture_data 
{ 
	int address;
	bits_of_data_memory* head;
	bits_of_data_memory* tail;
    struct memory_picture_data *next;
	struct memory_picture_data *prev;
} memory_picture_data;

typedef struct memory_picture_data_list
{
	memory_picture_data *data_memory_tail;
	memory_picture_data *data_memory_head;
}memory_picture_data_list;


typedef struct line_meaning 
{ 
	char instruction_type;
	char action_code[5]; 
	char Guidance_name[6];
	int num_of_bytes;
	int registers[3]; 
	char label_name[32]; 
	int immediate; 
	char label_as_data[32]; 
	char asciz[80];          
	int guidance_immed[80];  
	struct line_meaning *next;
} line_meaning;

typedef struct line_meaning_list
{
	line_meaning *line_head;
	line_meaning *line_tail;	
}line_meaning_list;

extern int DC;
extern symbol_list* p_symbol_list;
extern memory_picture_code_list* p_memory_picture_code_list;
extern memory_picture_data_list* p_memory_picture_data_list;
extern line_meaning_list* p_line_meaning_list;
extern char* ins_table[];
extern Table table[];

bool check_if_attribute_is_entry(char* current_symbol);

void add_to_symbol_table(char* current_symbol, int address, bool is_extern, int symbol_attributes);

bool label_exists(char* label_name);

int first_pass(FILE *file_ptr);

void add_to_line_meaning_linked_list(line_meaning_list* p_line_meaning_list, line_meaning* current_line);

void run_on_symbol_table_and_claasified_attribute_and_address(char* current_symbol, int symbol_attributes, int address);

void increase_DC(line_meaning* current_line, int* DC);

void add_to_memory_code_table(line_meaning* current_line);

void add_to_memory_data_table(line_meaning* current_line);

void add_asciz_to_memory_data_table(line_meaning* current_line, memory_picture_data* new_memory_data_pic);

void add_db_to_memory_data_table(line_meaning* current_line, memory_picture_data* new_memory_data_pic);

void add_dh_to_memory_data_table(line_meaning* current_line, memory_picture_data* new_memory_data_pic);

void add_dw_to_memory_data_table(line_meaning* current_line, memory_picture_data* new_memory_data_pic);

void add_R_sentence_to_memory_data_table(line_meaning* current_line, memory_picture_code* new_memory_pic); /* opcode, 3 registers, funct */

void add_I_sentence_to_memory_data_table(line_meaning* current_line, memory_picture_code* new_memory_pic);

void add_J_sentence_to_memory_data_table(line_meaning* current_line, memory_picture_code* new_memory_pic); /* opcode, 3 registers, func */

void copy_bits(bits memory_bit[], bits arr[], int start, int final);
void copy_bits_two(bits memory_bit[], bits arr[], int start, int final);
void copy_bits_three(bits_of_data_memory* head, bits arr[]);

void copy_bits_to_bits_list(memory_picture_data* pic ,bits arr[], int lenth);

void increase_DC_symbol_address_by_IC();

void give_address_to_data_memory_table(memory_picture_data *data_memory_head);

#endif