#include<stdio.h>		//for standard I/O
#include<unistd.h>		//defines miscellaneous symbolic constants and types, and declares miscellaneous functions.
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include <errno.h> 
#include<signal.h>
#include<stdlib.h>
#include <pthread.h>

int PORT;
char line[256];
char PORT_NUMBER[100];
FILE *fc1;
struct sockaddr_in servAddr, client_addr;

unsigned short sin_port;
int socket_desc, client_sock, c, *new_sock,last_fd;
int sock_fd;
int address, address1, data, data1, ader;

void *connection_handler (void *);

void sigintHandler (int sig_num);

/*
 * Socket_Creation of the Program
 */
int Socket_Creation(){
 

	int cnt = 1;			//To know NO of Connections

	/*
 	* Signal for Termination of the Program
 	*/
	  signal(SIGINT, sigintHandler);

	  if (signal (SIGINT, sigintHandler) == SIG_ERR)
	    {
		      printf("\n :::: UNABLE TO TERMINATE THE PROGRAM ! TRY AGAIN :::: \n");
	}


	/*
 	* Opening the FILE for PORT Number
 	*/
	
	 // sprintf(PORT_NUMBER,"%s/port_no.dat",(char*)getenv("TEST"));
	  //if ((fc1 = fopen (PORT_NUMBER,"r")) == NULL)	//opening particular dat file 
	   if ((fc1 = fopen ("port_no.dat","r")) == NULL)	//opening particular dat file 
	    {
		      printf("\n :::: *** port_no.dat *** FILE DOES NOT EXIST, PLEASE CONFIGURE THE FILE *** :::: \n");
		      return 0;
		    }
  
	  if ((fgets(line, sizeof (line), fc1)) == NULL)
	    {
		      perror("\n::::PORT_NO NOT CONFIGURE :::: \n");
	    }

	  sscanf(line, "%d", &PORT);

		#ifdef DEBUG
		  printf("\n :::: PORT is: %d ::::\n",PORT);
		#endif
	fclose(fc1);
	
	/*
	*CREATING THE SOCKET WITH NAME::::: socket_desc
	*/
	  socket_desc = socket (AF_INET, SOCK_STREAM, 0);

	  if(socket_desc < 0)
	    {

		      printf("\n :::: FAILED TO CREATE THE SOCKET :::: \n");
		      exit (0);
	    }
	sock_fd = socket_desc;

	/*
	*ASSIGNING THE IP AND PORT 
	*/
	 	memset(&servAddr, 0, sizeof (servAddr));
	 	servAddr.sin_family = AF_INET;
	  	servAddr.sin_addr.s_addr = INADDR_ANY;
		servAddr.sin_port = htons (PORT);

	/*
	*BINDING THE SOCKET TO THE PORT
	*/
	  if (bind(socket_desc, (struct sockaddr *) &servAddr,sizeof (struct sockaddr)) == -1)
	    {

		      printf("\n :::: UNABLE TO BIND THE PORT: %d :::: \n",PORT);

		      exit(1);
	    }

		#ifdef DEBUG
	  		printf("\n  :::: Bind Done Successfully with the Port: %d :::: \n",PORT);
		#endif

	/*
	*LISTEN TO ACCEPTED CONNECTIONS
	*/
	  if (listen (socket_desc, 100) != 0)
	    {
		      printf("\n :::: Unable to Listen to the the Socket Connected :::: \n");
	    }

	  printf("\n :::: SLDC Server Waiting for connection at port: %d at :%s:::: \n", PORT,__DATE__);
	  fflush(stdout);

	  c = sizeof(struct sockaddr_in);

	while(1)
	{
		/*
		*ACCEPT THE ALL THE INCOMING CONNECTIONS 
		*/
		    client_sock =accept(socket_desc, (struct sockaddr *) &client_addr, &c);

		    if (client_sock < 0)
			{
				  printf("\n:::: Error in Accepting the Connection ::::\n");
				  exit(1);
			}

		    fprintf(stderr,"\n:::: Waiting For Meter Connection :::: \n");

		    fprintf(stderr,"\n:::: Connection from Meter SBC with IP: %s and PORT: %d :::: \n",inet_ntoa (client_addr.sin_addr),ntohs (client_addr.sin_port));

		    fprintf(stderr,"\n:::: Connection Established Successfully :::: \n");

		/*
		*CREATING THE THREAD PROCESS FOR EACH AND EVERY CONNECTION
		*/
		    pthread_t sniffer_thread;
		    new_sock = malloc (1);
		    *new_sock = client_sock;

      	/*
       	* Calling the Function For New Thread Created
       	*/
		    if(pthread_create(&sniffer_thread, NULL, connection_handler ,(void *) new_sock) == -1)
			{
				perror("\n :::: Unable to Create Threads for accepting the ABT Meter Connections :::: \n");
				return -1;
			}

		    fprintf(stderr, "\n:::: ABT Meter Connection Connected:  %d ::::\n",cnt);
		    fflush (stdout);
		    cnt++;
	 }	      /*** END OF WHILE LOOP ***/
return 0;
}


/*******************************
* 5.FUNCTION FOR CODE TERMINATION*
*******************************/
void sigintHandler (int sig_num)
{
  if (sig_num == SIGINT)
    {
      
      	printf("\n::::  CONNECTION TERMINATED BY USING  Ctrl+C SIGNAL \n");

      	fflush(stdout);

	  	if(client_sock>0){
	      	close(client_sock);
      		printf("\n :::: CLOSING THE CLIENT SOCKET :::: \n");
      		sleep(1);
		}

      	close(socket_desc);

      	printf("\n:::: CLOSING THE SERVER SOCKET :::: \n");
     	sleep(2);
     
     	exit(0);
    }
}
