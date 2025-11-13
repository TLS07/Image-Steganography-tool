#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>
#include "types.h"
#include "common.h"   // for MAGIC_STRING

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;         // stego image file (e.g., default.bmp)
    FILE *fptr_stego_image;          // file pointer for stego image
    char image_data[MAX_IMAGE_BUF_SIZE]; // buffer for 8 bytes (1 char)

    /* Secret File Info */
    char *output_fname;                      // base name of output file
    char extn_secret_file[MAX_FILE_SUFFIX+1]; // extension (".txt")
    FILE *fptr_output;                       // file pointer for output file
    int size_secret_file;                    // size of secret file

} DecodeInfo;

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Step 1: Read and validate Decode args */
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

/* Step 2: Open stego image file */
Status open_img_file(DecodeInfo *decInfo);

/* Step 3: Skip BMP header */
Status skip_bmp_header(FILE *fptr_stego_image);

/* Step 4: Decode and check magic string */
Status decode_magic_string(DecodeInfo *decInfo);

/* Step 5: Decode secret file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo,int *extn_size);

/* Helper: Decode int from LSB */
Status decode_int_from_lsb(int *value, char *image_buffer,FILE *fptr_image);

/* Step 6: Decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo,int ext_size);

/* Step 7: Create output file with extension */
Status open_output_file(DecodeInfo *decInfo);

/* Step 8: Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Step 9: Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Helper: Decode one byte from LSB */
Status decode_byte_from_lsb(char *ch, char *buffer);

/* Close files */
void close_decode_files(DecodeInfo *decInfo);

#endif
