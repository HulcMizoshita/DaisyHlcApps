
#include "DualVca.h"

using namespace daisy;
using namespace daisysp;

DualVca::DualVca(DaisyPatch& patch, u_int8_t slot)
  :base(patch, slot, "Dual VCA")
{
}

DualVca::~DualVca()
{
}
  
void DualVca::AudioTickCallback(const float *const *in, float **out, size_t size)
{
  for (u_int8_t slot = 0; slot < 2; slot++)
  {
    if (m_slot != slot)
    {
      continue;
    }
    
    for (size_t i = 0; i < size; i++)
    {
      out[slot * 2][i] = in[slot * 2][i] * GetParamValue(kDualVcaParamIndexGain1);
      out[slot * 2 + 1][i] = in[slot * 2 + 1][i] * GetParamValue(kDualVcaParamIndexGain2);
    }
  }
}