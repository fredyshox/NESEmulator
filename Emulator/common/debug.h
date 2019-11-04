//
// debug io operations
// 

#ifndef common_debug_h
#define common_debug_h 

#include <stdio.h>

#ifdef DEBUG
#define DEBUG_MODE 1
#else
#define DEBUG_MODE 0
#endif

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define pretty_print(file, fmt, ...) \
  fprintf(file, "%s:%d:%s(): " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__);

#define debug_print(fmt, ...) \
  do { if (DEBUG_MODE) pretty_print(stderr, fmt, ##__VA_ARGS__) } while (0)

#endif /* common_debug_h */
