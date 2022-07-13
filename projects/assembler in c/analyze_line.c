#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "data_structures.h"
#include "analyze_line.h"


int action_code_line_number;
int guidance_number_of_bytes;
int guidance_count_immed_num=0;
int length_asciz;


/* function that crossing on every sentence on input file and insert the sentence values to line_meaning struct */
line_meaning* crossing_on_line(char line[], int line_number, char* error, bool* syntax_error)
{
	char delim[] = ", \t\n\r";
    char* token = strtok(line, delim);
	char copy_label[MAX_SYMBOL_SIZE];
    int count_reg = 0, ins_I_place = 1;
	bool flag_having_action_yet = false;
	bool flag_having_operands = false;  
	State state;

    line_meaning* current_line = (line_meaning*)calloc(1,sizeof(line_meaning));
	if (!current_line)
	{
		printf("Memory allocation for new line failed! \n");
		exit(1);
	}
	
	while(token != NULL) 
    {
		
		if(only_white_spaces(token) == true)
		{
			token = strtok(NULL, delim);
			continue;
		}
		if(!flag_having_action_yet) /* yet we can't know the type of sentence */
		{
			state = analyzing_token(token,error);
			switch(state) /* insert token into the right value in struct */
			{
				case label:
					memcpy(copy_label, token, strlen(token)-1); /* earse char ':' */
					strcpy(current_line->label_name, copy_label);
					break;
				case guidance: 
					strcpy(current_line->Guidance_name, token+1); 
					current_line->num_of_bytes = guidance_number_of_bytes;/* 1 OR 2 OR 4 bytes */
					flag_having_action_yet = true;
					break;
				case action_code: 
					strcpy(current_line->action_code, token);
					current_line->instruction_type = table[action_code_line_number].type_of_action;
					current_line->num_of_bytes = 4;
					flag_having_action_yet = true;
					if(strcmp(current_line->action_code, "stop") == 0)
						flag_having_operands = true;
					break; 
				case entry_or_external:
					strcpy(current_line->Guidance_name, token+1);
					current_line->num_of_bytes = guidance_number_of_bytes;
					flag_having_action_yet = true;
					break;
				case not_valid: 
					printf("ERROR: line %d: %s \n", line_number, error);
					error[0] = '\0';
					*syntax_error = true;
					break;
			}
			
		}
		else /*flag is true, meaning that finished instructions and moving to operands*/
		{
			if(current_line->Guidance_name[0] != '\0') 
			{
				if((strcmp(current_line->Guidance_name, "entry")==0) || (strcmp(current_line->Guidance_name, "extern") ==0))
				{
					check_label_for_entry_or_extern(token, current_line, error, line_number, syntax_error);
					flag_having_operands = true;
				}
				else
				{
					if(strcmp(current_line->Guidance_name, "asciz")==0)
					{
						check_operands_for_asciz(token,current_line,line_number,syntax_error);
						flag_having_operands = true;
					}
					else
					{
						check_operands_for_d_guidances(token, current_line,error, line_number,syntax_error);
						flag_having_operands = true;
					}
				}
			}
			else if(current_line->instruction_type == 'R')
			{
				check_operands_for_R(token,error,&count_reg, current_line, line_number, syntax_error);
				count_reg++;
				flag_having_operands = true;
			}
			
			else if(current_line->instruction_type == 'I')
			{
				check_operands_for_I(token,error,&count_reg,&ins_I_place, current_line, line_number, syntax_error);
				count_reg++;
				ins_I_place++;
				flag_having_operands = true;
			}
			else /* J*/ 
			{
				check_operands_for_J(token,error,current_line, line_number, syntax_error);
				flag_having_operands = true;
			}
		}
	if(strcmp(current_line->Guidance_name, "asciz")==0) /* read the string until the end of sentence */
		token = strtok(NULL,"\n");
	else
    		token = strtok(NULL, delim); 
    }

	if(!flag_having_operands)
	{
		printf("ERROR: line %d: there are no operands in the line \n", line_number);
		*syntax_error = true; 
	}
	return current_line;
}

/* the function check's if string is fitting to string rules for asciz */
void check_operands_for_asciz(char* token, line_meaning* current_line, int line_number, bool* syntax_error)
{
	length_asciz = strlen(token)-1;
	if(current_line->asciz[0] == '\0') 
	{
		while(token[length_asciz] != '"' && length_asciz > 1) /* earse spaces from token */
		{
			length_asciz--;
		}
		if(token[0] == '"' && token[length_asciz] == '"' && length_asciz > 1) /* not an empty string */
			strcpy(current_line->asciz, token);
		else if(token[0] == '"' && token[length_asciz] == '"' && length_asciz == 1)
		{
			printf("ERROR: line %d: asciz string is empty \n", line_number);
			*syntax_error = true;
		}
		else
		{
			printf("ERROR: line %d: operand %s is invalid according to string rules \n", line_number, token);
			*syntax_error = true; 
		}
	}
	else
	{
		printf("ERROR: line %d: too much operands after asciz guidance \n", line_number);
		*syntax_error = true; 
	}
}

