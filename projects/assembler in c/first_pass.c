#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "data_structures.h"
#include "free_linked_list.h"
#include "analyze_line.h"
#include "conversions.h"

/*defines*/
#define LINE_LENGTH 81
#define CODE_TABLE_START_ADDRESS 100

/*globals*/
char *current_symbol;
int line_number;
int IC;
int DC;
symbol_list* p_symbol_list;
memory_picture_code_list* p_memory_picture_code_list;
memory_picture_data_list* p_memory_picture_data_list;
line_meaning_list* p_line_meaning_list;
line_meaning *current_line;
char* ins_table[] = {"db","dw","dh","asciz","entry","extern"};
Table table[] = { 
	{ "add",  'R',0,1},
	{ "sub",  'R',0,2},
	{ "and",  'R',0,3},
	{ "or",   'R',0,4},
	{ "nor",  'R',0,5},
	{ "move", 'R',1,1},
	{ "mvhi", 'R',1,2},
	{ "mvlo", 'R',1,3},
	{ "addi", 'I',10,0},
	{ "subi", 'I',11,0},
	{ "andi", 'I',12,0},
	{ "ori",  'I',13,0},
	{ "nori", 'I',14,0},
	{ "bne",  'I',15,0},
	{ "beq",  'I',16,0},
	{ "blt",  'I',17,0},
    { "bgt",  'I',18,0},
    { "lb",  'I',19,0},
    { "sb",  'I',20,0},
    { "lw",  'I',21,0},
    { "sw",  'I',22,0},
    { "lh",  'I',23,0},
    { "sh",  'I',24,0},
    { "jmp", 'J',30,0},
    { "la",  'J',31,0},
    { "call",'J',32,0},
    { "stop",'J',63,0}
};

