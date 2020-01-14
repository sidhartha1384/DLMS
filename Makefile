
Server_DEBUG: Server_Main.o Socket.o Data_Exchange.o Data_Process.o Data_Base.o
	gcc Server_Main.o Socket.o Data_Exchange.o Data_Process.o Data_Base.o -lpthread -o Server_DEBUG -lm `mysql_config --cflags --libs`  -DDEBUG

Server: Server_Main.o Socket.o Data_Exchange.o Data_Process.o Data_Base.o
	gcc Server_Main.o Socket.o Data_Exchange.o Data_Process.o Data_Base.o -lpthread -o Server -lm `mysql_config --cflags --libs`


Server_Main.o: Server_Main.c
	gcc -c Server_Main.c -w

Socket.o: Socket.c
	gcc -c Socket.c -w

Data_Exchange.o: Data_Exchange.c
	gcc -c Data_Exchange.c -w

Data_Process.o: Data_Process.c
	gcc -c Data_Process.c -w

Data_Base.o: Data_Base.c
	gcc -c -I/usr/include/mysql Data_Base.c -w

clean:
	rm *.o Server Server_DEBUG

