#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include "common.h"
#include<unistd.h>
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

//**********************************Loading bar*****************************************************************

void loading_bar()
{
     /*        int i, k;
	       for (i = 1; i <= 100; i++) {
	       printf("\rLoading [");
	       for (k = 1; k <= 100; k++) {
	       k <= i ? putchar('-') : putchar(' ');
	       }
	       printf("]%3d%%", i);
	       fflush(stdout);
     // Delay using a loop
     for (int j = 0xFFFF; j--;);

     // Alternative method for delay (uncomment the line below and comment the loop above)
     //sleep(0.1); // Include unistd.h for the sleep function
     }
     printf("\n");
     printf("\n");
      */
}


//***********************************Encode the remaining data ***************************************************

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{

     /*
	int len=EncInfo->image_capacity + 54 -(ftell(encInfo->fptr_src_image));
	int arr[len];
	fread(arr,len,1,encInfo->fptr_src_image);
	fwrite(arr,len,1,encInfo->fptr_stego_image);
	printf("Remaining data encoded successfully\n");
      */
     char ch;
     while(fread(&ch,sizeof(char),1,fptr_src)>0)
     {
	  fwrite(&ch,1,1,fptr_dest);
     }
     return e_success;
}

//***********************************Encode secret file data ********************************************************

Status encode_secret_file_data(EncodeInfo *encInfo)
{
     char arr[encInfo->size_secret_file];
     rewind(encInfo->fptr_secret);
     fread(arr,1,encInfo->size_secret_file,encInfo->fptr_secret);
     encode_data_to_image(arr,strlen(arr),encInfo->fptr_src_image,encInfo->fptr_stego_image);

     return e_success; 
}


//*********************************Encode the size of secret file ****************************************************

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{

     encode_secret_file_extn_size_(file_size,encInfo->fptr_src_image,encInfo->fptr_stego_image);
     return e_success;
}

//*******************************Encode secret file extension*********************************************************

Status encode_secret_file_extn( char *file_extn, EncodeInfo *encInfo)
{
     if(encode_data_to_image(file_extn,strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image))
     {
	  return e_success;
     }
}

//*************************************Encode the size in lsb ********************************************************** 

Status encode_size_to_lsb(int data , char *buffer)
{

     for(int i=0;i<32;i++)
     {

	  buffer[i]=((buffer[i] & 0xFE ) | (data>>i) & 1);
     }     
     return e_success;
}

//***********************************Encode secret file extension size***************************************************

Status encode_secret_file_extn_size_(int size ,FILE *fptr_src_image,FILE *fptr_stego_image) 
{
     char buff_arr[32];
     fread(buff_arr,32,1,fptr_src_image);
     encode_size_to_lsb( size,buff_arr);
     fwrite(buff_arr,32,1,fptr_stego_image);
     return e_success;
}

//**********************************function to find the length of magic string********************************************

int magic_string_len(char *magic_str)
{
     int i=0;
     while(magic_str[i]!='\0')
     {
	  i++;
     }
     return i;
}

//***********************************encode magic string ********************************************************************

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
     encode_data_to_image(magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image);
}

/*Actual Encoding is done here */

//****************************Encoding the magic string (storing the details) ************************************************

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
     char image_buff[MAX_IMAGE_BUF_SIZE];
     //printf("magic string size -> %d\n",size);
     for(int i=0;i<size;i++)
     {
	  fread(image_buff,sizeof(char),8,fptr_src_image);
	  encode_byte_to_lsb(data[i],image_buff);
	  fwrite(image_buff,sizeof(char),8,fptr_stego_image);
     }	     
     // printf("after input bmp file postion -> %ld\n",ftell(fptr_src_image));
     // printf("after output bmp file postion -> %ld\n",ftell(fptr_stego_image));
     return e_success;
}

//************************************encode_byte_to_image*******************************************************************

Status encode_byte_to_lsb(char data,char *image_buffer)
{

     //for (int i = 0;i< 8;i++)
     //  printf("first -> 0%hhx\n",image_buffer[i]);
     for(int i=0;i<MAX_IMAGE_BUF_SIZE;i++)
     {
	  image_buffer[i]=((image_buffer[i]&0xFE) | ((data>>i)&1));
	  //image_buffer[i]=(((data&(1<<(7-i)))>>(7-i)) | image_buffer[i]&~1);
     }
     // for (int i = 0;i< 8;i++)
     //	  printf("next -> 0%hhx\n",image_buffer[i]);
     return e_success;
}

//********************************* Checking the capacity**********************************************************************

Status check_capacity(EncodeInfo *encInfo)
{

     encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
     // printf("image_size (image capacity) is %u\n",encInfo->image_capacity);

     encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
     // printf("secret_file_size is %ld\n",encInfo->size_secret_file);


     if((encInfo->image_capacity)>(16 + 32 + 32 +32 + (encInfo->size_secret_file*8)))
     {
	  return e_success;
     }    
     else
     {
	  return e_failure;
     }

}

//********************************* Getting the file size *********************************************************************

uint get_file_size(FILE *fptr)
{
     //setting fptr pointer to 0 and traversing till end to get number of character present in file  

     fseek(fptr,0,SEEK_END);
     return ftell(fptr);

}

//*****************************************copying the bmp header file********************************************************

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
     fseek(fptr_src_image,0,SEEK_SET);
     char arr[54];
     fread(arr,sizeof(char),54,fptr_src_image);
     fwrite(arr,sizeof(char),54,fptr_dest_image);
     fseek(fptr_src_image,54,SEEK_SET);
     fseek(fptr_dest_image,54,SEEK_SET);
     //printf("after input bmp file postion -> %ld\n",ftell(fptr_src_image));
     //printf("after output bmp file postion -> %ld\n",ftell(fptr_dest_image));
     return e_success;
}

