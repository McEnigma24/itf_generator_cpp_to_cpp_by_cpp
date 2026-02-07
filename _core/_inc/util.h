#pragma once

#include <cstdint>

#define __FILENAME__ ((__FILE__) + SOURCE_PATH_SIZE)
// #define __FILENAME__ (__FILE__)

#define var(x)   std::cout << __FILENAME__ << ":" << __LINE__ << " - " << #x << " = " << x << "\n";
#define varr(x)  std::cout << __FILENAME__ << ":" << __LINE__ << " - " << #x << " = " << x << " ";
#define line(x)  std::cout << __FILENAME__ << ":" << __LINE__ << " - " << x << "\n";
#define linee(x) std::cout << __FILENAME__ << ":" << __LINE__ << " - " << x << " ";
#define nline    std::cout << "\n";

// #define var(x)   cout << #x << " = " << x << '\n';
// #define varr(x)  cout << #x << " = " << x << ' ';
// #define line(x)  cout << x << '\n';
// #define linee(x) cout << x << ' ';
// #define nline    cout << '\n';

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;