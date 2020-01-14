#include<stdio.h>
#include <mysql.h>
#include "TAG_FILE.h"
#include <unistd.h>

static char *def_host_name = "127.0.0.1";	/* host to connect to (default = localhost) */
static char *def_user_name = "operator";	/* user name (default = your login name) */
static char *def_password = "operator123";	/* password (default = none) */
static char *def_db_name = "Data_Dlms";		/* database to use (default = none) */
unsigned char query[1000],query1[50];
int DB_connection();
// int ch=1;

 MYSQL *con;


 pthread_mutex_t lock; 

int Data_Insert(int Tag_ID,int Telegram_no, int M_ID, int DCU_ID,unsigned char Unit, double Value, unsigned char DateTime[13],int Check_ID)
{
    // ch=DB_connection();
    // if(ch == 1){
		// ch=DB_connection();
		// }
      usleep(1000);
      pthread_mutex_lock(&lock); 
		switch(Check_ID){
				case 0:
				//sprintf(query,"INSERT INTO Instantaneous_Data (Tag_ID,M_ID,DCU_ID,Value,Unit,DateTime) VALUE(%d,%d,%d,%lf,%d,'%s');",Tag_ID,M_ID,DCU_ID,Value,Unit,DateTime);
 				sprintf(query,"INSERT INTO PROFILE_DATA (TAG_ID,TELEGRAM_NO,METER_ID,DCU_ID,VALUE,UNIT,DATETIME) VALUE(%d,%d,%d,%d,%lf,%d,'%s');",Tag_ID,Telegram_no,M_ID,DCU_ID,Value,Unit,DateTime);
				break;
				case 1:
				sprintf(query,"INSERT IGNORE INTO Load_Profile_Data (Tag_ID,M_ID,DCU_ID,Value,Unit,DateTime) VALUE(%d,%d,%d,%lf,%d,'%s');",Tag_ID,M_ID,DCU_ID,Value,Unit,DateTime);
				break;
      }//Switch end
  
  /* Inserting the data into the data base */
	if (mysql_query(con, query)) {
			fprintf (stderr, "Query is unsuccessful to insert : %s\n", query);
      pthread_mutex_unlock(&lock);
      return -1;
				//	exit(1);
	  }
  //usleep(10000);
	sprintf(query1,"COMMIT;");
	 if (mysql_query(con, query1))
   {
      fprintf (stderr, "Query is unsuccessful to commit: %s\n", query1);
      pthread_mutex_unlock(&lock);
      return -1;
    }
    else{
      #ifdef DEBUG
      printf("\nCommit successful\n");
      #endif
    }

    #ifdef DEBUG
	  printf("\nEnd of the Code \n");
	 #endif
    pthread_mutex_unlock(&lock);
     return 0;
}


int DB_connection(){

  int DB_Call;
  printf("\nConnecting to the Database\n");
	con = mysql_init(NULL);

  if (con == NULL)
  {
    fprintf(stderr, "%s\n", mysql_error(con));
    DB_Call=1;
    return -1;
  }
  /* The mysql_real_connect() function establishes a connection to the database. */
  if (mysql_real_connect(con, def_host_name, def_user_name, def_password, def_db_name,0, NULL, 0) == NULL)
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      mysql_close(con);
      DB_Call=1;
      return -1;
  }
    //sleep(5);
	DB_Call=0;
  return DB_Call;
}

int TAG_DATA(){

  printf("\nI am in the TAG-DATA\n");

  MYSQL_RES *res;
  MYSQL_ROW row;
  int a,b,c,d,e;
  int i = 0;
  int j=0;

    /* send SQL query */
    if (mysql_query(con, "SELECT DISTINCT A.TAG_ID, A.TELEGRAM_NO,A.METER_ID, B.DCU_ID,A.CATEGORY FROM TAG_CONFIG A, METER B WHERE A.METER_ID=B.METER_ID;"))
    {
        printf("Failed to execute quesry. Error: %s\n", mysql_error(con));
        return -1;
    }

    res = mysql_store_result(con);
    if (res == NULL)
    {
        return -1;
    }     
      
    int columns = mysql_num_fields(res);

    while(row = mysql_fetch_row(res))
    {
        for (i = 0; i < columns; i++)
        {
          //printf("%d --- %s ",i, row[i] ? row[i] : "NULL");
          if(i == 0)
                  a=atoi(row[i]);
          else if(i == 1)
                  b=(atoi(row[i]));
          else if(i == 2)
                  c=(atoi(row[i]));
          else if(i == 3)
                  d=(atoi(row[i]));
          else if(i == 4)
                  e=(atoi(row[i]));
          else
                  printf("\nBetter Luck");
        }
        Tag_L[e][d][c][b]=a;
        j=j+1;
        //printf("\n");
    }  
  mysql_free_result(res);
  return 0;
}
