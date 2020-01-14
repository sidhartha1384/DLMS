#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "TAG_FILE.h"


#define MAX 1000
#define MAX1 1000000
#define N 13
// #define DCU_L 10
// #define METER_L 10
// #define SEQ_L 100
// #define LOAD_D 10

int Data_Insert();


// int Tag_L[LOAD_D][DCU_L][METER_L][SEQ_L];


// unsigned char DateTime[13];
// unsigned char Block_Buffer[MAX1];
//int pos;
//int num,check_ID,check;
// unsigned int value,Unit;
// double final_value;
// int tag1,tag2;  //check_ID is to store in Instantaneous or Load profile // check is the return value for DB
 //int size_final;
// int temp1,temp2;// not used

struct Scalar_Data{
  int S_Unit;
  int S_tag;
  double S_value;
};
struct Scalar_Data S_Data[MAX][50];

// struct OBIS
// {
//  int OBIS_Tag;
//  unsigned char OBIS_Value[7];
// };
// struct OBIS OBIS_ID[50];

int AARE_Process(unsigned char AARE_DATA[MAX]){

  int size,i;

  size = AARE_DATA[3]<<8|AARE_DATA[4];

 // #ifdef DEBUG
  printf("\n<<<<< ---- DATA RECEIVED IS AARE ---- >>>>>\n");
             
  for ( i = 0; i < (size+5); i++)
   {
      fprintf(stdout,"%X ",AARE_DATA[i]);
    }

  printf("\n");
  printf("\n>>>>>> DCU No: %d",AARE_DATA[0]);
  printf("\n>>>>>> Meter No: %d",AARE_DATA[1]);
  printf("\n>>>>>> Packet Id for Get: %d",AARE_DATA[2]);
  

  for ( i = 0; i < size; i++ )
    {
      switch (AARE_DATA[i])
      {
      case 0xA1:
        printf("\n>>>>>> Application Context :: ");
        for (size_t z = 0; z < AARE_DATA[i+3]; z++)
        {
          printf("%X ",AARE_DATA[i+4+z]);
        }    
        break;
      case 0xA2:
        printf("\n>>>>>> Association Result successful if result  is '0' :: ");
        printf("%X ",AARE_DATA[i+4]);
        break;
      case 0xA4:
        printf("\n>>>>>> Responding AP title :: ");
        for (size_t z = 0; z < AARE_DATA[i+3]; z++)
        {
          printf("%X ",AARE_DATA[i+4+z]);
        }    
        break;
      case 0xBE:
        printf("\n>>>>>> User Information :: ");
        for (size_t z = 0; z < AARE_DATA[i+3]; z++)
        {
          printf("%X ",AARE_DATA[i+4+z]);
        }    
        break;  
      
      default:
        break;
      }            
    }    
    //#endif
  // fflush(stdout);
  return 0;
}


int DATA_PROCESSING(unsigned char GET_DATA[MAX],int pos,unsigned char Block_Buffer[MAX1],int *size_final1,unsigned char DateTime[13],int sock){
    
    int size,i=0,obis_nxt,z,j,t;
    int DCU,Meter;
    DCU=GET_DATA[0];
    Meter=GET_DATA[1];
    size = GET_DATA[3]<<8|GET_DATA[4];
    int attr;
    unsigned char Get_Response[MAX];
    //unsigned char *Block_Buffer1;

      for (size_t k = 0; k < size; k++)
      {
          Get_Response[k]=GET_DATA[k+5];
      }
      #ifdef DEBUG
      printf("\nData from DCU :: %d",GET_DATA[0]);
      printf("\nData from Meter :: %d\n",GET_DATA[1]);
      #endif
      switch (Get_Response[9])
      {
      case 0x01:
        #ifdef DEBUG
        printf("\nResponse Received is the Get Response Normal\n");
        #endif
        Get_Response_Normal(Get_Response,size,DCU,Meter,Block_Buffer,size_final1,DateTime,sock);
        break;
      case 0x02:
        #ifdef DEBUG
        printf("\nResponse Received is the Get Response Block\n");
        #endif
        //Get_Response_Block(Get_Response,size);
        pos=Get_Response_Block_FINAL(Get_Response,size,DCU,Meter,pos,Block_Buffer,size_final1,DateTime,sock);
        break;
      case 0x03:
        #ifdef DEBUG
        printf("\nResponse Received is the Get Response List\n");
        #endif
        Get_Response_List(Get_Response,size,DCU,Meter,DateTime,sock);
        break;
      
      default:
        break;
      }
      
    
    return pos; // it will go to the Data_Exchange.c
}


