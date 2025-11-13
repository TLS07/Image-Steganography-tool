#ifndef TYPES_H
#define TYPES_H

/* User defined types */
typedef unsigned int uint;

/* Status will be used in fn. return type */
typedef enum
{
    e_success,  // Function executed successfully
    e_failure    // Function failed
} Status;

typedef enum
{
    e_encode,      // Encoding operation selected
    e_decode,      // Decoding operation selected
    e_unsupported  // Invalid or unsupported operation
} OperationType;

#endif
