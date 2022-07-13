#ifndef second_pass
#define second_pass

#define OBJECT_FILE_EXTENSION ".ob"
#define EXTERN_FILE_EXTENSION ".ext"
#define ENTRY_FILE_EXTENSION ".ent"



void second_pass_func(char* filename);

int label_address(char* label_name);

void print_bits (bits* bit, FILE* object_fd);
void print_hex_memory_data_list(memory_picture_data* head, FILE* object_fd);
void print_memory_code_list(memory_picture_code* head, FILE* object_fd);
void print_hex_memory_code_list(memory_picture_code* head, memory_picture_data* data_head, FILE* object_fd);

void print_memory_data_list(memory_picture_data* head, FILE* object_fd);

void combine_filename_with_new_file_extension(char * filename,char* output_filename, char * extension);

int open_second_pass_output_files();
void print_entry_file(symbol_line* symbol_head,FILE* entries_fd);
void print_extern_file(symbol_line* symbol_head, line_meaning * line_head, FILE* externals_fd, memory_picture_code* memory_head);

void close_second_pass_output_files();

#endif
 

