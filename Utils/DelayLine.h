#pragma once
#ifndef HLC_DELAY_LINE_H
#define HLC_DELAY_LINE_H

#include "daisy_patch.h"

#define DELAY_BUFFER_SIZE 240000
extern float DSY_SDRAM_BSS delayBuffer[4][DELAY_BUFFER_SIZE];

namespace hlc{
class DelayLine
{
public:
  DelayLine();
  ~DelayLine();
  void Init();
  void Write(u_int8_t ch, size_t index, float in);
  float Read(u_int8_t ch, size_t index);
  float ReadFrac(u_int8_t ch, float index);
};
} // namespace hlc

#endif

