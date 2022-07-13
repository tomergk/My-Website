#include <stdlib.h>

#include "data_structures.h"
#include "free_linked_list.h"

/* free_data_memory */
void free_data_memory_table(memory_picture_data *data_head) 
{
    memory_picture_data *temp;

    while (data_head)
    {
        temp = data_head->next;
        free_bits_of_data_memory_table(data_head->head);
        free(data_head);
        data_head = temp;
    }
}

/* free_bits data_memory */
void free_bits_of_data_memory_table(bits_of_data_memory *head) 
{
    bits_of_data_memory *temp;

    while (head)
    {
        temp = head->next;
        free(head);
        head = temp;
    }
}


/* free_code_memory*/
void free_code_memory_table(memory_picture_code *code_head)
{
    memory_picture_code *temp;

    while (code_head)
    {
        temp = code_head->next;
        free(code_head);
        code_head = temp;
    }
}

/* free_symbol memory. */
void free_symbol_table(symbol_line *symbol_head)
{
    symbol_line *temp;

    while (symbol_head)
    {
        temp = symbol_head->next;
        free(symbol_head);
        symbol_head = temp;
    }
}

/* free_sentences memory. */
void free_lines_table(line_meaning *line_head) 
{
    line_meaning *temp;

    while (line_head)
    {
        temp = line_head->next;
        free(line_head);
        line_head = temp;
    }
}