//****************************Getting image size for bmp ************************************************************************


uint get_image_size_for_bmp(FILE *fptr_image)
{
     uint width, height;
     // Seek to 18th byte
     fseek(fptr_image, 18, SEEK_SET);

     // Read the width (an int)
     fread(&width, sizeof(int), 1, fptr_image);
     printf("width = %u\n", width);

     // Read the height (an int)
     fread(&height, sizeof(int), 1, fptr_image);
     printf("height = %u\n", height);

     // Return image capacity
     return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

//*****************************opening files************************************************************************

Status open_files(EncodeInfo *encInfo)
{
     printf("Opening Required File\n");

     // opening Src Image file using fptr file pointer

     encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");

     // Do Error handling

     if (encInfo->fptr_src_image == NULL)
     {
	  perror("fopen");
	  fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

	  return e_failure;
     }
     printf("Opening %s File\n",encInfo->src_image_fname);

     //opening Secret file using fptr_secret file pointer

     encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");

     // Do Error handling

     if (encInfo->fptr_secret == NULL)
     {
	  perror("fopen");
	  fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

	  return e_failure;
     }
     printf("Opening %s File\n",encInfo->secret_fname);


     //opening  Stego Image file using fptr_stego file pointer

     encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");

     // Do Error handling
     if (encInfo->fptr_stego_image == NULL)
     {
	  perror("fopen");
	  fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

	  return e_failure;
     }
     printf("Opening %s File\n",encInfo->stego_image_fname);


     // No failure return e_success
     return e_success;

}

//***************************Read and validate arguments *******************************************************************

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
     if(strcmp(strstr(argv[2],"."),".bmp") == 0)
	  encInfo->src_image_fname = argv[2];
     else
     {
	  printf("pass correct extension\n");
	  return e_failure;
     }
     if((strcmp(strstr(argv[3],"."),".txt") == 0) || (strcmp(strstr(argv[3],"."),".c") == 0) || (strcmp(strstr(argv[3],"."),".sh")==0) || (strcmp(strstr(argv[3],"."),".py")==0) )
     {
	  encInfo->secret_fname = argv[3];
     }
     else
     {
	  printf("pass correct extension\n");
	  return e_failure;
     }
     if (argv[4] != NULL)
     {
	  if(strcmp(strstr(argv[4],"."),".bmp") == 0)
	       encInfo->stego_image_fname = argv[4];
	  else
	  {
	       printf("pass correct extension\n");
	       return e_failure;
	  }
     }
     else
	  encInfo->stego_image_fname = "output.bmp";
     return e_success;
}
//**********************************Performing Encodeing operation ***************************************************************

Status do_encoding(EncodeInfo *encInfo)
{

     if(open_files(encInfo)==e_success)
     {   
	  if(check_capacity(encInfo)==e_success)
	  {	
	       sleep(1);     
	       printf("INFO :Checking for %s capacity to handel %s file \n" ,encInfo->src_image_fname,encInfo->secret_fname);
	       printf("Done !!! Not Empty\n");

	       if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
	       {
		    sleep(1);
		    printf("INFO : Copying Image header\n");
		    printf("Done !!!\n");

		    if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)
		    {
			 sleep(1);
			 printf("INFO : Copying Image Signature\n");
			 printf("Done !!!\n");

			 int demo=strlen(strstr(encInfo->secret_fname,"."));

			 if((encode_secret_file_extn_size_(demo,encInfo->fptr_src_image,encInfo->fptr_stego_image))==e_success)
			 {
			      sleep(1);
			      printf("INFO : Encoding Secret file Extension Size \n");
			      printf("Done !!!\n");


			      if(encode_secret_file_extn(strstr(encInfo->secret_fname,"."),encInfo)==e_success)
			      {
				   sleep(1);
				   printf("INFO : Encoding Secret file Extension \n");
				   printf("Done !!!\n");


				   if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
				   {
					sleep(1);
					printf("INFO : Encoding Secret file  Size \n");
					printf("Done !!!\n");

					if(encode_secret_file_data(encInfo)==e_success)
					{
					     sleep(1);
					     printf("INFO : Encoding Secret file Data \n");
					     printf("Done !!!\n");

					     if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
					     {
						  sleep(1);
						  printf("INFO : Encoding Remaining data\n");
						  printf("Done !!!\n");
						  printf("Encoding Completed Successfully\n");					     }
					}
					else
					{
					     printf("INFO : ERROR -> Failed to copy the remaing data \n");
					}
				   }
				   else
				   {
					printf("INFO : ERROR -> Encoding secret file size failed\n");
				   }
			      }
			      else
			      {
				   printf("INFO : ERROR -> Encoding secret file extension failed\n");
			      }
			 }
			 else
			 {
			      printf("INFO : ERROR -> Encoding secret file failed\n");
			 }
		    }
		    else
		    {
			 printf("INFO : ERROR -> Failed to Encode secret file extension\n");
		    }			      
	       }
	       else
	       {
		    printf("INFO : ERROR -> Ecoding Magic String failed\n");
	       }
	  }
	  else
	  {
	       printf("INFO : ERROR -> Copying bmp header file failed\n");
	  }
     }
     else
     {
	  printf("INFO : ERROR -> Capacity of file <%s> not enough\n",encInfo->src_image_fname);
     }
/*
     else
     {
	  printf("INFO : ERROR -> File can't be opened\n");
     }
*/

}