/* function is checking the numbers that given in dh,dw,db sentences */
void check_operands_for_d_guidances(char* token, line_meaning* current_line,char* error, int line_number, bool* syntax_error)
{
	is_immed(token, error);
	if(error[0] == '\0')
		current_line->guidance_immed[guidance_count_immed_num++] = atoi(token);
	else
	{	
		printf("ERROR: line %d: %s \n", line_number, error);
		error[0] = '\0';
		*syntax_error = true;
	}
}

void check_label_for_entry_or_extern(char* token, line_meaning* current_line,char* error, int line_number, bool* syntax_error) 
{
	if(current_line->label_as_data[0] == '\0')
	{
		is_label(token,error,true);
		if(error[0]=='\0')
		{
			strcpy(current_line->label_as_data, token);
		}
		else
		{
			printf("ERROR: line %d: %s \n", line_number, error);
			error[0] = '\0';
			*syntax_error = true;
		}
	}
	else
	{
		printf("ERROR: line %d: too much operands after label word \n", line_number);
		*syntax_error = true;
	}
}

void check_operands_for_R(char* token,char* error, int* count_reg, line_meaning* current_line, int line_number, bool* syntax_error)
{
	is_reg(token,error);
	if(error[0]=='\0')
	{
		if((strcmp(current_line->action_code, "move")==0) || (strcmp(current_line->action_code, "mvhi")==0) || (strcmp(current_line->action_code, "mvlo"))==0)
		{
			/* actions with 2 registers */
			if(*count_reg < 2)
			{
				current_line->registers[*count_reg] = atoi(token+1);
			}
			else
            {
				printf("ERROR: line %d: too much registers on a copy instruction \n", line_number);
                *syntax_error = true;
            }
		}
		else
		{
			if(*count_reg<3)
			{
				current_line->registers[*count_reg] = atoi(token+1);
			}
			else
            {
				printf("ERROR: line %d: too much registers on an aritmetic instruction \n", line_number);
                *syntax_error = true;
            }
		}
	}
	else
	{
		printf("ERROR: line %d: %s \n", line_number, error);
		error[0] = '\0';
        *syntax_error = true;
	}
}

void check_operands_for_I(char* token,char* error, int* count_reg, int* ins_I_place, line_meaning* current_line, int line_number, bool* syntax_error)
{
	if(current_line->action_code[0]=='b') /* action bgt, blt, bne, beq, with label_as_data */
	{
		if(*count_reg<2)
		{
			is_reg(token,error);
			if(error[0] == '\0')
			{
				current_line->registers[*count_reg] = atoi(token+1);
			}
			else
			{
				printf("ERROR: line %d: %s \n", line_number, error);
                *syntax_error = true;
			}
		}
		else if(*count_reg==2) /* check the label_as_data*/
		{
			is_label(token,error,true);
			if(error[0] == '\0')
			{
				strcpy(current_line->label_as_data, token);
			}
			else
			{
				printf("ERROR: line %d: %s \n", line_number, error);
				error[0]='\0';
                *syntax_error = true;
			}
		}
		else
        {
			printf("ERROR: line %d: too much operands in the sentence \n", line_number);
            *syntax_error = true;
        }
	}
	else /* I sentences with 2 registers and immediate */
	{	
		if(*ins_I_place == 1 || *ins_I_place == 3)
		{
			is_reg(token,error);
			if(error[0]=='\0')
				current_line->registers[*ins_I_place-1] = atoi(token+1);
			else
			{	
				printf("ERROR: line %d: %s \n", line_number, error);
				error[0] = '\0';
                *syntax_error = true;
			}
		}
		else if(*ins_I_place == 2)
		{
			is_immed(token, error);
			if(error[0]=='\0')
			{
				current_line->immediate = atoi(token);
			}
			else
			{	
				printf("ERROR: line %d: %s \n", line_number, error);
				error[0]='\0';
                *syntax_error = true;
			}
		}
		else
        {
			printf("ERROR: line %d: too much operands in the sentence \n", line_number);
            *syntax_error = true;
        }
    }
}

