#ifndef _LLVMFuzzerTestOneInput_h
#define _LLVMFuzzerTestOneInput_h
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

  int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size);

#ifdef __cplusplus
}
#endif


#endif
