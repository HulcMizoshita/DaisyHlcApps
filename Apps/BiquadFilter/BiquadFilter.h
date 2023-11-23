#pragma once
#ifndef MULTI_MODE_FILTER_H
#define MULTI_MODE_FILTER_H

#include <string>

#include "../AppUnit.h"

using namespace daisy;
using namespace daisysp;

class BiquadFilterCoeffs
{
public:
  BiquadFilterCoeffs(){};
  ~BiquadFilterCoeffs(){};

  void UpdateCoeffs(u_int8_t mode, float freq, float q, float gain)
  {
    switch (mode)
    {
    case kFilterModeLowPass:
      omega = 2.0f * PI_F *  freq / samplerate;
      alpha = sin(omega) / (2.0f * q);

      a0 =  1.0f + alpha;
      a1 = -2.0f * cosf(omega);
      a2 =  1.0f - alpha;
      b0 = (1.0f - cosf(omega)) / 2.0f;
      b1 =  1.0f - cosf(omega);
      b2 = (1.0f - cosf(omega)) / 2.0f;
      break;
    case kFilterModeBandPass:
      omega = 2.0f * PI_F *  freq / samplerate;
      alpha = sin(omega) * sinh(log(2.0f) / 2.0 * (1.0f / q) * omega / sin(omega));
      
      a0 =  1.0f + alpha;
      a1 = -2.0f * cos(omega);
      a2 =  1.0f - alpha;
      b0 =  alpha;
      b1 =  0.0f;
      b2 = -alpha;
      break;
    case kFilterModeHighPass:
      omega = 2.0f * PI_F *  freq / samplerate;
      alpha = sinf(omega) / (2.0f * q);
      
      a0 =   1.0f + alpha;
      a1 =  -2.0f * cosf(omega);
      a2 =   1.0f - alpha;
      b0 =  (1.0f + cosf(omega)) / 2.0f;
      b1 = -(1.0f + cosf(omega));
      b2 =  (1.0f + cosf(omega)) / 2.0f;
      break;
    default:
      break;
    }
  }


  float samplerate = 0.0f;
  float a0 = 0.0f;
  float a1 = 0.0f;
  float a2 = 0.0f;
  float b0 = 0.0f;
  float b1 = 0.0f;
  float b2 = 0.0f;

private:
  float omega = 0.0f;
  float alpha = 0.0f;
};

class BiquadFilter : public AppUnit
{
public:
  BiquadFilter(daisy::DaisyPatch&, u_int8_t slot);
  ~BiquadFilter();

  void Init() override { coeffs.samplerate = m_patch.AudioSampleRate(); };

  u_int8_t GetParamsNum() override { return kNumBiquadFilterParams; };

  Param GetParam(u_int8_t index) override
  {
    return params[index];
  };

  void SetParam(u_int8_t index, Param def) override
  {
    params[index] = def;
  };

  std::string GetParamValueStr(u_int8_t index) override;
  
  void AudioTickCallback(const float * const*in, float **out, size_t size) override;

private:
  using base = AppUnit;

  enum : u_int8_t {
    kBiquadFilterParamIndexMode = 0,
    kBiquadFilterParamIndexFreqency,
    kBiquadFilterParamIndexResonance,
    kBiquadFilterParamIndexGain,
    kNumBiquadFilterParams
  } ParamIndex;

  Param params[kNumBiquadFilterParams] = {
    {"mode",  kParamTypeString, kParamAssignTypeNone, kFilterModeLowPass, kNumFilterMode - 1, 1, 0.0f, 0.0f},
    {"freq",  kParamTypeFloat, kParamAssignTypeCV1,   0.0f, 127.0f, 1.0f, 0.0f, 0.0f},
    {"res",   kParamTypeFloat, kParamAssignTypeCV2,   0.01f, 6.0f, 0.01f, 0.01f, 0.01f},
    {"gain",  kParamTypeFloat, kParamAssignTypeNone,  0.0f, 1.0f, 0.01f, 0.0f, 0.0f},
  };

  float in1[4] = { 0, 0 };
  float in2[4] = { 0, 0 };
  float out1[4] = { 0, 0 };
  float out2[4] = { 0, 0 };

  BiquadFilterCoeffs coeffs;
};

#endif //MULTI_MODE_FILTER_H