int first_pass(FILE *file_ptr)
{
	char line[LINE_LENGTH];
	bool syntax_errors = false;
	char* error;
	line_number = 0; 
	IC = CODE_TABLE_START_ADDRESS;
	DC = 0;

	/* memory dynamic allocation for all linked lists and error buffer */
	error = (char*)calloc(100,sizeof(char));
	if (!error)
	{
		printf("Memory allocation for error buffer failed! \n");
		exit(1);
	}

	p_symbol_list = (symbol_list*)calloc(1,sizeof(symbol_list));
	if (!p_symbol_list)
	{
		printf("Memory allocation for new symbol list failed! \n");
		exit(1);
	} 

	p_memory_picture_code_list = (memory_picture_code_list*)calloc(1,sizeof(memory_picture_code_list));
	if (!p_memory_picture_code_list)
	{
		printf("Memory allocation for new memory picture code list failed! \n");
		exit(1);
	} 
	p_memory_picture_data_list = (memory_picture_data_list*)calloc(1,sizeof(memory_picture_data_list)); 
	if (!p_memory_picture_data_list)
	{
		printf("Memory allocation for new memory picture data list failed! \n");
		exit(1);
	} 
	p_line_meaning_list = (line_meaning_list*)calloc(1,sizeof(line_meaning_list)); 
	if (!p_line_meaning_list)
	{
		printf("Memory allocation for new line meaning list failed! \n");
		exit(1);
	} 
	p_symbol_list->symbol_head = NULL;
	p_symbol_list->symbol_tail = NULL;
	p_memory_picture_data_list->data_memory_tail = NULL;
	p_memory_picture_data_list->data_memory_head = NULL;
	p_memory_picture_code_list->code_memory_tail = NULL;
	p_memory_picture_code_list->code_memory_head = NULL;
	p_line_meaning_list->line_head = NULL;
	p_line_meaning_list->line_tail = NULL;
	current_line = NULL;
	
	/* read one sentence in a time from the file */
	while(fgets(line,LINE_LENGTH,file_ptr)) 
	{
		guidance_count_immed_num = 0;	
		line_number++;
		if(line[0] == ';') /* If its a note sentence*/
			continue;
		
		if (only_white_spaces(line)) /* if line is empty */
			continue;
        
		current_line = crossing_on_line(line, line_number, error, &syntax_errors);
	
		/*add symbols to symbol tabel according the sentence */
		if((current_line->action_code[0] != '\0') && (current_line->instruction_type != 'j')) 
		{
			if (current_line->label_name[0] != '\0') /*if there is a label in current sentence*/
			{
				if (!label_exists(current_line->label_name)) 
					add_to_symbol_table(current_line->label_name, IC, false, code);
				else if(check_if_attribute_is_entry(current_line->label_name))
					run_on_symbol_table_and_claasified_attribute_and_address(current_line->label_name, code, IC);
				else
				{
					printf("ERROR: line %d: label %s is already exists in symbol table \n", line_number, current_line->label_name);
					syntax_errors = true;
				}	
			}
			add_to_memory_code_table(current_line); /* add sentence to memory table */
		}
		else if((strcmp(current_line->Guidance_name, "entry") == 0) || (strcmp(current_line->Guidance_name, "extern") == 0))
		{
			if(strcmp(current_line->Guidance_name, "entry") == 0)
			{
				if(!label_exists(current_line->label_as_data))
					add_to_symbol_table(current_line->label_as_data, DC, false, entry);
				else
					run_on_symbol_table_and_claasified_attribute_and_address(current_line->label_as_data, entry,DC);
			}
			else /*extern*/
			{
				if (!label_exists(current_line->label_as_data)) 
					add_to_symbol_table(current_line->label_as_data, 0, true, external); 
			}
				
		}
		else /* guidance line - asciz, db, dh, dw */
		{
			if (current_line->label_name[0] != '\0') /*if there is a label in current sentence*/
			{
				if (!label_exists(current_line->label_name)) 
					add_to_symbol_table(current_line->label_name, DC, false, data);
				else if(check_if_attribute_is_entry(current_line->label_name))
					run_on_symbol_table_and_claasified_attribute_and_address(current_line->label_name, data,DC);
				else
				{
					printf("ERROR: line %d: label %s is already exists in symbol table \n", line_number, current_line->label_name);
					syntax_errors = true;
				}
			}
			add_to_memory_data_table(current_line); /* add sentence to memory table */
		}

		if(current_line->action_code[0] != '\0')
			IC += 4;
		else
			increase_DC(current_line, &DC);

		if (p_line_meaning_list->line_head == NULL)
		{ 
			p_line_meaning_list->line_head = current_line;
			p_line_meaning_list->line_tail = p_line_meaning_list->line_head;
		}
		else 
		{
			p_line_meaning_list->line_tail->next = current_line;
			p_line_meaning_list->line_tail = p_line_meaning_list->line_tail->next;
		}
		current_line = NULL;
	}
	
	if (syntax_errors) 
	{
		/*free linked lists */
		free_data_memory_table(p_memory_picture_data_list->data_memory_head);
		free_code_memory_table(p_memory_picture_code_list->code_memory_head);
		free_symbol_table(p_symbol_list->symbol_head);
		free_lines_table(p_line_meaning_list->line_head);
		return false;
	}
	else
	{
		increase_DC_symbol_address_by_IC();
		give_address_to_data_memory_table(p_memory_picture_data_list->data_memory_head);
		return true;
	}
}

/* the function check if label is already exist in symbol table */
bool label_exists(char* label_name)
{
	symbol_line *temp = p_symbol_list->symbol_head; 

	while (temp!=NULL)
	{
		if (strcmp(temp->symbol, label_name) == 0)
			return true; 
		else
			temp = temp->next;
	}
	return false;
}


/* adding_symbols_to symbol table: */  
void add_to_symbol_table(char* current_symbol, int address, bool is_extern, int symbol_attributes)
{
	symbol_line* new_symbol = (symbol_line*)calloc(1,sizeof(symbol_line));
	if (!new_symbol)
	{
		printf("Memory allocation for new symbol failed! \n");
		exit(1);
	}
	
	strcpy(new_symbol->symbol,current_symbol); 

	new_symbol->address = address;
	new_symbol->is_external = is_extern;
	new_symbol->symbol_attributes[0] = symbol_attributes;
	
	if(!p_symbol_list->symbol_head) 
	{
		p_symbol_list->symbol_head = new_symbol;
		p_symbol_list->symbol_tail = p_symbol_list->symbol_head;
	}
	else 
	{
		p_symbol_list->symbol_tail->next = new_symbol;
		p_symbol_list->symbol_tail = p_symbol_list->symbol_tail->next;
	}

	p_symbol_list->symbol_tail->next = NULL;

}

