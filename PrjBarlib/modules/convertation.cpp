#ifndef SKIP_M_INC
#include "convertion.h"
#endif

uint16_t increnenter::x = 0x0001;
uchar increnenter::sysByteOredr = *((uint8_t*)&x) ? 77 : 73;
uchar increnenter::GLOBALloclByteOreder = *((uint8_t*)&x) ? 77 : 73;
