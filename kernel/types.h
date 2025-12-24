 

#ifndef ICE_TYPES_H
#define ICE_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

 
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

 
typedef u32 phys_addr_t;
typedef u32 virt_addr_t;

 
typedef u32 ice_pid_t;
#define PID_INVALID 0

 
typedef u32 exec_id_t;
#define EXEC_ID_INVALID 0
#define EXEC_ID_FORMAT "#%08X"

#endif  
