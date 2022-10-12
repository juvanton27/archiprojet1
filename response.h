#ifndef _RESPONSE_H

#include "utils.h"

typedef struct {
    uint8_t errorCode;
    uint32_t size;
    uint8_t **file;
} Response;

#endif
