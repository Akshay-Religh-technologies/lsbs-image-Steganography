#include <stdio.h>
#include "decode.h"
#include "types.h"
#include<string.h>
#include "common.h"
#include<unistd.h>

/* Function Definitions */

//*****************************Read and validate the arguments for decoding*****************************************************

//check if the extension of the encoded file is .bmp or not 

Status	read_and_validate_arguments_for_decoding(char *argv[],DecodeInfo *decInfo)
{
     if( strcmp(strstr(argv[2],"."),".bmp") == 0)
     {

	  // Store the FILE NAME in a structure member 'stego_image_fname' (decode.h line--> ) if file passed is of .bmp extension.

	  decInfo->stego_image_filename=argv[2];
	  char extension[50];

	  if(argv[3]!=NULL)
	  {
	       // Store the FILE NAME in a structure member 'output_fname' (decode.h line--> ) if file name is passed.
	       strcpy(decInfo->output_filename , argv[3]);
	       strcpy(extension,strstr(argv[3],"."));
	       printf("extension stored is %s\n",extension);
	       printf("The output file is created by the name : %s\n",decInfo->output_filename);
	       return e_success;
	  } 
	  else
	  {
	       strcpy(decInfo->output_filename,"output.txt");
	       printf("The output file is created by the name : %s\n",decInfo->output_filename);

	       return e_success;
	  }
     }
     else
     {
	  printf("ERROR : INFO -> Image file Extension Din Match\n");
	  return e_failure;
     }


}

//**********************************opening the encoded image file and setting the file pointer *********************************

Status open_files_for_decoding(DecodeInfo *decInfo)

{

     decInfo->fptr_stego_image=fopen(decInfo->stego_image_filename,"r");
     decInfo->fptr_output_filename=fopen(decInfo->output_filename,"w");
     if(decInfo->fptr_stego_image==NULL)
     {

	  printf("%s File not opened\n",decInfo->stego_image_filename);
	  return e_failure;
     }
     else
     {
	  printf("%s File is opened\n",decInfo->stego_image_filename);
	  return e_success;
     }
}

//***************************************Moving file pointer to 54th byte in oreder to get the magic string***************************

Status move_file_pointer(DecodeInfo *decInfo)
{
     fseek(decInfo->fptr_stego_image,54,SEEK_SET);
     return e_success;	
}

//**************************************Decode Magic string************************************************************************

Status decode_magic_string(DecodeInfo *decInfo)
{
     char magic_string[3];
     char image_buffer[8];
     int i;
     for(i=0;i<2;i++)
     {
	  fread(image_buffer,8,1,decInfo->fptr_stego_image);

	  magic_string[i]=decode_bytes_from_lsb(image_buffer);
     }

     magic_string[i]='\0';

     //printf("Magic string is %s\n",magic_string);
     if(strcmp(magic_string,"#*")==0)
     {
	  return e_success;
     }	 
     else
     {
	  return e_failure;
     }

}

//************************************************Decode bye to lsb*********************************************************************

char  decode_bytes_from_lsb(char *buff)
{

     char ch=0;   
     for(int i=0;i<8;i++)
     {
	  ch = ((( buff[i] & 1 )<<i)  | ch );

     } 
     return ch;
}

//****************************************Decode secret file extension size *********************************************************

Status decode_secret_file_extension_size(DecodeInfo *decInfo)
{
     char buffer[32];
     fread(buffer,32,1,decInfo->fptr_stego_image);
     decInfo->extension_size=decode_size_from_lsb(buffer);
     //printf("size is %d\n",decInfo->extension_size);
     return e_success;	
}

//*****************************************decode size from lsb**********************************************************************

Status decode_size_from_lsb(char *buffer)
{
     char ch=0;
     for(int i=0;i<32;i++)
     {
	  ch = ((( buffer[i] & 1 )<<i)  | ch );
	  //printf("%0hx  " , buffer[i]);
     }
     return ch;
}


//********************************************decode secret file extensio based on size we got ***************************************

Status decode_secret_file_extension(DecodeInfo *decInfo, int size)

{
     int i;
     //char store[size];
     char arr[8];
     for( i=0;i<size;i++)
     {
	  fread(arr,8,1,decInfo->fptr_stego_image);
	  decInfo->store_extension[i]=decode_bytes_from_lsb(arr);

     }
     decInfo->store_extension[i]='\0';
     //printf("decode extension we got is %s\n",decInfo->store_extension);
     return e_success;


}

//***************************************decode secret file data size ***************************************************************

Status decode_secret_file_data_size(DecodeInfo *decInfo)
{
     char arr[32];
     fread(arr,32,1,decInfo->fptr_stego_image); 
     decInfo->data_size=decode_size_from_lsb(arr);
     //printf("size of the data %d\n",decInfo->data_size);	
     return e_success;

}


Status decode_secret_file_data(DecodeInfo *decInfo,int size)
{
     int i;  
     char arr[8];
     for(i=0;i<size;i++)
     {
	  fread(arr,8,1,decInfo->fptr_stego_image);
	  decInfo->data[i]= decode_bytes_from_lsb(arr);	
     }	
     decInfo->data[i]='\0';
     fwrite(decInfo->data,decInfo->data_size,1,decInfo->fptr_output_filename);
     //printf("The data stored in the secret file is :  %s \n",decInfo->data);
     return e_success;
}


//****************************************do decoding ********************************************************************************

Status do_decoding(DecodeInfo *decInfo)
{
     open_files_for_decoding(decInfo);
     move_file_pointer(decInfo);	
     if(decode_magic_string(decInfo)==e_success)
     {
	  // printf("Lets continue\n");
	  sleep(1);
	  printf("INFO -> Decoding the Magic String\n");
	  printf("Done... !!!\n");

	  if((decode_secret_file_extension_size(decInfo))==e_success)
	  {
	       sleep(1);
	       printf("INFO -> Decoding the Secret File Extension Size\n");
	       printf("Done... !!!\n");

	       if((decode_secret_file_extension(decInfo,decInfo->extension_size)==e_success))
	       {
		    sleep(1);
		    printf("INFO -> Decoding the Secret File Extension\n");
		    printf("Done... !!!\n");

		    if((decode_secret_file_data_size(decInfo))==e_success)
		    {
			 sleep(1);
			 printf("INFO -> Decoding the Secret File Size\n");
			 printf("Done... !!!\n");

			 if((decode_secret_file_data(decInfo,decInfo->data_size))==e_success)
			 {
			      sleep(1);
			      printf("INFO -> Decoding the Secret File Data\n");
			      printf("Done... !!!\n");
			      printf("INFO -> Secret Data Available in %s File\n",decInfo->output_filename);
			 }
			 else
			 {
			      printf("ERROR : INFO -> Error In Decoding the Secret File Data\n");

			 }
		    }
		    else
		    {
			 printf("ERROR : INFO -> Error In Decoding Secret File Data Size\n" );
		    }
	       }
	       else
	       {
		    printf("ERROR : INFO -> Error In Decoding the Secret File Extension\n");

	       }
	  }
	  else
	  {
	       printf("ERROR : INFO -> Error In Decoding Secret File Extension Size\n");
	  }		  
     }
     else
     {
	  printf("Magic string errror\n");
     }

     return e_success;
}
