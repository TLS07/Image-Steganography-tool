# Image-Steganography-tool
Developed an image steganography tool that can encode a text file into a .bmp image and later decode it to retrieve the original text into a .txt file.

Project Overview

This project implements a simple image steganography tool that can hide a secret text file inside a BMP image using the Least Significant Bit (LSB) technique. The tool can also retrieve the hidden text from the image.

LSB steganography modifies the least significant bits of the image pixels to store secret data without noticeably altering the image visually.

Features

Encode a secret .txt file into a .bmp image.

Decode a stego .bmp image to retrieve the hidden file.

Supports optional output file names.

Validates file types and image capacity before encoding.

Automatically handles BMP header and preserves image data.

Functionality / Workflow
Encoding

Open files – source image, secret file, and stego output image.

Check capacity – ensures the image has enough space for secret data.

Copy BMP header – preserve image metadata.

Encode data –

Magic string (#*) for identification

Secret file extension and size

Secret file content bit-by-bit into image LSBs

Copy remaining image data – ensures image integrity.

Decoding

Open stego image.

Validate magic string – ensures image contains hidden data.

Decode secret file metadata – extension and size.

Decode secret file data – reconstruct the original file.

Close files – release resources.
