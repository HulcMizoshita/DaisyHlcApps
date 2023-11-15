#include "AppUnit.h"

using namespace daisy;
using namespace daisysp;

#define CLAMP(value, min, max) ((value < min) ? min : ((value > max) ? max : value))

AppUnit::AppUnit(daisy::DaisyPatch& patch, u_int8_t slot, std::string title) 
: m_title(title),
  m_slot(slot),
  m_patch(patch)
{
}

float AppUnit::GetParamValue(u_int8_t index) { 
  Param param = GetParam(index);
  if (param.assignType == kParamAssignTypeNone)
  {
    return param.fixedValue;
  }
  else
  {
    return param.autoValue;
  }
}

void AppUnit::SetParamValue(u_int8_t index, float value)
{
  Param param = GetParam(index);
  if (param.assignType == kParamAssignTypeNone)
  {
    param.fixedValue = value;
  }
  else
  {
    param.autoValue = value;
  }
  SetParam(index, param);
}

void AppUnit::UpdateAutoControls(float ctrlVal[4])
{
  const u_int8_t numParam = GetParamsNum();

  for (u_int8_t i = 0; i < numParam; i++)
  {
    Param param = GetParam(i);
    switch (param.assignType)
    {
    case kParamAssignTypeCV1:
      param.autoValue = ctrlVal[0] * (param.max - param.min) + param.min;
      break;
    case kParamAssignTypeCV2:
      param.autoValue = ctrlVal[1] * (param.max - param.min) + param.min;
      break;
    case kParamAssignTypeCV3:
      param.autoValue = ctrlVal[2] * (param.max - param.min) + param.min;
      break;
    case kParamAssignTypeCV4:
      param.autoValue = ctrlVal[3] * (param.max - param.min) + param.min;
      break;
    default:
      break;
    }
    SetParam(i, param);
  }
}

void AppUnit::UpdateFixedControls(u_int8_t& row, u_int8_t& column, int32_t inc)
{
  const u_int8_t numParam = GetParamsNum();

  column = column > numParam ? numParam : column;
  u_int8_t selectParamId = column - 1;
  displayTopParamId = CLAMP((int8_t)displayTopParamId, (int8_t)selectParamId - 3, selectParamId);

  if (column == 0)
  {
    return;
  }
  

  switch (row)
  {
  case kRowStateInFront:
  {
    Param param = GetParam(selectParamId);
    if (param.assignType != kParamAssignTypeNone) break;

    param.fixedValue = CLAMP(
      param.fixedValue + param.res * inc,
      param.min,
      param.max
    );
    SetParam(selectParamId, param);
  }
    break;
  case kRowStateInBack:
  {
    Param param = GetParam(selectParamId);
    param.assignType = (u_int8_t)CLAMP(
      (int8_t)param.assignType + inc,
      (int8_t)kParamAssignTypeNone,
      (int8_t)kParamAssignTypeCV4
    );
    SetParam(selectParamId, param);
  }
    break;
  default:
    break;
  }
}

void AppUnit::AudioTickCallback(const float * const*in, float **out, size_t size)
{
  for (size_t i = 0; i < size; i++)
  {
      out[0][i] = 0;
      out[1][i] = 0;
      out[2][i] = 0;
      out[3][i] = 0;
  }
}

void AppUnit::UpdateOled(u_int8_t row, u_int8_t column)
{
  static char buf[64];

  m_patch.display.SetCursor(0, 10);
  sprintf(buf, "  name: as:  val  ");
  m_patch.display.WriteString(buf, Font_7x10, false);

  const u_int8_t numParam = GetParamsNum();
  u_int8_t selectParamId = column - 1;
  for (u_int8_t i = 0; i < 4; i++)
  {
    u_int8_t paramId = displayTopParamId + i;

    if (paramId >= numParam)
    {
      break;
    }
    
    Param param = GetParam(paramId);

    const char* select = (paramId == selectParamId)? ">" : " ";
    size_t y = 20 + 10 * i;

    m_patch.display.SetCursor(0, y);
    sprintf(buf, "%s", select);
    m_patch.display.WriteString(buf, Font_7x10, true);

    m_patch.display.SetCursor(7 * 1, y);
    sprintf(buf, "%s", param.name.c_str());
    m_patch.display.WriteString( buf, Font_7x10, true);

    m_patch.display.SetCursor(7 * 6, y);
    sprintf(buf, ":%s", AssignTypeName[param.assignType].c_str());
    m_patch.display.WriteString(buf, Font_7x10, !(paramId == selectParamId && row == kRowStateInBack));

    m_patch.display.SetCursor(7 * 10, y);
    switch (param.paramType)
    {
    case kParamTypeFloat:
    {
      float val = GetParamValue(paramId);
      sprintf(buf, ":%s%2d.%03d", val > 0 ? "+" : "-", (int)abs(val), (int)(abs(fmod(val, 1.0f)) * 1000.0f));
    }
      break;
    case kParamTypeString:
      sprintf(buf, ": %s", GetParamValueStr(paramId).c_str());
      break;
    case kParamTypeToggle:
      sprintf(buf, ": %s", GetParamValue(paramId) > 0.5f ? "ON" : "OFF");
      break;
    }
    m_patch.display.WriteString(buf, Font_7x10, !(paramId == selectParamId && row == kRowStateInFront));
  }
}