void check_operands_for_J(char* token, char* error, line_meaning* current_line, int line_number, bool* syntax_error)
{
	if(strcmp(current_line->action_code, "stop") == 0)
		printf("ERROR: line %d: cannot be operands after stop instruction \n", line_number);
	if((strcmp(current_line->action_code, "la") == 0) || (strcmp(current_line->action_code, "call") == 0))
	{
		is_label(token,error,true);
		if(error[0]=='\0')
			strcpy(current_line->label_as_data, token);
		else
		{	
			printf("ERROR: line %d: %s \n", line_number, error);



			error[0]='\0';
            *syntax_error = true;
		}
	}
	else /* action_code == "jmp" */
 	{
		if(token[0] == '$')
		{
			is_reg(token,error);
			if(error[0]=='\0')
				current_line->registers[0]=atoi(token+1);
			else
			{	
				printf("ERROR: line %d: %s \n", line_number, error);
                *syntax_error = true;
				error[0]='\0';
			}
		}
		else /* check if token is label */
		{
			is_label(token,error,true);
			if(error[0]=='\0')
				strcpy(current_line->label_as_data, token);
			else
			{
				printf("ERROR: line %d: %s \n", line_number, error);
				error[0]='\0';
                *syntax_error = true;
			}
		}
	}
}

/* function is checking if label is fitting to rules */
void is_label(char* token, char* error, bool is_label_as_data)
{
	int i;
	if(strlen(token) > MAX_SYMBOL_SIZE-1)
	{
		strcpy(error, "label's length is to long");
		return;
	}
	if(!isalpha(token[0]))
	{
		strcpy(error, "label word must start with alpha letter");
		return;
	}
	if((token[strlen(token)-1] != ':') && (!is_label_as_data))
	{
		strcpy(error,  "Label must finished in : operand");
		return;
	}
	for (i = 0; i < MAX_ACTIONS_CODE; i++) /* check if token is not an operation name in "table" */
	{
		if (strcmp(token, table[i].name) == 0)
		{
			strcpy(error,  "Name of label is not valid because its an operation name");
			return;
		}
	}
	for(i= 0; i< MAX_GAUIDANCE; i++)
	{
		if (strcmp(token, ins_table[i]) == 0)
		{
			strcpy(error, "Name of label is not valid because its an gauidance name");
			return;
		}
	}
}

/* function is checking which kind of token we get - label, action code, guaidance, entry or external */
int analyzing_token(char* token, char* error) /*analyzing the labels and instructions columns*/
{
	int i;
	int last_index = strlen(token)-1;     /*for finding label*/
	char last_letter = token[last_index]; /*for finding label*/
	
	if(last_letter == ':')
	{
		is_label(token, error,false);
		if(error[0] == '\0')
			return label;
		else
			return not_valid;
	}
		

	if(token[0] == '.')
	{
		if(strcmp(token+1, "db") == 0)
		{
			guidance_number_of_bytes = 1;
            return guidance;
		}
		
		if(strcmp(token+1, "dh") == 0)
		{
			guidance_number_of_bytes = 2;
            return guidance;
		}
		
		if(strcmp(token+1, "dw") == 0)
		{
			guidance_number_of_bytes = 4;
            return guidance;
		}
		
		if(strcmp(token+1, "asciz") == 0) 
		{
			guidance_number_of_bytes = 0;
            return guidance;
		}
		if((strcmp(token+1, "entry") == 0) || strcmp(token+1, "extern") == 0)
        {
            guidance_number_of_bytes = 0;
            return entry_or_external;
        }
	}
	
	/* searching instruction from table */ 
	for (i = 0; i < MAX_ACTIONS_CODE; i++) 
	{
		if (strcmp(token, table[i].name) == 0)
		{
			action_code_line_number = i;
			return action_code;
		}	
	}

	strcpy(error, "The operation or guidance name is not valid");
	return not_valid;
	
}

/* checking if register is fitting the rules */
void is_reg(char* token, char* error)
{
	if(token[0] != '$')
	{
		strcpy(error, "register must start with $");
		return;
	}
	if(atoi(token+1) > MAX_REGISTERS || atoi(token+1) < 0)
	{
		strcpy(error, "reg number is not valid");
		return;
	}	
}

/* checking if number is fitting the rules */
void is_immed(char* token, char* error)
{
	int i=0;
	if(token[i] == '+' || token[i] == '-')
		i++;
	while(token[i] != '\0')
	{
		if(isdigit(token[i++]) == 0) /*its not a digit*/
		{
			strcpy(error, "number is not valid");
			return;
		}
	}
}

bool only_white_spaces (char* token)
{
	int len = strlen(token);
	int i;
	for(i=0; i < len; i++)
	{
		if(isspace(token[i]) == 0)
			return false;
	}
	return true;
}