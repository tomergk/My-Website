#include "data_structures.h"

#ifndef analyze_line
#define analyze_line

#define MAX_ACTIONS_CODE 27
#define MAX_GAUIDANCE 6
#define MAX_REGISTERS 31 

extern int action_code_line_number;
extern int guidance_number_of_bytes;
extern int guidance_count_immed_num;
extern int length_asciz;

typedef enum State{
	label,              /* 0 */
	action_code,        /* 1 */
	guidance,			/* 2 */
	not_valid,          /* 3 */
    entry_or_external   /* 4 */
}State;

line_meaning* crossing_on_line(char line[], int line_number, char* error, bool* syntax_error);

void check_operands_for_asciz(char* token, line_meaning* current_line, int line_number, bool* syntax_error);

void check_operands_for_d_guidances(char* token, line_meaning* current_line,char* error, int line_number, bool* syntax_error);

void check_label_for_entry_or_extern(char* token, line_meaning* current_line,char* error, int line_number, bool* syntax_error);

void check_operands_for_R(char* token,char* error, int* count_reg, line_meaning* current_line, int line_number, bool* syntax_error);

void check_operands_for_I(char* token,char* error, int* count_reg, int* ins_I_place, line_meaning* current_line, int line_number, bool* syntax_error);

void check_operands_for_J(char* token, char* error, line_meaning* current_line, int line_number, bool* syntax_error);

void is_label(char* token, char* error, bool is_label_as_data);

int analyzing_token(char* token, char* error); /*analyzing the labels and instructions columns*/

void is_reg(char* token, char* error);

void is_immed(char* token, char* error);

bool only_white_spaces (char* token);

#endif