/* if symbol is alredy exist and the attribute of symbol is entry - we will add atrribute to the symbol: data/code */
void run_on_symbol_table_and_claasified_attribute_and_address(char* current_symbol, int symbol_attributes, int address)
{
	symbol_line *temp = p_symbol_list->symbol_head; 

	while (temp!=NULL)
	{
		if (strcmp(temp->symbol, current_symbol) == 0)
		{
			if(symbol_attributes != entry)
				temp->address = address;
			temp->symbol_attributes[1] = symbol_attributes;
			return;
		}
		else
			temp = temp->next;
	}
}

/* if symbol is already exist, the function check if the attribute of the symbol is entry */
bool check_if_attribute_is_entry(char* current_symbol)
{
	symbol_line *temp = p_symbol_list->symbol_head; 

	while (temp!=NULL)
	{
		if (strcmp(temp->symbol, current_symbol) == 0)
		{
			if(temp->symbol_attributes[0] == entry)
				return true;
		}
		else
			temp = temp->next;
	}
	return false;
}


void increase_DC(line_meaning* current_line, int* DC)
{
	if(strcmp(current_line->Guidance_name, "asciz")==0)
		*DC += length_asciz;
	if(strcmp(current_line->Guidance_name, "db")==0)
		*DC += guidance_count_immed_num;
	if(strcmp(current_line->Guidance_name, "dh")==0)
		*DC += guidance_count_immed_num * 2;
	if(strcmp(current_line->Guidance_name, "dw")==0)
		*DC += guidance_count_immed_num * 4;	
}

void add_to_memory_code_table(line_meaning* current_line)
{
	memory_picture_code* new_memory_code_pic;
	new_memory_code_pic = (memory_picture_code*)calloc(1,sizeof(memory_picture_code));
	if (!new_memory_code_pic)
	{
		fprintf(stderr, "Memory allocation for new memory picture failed!");
		exit(1);
	}
	
	if(current_line->instruction_type =='R')
		add_R_sentence_to_memory_data_table(current_line, new_memory_code_pic);
	else if(current_line->instruction_type =='I')
		add_I_sentence_to_memory_data_table(current_line, new_memory_code_pic);
	else /* J */
		add_J_sentence_to_memory_data_table(current_line, new_memory_code_pic);
	
	new_memory_code_pic->address = IC;
	if (p_memory_picture_code_list->code_memory_head == NULL) 
	{ 
		p_memory_picture_code_list->code_memory_head = new_memory_code_pic;
		p_memory_picture_code_list->code_memory_tail = p_memory_picture_code_list->code_memory_head;
	}
	else 
	{
		p_memory_picture_code_list->code_memory_tail->next = new_memory_code_pic;
		p_memory_picture_code_list->code_memory_tail = p_memory_picture_code_list->code_memory_tail->next;
	}
}

void add_to_memory_data_table(line_meaning* current_line)
{
	memory_picture_data* new_memory_data_pic;
	bits_of_data_memory* list;
	new_memory_data_pic = (memory_picture_data*)calloc(1,sizeof(memory_picture_data));
	if (!new_memory_data_pic)
	{
		fprintf(stderr, "Memory allocation for new memory picture failed!");
		exit(1);
	}
	list = (bits_of_data_memory*)calloc(1,sizeof(bits_of_data_memory));
	if (!list)
	{
		printf("Memory allocation for new bits_of_data_memory list failed! \n");
		exit(1);
	} 
	new_memory_data_pic->head = NULL;
	if(strcmp(current_line->Guidance_name,"asciz")==0)
		add_asciz_to_memory_data_table(current_line,new_memory_data_pic);
	else if(strcmp(current_line->Guidance_name,"db")==0)
		add_db_to_memory_data_table(current_line,new_memory_data_pic);
	else if(strcmp(current_line->Guidance_name,"dh")==0)
		add_dh_to_memory_data_table(current_line,new_memory_data_pic);
	else /*dw */
		add_dw_to_memory_data_table(current_line,new_memory_data_pic);

	new_memory_data_pic->address=DC;
	if (p_memory_picture_data_list->data_memory_head == NULL) 
	{ 
		p_memory_picture_data_list->data_memory_head = new_memory_data_pic;
		p_memory_picture_data_list->data_memory_tail = p_memory_picture_data_list->data_memory_head;
	}
	else 
	{
		p_memory_picture_data_list->data_memory_tail->next = new_memory_data_pic;
		p_memory_picture_data_list->data_memory_tail = p_memory_picture_data_list->data_memory_tail->next;
	}
}

