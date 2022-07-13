all:	analyze_line.o	conversions.o	first_pass.o	free_linked_list.o	second_pass.o	main.o
	gcc -g main.c analyze_line.o conversions.o first_pass.o free_linked_list.o second_pass.o -ansi -pedantic -Wall -o assembler

analyze_line.o:	analyze_line.c	analyze_line.h	data_structures.h
	gcc -c analyze_line.c -ansi -pedantic -Wall -o analyze_line.o

conversions.o:	conversions.c	conversions.h	data_structures.h
	gcc -c conversions.c -ansi -pedantic -Wall -o conversions.o

first_pass.o:	first_pass.c	conversions.h	analyze_line.h	data_structures.h	free_linked_list.h
	gcc -c first_pass.c -ansi -pedantic -Wall -o first_pass.o

free_linked_list.o:	free_linked_list.c	free_linked_list.h	data_structures.h
	gcc -c free_linked_list.c -ansi -pedantic -Wall -o free_linked_list.o

second_pass.o:	second_pass.c	conversions.h	data_structures.h	free_linked_list.h	second_pass.h
	gcc -c second_pass.c -ansi -pedantic -Wall -o second_pass.o
	
main.o:	main.c	analyze_line.h	conversions.h	data_structures.h	free_linked_list.h	second_pass.h
	gcc -c main.c -ansi -pedantic -Wall -o main.o
