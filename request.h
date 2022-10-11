#ifndef _REQUEST_H

#include "utils.h"

typedef struct {
    uint32_t index;
    uint32_t size;
    uint8_t **key;
} Request;

#endif