void add_asciz_to_memory_data_table(line_meaning* current_line, memory_picture_data* new_memory_data_pic)
{
	bits ascii[BYTE];
	int i=1;
	while(current_line->asciz[i] != '"')
	{
		convert_dec_to_x_bit_binary(current_line->asciz[i], BYTE, ascii); 
		copy_bits_to_bits_list(new_memory_data_pic, ascii, BYTE);
		i++;
	
	}
	convert_dec_to_x_bit_binary(0, BYTE, ascii); /* convert '\0' */
	copy_bits_to_bits_list(new_memory_data_pic, ascii, BYTE);
}

void add_db_to_memory_data_table(line_meaning* current_line, memory_picture_data* new_memory_data_pic)
{
	bits number[BYTE]; 
	int i=0;
	for(;i<guidance_count_immed_num; i++)
	{
		convert_dec_to_x_bit_binary(current_line->guidance_immed[i], BYTE, number); 
		copy_bits_to_bits_list(new_memory_data_pic, number, BYTE);
	}
}

void add_dh_to_memory_data_table(line_meaning* current_line, memory_picture_data* new_memory_data_pic)
{
	bits number[BYTE*2]; 
	bits little_endien[BYTE];
	int i=0, start, end ;
	for(;i<guidance_count_immed_num; i++)
	{
		/* add bits to memory table by little endian method */
		start = DH_COUNT_OF_BITS-BYTE;
		end = DH_COUNT_OF_BITS;
		convert_dec_to_x_bit_binary(current_line->guidance_immed[i], BYTE*2, number); 
		while(start >= 0) 
		{
			copy_bits_two(number,little_endien,start,end);
			copy_bits_to_bits_list(new_memory_data_pic, little_endien, BYTE);
			start -= BYTE;
			end -= BYTE;
		}
	}
}

void add_dw_to_memory_data_table(line_meaning* current_line, memory_picture_data* new_memory_data_pic)
{
	bits number[BYTE*4];
	bits little_endien[BYTE];
	int i=0, start , end;
	for(;i<guidance_count_immed_num; i++)
	{
		/* add bits to memory table by little endian method */
		start = DW_COUNT_OF_BITS-BYTE;
		end = DW_COUNT_OF_BITS;
		convert_dec_to_x_bit_binary(current_line->guidance_immed[i], BYTE*4, number);
		while(start >= 0)
		{
			copy_bits_two(number,little_endien,start,end);
			copy_bits_to_bits_list(new_memory_data_pic, little_endien, BYTE);
			start -= BYTE;
			end -= BYTE;
		}
	}
}

void add_R_sentence_to_memory_data_table(line_meaning* current_line, memory_picture_code* new_memory_pic)
{
	bits opcode[6], funct[5], rs[5], rt[5], rd[5]; 
	
	convert_dec_to_x_bit_binary(table[action_code_line_number].opcode_num, 6, opcode);
	copy_bits(new_memory_pic->bit,opcode,0,6);
	convert_dec_to_x_bit_binary(table[action_code_line_number].funct_num, 5, funct);
	copy_bits(new_memory_pic->bit,funct,21,26);
	if(table[action_code_line_number].opcode_num == 0) /* actions such as add,sub.. with 3 registers */
	{
		convert_dec_to_x_bit_binary(current_line->registers[0], 5, rs);
		copy_bits(new_memory_pic->bit,rs,6,11);
		convert_dec_to_x_bit_binary(current_line->registers[1], 5, rt);
		copy_bits(new_memory_pic->bit,rt,11,16);
		convert_dec_to_x_bit_binary(current_line->registers[2], 5, rd);
		copy_bits(new_memory_pic->bit,rd,16,21);
	}
	else /*actions such as mvlo.. with 2 registers */
	{
		convert_dec_to_x_bit_binary(current_line->registers[0], 5, rs);
		copy_bits(new_memory_pic->bit,rs,6,11);
		convert_dec_to_x_bit_binary(current_line->registers[1], 5, rd);
		copy_bits(new_memory_pic->bit,rd,16,21);
	}
}

