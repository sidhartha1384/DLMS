#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/timeb.h>
#include <sys/time.h>
//#include "TAG_FILE.h"

#define DATA_SIZE 10000
#define MAX1 1000000
struct timeval tv;



int numBytes;
int i;
int retval;
// int DB_connection();
int AARE_Process();
int DATA_PROCESSING();
int Set_Process();
int TAG_DATA();

void *connection_handler (void *socket_desc)
{
    unsigned char buffer[DATA_SIZE];
    unsigned char Block_Buffer[MAX1];
    unsigned char DateTime[13];

    int sock = *(int *) socket_desc;
    int fun_check;
    int size_final1;
    int size;
    int pos;
    int DB_Call;
    
    pos=0;
    
    fprintf(stderr, "\n:::: Waiting for the data from the Special Energy meter ::::\n");

    printf("\n:::: New Client Socket Number is :- %d ::::\n",sock);
    bzero(&Block_Buffer,sizeof(Block_Buffer));
    
    DB_Call=TAG_DATA();
        printf("\n:::: New Client Socket Number is :- %d ::::\n",sock);

     while (1)
      {
        printf("\nReturn Value of TAG_DATA is %d\n",DB_Call);
        sleep(5);
          if(DB_Call != 0)
          {
            DB_Call=TAG_DATA();
          }
          else
          {
            break;
          }
      }
       
    while (1)
    { 
      bzero(&buffer,sizeof(buffer));  
     
	    /* Wait up to  seconds.*/
	    tv.tv_sec = 120;
     	tv.tv_usec = 0;		
	    fd_set readset;//for select function
	    FD_ZERO(&readset);
	    FD_SET(sock, &readset);
	    retval = select(sock + 1, &readset, NULL, NULL, &tv);

      numBytes = recv (sock,buffer,5, 0);	     
	    	
	    if (retval <= 0)
        {
          sleep(2);
          close(sock);
          sleep(2);
          pthread_exit(0);
          return -1;
	    }

		  size=buffer[3]<<8|buffer[4]; 

      #ifdef DEBUG
      printf("\n>>>> Total Size of the Packet is %d\n",size);
      #endif

	    for ( i = 0; i < size; i++)
        {
            numBytes = recv (sock,&buffer[i+5], 1, 0);
        }
        #ifdef DEBUG
        printf("\n");
        #endif
        fflush(stdout);
     
        switch (buffer[2])
        {
            case 0x01:             
                 #ifdef DEBUG
                 printf("\n>>>> The AARE Response \n DCU: %d \n Meter No: %d \n",buffer[0],buffer[1]);
                 #endif
                
                fun_check = AARE_Process(buffer);
                if (fun_check == -1)
                {
                  #ifdef DEBUG
                  printf("\n>>>> Received the response\n");
                  #endif

                  close(sock);
                  pthread_exit(0);
                  return -1;
                }
            break;
            case 0x02:    

                 #ifdef DEBUG
                 printf("\n>>>> GET Data -- %X\n",buffer[1]);
                 #endif
               
                fun_check = DATA_PROCESSING(buffer,pos,Block_Buffer,&size_final1,DateTime,sock);
                
                if (fun_check == -1)
                {
                  #ifdef DEBUG
                  printf("\n>>>> Received the response\n");
                  #endif
                  
                  close(sock);
                  pthread_exit(0);
                  return -1;
                }
                else
                {
                  pos=fun_check;
                }
                
            break;

            default:
              printf("\nClosing the socket\n");
              
              close(sock);
              pthread_exit(0);
              return -1;
            break;
        }//Switch End
    }//while  End	
}


