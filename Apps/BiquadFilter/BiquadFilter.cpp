
#include "BiquadFilter.h"

using namespace daisy;
using namespace daisysp;

BiquadFilter::BiquadFilter(DaisyPatch& patch, u_int8_t slot)
  :base(patch, slot, "Biquad Filter")
{
}

BiquadFilter::~BiquadFilter()
{
}

std::string BiquadFilter::GetParamValueStr(u_int8_t index)
{
  switch (index)
  {
  case kBiquadFilterParamIndexMode:
    return FilterModeName[(u_int8_t)GetParamValue(kBiquadFilterParamIndexMode)];
  default:
    break;
  }
}
  
void BiquadFilter::AudioTickCallback(const float *const *in, float **out, size_t size)
{
  for (u_int8_t slot = 0; slot < 2; slot++)
  {
    if (m_slot != slot)
    {
      continue;
    }

    coeffs.UpdateCoeffs(
      GetParamValue(kBiquadFilterParamIndexMode),
      mtof(GetParamValue(kBiquadFilterParamIndexFreqency)),
      GetParamValue(kBiquadFilterParamIndexResonance),
      GetParamValue(kBiquadFilterParamIndexGain)
    );
    
    for (u_int8_t ch = 0; ch < 2; ch++)
    {
      for (size_t i = 0; i < size; i++)
      {
        float in0 = in[slot * 2 + ch][i];

        float out0 = coeffs.b0 / coeffs.a0 * in0 + 
                     coeffs.b1 / coeffs.a0 * in1[ch] + coeffs.b2 / coeffs.a0 * in2[ch] - 
                     coeffs.a1 / coeffs.a0 * out1[ch] - coeffs.a2 / coeffs.a0 * out2[ch];
        
        in2[ch] = in1[ch];
        in1[ch] = in0;
      
        out2[ch] = out1[ch];
        out1[ch] = out0;
        
        out[slot * 2 + ch][i] = out0;
      }
    }
  }
}