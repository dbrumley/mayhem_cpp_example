// AFL driver posix-style:
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "LLVMFuzzerTestOneInput.h"


constexpr size_t MAX_INPUT_SIZE = 1024 * 1024; // 1MB max input

int main(int argc, char** argv) {
  int ret = 0;
  uint8_t* buffer = static_cast<uint8_t*>(malloc(MAX_INPUT_SIZE));
  if (!buffer) {
    perror("malloc failed");
    return 1;
  }

  int fd = STDIN_FILENO;
  if (argc > 1) {
    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
      perror("open failed");
      free(buffer);
      return 1;
    }
  }

  ssize_t bytes_read = read(fd, buffer, MAX_INPUT_SIZE);
  if (bytes_read < 0) {
    perror("read failed");
    if (fd != STDIN_FILENO) close(fd);
    free(buffer);
    return 1;
  }

  // Optionally close the file descriptor if we opened it
  if (fd != STDIN_FILENO) close(fd);

  // Call the fuzz logic with the input
  ret = LLVMFuzzerTestOneInput(buffer, static_cast<size_t>(bytes_read));

  free(buffer);
  return ret;
}

/*
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

// Optional: any global initialization
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv) {
    // Setup code here, e.g., open files, initialize data, etc.
    printf("[+] LLVMFuzzerInitialize called\n");
    return 0;
}

// Required: fuzz entry point
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    // Your fuzzing target code
    if (size > 0 && data[0] == 'F') {
        if (size > 1 && data[1] == 'U') {
            if (size > 2 && data[2] == 'Z') {
                __builtin_trap(); // Crash
            }
        }
    }
    return 0;
}
    */