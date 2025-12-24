 

#ifndef ICE_PIT_H
#define ICE_PIT_H

#include "../types.h"

 
#define PIT_CHANNEL0    0x40
#define PIT_CHANNEL1    0x41
#define PIT_CHANNEL2    0x42
#define PIT_COMMAND     0x43

 
#define PIT_FREQUENCY   1193182

 
void pit_init(u32 frequency);

 
u64 pit_get_ticks(void);

 
void pit_sleep_ms(u32 ms);

#endif  
