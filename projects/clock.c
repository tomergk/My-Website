/*
CREATING A DIGITAL CLOCK.
the broblem here is we need to use clrscn() func to cleer the screen but it works only on Turbo C++ compiler only
*/
#include <stdio.h>
#include <conio.h>
void main()
{
    int h=0, m=0, s=0, i;
    printf("please enter a time in a format of: HH : MM : SS \n");
    scanf("%d%d%d", &h,&m,&s);
    start: 
    for(h; h<24; h++)
    {
        for(m; m<60; m++)
        {
            for(s; s<60; s++)
            {
                printf("\n\n\n\t\t\t%d:%d:%d", h,m,s);
                if(h<12)
                    printf("AM");
                else
                    printf("PM");
                for(double i=0; i<3619999; i++)
                {
                    i++;
                    i--;
                }
            }
        s=0;
        }
    m=0;
    }
    h=0;
    goto start;
    getch();
}