#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include "decode.h"

#define OUTPUT_FNAME_MAX 256



//////////////////////////////////////////////////////////////////////////

Status do_decoding(DecodeInfo *decInfo)
{
    int extn_size = 0;

    // Open stego image file
    if (open_img_file(decInfo) == e_success)
    {
        printf("INFO: Image file opened successfully\n");

        // Decode magic string
        if (decode_magic_string(decInfo) == e_success)
        {
            printf("INFO: Magic string decoded successfully\n");

            // Decode secret file extension size
            if (decode_secret_file_extn_size(decInfo, &extn_size) == e_success)
            {
                printf("INFO: Secret file extension size decoded: %d\n", extn_size);

                // Decode secret file extension and open output file
                if (decode_secret_file_extn(decInfo, extn_size) == e_success)
                {
                    printf("INFO: Secret file extension decoded successfully: %s\n", decInfo->extn_secret_file);

                    // Decode secret file size
                    if (decode_secret_file_size(decInfo) == e_success)
                    {
                        printf("INFO: Secret file size decoded: %d bytes\n", decInfo->size_secret_file);

                        // Decode secret file data
                        if (decode_secret_file_data(decInfo) == e_success)
                        {
                            printf("INFO: Secret file data decoded successfully\n");

                            // Close files after decoding
                            close_decode_files(decInfo);
                            printf("INFO: Decoding completed successfully.\n");
                        }
                        else
                        {
                            printf("ERROR: Failed to decode secret file data\n");
                            close_decode_files(decInfo);
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("ERROR: Failed to decode secret file size\n");
                        close_decode_files(decInfo);
                        return e_failure;
                    }
                }
                else
                {
                    printf("ERROR: Failed to decode secret file extension\n");
                    close_decode_files(decInfo);
                    return e_failure;
                }
            }
            else
            {
                printf("ERROR: Failed to decode secret file extension size\n");
                close_decode_files(decInfo);
                return e_failure;
            }
        }
        else
        {
            printf("ERROR: Failed to decode magic string\n");
            close_decode_files(decInfo);
            return e_failure;
        }
    }
    else
    {
        printf("ERROR: Failed to open stego image file\n");
        return e_failure;
    }

    return e_success;
}

/* Validate decode args:
   argv[2] -> stego image (.bmp)
   optional argv[3] -> output base name (without extension)
*/
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo)
{
    /* stego image */
    if (argv[2] == NULL || strstr(argv[2], ".bmp") == NULL)
    {
        fprintf(stderr, "ERROR: Stego image file must be a .bmp file\n");
        return e_failure;
    }
    decInfo->stego_image_fname = argv[2];

    /* output base name: allocate writable buffer so we can strcat extension later */
    if (argc > 3 && argv[3] != NULL)
    {
        decInfo->output_fname = malloc(strlen(argv[3]) + 1 + 32); /* extra room for ext */
        if (decInfo->output_fname == NULL)
        {
            fprintf(stderr, "ERROR: memory allocation failed\n");
            return e_failure;
        }
        strcpy(decInfo->output_fname, argv[3]);
    }
    else
    {
        decInfo->output_fname = malloc(OUTPUT_FNAME_MAX);
        if (decInfo->output_fname == NULL)
        {
            fprintf(stderr, "ERROR: memory allocation failed\n");
            return e_failure;
        }
        strcpy(decInfo->output_fname, "output");
    }

    return e_success;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Opens the stego image file in read mode
Status open_img_file(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");// Open in binary read mode
    if (decInfo->fptr_stego_image == NULL)
    {
        fprintf(stderr, "ERROR: unable to open stego image file %s\n", decInfo->stego_image_fname);
        return e_failure;
    }

    return e_success;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

  // decode magic string frm image
Status decode_magic_string(DecodeInfo *decInfo)
{
    char ch;
    int magic_len = strlen(MAGIC_STRING);  // Length of magic string
    /*to skip bmpheader*/
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
    
    /* Local buffer: 8 bytes per encoded character */
     char buffer[8];
    char *magic_str = malloc(magic_len + 1);
   
    for (int i = 0; i < magic_len; ++i)
    {
        fread(buffer, 1, 8, decInfo->fptr_stego_image); 
        decode_byte_from_lsb(&ch,buffer);
        magic_str[i] = ch;
    }
    magic_str[magic_len] = '\0';

    if (strcmp(magic_str, MAGIC_STRING) != NULL)
    {
        fprintf(stderr, "ERROR: Magic string mismatch (expected \"%s\", got \"%s\")\n", MAGIC_STRING, magic_str);
        free(magic_str);
        return e_failure;
    }

    free(magic_str);
    return e_success;
}
//////////////////////////////////////////////////////////////////////

/* Decode a byte from the 8 LSBs 
  MSB first ,reconstruct the byte in same order.
*/
Status decode_byte_from_lsb(char *ch, char *buffer)
{
    unsigned char decoded = 0;
    for (int i = 0; i < 8; i++)
    {
        decoded = (decoded << 1) | ( (unsigned char)buffer[i] & 1 );
    }
    *ch = (char)decoded;
    return e_success;
}

/////////////////////////////////////////////////////////////////////////
/* Read a 32-bit integer (MSB first) from the next 32 image bytes:
*/
Status decode_int_from_lsb(int *value, char *image_buffer, FILE *fptr_image)
{
    int result = 0;
    for (int i = 0; i < 32; i++)
    {
        fread(&image_buffer[i], 1, 1, fptr_image);
        result = (result << 1) | ( (unsigned char)image_buffer[i] & 1 );
    }

    *value = result;
    return e_success;
}

/////////////////////////////////////////////////////////////////////////
// To Reads the size of the secret file extension.
Status decode_secret_file_extn_size(DecodeInfo *decInfo, int *extn_size)
{
    unsigned char buf32[32];
    if (decode_int_from_lsb(extn_size, (char *)buf32, decInfo->fptr_stego_image) != e_success){
        return e_failure;
    }
        
    return e_success;
}
///////////////////////////////////////////////////////////////////////////////////////
// Decodes the secret file extension and opens the output file with that extension
Status decode_secret_file_extn(DecodeInfo *decInfo, int extn_size)
{
    if (extn_size <= 0 || extn_size >= (int)sizeof(decInfo->extn_secret_file))
    {
        fprintf(stderr, "ERROR: invalid extension size %d\n", extn_size);
        return e_failure;
    }

    unsigned char buf8[8];
    for (int i = 0; i < extn_size; ++i)
    {
        if (fread(buf8, 1, 8, decInfo->fptr_stego_image) != 8)
        {
            fprintf(stderr, "ERROR: unexpected EOF while reading extension\n");
            return e_failure;
        }
        char ch;
        decode_byte_from_lsb(&ch, (char *)buf8);
        decInfo->extn_secret_file[i] = ch;
    }
    decInfo->extn_secret_file[extn_size] = '\0';


    /* Attach extension to output file name.
       decInfo->output_fname was allocated in read_and_validate_decode_args, so safe to strcat.
    */
    if (strlen(decInfo->output_fname) + strlen(decInfo->extn_secret_file) + 1 > OUTPUT_FNAME_MAX)
    {
        /* attempt to realloc if user provided a longer name earlier */
        char *newp = realloc(decInfo->output_fname, strlen(decInfo->output_fname) + strlen(decInfo->extn_secret_file) + 1);
        if (!newp)
        {
            fprintf(stderr, "ERROR: cannot allocate memory for output filename\n");
            return e_failure;
        }
        decInfo->output_fname = newp;
    }
    strcat(decInfo->output_fname, decInfo->extn_secret_file);

    /* Open the output file for binary writing */
    decInfo->fptr_output = fopen(decInfo->output_fname, "wb");
    if (decInfo->fptr_output == NULL)
    {
        fprintf(stderr, "ERROR: Unable to create output file %s\n", decInfo->output_fname);
        return e_failure;
    }
    return e_success;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    unsigned char buf32[32];
    if (decode_int_from_lsb(&decInfo->size_secret_file, (char *)buf32, decInfo->fptr_stego_image) != e_success)
        return e_failure;

    if (decInfo->size_secret_file < 0)
    {
        fprintf(stderr, "ERROR: invalid secret file size %d\n", decInfo->size_secret_file);
        return e_failure;
    }
    return e_success;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    if (decInfo->size_secret_file == 0)
    {
        printf("INFO: Secret file size is 0 — nothing to decode\n");
        return e_success;
    }

    unsigned char buf8[8];
    printf("INFO: Starting to decode secret file data...\n");

    for (int i = 0; i < decInfo->size_secret_file; i++)
    {
        if (fread(buf8, 1, 8, decInfo->fptr_stego_image) != 8)
        {
            fprintf(stderr, "ERROR: unexpected EOF while reading secret file data\n");
            return e_failure;
        }

        char ch;
        decode_byte_from_lsb(&ch, (char *)buf8);

        if (fputc((unsigned char)ch, decInfo->fptr_output) == EOF)
        {
            fprintf(stderr, "ERROR: write error while writing to output file\n");
            return e_failure;
        }
    }

    return e_success;
}
////////////////////////////////////////////////////////////////////////////////////////////////
void close_decode_files(DecodeInfo *decInfo)
{
    if (decInfo->fptr_stego_image)
    {
        fclose(decInfo->fptr_stego_image);
        decInfo->fptr_stego_image = NULL;
        printf("INFO: Stego image file closed\n");
    }

    if (decInfo->fptr_output)
    {
        fclose(decInfo->fptr_output);
        decInfo->fptr_output = NULL;
        printf("INFO: Output file closed\n");
    }

    if (decInfo->output_fname)
    {
        free(decInfo->output_fname);
        decInfo->output_fname = NULL;
    }
}


