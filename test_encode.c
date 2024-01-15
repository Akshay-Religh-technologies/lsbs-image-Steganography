#include <stdio.h>
#include "encode.h"
#include<string.h>
#include "types.h"
#include "decode.h"
#include<unistd.h>
//function defination
OperationType check_operation_type(char *argv[])
{
     if (strcmp(argv[1], "-e") == 0)
     {
	  return e_encode;
     } else if (strcmp(argv[1], "-d") == 0)
     {
	  return e_decode;
     } else
     {
	  return e_unsupported;
     }
}


int main(int argc,char *argv[])
{
     //Structur variable for encoding
     EncodeInfo encInfo;
     uint img_size;
     OperationType op_type = check_operation_type(argv);
     //checking the argument count	
     if (op_type == e_encode)
     {  
	 // OperationType op_type = check_operation_type(argv);
	  if (argc >= 4)
	  {
	       printf("\n");
	       loading_bar();
	       printf("**********************Encoding started*****************************\n");
	       printf("\n");
	       sleep(1);	
	       //checking if bmp and txt file is passed or not

	       if(read_and_validate_encode_args(argv,&encInfo)==e_success)
	       {
		    printf("Started Validating .....\n");
		    sleep(1);
		    printf("Validation Successfully Completed \n");
		    sleep(1);
		    printf("file_name is %s\n",encInfo.src_image_fname);
		    sleep(1);
		    printf("secret_fname is %s\n",encInfo.secret_fname);
		    sleep(1);
		    do_encoding(&encInfo);

	       }
	       else
	       {
		    printf("Error in Validating....\n");
		    printf("Following maybe the reasons : \n");
		    printf("-------------------------------------------------------------------\n");
		    printf("%s file extension incorrect (extension must be .bmp)\n",argv[2] );
		    printf("%s secret file extension incorrect (extension must be .txt\n",argv[3]);
		    printf("%s extension mismatch\n",argv[4]);
	       }


	  } 
     }
     else if (op_type == e_decode)
     {
	 if(argc >=3 && argc < 5)
	 {  
  
	 	DecodeInfo decInfo;  
		printf("\n");
	 	printf("**************************Decoding Started ***********************************\n");
		printf("\n");
		sleep(1);
		printf("Started Validating.......\n");
		read_and_validate_arguments_for_decoding(argv,&decInfo);
		sleep(1);
		do_decoding(&decInfo);
		sleep(1);
		//printf("encoded image is %s ..\n",decInfo.output_filename); 
		//printf("outfile name will be created by the name %s \n",decInfo.stego_image_filename); 
	 }
	 else
	 {
	 	printf("Invalid arguments were passed\n");
	 }

     }
     else
     {
     	printf("Un -supported format\n");
     }



     /*
     // Fill with sample filenames
     encInfo.src_image_fname = "beautiful.bmp";
     encInfo.secret_fname = "secret.txt";
     encInfo.stego_image_fname = "stego_img.bmp";

     // Test open_files
     if (open_files(&encInfo) == e_failure)
     {
     printf("ERROR: %s function failed\n", "open_files" );
     return 1;
     }
     else
     {
     printf("SUCCESS: %s function completed\n", "open_files" );
     }

     // Test get_image_size_for_bmp
     img_size = get_image_size_for_bmp(encInfo.fptr_src_image);
     printf("INFO: Image size = %u\n", img_size);
      */	

     return 0;
}
