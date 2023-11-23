#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

enum : u_int8_t {
  kTabA = 0,
  kTabB,
};

enum : u_int8_t {
  kRowStateEx = 0,
  kRowStateInFront,
  kRowStateInBack,
};

enum : u_int8_t
{
  kAppTypeBlank = 0,
  kAppTypeDualVca,
  kAppTypeBiquadFilter,
  kNumAppType
};

enum : u_int8_t
{
  kParamTypeFloat,
  kParamTypeString,
  kParamTypeToggle,
  NumParamType
};

enum : u_int8_t
{
  kParamAssignTypeNone = 0,
  kParamAssignTypeCV1,
  kParamAssignTypeCV2,
  kParamAssignTypeCV3,
  kParamAssignTypeCV4,
  kNumParamAssignType
};

const std::string ParamAssignTypeName[] = {
  "   ", "cv1", "cv2", "cv3", "cv4"
};

enum : u_int8_t
{
  kFilterModeLowPass,
  kFilterModeBandPass,
  kFilterModeHighPass,
  kNumFilterMode
};

const std::string FilterModeName[] = {
  "lpf", "bpf", "hpf"
};

#endif