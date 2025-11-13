#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/*Function performing encoding*/
Status do_encoding(EncodeInfo *encInfo){

    //open files
    if(open_files(encInfo)==e_success){
        printf("INFO: Fopen is success\n");

      // to check capacity whether image can store secret message
        if(check_capacity(encInfo)==e_success){
            printf("INFO: secret data can be fit in the image file\n");

            // to copy the beautiful.bmp header to output file
            if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success){
                printf("INFO: BMP header copied successfully\n");
               
                // encode magic string
                if (encode_magic_string(MAGIC_STRING,encInfo)==e_success){
                    printf("INFO: Encoded magic string successfully\n");
                    strcpy(encInfo->extn_secret_file, strchr(encInfo->secret_fname,'.'));

                // Encode secret file extension size and extension
                   if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo)==e_success){
                    printf("INFO: Encoded secret_file_extn_size successfully\n");

                    if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success){
                        printf("INFO: Encoded secret file extn sucessfully\n");


                        //Encode secret file size
                        if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success){
                            printf("INFO: Encoded secret file size successfully\n");

                           //Encode secret file data
                            if(encode_secret_file_data(encInfo)==e_success){
                                printf("INFO: Encoded secret file data successfully\n");

                                //Copy remaining image data
                                if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success){
                                    
                                    printf("INFO: Image Encoded succesfull\n");
                                }
                                else{
                                    printf("ERROR: Failed to copy RGB data \n");
                                    return e_failure;
                                }
                            }
                            else{
                                printf("ERROR: Failed to encode file data\n");
                                return e_failure;
                            }
                        }
                        else{
                            printf("ERROR: Failed to encode file size\n");
                            return e_failure;
                        }
                    }
                    else{
                        printf("ERROR: Failed to encode secret file extn successfully\n");
                        return e_failure;
                    }
                   }
                   else{
                    printf("ERROR: Failed to encode secret_file_extn_size\n");
                   }
                }
                else{
                    printf("ERROR: Failed to encode magic string \n");
                    return e_failure;
                }
            }
            else{
                printf("ERROR : Failed to copy bmp header\n");
                return e_failure;
            }
        }
        else{
            printf("ERROR: Image file capacity is less to encode secret message\n");
            return e_failure;
        }
    }
    else{
        printf("ERROR : Failed to open files\n");
        return e_failure;
    }

    return e_success;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte , bmp file stores image width
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    //Since each pixel has 3 bytes (R, G, B), capacity = width × height × 3
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

 // open all the required files
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");

    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");

    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

///////////////////////////////////////////////////////////////////

// fucntion to check encoding or decoding  to be performed
OperationType check_operation_type(char *argv[]){
    if(strcmp(argv[1],"-e")==0){
        return e_encode;
    }

    else if(strcmp(argv[1],"-d")==0){
        return e_decode;
    }

    else{
        return e_unsupported;
    }

}



// to validate encode arguments passed
Status read_and_validate_encode_args(int argc,char *argv[], EncodeInfo *encInfo){
   
    //  to check src_image file
    if(strstr(argv[2],".bmp")!= NULL){
        encInfo->src_image_fname=argv[2];
    }else{
        return e_failure;
    }

    // to check secret file
    if(strstr(argv[3],".txt")!=NULL){
        encInfo->secret_fname=argv[3];
    }else{
        return e_failure;
    }

    // to check output file name given or not , if not default
    if(argc>4 && strstr(argv[4],".bmp")!=NULL){
        encInfo->stego_image_fname=argv[4];
    }
    else{
        encInfo->stego_image_fname="default.bmp";
    }

    return e_success;

}






