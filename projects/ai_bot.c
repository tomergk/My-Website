/*ai chat bot*/
#include<stdio.h>                            
#include<conio.h>
#include<string.h>
#include<time.h>

char user_input[];

int main()
{   
    system("cls"); /*clear screen*/
    printf("welcome sir!\n"); 
    printf(" I'm an AI bot, and my mission is to help you.\n");
    printf("please write from the following: \n");
    
    printf("exit -    to exit\n");
    printf("hi -      to say hello\n");
    printf("chrome -  to open chrome browser\n");
    printf("notepad - to open notepad editor\n");
    printf("cmd -     to open cmd\n");
    printf("time -    to show current time\n");
    printf("youtube - to open youtube on your default browser\n");
    
    while(1)
    {
        printf("user ==> "); /*for showing user that we are waiting for his response*/
        gets(user_input);
        if(strcmp(user_input,"exit")==0)
        {
            system("cls");
            printf("bot ==> ok sir, bye bye\n");
            break;
        }
        else if(strcmp(user_input,"hi")==0)
        {
            printf("bot ==> hi sir, welcome\n");
        }
        else if(strcmp(user_input,"chrome")==0)
        {
            printf("ok sir, opeaning chrome browser for you\n");
            system("start chrome");
        }
        else if(strcmp(user_input,"notepad")==0)
        {
            printf("ok sir, opeaning notpad editor for you\n");
            system("start notepad");
        }
        else if(strcmp(user_input,"cmd")==0)
        {
            printf("ok sir, opeaning cmd terminal for you\n");
            system("start cmd");
        }
        else if(strcmp(user_input,"time")==0)
        {
            time_t s, val = 1 ;      /*size_t - This is the unsigned integral type and is the result of the sizeof keyword*/
            struct tm* current_time; /*struct tm - This is a structure used to hold the time and date*/
            /* time in second */
            s = time(NULL);
            current_time = localtime(&s);
            /* print time in minute */
            /* print hour and second */
            printf("bot ==> %02d:%02d:%02d\n",current_time->tm_hour,current_time->tm_min,current_time->tm_sec);
        }
        else if(strcmp(user_input,"youtube")==0)
        {
            printf("ok sir, opeaning youtube for you\n");   
            system("start http://youtube.com");   
        }
    }
    return 0;
}
