#pragma once
#ifndef APP_UNIT_H
#define APP_UNIT_H

#include "daisy_patch.h"
#include "daisysp.h"

#include "../Utils/Constants.h"

using namespace daisy;
using namespace daisysp;

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

const std::string AssignTypeName[] = {
  "   ", "cv1", "cv2", "cv3", "cv4"
};

struct Param
{
  std::string name;
  u_int8_t paramType;
  u_int8_t assignType;
  float min;
  float max;
  float res;
  float fixedValue;
  float autoValue;
};

class AppUnit
{
public:
  AppUnit(DaisyPatch& patch, u_int8_t slot, std::string title);
  virtual ~AppUnit() {  }

  virtual void Init() { };
  virtual void Exit() {};

  #pragma region about parameters
  virtual u_int8_t GetParamsNum() { return 0; };
  virtual Param GetParam(u_int8_t index)
  {
    Param blankParam = {"",  kParamTypeFloat, kParamAssignTypeNone, 0.0f, 0.0f, 0.0f, 0.0f};
    return blankParam;
  };
  virtual void SetParam(u_int8_t index, Param param) {};
  virtual float GetParamValue(u_int8_t index);
  virtual void SetParamValue(u_int8_t index, float value);
  virtual std::string GetParamValueStr(u_int8_t index) { return ""; };
  #pragma endregion

  #pragma region daisy main loops
  virtual void UpdateAutoControls(float ctrlVal[4]);
  virtual void UpdateFixedControls(u_int8_t& row, u_int8_t& column, int32_t inc);
  virtual void AudioTickCallback(const float * const*in, float **out, size_t size);
  virtual void UpdateOled(u_int8_t row, u_int8_t column);
  virtual void MainLoopCallback() {};
  #pragma endregion

  virtual const std::string& GetName() const
  {
    return m_title;
  }

protected:
  DaisyPatch& m_patch;
  u_int8_t m_slot;
  std::string m_title;
  u_int8_t displayTopParamId = 0;
};

#endif