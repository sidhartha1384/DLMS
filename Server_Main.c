#include <stdio.h>

int Server_Socket();
int DB_connection();


int main(){

    int sc;
    int DB_Call;

    DB_Call=DB_connection();
     while (1)
      {
        printf("\nValue of DB is %d\n",DB_Call);
        sleep(5);
          if(DB_Call != 0)
          {
            DB_Call=DB_connection();
          }
          else
          {
            break;
          }

      }
    sc=Socket_Creation();
   
    while (1)
    { 
        if (sc == -1){
            sc=Socket_Creation();
        }
   }
return 0;
}
