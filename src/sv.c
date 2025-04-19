#define SV_IMPLEMENTATION
#include <sv.h>

/// Consumes one char
char consume(SV* input) {
    input->count--;
    input->data++;
    return *(input->data - 1);
}