void add_I_sentence_to_memory_data_table(line_meaning* current_line, memory_picture_code* new_memory_pic) 
{
	bits opcode[6], rs[5], rt[5], immed[16]; 
	
	convert_dec_to_x_bit_binary(table[action_code_line_number].opcode_num, 6, opcode);
	copy_bits(new_memory_pic->bit,opcode,0,6);
	convert_dec_to_x_bit_binary(current_line->registers[0], 5, rs);
	copy_bits(new_memory_pic->bit,rs,6,11);
	if(current_line->action_code[0] == 'b') /* action bgt,blt,bne,beq with label_as_data */
		convert_dec_to_x_bit_binary(current_line->registers[1], 5, rt);	
	else
		convert_dec_to_x_bit_binary(current_line->registers[2], 5, rt);
	copy_bits(new_memory_pic->bit,rt,11,16);

	if(current_line->label_as_data[0] == '\0')
	{
		convert_dec_to_x_bit_binary(current_line->immediate, 16, immed);
		copy_bits(new_memory_pic->bit,immed,16,32);
	}
	
}
/* J */
void add_J_sentence_to_memory_data_table(line_meaning* current_line, memory_picture_code* new_memory_pic) 
{
	bits opcode[6], is_register[1], register_or_label_address[25];
	
	convert_dec_to_x_bit_binary(table[action_code_line_number].opcode_num, 6, opcode);
	copy_bits(new_memory_pic->bit,opcode,0,6);
	if(!strcmp(current_line->action_code,"stop")==0)
	{
		if(current_line->label_as_data[0] == '\0')
		{
			is_register->value=1; 
			copy_bits(new_memory_pic->bit,is_register,6,7);
			convert_dec_to_x_bit_binary(current_line->registers[0], 25, register_or_label_address);
			copy_bits(new_memory_pic->bit,register_or_label_address,7,32);
		}
	}
}

/* the function is getting 2 arrays of bits and delimeter of indexs, and copy the bits in delimeter to the new array */
void copy_bits(bits memory_bit[], bits arr[], int start, int final)
{
	int i = 0;
	for(; start<final; start++)
	{
		memory_bit[start].value = arr[i].value; 
		i++;
	}
}

/* function alike "copy bits" but comparing between the arrays is reverse */
void copy_bits_two(bits memory_bit[], bits arr[], int start, int final)
{
	int i = 0;
	for(; start<final; start++)
	{
		arr[i].value = memory_bit[start].value; 
		i++;
	}
}

/* function to copy bits from the linked lists bits in data memory to new array*/
void copy_bits_three(bits_of_data_memory* head, bits arr[])
{
	int i = 0,count = 0;
	bits_of_data_memory* temp = head;
	while(count<BYTE)
	{
		arr[i].value = temp->value;
		temp = temp->next;
		count++;
		i++;
	}
}

/* function to add bits from array to linked list bits in data memory */
void copy_bits_to_bits_list(memory_picture_data* pic ,bits arr[], int length)
{
	bits_of_data_memory* node, *temp;
	int i;
	for(i = 0; i < length; i++)
	{
		node = (bits_of_data_memory*)calloc(1,sizeof(bits_of_data_memory));
		node->value = arr[i].value;
		if (pic->head == NULL) /* empty list */
		{ 
			pic->head = node;
			temp = pic->head;
			pic->tail = pic->head;
			pic->head->prev = NULL;
		}
		else 
		{
			pic->tail->next = node;
			pic->tail = pic->tail->next; 
			pic->tail->prev = temp;
			temp = pic->tail; 
		}
	}

}

/* crossing on symbol table and increase addresses of data symbols according to current IC */
void increase_DC_symbol_address_by_IC()
{
	symbol_line *temp;
	temp = p_symbol_list->symbol_head;
	
	while(temp) 
	{
		if (temp->symbol_attributes[0] == data || temp->symbol_attributes[1] == data)
			temp->address += IC;
	
		temp = temp->next;
	}

}

/* function that crossing on data memory table and increase addresses according to current IC */
void give_address_to_data_memory_table(memory_picture_data *data_memory_head)
{
	memory_picture_data *temp;
	temp = p_memory_picture_data_list->data_memory_head;
	
	while(temp) 
	{	
		temp->address += IC;
		temp = temp->next;
	}
}