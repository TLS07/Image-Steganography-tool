/*Name-T.L.Srinath
Project- LSB image stegonogrpahy*/


#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include<stdlib.h>
#include "decode.h"

int main(int argc, char *argv[])
{
    
    EncodeInfo E1;
    DecodeInfo  D1;
    // to validate command line arguments
    // for encoding minimum 4 and decoding 3

      /* Check for minimum argument count */
    if(argc<3){

        printf("Encoding: ./a.out  -e  <src_image_name.bmp>  <secret_file.txt>  <optional_output_image_name.bmp>\n");
        printf("Decoding: ./a.out  -d  <stego_image.bmp>     <optional_secret_file_name.txt>\n");
        return 1;

    }

    // to determine whether encoding or decoding to be done
    int res=check_operation_type(argv);

    if(res==e_encode){
        printf("INFO: Encoding is selected\n");

        if(read_and_validate_encode_args(argc,argv,&E1)==e_success){
            printf("INFO: Read and Validate encode args is Success\n");

            if(do_encoding(&E1)==e_success){
                printf("INFO : Encoding is sucess\n");
            }
        }
        else{
            printf("ERROR: Failded to Read and Validate encode args\n");
        }

    }
    else if(res ==e_decode){
        printf("INFO: Decoding is selected\n");
        

        if(read_and_validate_decode_args(argc,argv,&D1)==e_success){
            printf("INFO: Read and Validate decode args is Success\n");

            if(do_decoding(&D1) == e_success){
            printf("INFO: Decoding is successful\n");
           }
         else {
            printf("ERROR: Decoding failed\n");
        }
        }

     
    }
    else{
        printf("Invalid option\n");
        printf("Enter for :\n");
        printf("Encoding -> ./a.out  -e  <src_image_name.bmp>  <secret_file.txt>  <optional_output_image_name.bmp>\n");
        printf("Decoding -> ./a.out  -d  <stego_image.bmp>     <optional_secret_file_name.txt>\n");
    }

    return 0;
}


