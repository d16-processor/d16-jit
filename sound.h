#include <stdint.h>
void init_sound(void);
void cleanup_sound(void);
void sound_set(uint16_t);
#ifdef NO_MIDI
#define IF_MIDI(x)
#else
#define IF_MIDI(x) x
#endif
