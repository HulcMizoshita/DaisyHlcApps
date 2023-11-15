#pragma once
#ifndef DUAL_VCA_H
#define DUAL_VCA_H

#include <string>

#include "../AppUnit.h"

using namespace daisy;
using namespace daisysp;

class DualVca : public AppUnit
{
public:
  DualVca(daisy::DaisyPatch&, u_int8_t slot);
  ~DualVca();

  u_int8_t GetParamsNum() override { return kNumDualVcaParams; };

  Param GetParam(u_int8_t index) override
  {
    return params[index];
  };

  void SetParam(u_int8_t index, Param def) override
  {
    params[index] = def;
  };
  
  void AudioTickCallback(const float * const*in, float **out, size_t size) override;

private:
  using base = AppUnit;

  enum : u_int8_t {
    kDualVcaParamIndexGain1 = 0,
    kDualVcaParamIndexGain2,
    kNumDualVcaParams
  } ParamIndex;

  Param params[kNumDualVcaParams] = {
    {"gain1",  kParamTypeFloat, kParamAssignTypeCV1, 0.0f, 1.0f, 0.01f, 0.0f, 0.0f},
    {"gain2",  kParamTypeFloat, kParamAssignTypeCV2, 0.0f, 1.0f, 0.01f, 0.0f, 0.0f}
  };
};

#endif //DUAL_VCA_H
