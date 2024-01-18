#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
#include<stdio.h>

/* 
 * Structure to store information required for
 * decoding secret file to get the data
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
 
     char output_filename[100];
     FILE *fptr_output_filename;
     char extension_size;
     char data_size;
     char data[200];
     char store_extension[MAX_FILE_SUFFIX];
     char *stego_image_filename;
     FILE *fptr_stego_image;
     
     char extn_output_file[50];
     
} DecodeInfo;

/* Decoding function prototype */

//OperationType check_operation_type(char *argv[]);

//Read and validate the arguments (to check if the encoded image file is having .bmp extension or not)

Status read_and_validate_arguments_for_decoding(int argc,char *argv[], DecodeInfo *decInfo);

//Open image file thats now stored in stego_image_filname

Status open_files_for_decoding(DecodeInfo *decInfo);

//do decoding function 

Status do_decoding(DecodeInfo *decInfo);

//our data starts from 54th byte so moving file pointer to 54 position

Status move_file_pointer(DecodeInfo *decInfo);

//decoding the magic string #*

Status decode_magic_string(DecodeInfo *decInfo);

//decoding the bytes from lsb

char decode_bytes_from_lsb(char *buff);

//decode secret file extension size

Status decode_secret_file_extension_size(DecodeInfo *decInfo);

//decode size from lsb

Status decode_size_from_lsb(char *buffer);

//decode the the secret file extension

Status decode_secret_file_extension(DecodeInfo *decInfo, int size);

//decode the size of the secert data

Status decode_secret_file_data_size(DecodeInfo *decInfo);

//decode the data of secrete file

Status decode_secret_file_data(DecodeInfo *decInfo,int size);

#endif
