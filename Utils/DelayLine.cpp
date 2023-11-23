#include "DelayLine.h"

using namespace hlc;

DelayLine::DelayLine()
{
}

DelayLine::~DelayLine()
{
}

void DelayLine::Init()
{
  for (size_t i = 0; i < DELAY_BUFFER_SIZE; i++)
  {
    delayBuffer[0][i] = 0;
    delayBuffer[1][i] = 0;
    delayBuffer[2][i] = 0;
    delayBuffer[3][i] = 0;
  }
}

void DelayLine::Write(u_int8_t ch, size_t index, float in)
{
  size_t writeIndex = (index + DELAY_BUFFER_SIZE) % DELAY_BUFFER_SIZE;
  delayBuffer[ch][writeIndex] = in;
}

float DelayLine::Read(u_int8_t ch, size_t index)
{
  size_t readIndex = (index + DELAY_BUFFER_SIZE) % DELAY_BUFFER_SIZE;
  return delayBuffer[ch][readIndex];
}


float DelayLine::ReadFrac(u_int8_t ch, float index)
{
  float readIndex = fmodf(index + DELAY_BUFFER_SIZE, DELAY_BUFFER_SIZE);
  size_t readIndexFloor = floorf(readIndex);
  float frac = readIndex - (float)readIndexFloor;

  return Read(ch, readIndexFloor) * (1.0f - frac) + Read(ch, readIndexFloor + 1) * frac;
}