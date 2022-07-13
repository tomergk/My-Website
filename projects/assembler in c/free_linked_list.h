#include <stdio.h>
#include "data_structures.h"

#ifndef free_linked_list
#define free_linked_list

void free_data_memory_table(memory_picture_data *data_head);
void free_bits_of_data_memory_table(bits_of_data_memory *head);
void free_code_memory_table(memory_picture_code *code_head);
void free_symbol_table(symbol_line *symbol_head);
void free_lines_table(line_meaning *line_head);

#endif