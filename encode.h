#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;           // stores beautiful.bmp
    FILE *fptr_src_image;            // file pointer to perform operation in beautiful.bmp
    uint image_capacity;             // to store capcity of image
    uint bits_per_pixel;             // 1 pixel 3 bytes ->24 bits
    char image_data[MAX_IMAGE_BUF_SIZE]; // buffer of 8 bytes of beatuful image

    /* Secret File Info */
    char *secret_fname;           // stores secret.txt
    FILE *fptr_secret;             // file pointer to perform opertaion in secret.txt
    char extn_secret_file[MAX_FILE_SUFFIX]; // 4bytes to store .txt,.mp3 etc
    char secret_data[MAX_SECRET_BUF_SIZE];  // buffer of 8 bytes of secret file
    long size_secret_file;                 // size of secreat file eg hello ->4

    /* Stego Image Info */
    char *stego_image_fname;       // stores output filename default.bmp
    FILE *fptr_stego_image;        // file pointer to perform opertion on outptu file

} EncodeInfo;


/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(int argc,char *argv[], EncodeInfo *encInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo);

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo);

Status encode_secret_file_extn(char *file_extn,EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer);
/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

Status encode_size_to_lsb(unsigned int size,EncodeInfo *encInfo);
Status encode_byte_to_lsb(char data,char *image_buffer);

#endif
