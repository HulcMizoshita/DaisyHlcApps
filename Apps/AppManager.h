#pragma once
#ifndef APP_MANAGER_H
#define APP_MANAGER_H

#include "../Utils/Constants.h"
#include "AppUnit.h"
#include "DualVca/DualVca.h"
#include "BiquadFilter/BiquadFilter.h"

using namespace daisy;
using namespace daisysp;

class AppManager
{
public:
  AppManager(u_int8_t s) { slot = s; }
  ~AppManager() {}
  
  void ReloadApp(u_int8_t type, DaisyPatch& patch)
  {
    switch (type)
    {
    case kAppTypeDualVca:
      app.reset(new DualVca(patch, slot));
      break;
    case kAppTypeBiquadFilter:
      app.reset(new BiquadFilter(patch, slot));
      break;
    default:
      app.reset(new AppUnit(patch, slot, "Blank"));
      break;
    }
    app->Init();
  }

  std::unique_ptr<AppUnit> app;
private:
  u_int8_t slot;
};
#endif