///////////////////////////////////////////////////////////////////////////////
Status check_capacity(EncodeInfo *encInfo){
    /*    // Check if image has enough capacity:
    // Required capacity = 16 + 32 + 32 + 32 + (size of secret file * 8)
    // 16  -> for magic string (signature marker)
    // 32  -> for secret file extension size(4)
    // 32  -> for secret file size
    // 32  -> for secret file data extension (.txt, .c, etc.)
    // size_secret_file * 8 -> every byte of secret file needs 8 bits*/
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);

    if(encInfo->image_capacity>(16+32+32+32+ encInfo->size_secret_file*8)){
        return e_success;
    }
    else{
        return e_failure;
    }
}



//////////////////////////////////////////////////////////////////////

uint get_file_size(FILE *fptr){
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}

/////////////////////////////////////////////////////////////////////
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image){

    char str[54];                           // buffer to hold header
    fseek(fptr_src_image,0,SEEK_SET);       // file pointer to point at beggining of file
    fread(str,54,1,fptr_src_image);         // read 54 bytes of header
    fwrite(str,54,1,fptr_dest_image);       // write them onto stegno image
    return e_success;
}




//////////////////////////////////////////////////////////////////////////
/*to encode magic string #* to the image file*/
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo){
    if(encode_data_to_image(magic_string,strlen(magic_string),encInfo )==e_success){
        return e_success;
    }
    else{
        return e_failure;
    }
}


///////////////////////////////////////////////////////////////////////////////
Status encode_byte_to_lsb(char data, char *image_buffer){
    for(int i=0;i<8;i++){
         int bit =(data>>(7-i)) &1;
        image_buffer[i]=(image_buffer[i] &0XFE) | bit;
    }
    return e_success;


}
/////////////////////////////////////////////////////////////////////////////

/*here # is encoded into image and then * into the image 1byte by byte*/
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo){

    for(int i=0;i<size;i++){

    fread(encInfo->image_data,8,1,encInfo->fptr_src_image); 
    encode_byte_to_lsb(data[i],encInfo->image_data);
    fwrite(encInfo->image_data,8,1,encInfo->fptr_stego_image);
    }
    return e_success;

    

    
}

///////////////////////////////////////////////////////////////////////////////////
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo){
    encode_size_to_lsb(size,encInfo);
    return e_success;
}
////////////////////////////////////////////////////////////////



Status encode_size_to_lsb(unsigned int size,EncodeInfo *encInfo){
    char buffer[32];
    char byte;
    
    for(int i=0; i<32; i++){
        // Read 1 byte from source image
        if(fread(&byte, 1, 1, encInfo->fptr_src_image) != 1){
            fprintf(stderr,"ERROR: Unexpected EOF while reading image for size encoding\n");
            return e_failure;
        }

        // Encode size bit into LSB (MSB-first)
        byte = (byte & 0xFE) | ((size >> (31 - i)) & 1);
        buffer[i] = byte;
    }

    // Write 32 bytes to stego image
    fwrite(buffer, 1, 32, encInfo->fptr_stego_image);

    return e_success;

}
////////////////////////////////////////////////////////////////////////////////////////////
Status encode_secret_file_extn(char *file_extn,EncodeInfo *encInfo){
    encode_data_to_image(file_extn,strlen(file_extn),encInfo);
    return e_success;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
Status encode_secret_file_size(long file_size,EncodeInfo *encInfo){
    encode_size_to_lsb(file_size,encInfo);
    return e_success;
}

/////////////////////////////////////////////////////////////////////////////////////
Status  encode_secret_file_data(EncodeInfo *encInfo){
    char data[encInfo->size_secret_file+1];
    rewind(encInfo->fptr_secret);
    fread(data,1,encInfo->size_secret_file,encInfo->fptr_secret);
    encode_data_to_image(data,encInfo->size_secret_file,encInfo);
    return e_success;
}



///////////////////////////////////////////////////////////////////////////////////////
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest){

    char buffer;
 // v can also use feof
    while(fread(&buffer,1,1,fptr_src)){ // when *fp reaches EOF while fails
        fwrite(&buffer,1,1,fptr_dest);
    }
    return e_success;
    
}

//////////////////////////////////////////////////////////////////////////////////////////


