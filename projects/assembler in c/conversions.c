#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data_structures.h"
#include "conversions.h"

/* Converts a decimal number to a binary number, then padding zeroes up to x bits. */
void convert_dec_to_x_bit_binary(int num, int bit, bits result[])
{
    int i = bit - 1;
    long binary_num = 0, copy = num, temp1 = 1, temp2 = 1;

	while (copy!=0)
    {
        temp1 = copy%2;
        copy = copy / 2;
        binary_num = binary_num + temp1*temp2;
        temp2 = temp2 * 10;
    } 

    do 
    {
        if((result[i].value = (binary_num % 10)) == 0)
             result[i].value = 0;
        else
            result[i].value = 1;
        binary_num = binary_num / 10;
        i--;
    }
    while (binary_num != 0); 

    for (; i >= 0; i--) /* padding with zeros */
    { 
        result[i].value = 0;
    }
    /* perform 2's complement if it's a negative number */
    if (num < 0) 
	{
        int i = bit - 1;
        int first_one_encountered = 0;
        for (; i >= 0; i--) 
		{
            if (first_one_encountered == 0) 
			{
                if (result[i].value == 1) 
				{
                    first_one_encountered = 1;
                    continue;
                }
            }
            else 
			{
                if(result[i].value == 1)
					result[i].value = 0;
				else
					result[i].value = 1;
            }
        }
    }
}
/*converting 8 digit binary number to 2 digit hex number*/
void convert_to_hex(bits array[], char hexnum[])
{
	int hex=0, mul=1, count=1, reminder, i=0, j = 7;
    char temp;
	while(j>=0)
    {
        reminder = array[j].value;
        hex += (reminder*mul);
        if(count%4==0)
        {
            if(hex<10)
                hexnum[i] = hex+48;
            else
                hexnum[i] = hex+55;
            mul = 1;
            hex = 0;
            count = 1;
            i++;
        }
        else
        {
            mul = mul*2;
            count++;
        }
        j--;
    }
    if(count!=1)
        hexnum[i] = hex+48;
    if(count==1)
        i--;

    temp = hexnum[0];
    hexnum[0] = hexnum[1];
    hexnum[1] = temp;
}

