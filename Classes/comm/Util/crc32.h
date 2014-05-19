#ifndef PokeBalloon_crc32_h
#define PokeBalloon_crc32_h

#include "stdio.h"
#include "stdlib.h"

static int Crc32_ComputeFile( FILE *file, unsigned long *outCrc32 );
static unsigned long Crc32_ComputeBuf( unsigned long inCrc32, const void *buf, size_t bufLen );

#endif