int Get_Response_Normal(unsigned char Get_Response_Nr[MAX],int size,int DCU,int Meter,unsigned char Block_Buffer[MAX1],int *size_final1,unsigned char DateTime[13],int sock){
    int i,j;
    int len_1,length;
    int s1,s2,s3,element_1,element_2;
    int p,tag,check,check_ID;
    unsigned int value,Unit;
    double final_value;
    int tag1,tag2; 
    int M_TAG;
    int a,b,c,d;

    int Year;
    int Month;
    int Date;
    int Week_of_Day;
    int Hour;
    int Minute;
    int Second;
    int MilliSecond;
    int Deviation;
    int Status;

    p=0;
    
    #ifdef DEBUG
    printf("\nGet Response Normal Data\n");
    for ( i = 0; i < size; i++)
    {
      printf(" %X",Get_Response_Nr[i]);
    }
    printf("\n");
    #endif
    if ((Get_Response_Nr[10] == 194)||(Get_Response_Nr[10] == 193))
    {
      check_ID=0;
    }
    else if ((Get_Response_Nr[10] == 196)||(Get_Response_Nr[10] == 195))
    {
      check_ID=1;
    }
    else
    {
      #ifdef DEBUG
      printf("\n");
      #endif
    }

    switch (Get_Response_Nr[11])
    {
    case 0x00:
            #ifdef DEBUG
            printf("Data Received is the OctetString Data");
            #endif
            if (Get_Response_Nr[12] == 0x82)
            {
                len_1=(Get_Response_Nr[13]<<8)|(Get_Response_Nr[14]);
                s1=15;
                #ifdef DEBUG
                printf("\nReceived the Octet String if %d\n",len_1);
                #endif
            }
            else if (Get_Response_Nr[12] == 0x81)
            {
                len_1=Get_Response_Nr[13];
                s1=14;
                #ifdef DEBUG
                printf("\nReceived the Octet String if %d\n",len_1);
                #endif
            }
            else
            {
                #ifdef DEBUG
                printf("\nNeed to code the get Normal function\n"); 
                #endif               
                s1=12;    //s1 is next location of data
            }  
      break;

    case 0x01:
      printf("\nData Received is the DataAccessResult\n");
            switch (Get_Response_Nr[11])
            {
            case 0x00:
              printf("\n Success \n");
              break;
            case 0x01:
              printf("\n HardwareFault \n");
              break;
            case 0x02:
              printf("\n TemporaryFailure \n");
              break;
            case 0x03:
              printf("\n ReadWriteDenied \n");
              break;
            case 0x04:
              printf("\n ObjectUndefined \n");
              break;
            case 0x09:
              printf("\n ObjectClassInconsistent \n");
              break;
            case 0x0B:
              printf("\n ObjectUnavailable \n");
              break;
            case 0x0C:
              printf("\n TypeUnmatched \n");
              break;
            case 0x0D:
              printf("\n ScopeOfAccessViolated \n");
              break;
            case 0x0E:
              printf("\n DataBlockUnavailable \n");
              break;
            case 0x0F:
              printf("\n LongGetOrReadAborted \n");
              break;
            case 0x10:
              printf("\n NoLongGetOrReadInProgress \n");
              break;
            case 0x11:
              printf("\n LongSetOrWriteAborted \n");
              break;
            case 0x12:
              printf("\n NoLongSetOrWriteInProgress \n");
              break;   
            case 0x13:
              printf("\n DataBlockNumberInvalid \n");
              break;    
            default:
              printf("\n OtherReason\n");
              break;
            }
      break;
    default:
      break;
    }


  switch (Get_Response_Nr[s1])
  {
  case 0x01:  
        if (Get_Response_Nr[s1+1] == 0x82)
        {
            element_1=(Get_Response_Nr[s1+2]<<8)|(Get_Response_Nr[s1+3]);
            #ifdef DEBUG
            printf("\nReceived_1 the Array of %d elements\n",element_1);
            #endif
            s2=s1+4;
        }
        else if (Get_Response_Nr[s1+1] == 0x81)
        {
          element_1=Get_Response_Nr[s1+2];
          #ifdef DEBUG
          printf("\nReceived_2 the Array of %d elements\n",element_1);
          #endif
          s2=s1+3;
        }
        else
        {
          element_1=Get_Response_Nr[s1+1];
          #ifdef DEBUG
          printf("\nReceived_3 the Array of %d elements\n",element_1);
          #endif
          s2=s1+2;  // next location of data
        }
        break;
  case 0x02:
        if (Get_Response_Nr[s1+1] == 0x82)
        {
            element_1=(Get_Response_Nr[s1+2]<<8)|(Get_Response_Nr[s1+3]);
            #ifdef DEBUG
            printf("\nReceived_1 the Structure of %d elements\n",element_1);
            #endif
            s2=s1+4;
        }
        else if (Get_Response_Nr[s1+1] == 0x81)
        {
          element_1=Get_Response_Nr[s1+2];
          #ifdef DEBUG
          printf("\nReceived_2 the Structure of %d elements\n",element_1);
          #endif
          s2=s1+3;
        }
        else
        {
          element_1=Get_Response_Nr[s1+1];
          #ifdef DEBUG
          printf("\nReceived_3 the Structure of %d elements\n",element_1);
          #endif
          s2=s1+2;  // next location of data
        }
    break;
    default:
      s2=s1+1;
    break;  
  }

  if (Get_Response_Nr[s2] == 0x02)
  {
    
    if (Get_Response_Nr[s2+1] == 0x82)
        {
            element_2=(Get_Response_Nr[s2+2]<<8)|(Get_Response_Nr[s2+3]);
            #ifdef DEBUG
            printf("\nReceived_1 the Structure of %d elements\n",element_2);
            #endif
            s3=s2+4;
        }
        else if (Get_Response_Nr[s2+1] == 0x81)
        {
          element_2=Get_Response_Nr[s2+2];
          #ifdef DEBUG
          printf("\nReceived_2 the Structure of %d elements\n",element_2);
          #endif
          s3=s2+3;
        }
        else
        {
          element_2=Get_Response_Nr[s2+1];
          #ifdef DEBUG
          printf("\nReceived_3 the Structure of %d elements\n",element_2);
          #endif
          s3=s2+2;  // next location of data
        }
  }

  if (Get_Response_Nr[10] == 194)
  {
    element_2=element_2+5;
    #ifdef DEBUG
    printf("\nNumber of Scalar units for Instantaneous data is %d\n",element_2);
    #endif
    S_Data[sock][0].S_tag=1;
    S_Data[sock][16].S_tag=17;
    S_Data[sock][18].S_tag=19;
    S_Data[sock][19].S_tag=20;
    S_Data[sock][20].S_tag=21;
  }

  if (Get_Response_Nr[10] == 196)
  {
    S_Data[sock][0].S_tag=1;
  }

  // if (Get_Response_Nr[10] == 206){
  //       tag2=1;
  // }

    for ( i = 0; i < element_1; i++)
    {  
        tag=0;
        tag1=0;     
   
      for (j = 0; j < element_2; j++)
      {  
        #ifdef DEBUG
        printf("\n---------------------------------------------------------\n"); 
        printf("\nValue of J is %d\n",j);
        #endif
          if ((Get_Response_Nr[10] == 194) && (S_Data[sock][tag].S_tag ==17) )
          {
              S_Data[sock][tag].S_tag=tag+1;
              #ifdef DEBUG
              printf("\nI am Tag 17 :: %d\n",S_Data[sock][tag].S_tag);
              #endif
              S_Data[sock][tag].S_Unit=0xFF;
              S_Data[sock][tag].S_value=1;
              tag=tag+1;
          }
          if ((Get_Response_Nr[10] == 194) && (S_Data[sock][tag].S_tag ==1))
          {
              S_Data[sock][tag].S_tag = tag+1;
              #ifdef DEBUG
              printf("\nI am Tag 1 :: %d\n",S_Data[sock][tag].S_tag);
              #endif
              S_Data[sock][tag].S_Unit=0xFF;
              S_Data[sock][tag].S_value=1;
              tag=tag+1;
          }
           if ((Get_Response_Nr[10] == 196) && (S_Data[sock][tag].S_tag ==1))
          {
              S_Data[sock][tag].S_tag = tag+1;
              #ifdef DEBUG
              printf("\nI am Tag 1 :: %d\n",S_Data[sock][tag].S_tag);
              #endif
              S_Data[sock][tag].S_Unit=0xFF;
              S_Data[sock][tag].S_value=1;
              tag=tag+1;
          }
          if ((Get_Response_Nr[10] == 194) && (S_Data[sock][tag].S_tag ==19))
          {
              S_Data[sock][tag].S_tag=tag+1;
              #ifdef DEBUG
              printf("\nI am Tag 19 :: %d\n",S_Data[sock][tag].S_tag);
              #endif
              S_Data[sock][tag].S_Unit=0xFF;
              S_Data[sock][tag].S_value=1;
              tag=tag+1;
          }
          if ((Get_Response_Nr[10] == 194) && (S_Data[sock][tag].S_tag ==20))
          {
              S_Data[sock][tag].S_tag=tag+1;
              #ifdef DEBUG
              printf("\nI am Tag 20:: %d\n",S_Data[sock][tag].S_tag);
              #endif
              S_Data[sock][tag].S_Unit=0xFF;
              S_Data[sock][tag].S_value=1;
              tag=tag+1;
          }
          if ((Get_Response_Nr[10] == 194) && (S_Data[sock][tag].S_tag ==21))
          {
              S_Data[sock][tag].S_tag=tag+1;
              #ifdef DEBUG
              printf("\nI am Tag 21 :: %d\n",S_Data[sock][tag].S_tag);
              #endif
              S_Data[sock][tag].S_Unit=0xFF;
              S_Data[sock][tag].S_value=1;
              tag=tag+1;
          }
          else
          {
            #ifdef DEBUG
            printf("\n");
            #endif
          }     

          if (Get_Response_Nr[s3+p] == 0x09)
          {
            int k;
            #ifdef DEBUG
              printf("\nReceived the OctetString of size %d\n",Get_Response_Nr[s3+1+p]);
              printf("OctetString :: ");
              #endif
              for (k = 0; k < Get_Response_Nr[s3+1+p]; k++)    /* loc == ss_1+3 */
              {      
                  #ifdef DEBUG            
                  printf("%X ",Get_Response_Nr[s3+2+k+p]);
                  #endif
                  // if (Get_Response_Nr[10] == 206)
                  // {
                  //   OBIS_ID[i].OBIS_Value[k] =Get_Response_Nr[s3+2+k+p];
                  // }
                  
              }
              if (Get_Response_Nr[s3+2+0+p] == 0x07)
              {
                  Year=(Get_Response_Nr[s3+2+0+p]<<8)|(Get_Response_Nr[s3+2+1+p]);
                  Month=Get_Response_Nr[s3+2+2+p];
                  Date=Get_Response_Nr[s3+2+3+p];
                  Week_of_Day=Get_Response_Nr[s3+2+4+p];
                  Hour=Get_Response_Nr[s3+2+5+p];
                  Minute=Get_Response_Nr[s3+2+6+p];
                  Second=Get_Response_Nr[s3+2+7+p];
                  MilliSecond=Get_Response_Nr[s3+2+8+p];
                  Deviation=(Get_Response_Nr[s3+2+9+p]<<8)|(Get_Response_Nr[s3+2+10+p]);
                  Status=Get_Response_Nr[s3+2+11+p];
                  sprintf(DateTime,"%d-%d-%d %d:%d:%d",Year,Month,Date,Hour,Minute,Second);
                  #ifdef DEBUG
                  printf("\nTag:: %d, DataTime is :: %s\n",DateTime,tag); 
                  #endif
                  final_value=0;
                  #ifdef DEBUG
                  printf("\nCheck_ID::%d,Tag:: %d, M_ID:: %d, DCU_ID::%d, Unit:: %X ,Final Value :: %lf, DateTime :: %s\n",check_ID,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime);            
                  #endif
                  while (1)
                  {
                      a=DCU;
                      b=Meter;
                      c=(S_Data[sock][tag-1].S_tag);
                      M_TAG=Tag_L[check_ID][a][b][c];
                      check=Data_Insert(M_TAG,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
                     //check=Data_Insert(S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);

                      if (check == 0)
                      {
                        break;
                      }            
                  }
                  //sleep(5);
              } 
              // if (Get_Response_Nr[10] == 206){
              //       OBIS_ID[i].OBIS_Tag=tag2;
              //       #ifdef DEBUG
              //       printf("OBIS :: %s , Tag (206):: %d\n",OBIS_ID[i].OBIS_Value,OBIS_ID[i].OBIS_Tag);
              //       #endif
              //       tag2=tag2+1;
              // }   
              #ifdef DEBUG
              printf("Tag :: %d\n",tag);
              #endif
              tag=tag+1;
              s3 = s3+1+k+p;
              p=1;

              #ifdef DEBUG
              printf("\nOctetString byte last position -- %d\n",s3);   
              #endif                                         
          }
          else if (Get_Response_Nr[s3+p] == 0x06)
          {
              #ifdef DEBUG
              printf("\n>>>> Double Long Unsigned :: ");
              #endif
              value=(Get_Response_Nr[s3+1+p]<<24)|(Get_Response_Nr[s3+2+p]<<16)|(Get_Response_Nr[s3+3+p]<<8)|(Get_Response_Nr[s3+4+p]);
              #ifdef DEBUG
              printf("\n\t>>>> The Value :: %d and tag :: %d --- DataTime :: %s ",value,tag,DateTime); 
              #endif
              final_value=value*S_Data[sock][tag-1].S_value;
              #ifdef DEBUG
              printf("\nCheck_ID::%d,Tag:: %d, M_ID:: %d, DCU_ID::%d, Unit:: %X ,Final Value :: %lf, DateTime :: %s\n",check_ID,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime);            
              #endif
              while (1)
              {
                      a=DCU;
                      b=Meter;
                      c=(S_Data[sock][tag-1].S_tag);
                      M_TAG=Tag_L[check_ID][a][b][c];
                      check=Data_Insert(M_TAG,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
                  
                  //check=Data_Insert(S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);

                  if (check == 0)
                  {
                    break;
                  }            
              }
              
              tag=tag+1;
              s3 = s3+4+p;
              #ifdef DEBUG
              printf("\n");
              #endif
              p=1;     
              #ifdef DEBUG
              printf("\nDouble Long Unsigned byte last position -- %d\n",s3);
              #endif                                             
          }          
          else if (Get_Response_Nr[s3+p] == 0x17)
          {
              #ifdef DEBUG
              printf("\nUnsigned :: ");  
                  
              for (j = 0; j < 4; j++)
              {                
                  printf("%X ",Get_Response_Nr[s3+2+j]);
              } 
              #endif 
              value=(Get_Response_Nr[s3+2+0]<<24)|(Get_Response_Nr[s3+2+1]<<16)|(Get_Response_Nr[s3+3+2]<<8)|(Get_Response_Nr[s3+4+3]);
              final_value=value*S_Data[sock][tag-1].S_value;
              #ifdef DEBUG
              printf("\n\t>>>> The Value :: %d and tag :: %d --- DateTime :: %s",value,tag,DateTime);   
              printf("\nCheck_ID::%d,Tag:: %d, M_ID:: %d, DCU_ID::%d, Unit:: %X ,Final Value :: %lf, DateTime :: %s\n",check_ID,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime);            
              #endif

              while (1)
              {
                      a=DCU;
                      b=Meter;
                      c=(S_Data[sock][tag-1].S_tag);
                      M_TAG=Tag_L[check_ID][a][b][c];
                      check=Data_Insert(M_TAG,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
                //check=Data_Insert(S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
                if (check == 0)
                {
                  break;
                }            
              }
          
              tag=tag+1;
              s3 = s3+1+j;
              #ifdef DEBUG
              printf("\n");
              #endif
              p=1;
              #ifdef DEBUG
              printf("\nUnsigned byte last position -- %d\n",s3);  
              #endif            
          }
          else if (Get_Response_Nr[s3+p] == 0x11)
          {
            value=Get_Response_Nr[s3+2];
            final_value=value*S_Data[sock][tag-1].S_value;
            #ifdef DEBUG
            printf("\nUnsigned ::");
            printf("%X \n",Get_Response_Nr[s3+2]);
            printf("\n\t>>>> The Value :: %d and tag :: %d ---- DataTime :: %s",value,tag,DateTime);
            printf("\nCheck_ID::%d,Tag:: %d, M_ID:: %d, DCU_ID::%d, Unit:: %X ,Final Value :: %lf, DateTime :: %s\n",check_ID,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime);            
            #endif
            while (1)
            {
                      a=DCU;
                      b=Meter;
                      c=(S_Data[sock][tag-1].S_tag);
                      M_TAG=Tag_L[check_ID][a][b][c];
                      check=Data_Insert(M_TAG,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
                //check=Data_Insert(S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
                if (check == 0)
                {
                  break;
                }            
            }
            
            tag=tag+1;
            s3 = s3+2;
            p=1;
            #ifdef DEBUG
            printf("\n");
            printf("\nUnsigned byte last position 22 -- %d\n",s3);
            #endif
          }   
          else if (Get_Response_Nr[s3+p] == 0x12)
          {
            value=(Get_Response_Nr[s3+1+p]<<8)|(Get_Response_Nr[s3+2+p]);
            final_value=value*S_Data[sock][tag-1].S_value;
            #ifdef DEBUG
            printf("\nLong Unsigned Data :: %d and tag :: %d  --- DateTime",value,tag,DateTime);
            printf("\nCheck_ID::%d,Tag:: %d, M_ID:: %d, DCU_ID::%d, Unit:: %X ,Final Value :: %lf, DateTime :: %s\n",check_ID,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime);            
            #endif
            while (1)
            {
                      a=DCU;
                      b=Meter;
                      c=(S_Data[sock][tag-1].S_tag);
                      M_TAG=Tag_L[check_ID][a][b][c];
                      check=Data_Insert(M_TAG,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
                //check=Data_Insert(S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
                if (check == 0)
                {
                  break;
                }            
            }          
            tag=tag+1;
            s3 = s3+2+p;
            p=1;
            #ifdef DEBUG
            printf("\n");
            printf("\nUnsigned byte last position 22 -- %d\n",s3);
            #endif
          }   
          else if (Get_Response_Nr[s3+p] == 0x02)
          {
                  int element_3,s4;

                    if (Get_Response_Nr[s3+1] == 0x82)
                    {
                        element_3=(Get_Response_Nr[s3+2]<<8)|(Get_Response_Nr[s3+3]);
                        #ifdef DEBUG
                        printf("\nReceived_1 the Structure of %d elements\n",element_3);
                        #endif
                        s4=s3+4;
                    }
                    else if (Get_Response_Nr[s3+1] == 0x81)
                    {
                      element_3=Get_Response_Nr[s3+2];
                      #ifdef DEBUG
                      printf("\nReceived_2 the Structure of %d elements\n",element_3);
                      #endif
                      s4=s3+3;
                    }
                    else
                    {
                      element_3=Get_Response_Nr[s3+1];
                      #ifdef DEBUG
                      printf("\nReceived_3 the Structure of %d elements\n",element_3);
                      #endif
                      s4=s3+2;  // next location of data
                    }

                    for (size_t k = 0; k < element_3; k++)
                    {
                      if (Get_Response_Nr[s4+p] == 0x16)
                      {
                        #ifdef DEBUG
                        printf("\nData Received is ENUM Value :: %d\n",Get_Response_Nr[s4+p+1]);
                        #endif
                        S_Data[sock][tag].S_Unit=Get_Response_Nr[s4+p+1];
                        S_Data[sock][tag].S_tag=tag+1;
                        
                        #ifdef DEBUG
                        printf("\nENUM Value :: %d and Tag :: %d --- DateTime :: %s\n",S_Data[sock][tag].S_Unit,S_Data[sock][tag].S_tag,DateTime);
                        #endif
                        s4 = s4+1+p;
                        #ifdef DEBUG
                        printf("\n");
                        #endif
                        p=1;
                        tag=tag+1;
                      }
                      else if (Get_Response_Nr[s4+p] == 0x0F)
                      {
                        #ifdef DEBUG
                        printf("\nData Received is Integer Value :: %d\n",Get_Response_Nr[s4+p+1]);
                        #endif
                        //S_Data[tag].S_value = 256-(Get_Response_Nr[s4+p+1]);
                        value = 256-(Get_Response_Nr[s4+p+1]);
                        if (value == 256)
                        {
                          value = 0;
                          S_Data[sock][tag].S_value=pow(10,-value);
                          #ifdef DEBUG
                          printf("\n1.conversion value is (0) %lf\n",S_Data[sock][tag].S_value);  
                          #endif                  
                        }
                        else
                        {
                          S_Data[sock][tag].S_value=pow(10,-value);
                          #ifdef DEBUG
                          printf("\n1.conversion value is %lf\n",S_Data[sock][tag].S_value);      
                          #endif              
                        }   
                        s4 = s4+1+p;
                        #ifdef DEBUG
                        printf("\n");
                        #endif
                        p=1;
                      } 
                      else if (Get_Response_Nr[s3+p] == 0x12)
                      {
                        int value;
                        
                        value=(Get_Response_Nr[s3+1+p]<<8)|(Get_Response_Nr[s3+2+p]);
                        final_value=value*S_Data[sock][tag-1].S_value;
                        #ifdef DEBUG
                        printf("\nLong Unsigned Data :: %d and tag :: %d ----- DateTime :: %s",value,tag,DateTime);
                        printf("\nCheck_ID::%d,Tag:: %d, M_ID:: %d, DCU_ID::%d, Unit:: %X ,Final Value :: %lf, DateTime :: %s\n",check_ID,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime);            
                        #endif
                        while (1)
                        {
                          a=DCU;
                          b=Meter;
                          c=(S_Data[sock][tag-1].S_tag);
                          M_TAG=Tag_L[check_ID][a][b][c];
                          check=Data_Insert(M_TAG,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
                          //check=Data_Insert(S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
                          if (check == 0)
                          {
                            break;
                          }            
                        }                
                        tag=tag+1;
                        s3 = s3+2+p;                        
                        p=1;
                        #ifdef DEBUG
                        printf("\n");
                        printf("\nUnsigned byte last position 22 -- %d\n",s3);
                        #endif
                      }
                      else if (Get_Response_Nr[s3+p] == 0x06)
                      {
                          value=(Get_Response_Nr[s3+1+p]<<24)|(Get_Response_Nr[s3+2+p]<<16)|(Get_Response_Nr[s3+3+p]<<8)|(Get_Response_Nr[s3+4+p]);
                          final_value=value*S_Data[sock][tag-1].S_value;
                          #ifdef DEBUG
                          printf("\n>>>> Double Long Unsigned :: ");
                          printf("\n\t>>>> Double Long Unsigned Value :: %d and Tag :: %d --- DateTime :: %s",value,tag,DateTime);   
                          printf("\nCheck_ID::%d,Tag:: %d, M_ID:: %d, DCU_ID::%d, Unit:: %X ,Final Value :: %lf, DateTime :: %s\n",check_ID,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime);            
                          #endif
                          while (1)
                          {
                            a=DCU;
                            b=Meter;
                            c=(S_Data[sock][tag-1].S_tag);
                            M_TAG=Tag_L[check_ID][a][b][c];
                            check=Data_Insert(M_TAG,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
                            //check=Data_Insert(S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
                            if (check == 0)
                            {
                              break;
                            }            
                          }
                          tag=tag+1;
                          s3 = s3+4+p;
                          p=1;   
                          #ifdef DEBUG  
                          printf("\n");
                          printf("\nDouble Long Unsigned byte last position -- %d\n",s3); 
                          #endif                                            
                      }        
                    }//for k
                    s3=s4;                        
          }
          else if (Get_Response_Nr[s3+p] == 0x16)
          {
            S_Data[sock][tag].S_Unit=Get_Response_Nr[s3+p+1];
            S_Data[sock][tag].S_tag=tag+1;
            #ifdef DEBUG
            printf("\nData Received is ENUM Value :: %d\n",Get_Response_Nr[s3+p+1]);
            printf("\nENUM Value :: %d and Tag :: %d --- DateTime :: %s\n",S_Data[sock][tag].S_Unit,S_Data[sock][tag].S_tag,DateTime);             
            #endif
            s3 = s3+1+p;
            #ifdef DEBUG
            printf("\n");
            #endif
            p=1;
            tag=tag+1;
          }
          else if (Get_Response_Nr[s3+p] == 0x0F)
          {
            #ifdef DEBUG
            printf("\nData Received is Integer Value :: %d\n",Get_Response_Nr[s3+p+1]);
            #endif
            value = 256-(Get_Response_Nr[s3+p+1]);
            
            //  S_Data[sock][tag].S_value = 256-(Get_Response_Nr[s3+p+1]);
            if (value == 256)
            {
              value = 0;
              S_Data[sock][tag].S_value=pow(10,-value);
              #ifdef DEBUG
              printf("\n2.conversion value is (0) %lf\n",S_Data[sock][tag].S_value);             
              #endif
            }
            else
            {
              S_Data[sock][tag].S_value=pow(10,-value);
              #ifdef DEBUG
              printf("\n2.conversion value is %lf\n",S_Data[sock][tag].S_value);             
              #endif
            }          
            s3 = s3+1+p;
            #ifdef DEBUG
            printf("\n");
            #endif
            p=1;
          }                     
        else
        {
            #ifdef DEBUG
            printf("\n Get_Response_Normal Condition Need to be written\n");
            sleep(3);
            #endif
            
            break;
          }                                          
      }//for j
    }// for i 
  if (Get_Response_Nr[10] == 194)
  {
    Get_Response_Block(Block_Buffer,(*size_final1),DCU,Meter,DateTime,sock);
  }
  else if (Get_Response_Nr[10] == 196)
  {
    Get_Response_Block(Block_Buffer,(*size_final1),DCU,Meter,DateTime,sock);
  }
  else
  {
    #ifdef DEBUG
    printf("\n");
    #endif
  }
  return 0; 
}



int Get_Response_List(unsigned char Get_Response[MAX],int size,int DCU,int Meter,unsigned char DateTime[13],int sock){


}



int Get_Response_Block_FINAL(unsigned char Get_Response_Final[MAX],int size_max,int DCU,int Meter,int pos,unsigned char Block_Buffer[MAX1],int *size_final1,unsigned char DateTime[13],int sock){

    int i,j;
    int Block_No;
    int size;
    int ss;
    int check;
    //int pos;
    int size_final;
    size_final=0;  

    Block_No=(Get_Response_Final[12]<<24)|(Get_Response_Final[13]<<16)|(Get_Response_Final[14]<<8)|(Get_Response_Final[15]);

    #ifdef DEBUG
    printf("\nReceived Block_No :: %d\n",Block_No);
    #endif
    switch (Get_Response_Final[16])
      {
       case 0x00:
        {
            #ifdef DEBUG
            printf("Data Received is the OctetString Data");
            #endif

            if ( (Get_Response_Final[11]==0x00) && (Block_No == 1) )
                {
                  pos=0;
                  #ifdef DEBUG
                  printf("\nReceived the 1st Block\n");
                  #endif
                  for (i = 0; i < size_max; i++)
                  {
                    Block_Buffer[i] = Get_Response_Final[i];
                    #ifdef DEBUG
                    printf("%X ",Block_Buffer[i]);
                    #endif
                  }
                  pos=pos+i-1;  //saving the buffer location along with size
                }
                else if ((Get_Response_Final[11]==0x01) && (Block_No > 1))
                {
                  #ifdef DEBUG
                  printf("\nReceived the Last Block\n");
                  #endif
                  
                  if (Get_Response_Final[17] == 0x82)
                  {
                    size=(Get_Response_Final[18]<<8)|(Get_Response_Final[19]);  //size of entire 1 block  
                    ss=20;                   
                  }
                  else if (Get_Response_Final[17] == 0x81)
                  {
                    size=Get_Response_Final[18];     //size of entire 1 block 
                    ss=19;                    
                  }
                  else
                  {
                    size=Get_Response_Final[17];     //size of entire 1 block    
                    ss=18;                  
                  }
                    
                  for (i = 0; i < size; i++)
                  {
                    Block_Buffer[pos+i+1] = Get_Response_Final[ss+i];
                    #ifdef DEBUG
                    printf("%X ",Block_Buffer[pos+i+1]);
                    #endif
                  }
                  pos=pos+i;    
                  size_final = pos;  
                  #ifdef DEBUG
                  printf("\nBuffer position is %d\n",pos);               
                  #endif
                }
                else
                {
                  #ifdef DEBUG
                    printf("\nReceived the %d Block\n",Block_No);
                    #endif
                    if (Get_Response_Final[17] == 0x82)
                    {
                      size=(Get_Response_Final[18]<<8)|(Get_Response_Final[19]);  //size of entire 1 block 
                      ss=20;                    
                    }
                    else if (Get_Response_Final[17] == 0x81)
                    {
                      size=Get_Response_Final[18];     //size of entire 1 block                     
                      ss=19;
                    }
                    else
                    {
                      size=Get_Response_Final[17];     //size of entire 1 block                      
                      ss=18;
                    }
                    #ifdef DEBUG
                    printf("\nSize of the Raw Data: %d bytes\n",size);
                    #endif
                    for (i = 0; i < size; i++)
                    {
                      Block_Buffer[pos+i+1] = Get_Response_Final[ss+i];
                      #ifdef DEBUG
                      printf("%X ",Block_Buffer[pos+i+1]);
                      #endif
                    }
                    pos=pos+i;   
                    #ifdef DEBUG
                    printf("\nBuffer position is %d\n",pos);
                    #endif
                } 
        }//Case 00
        break;

      case 0x01:
        #ifdef DEBUG
        printf("\nData Received is the DataAccessResult\n");
        #endif
        break;

      default:
        break;               
    }//switch end 

    if (size_final > 0)
    {
        (*size_final1)=size_final;
        #ifdef DEBUG
        printf("\nReceived the Full Block_Buffer of size :: %d bytes\n",size_final);
        sleep(3);
        for (i = 0; i <= size_final; i++)
        {
          printf("%X ",Block_Buffer[i]);
        }
        printf("\n");
        sleep(3);
        #endif

        if ( (Get_Response_Final[10] == 193) || (Get_Response_Final[10] == 195) )
        {
          //check=Get_Response_Block(Block_Buffer,size_final);
          //return &Block_Buffer;
          return 0;
        }
        else
        {
          check=Get_Response_Block(Block_Buffer,size_final,DCU,Meter,DateTime,sock);
        }                 
    }
   /***************************/   
    return pos;
}

int Get_Response_Block(unsigned char Get_Response[MAX1],int size,int DCU,int Meter,unsigned char DateTime[13],int sock){

      int W,j,i;      
      int size_1,size_2,size_3,ss,ss_1,loc;
      int DR; //Data Received
      int elements,M_tag;
      int num,check,check_ID;

      W=10;
      DR=0;
      num=0;
      M_tag=1;
      #ifdef DEBUG
      printf("\n\nData Response :: %X \n",Get_Response[8]);
      printf("Data Block    :: %X \n",Get_Response[9]);
      printf("InvokeIdAndPriority :: %X \n",Get_Response[10]);
      #endif

       if ((Get_Response[10] == 194)||(Get_Response[10] == 193))
      {
        check_ID=0;
      }
      else if ((Get_Response[10] == 196)||(Get_Response[10] == 195))
      {
        check_ID=1;
      }
      else
      {
        #ifdef DEBUG
        printf("\n");
        #endif
      }

      switch (Get_Response[16])
      {
       case 0x00:
        if (Get_Response[16] == 0x00)
        {
          #ifdef DEBUG
            printf("\nData Received is the OctetString Data\n");
            #endif
                     
            if (Get_Response[17] == 0x82)
            {
                size_2=(Get_Response[18]<<8)|(Get_Response[19]);
                ss=19;
            }
            else if (Get_Response[17] == 0x81)
            {
                size_2=Get_Response[18];
                ss=18;
            }
            else
            {
                size_2=Get_Response[17];
                ss=17;
            }  
          /*-----------*/
          // while (1)
          // {  

            if (Get_Response[ss+1] == 0x01)
            {
                        if (Get_Response[ss+2] == 0x82)
                          {
                              elements=(Get_Response[ss+3]<<8)|(Get_Response[ss+4]);
                              #ifdef DEBUG
                              printf("\nReceived_1 the Array of %d elements\n",elements);
                              #endif
                              ss_1=ss+4;
                            }
                        else if (Get_Response[ss+2] == 0x81)
                            {
                              elements=Get_Response[ss+3];
                              #ifdef DEBUG
                              printf("\nReceived_2 the Array of %d elements\n",elements);
                              #endif
                              ss_1=ss+3;
                            }
                        else
                            {
                              elements=Get_Response[ss+2];
                              #ifdef DEBUG
                              printf("\nReceived_3 the Array of %d elements\n",elements);
                              #endif
                              ss_1=ss+2;
                            }

                        while (1)
                        {
                                #ifdef DEBUG
                                printf("\nThe Size is while %d and num is %d\n",size,num);
                                #endif
                                num=num+1;
                                if (size <= (DR+1))
                                {
                                  #ifdef DEBUG
                                  printf("\n breaking the while size \n");
                                  #endif
                                  break;
                                }
                                else if ((num)>elements)
                                {
                                  #ifdef DEBUG
                                  printf("\n breaking the while num \n");
                                  #endif
                                  break;
                                }                                
                                else
                                {  
                                    if (DR != 0)
                                    {
                                      ss_1 = DR;
                                    }                                                                       
                                                                  
                                    if (Get_Response[ss_1+1] == 0x02) //structure inside the array
                                    {
                                      if (Get_Response[ss_1+2] == 0x82)
                                        {
                                          size_3=(Get_Response[ss_1+3]<<8)|(Get_Response[ss_1+4]);  //size_3 is for number of structure elements
                                          loc=ss_1+5;
                                          DR=Get_Response_Block_Dec(Get_Response,size_3,loc,M_tag,DCU,Meter,check_ID,DateTime,sock);
                                          #ifdef DEBUG
                                          printf("\nDR position is %d\n",DR);
                                          #endif
                                          //sleep(3);
                                        }
                                        else if (Get_Response[ss_1+2] == 0x81)
                                        {
                                          size_3=Get_Response[ss_1+3];
                                          loc=ss_1+4;
                                          DR=Get_Response_Block_Dec(Get_Response,size_3,loc,M_tag,DCU,Meter,check_ID,DateTime,sock);
                                          #ifdef DEBUG
                                          printf("\nDR position is %d\n",DR);
                                          #endif
                                          //sleep(3);
                                        }                      
                                        else
                                        {
                                          size_3=Get_Response[ss_1+2];
                                          loc=ss_1+3;
                                          DR=Get_Response_Block_Dec(Get_Response,size_3,loc,M_tag,DCU,Meter,check_ID,DateTime,sock);
                                          #ifdef DEBUG
                                          printf("\nDR position is %d\n",DR);
                                          #endif
                                          //sleep(3);
                                        }//else   
                                    }//if
                                  }//else                                      
                        }//while
            }//if
            else if (Get_Response[ss+1] == 0x02)
            {
                if (Get_Response[ss+2] == 0x82)
                {
                    size_3=(Get_Response[ss+1+2]<<8)|(Get_Response[ss+1+3]);
                    // loc=DR1+4;
                     loc=ss+5;                    
                    DR=Get_Response_Block_Dec(Get_Response,size_3,loc,M_tag,DCU,Meter,check_ID,DateTime,sock);
                }
                else if (Get_Response[ss+1+1] == 0x81)
                {
                    size_3=Get_Response[ss+1+2];
                    loc=ss+1+3;                    
                    DR=Get_Response_Block_Dec(Get_Response,size_3,loc,M_tag,DCU,Meter,check_ID,DateTime,sock);
                }                      
                else
                {
                    size_3=Get_Response[ss+1+1];
                    loc=ss+1+2;                    
                    DR=Get_Response_Block_Dec(Get_Response,size_3,loc,M_tag,DCU,Meter,check_ID,DateTime,sock);
                }                               
            }
            else if (Get_Response[ss+1] == 0x17)
            {
              #ifdef DEBUG
              printf("\nUnsigned :: ");
             for (j = 0; j < 4; j++)
              {                
                  printf("%X ",Get_Response[ss+2+j]);
              }
              #endif
              DR = ss+1+j;
              #ifdef DEBUG
              printf("\n");  
              #endif           
            }            
            else
            {
              #ifdef DEBUG
                printf("\nGet_Response_Block Condition Need to be written 1\n");
                sleep(3);
                #endif
                break;
            }  
          // } //while           
        }//if for OctetString
        else
        {
          #ifdef DEBUG
          printf("\nData Received is the DataAccessResult\n");
          #endif
        } 
        break;

       case 0x01:
        printf("\nData Received is the DataAccessResult\n");
        switch (Get_Response[11])
            {
            case 0x00:
              printf("\n Success \n");
              break;
            case 0x01:
              printf("\n HardwareFault \n");
              break;
            case 0x02:
              printf("\n TemporaryFailure \n");
              break;
            case 0x03:
              printf("\n ReadWriteDenied \n");
              break;
            case 0x04:
              printf("\n ObjectUndefined \n");
              break;
            case 0x09:
              printf("\n ObjectClassInconsistent \n");
              break;
            case 0x0B:
              printf("\n ObjectUnavailable \n");
              break;
            case 0x0C:
              printf("\n TypeUnmatched \n");
              break;
            case 0x0D:
              printf("\n ScopeOfAccessViolated \n");
              break;
            case 0x0E:
              printf("\n DataBlockUnavailable \n");
              break;
            case 0x0F:
              printf("\n LongGetOrReadAborted \n");
              break;
            case 0x10:
              printf("\n NoLongGetOrReadInProgress \n");
              break;
            case 0x11:
              printf("\n LongSetOrWriteAborted \n");
              break;
            case 0x12:
              printf("\n NoLongSetOrWriteInProgress \n");
              break;   
            case 0x13:
              printf("\n DataBlockNumberInvalid \n");
              break;    
            default:
              printf("\n OtherReason\n");
              break;
            }
        break;

       default:
        break;
      }//switch
      printf("\n DATA WRITTEN INTO DB\n");

}


int Get_Response_Block_Dec(unsigned char Get_Response1[MAX],int size1,int loc1,int tag,int DCU,int Meter,int check_ID,unsigned char DateTime[13],int sock){

    int p,i,j,check;
    int Year;
    int Month;
    int Date;
    int Week_of_Day;
    int Hour;
    int Minute;
    int Second;
    int MilliSecond;
    int Deviation;
    int Status;
    unsigned int value,Unit;
    double final_value;
    unsigned char Date_Buffer[13];
    int M_TAG;
    int a,b,c,d;
   // #ifdef DEBUG   
    printf("\n>>>> Get_Response_Block_Dec:: position %d \n",loc1);
    printf("\nReceived the Structure of %d elements\n",size1);
    //#endif
    
    p=0;
    for (i = 0; i < size1; i++)
    {
        //#ifdef DEBUG
        printf("\n---------------------------------------------------------\n");
        //#endif
        if (Get_Response1[loc1+p] == 0x09)
        {
            //#ifdef DEBUG
            printf("\nReceived the OctetString(9) of size %d\n",Get_Response1[loc1+1+p]);
            printf("OctetString :: \n");
            //#endif

            for (j = 0; j < Get_Response1[loc1+1+p]; j++)    /* loc == ss_1+3 */
            {          
                //#ifdef DEBUG      
                printf("%X ",Get_Response1[loc1+2+j+p]);
                //#endif
                Date_Buffer[j]=Get_Response1[loc1+2+j+p];
            }
           
            if ((Get_Response1[loc1+2+p] == 0x07) && (tag == 1))
              {
                  Year = (Get_Response1[loc1+2+p]<<8)|(Get_Response1[loc1+2+1+p]);
                  Month = Get_Response1[loc1+2+2+p];
                  Date = Get_Response1[loc1+2+3+p];
                  Week_of_Day = Get_Response1[loc1+2+4+p];
                  Hour = Get_Response1[loc1+2+5+p];
                  Minute = Get_Response1[loc1+2+6+p];
                  Second = Get_Response1[loc1+2+7+p];
                  MilliSecond = Get_Response1[loc1+2+8+p];
                  Deviation = (Get_Response1[loc1+2+9+p]<<8)|(Get_Response1[loc1+2+10+p]);
                  Status = Get_Response1[loc1+2+11+p];

                  if (tag == 1)
                  {
                    sprintf(DateTime,"%d-%.2d-%.2d %.2d:%.2d:%.2d",Year,Month,Date,Hour,Minute,Second);
                    final_value=0;
                    //#ifdef DEBUG
                    printf("\nTag :: %d, DataTime is :: %s\n",tag,DateTime); 
                    printf("\nCheck_ID::%d,Tag:: %d, M_ID:: %d, DCU_ID::%d, Unit:: %X ,Final Value :: %lf, DateTime :: %s\n",check_ID,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime);            
                    //#endif
                    while (1)
                    {
                      a=DCU;
                      b=Meter;
                      c=(S_Data[sock][tag-1].S_tag);
                      M_TAG=Tag_L[check_ID][a][b][c];
                      check=Data_Insert(M_TAG,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
                      //check=Data_Insert(S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
                      if (check == 0)
                      {
                       break;
                      } 
                      if (check == -1)
                      {
                        //sleep(10);
                        break;
                      }           
                    }
                    //sleep(5);
                  } 
              }//if
                  else
                  {
                    final_value=0;
                    //#ifdef DEBUG
                     printf("\nCheck_ID::%d,Tag:: %d, M_ID:: %d, DCU_ID::%d, Unit:: %X ,Final Value :: %lf, DateTime :: %s\n",check_ID,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime);            
                    //#endif
                    while (1)
                    {
                      a=DCU;
                      b=Meter;
                      c=(S_Data[sock][tag-1].S_tag);
                      M_TAG=Tag_L[check_ID][a][b][c];
                      check=Data_Insert(M_TAG,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
                      //check=Data_Insert(S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
                      if (check == 0)
                      {
                        break;
                      } 
                      if (check == -1)
                      {
                        //sleep(10);
                        break;
                      }           
                    }          
                  } //else                 
   //           }//if
          
            tag=tag+1;
            loc1 = loc1+1+j+p;          
            p=1;               
            //#ifdef DEBUG
            printf("\nOctetString byte last position -- %d\n",loc1);                                            
            //#endif
        }
        else if (Get_Response1[loc1+p] == 0x17)
        {
            //#ifdef DEBUG
            printf("\nUnsigned(17) :: ");
            for (j = 0; j < 4; j++)
            {                
                printf("%X ",Get_Response1[loc1+1+p+j]);
            }
            //#endif
            value=(Get_Response1[loc1+1+p+0]<<24)|(Get_Response1[loc1+1+p+1]<<16)|(Get_Response1[loc1+1+p+2]<<8)|(Get_Response1[loc1+1+p+3]);
            final_value=value*(S_Data[sock][tag-1].S_value);
            //#ifdef DEBUG
            printf("\nCheck_ID::%d,Tag:: %d, M_ID:: %d, DCU_ID::%d, Unit:: %X ,Final Value :: %lf, DateTime :: %s\n",check_ID,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime);            
            printf("\nValue %d  and unit :: %lf\n",value,S_Data[sock][tag-1].S_value);
            //#endif
           
            while (1)
            {
                      a=DCU;
                      b=Meter;
                      c=(S_Data[sock][tag-1].S_tag);
                      M_TAG=Tag_L[check_ID][a][b][c];
                      check=Data_Insert(M_TAG,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
              //check=Data_Insert(S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
              if (check == 0)
              {
                break;
              }  
              if (check == -1)
            {
              //sleep(10);
              break;
            }          
            }          
            loc1 = loc1+1+j;            
            tag=tag+1;
            p=1;

            //#ifdef DEBUG
            printf("\nUnsigned byte last position -- %d\n",loc1);
            //#endif
        }
        else if (Get_Response1[loc1+p] == 0x12)
        {                            
          value=(Get_Response1[loc1+1+p]<<8)|(Get_Response1[loc1+2+p]);
          final_value=value*S_Data[sock][tag-1].S_value;
          //#ifdef DEBUG
          printf("\nLong Unsigned Data (12):: %d and tag :: %d ----- DateTime :: %s",value,tag,DateTime);
          printf("\nCheck_ID::%d,Tag:: %d, M_ID:: %d, DCU_ID::%d, Unit:: %X ,Final Value :: %lf, DateTime :: %s\n",check_ID,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime);            
          printf("\nValue %d  and unit :: %lf\n",value,S_Data[sock][tag-1].S_value);
          //#endif
         
          while (1)
          {
                      a=DCU;
                      b=Meter;
                      c=(S_Data[sock][tag-1].S_tag);
                      M_TAG=Tag_L[check_ID][a][b][c];
                      check=Data_Insert(M_TAG,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
            //check=Data_Insert(S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);

            if (check == 0)
            {
              break;
            } 
            if (check == -1)
            {
              //sleep(10);
              break;
            }           
          }          
          tag=tag+1;
          loc1 = loc1+2+p;                     
          p=1;

          //#ifdef DEBUG
          printf("\nUnsigned byte last position 22 -- %d\n",loc1);
          //#endif
        }
        else if (Get_Response1[loc1+p] == 0x06)
        {  
          value=(Get_Response1[loc1+1+p]<<24)|(Get_Response1[loc1+2+p]<<16)|(Get_Response1[loc1+3+p]<<8)|(Get_Response1[loc1+4+p]);
          final_value=value*S_Data[sock][tag-1].S_value;
          //#ifdef DEBUG
          printf("\n>>>> Double Long Unsigned :: ");
          printf("\n\t>>>  Double Long Unsigned Value(6) :: %d and Tag :: %d --- DateTime :: %s",value,tag,DateTime);   
          printf("\nCheck_ID::%d,Tag:: %d, M_ID:: %d, DCU_ID::%d, Unit:: %X ,Final Value :: %lf, DateTime :: %s\n",check_ID,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime);            
          //#endif
         
          while (1)
          {
                      a=DCU;
                      b=Meter;
                      c=(S_Data[sock][tag-1].S_tag);
                      M_TAG=Tag_L[check_ID][a][b][c];
                      check=Data_Insert(M_TAG,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
            //check=Data_Insert(S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);

            if (check == 0)
            {
              break;
            }  
            if (check == -1)
            {
              //sleep(10);
              break;
            }          
          }          
          tag=tag+1;
          loc1 = loc1+4+p;
          p=1;     
         // #ifdef DEBUG
          printf("\nDouble Long Unsigned byte last position -- %d\n",loc1);                                             
          //#endif
        }        
        else if (Get_Response1[loc1+p] == 0x11)
        {
          //#ifdef DEBUG
          printf("\nUnsigned(11) ::");
          printf("%X \n",Get_Response1[loc1+2]);
          //#endif
          
          value=Get_Response1[loc1+2];
          loc1 = loc1+2;         
          final_value=value*S_Data[sock][tag-1].S_value;
          
          //#ifdef DEBUG                   
          printf("\nValue of the Unsigned (11):: %d and Tag: %d --- DateTime :: %s\n",value,tag,DateTime);
          printf("\nCheck_ID::%d,Tag:: %d, M_ID:: %d, DCU_ID::%d, Unit:: %X ,Final Value :: %lf, DateTime :: %s\n",check_ID,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime);            
          //#endif
          
          while (1)
          {
                      a=DCU;
                      b=Meter;
                      c=(S_Data[sock][tag-1].S_tag);
                      M_TAG=Tag_L[check_ID][a][b][c];
                      check=Data_Insert(M_TAG,S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
            //check=Data_Insert(S_Data[sock][tag-1].S_tag,Meter,DCU,S_Data[sock][tag-1].S_Unit,final_value,DateTime,check_ID);
            if (check == 0)
            {
              break;
            } 
            if (check == -1)
            {
              //sleep(10);
              break;
            }
                       
          }
          
          tag=tag+1;
          p=1;
          //#ifdef DEBUG
          printf("\nUnsigned byte last position 22 -- %d\n",loc1);
          //#endif
        }   
        else if (Get_Response1[loc1+p] == 0x02)
        {
          int size_4,loc2,loc3;
          //#ifdef DEBUG
          printf("\nReceived the Internal Structure(2) --- %d/%d \n",i,size1);
          //#endif
          size_4=Get_Response1[loc1+p+1];
          loc2=loc1+p+2;
          loc3=Get_Response_Block_Dec(Get_Response1,size_4,loc2,tag,DCU,Meter,check_ID,DateTime,sock);
          p=1;
          loc1=loc3;// Returning the location back          
        }
        else if (Get_Response1[loc1+p] == 0x16)
        {
         // #ifdef DEBUG
          printf("\nData Received is ENUM Value(16) :: %d\n",Get_Response1[loc1+p+1]);
          //#endif
         
          Unit=Get_Response1[loc1+p+1];
          
          //#ifdef DEBUG
          printf("\nValue of the ENUM Value  :: %d and Tag: %d --- DateTime :: %s\n",Unit,tag,DateTime);
         // #endif
          
          tag=tag+1;
          loc1 = loc1+1+p;
          p=1;
        }
        else if (Get_Response1[loc1+p] == 0x0F)
        {
          //#ifdef DEBUG
          printf("\nData Received is Integer Value(F) :: %d\n",Get_Response1[loc1+p+1]);
          //#endif
          
          value = 256-(Get_Response1[loc1+p+1]);
          if (value == 256)
          {
            value = 0;
            S_Data[sock][tag].S_value=pow(10,-value);
            
           // #ifdef DEBUG
            printf("\n3.conversion value is (0) %lf\n",S_Data[sock][tag].S_value);
            //#endif
          }
          else
          {
            S_Data[sock][tag].S_value=pow(10,-value);
            
            //#ifdef DEBUG
            printf("\n3.conversion value is %lf\n",S_Data[sock][tag].S_value);
           // #endif
          }
          loc1 = loc1+1+p;
          p=1;
          
        }                     
       else
       {
            printf("\nGet_Response_Block_Dec Condition Need to be written\n");
            //sleep(3);
            break;
        }                                          
    }//for   

    #ifdef DEBUG
    printf("\n Returning the location %d\n",loc1);
    #endif

    return loc1; 
}



