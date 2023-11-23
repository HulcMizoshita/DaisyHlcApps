#include "daisy_patch.h"
#include "daisysp.h"

#include <string>
#include <memory>
#include "fatfs.h"

#include "Utils/printf.h"
#include "Utils/printf.c"

#include "Utils/Constants.h"
#include "Utils/SampleHolder.h"
#include "Utils/DelayLine.h"

#include "Apps/AppManager.h"

#define CLAMP(value, min, max) ((value < min) ? min : ((value > max) ? max : value))

using namespace daisy;
using namespace daisysp;

#pragma region global objects
char buf[64];
float DSY_SDRAM_BSS delayBuffer[4][DELAY_BUFFER_SIZE];
int16_t DSY_SDRAM_BSS sampleBuffer[2][SAMPLE_BUFFER_SIZE];

static DaisyPatch patch;

hlc::DelayLine delayLine;

static SdmmcHandler sd_handler;
FatFSInterface fsi;
FIL SDFile;
hlc::SampleHolder sampleHolder;

// std::unique_ptr<AppUnit> appA;
// std::unique_ptr<AppUnit> appB;
AppManager appA(kTabA);
AppManager appB(kTabB);
#pragma endregion

#pragma region controller variables

float ctrlVal[4]={ 1, 1, 1, 1 };
size_t pressCount = 0;
bool isPressing = false;

u_int8_t selectTab = kTabA;
u_int8_t selectRow = kRowStateEx;
u_int8_t selectColumn = 0;
u_int8_t selectAppA = kAppTypeDualVca;
u_int8_t selectAppB = kAppTypeBlank;

#pragma endregion

void updateControls()
{
  patch.ProcessAllControls();

  for (int i = 0; i < 4; i++)
  {
    ctrlVal[i] = patch.controls[i].Process();
    ctrlVal[i] = CLAMP(ctrlVal[i], 0.01, 0.98);
    ctrlVal[i] = (ctrlVal[i] - 0.01) / 0.97;
  }

  if (patch.encoder.RisingEdge())
  {
    isPressing = true;
    pressCount = 0;
  }

  if (patch.encoder.FallingEdge())
  {
    if (selectRow != 0)
    {
      selectRow = kRowStateEx;
    }
    else
    {
      if (pressCount < patch.AudioSampleRate() / patch.AudioBlockSize() * 0.5f)
      {
        selectRow = kRowStateInFront;
      }
      else
      {
        selectRow = kRowStateInBack;
      }
    }
    isPressing = false;
  }
  int32_t inc = patch.encoder.Increment();
  if (isPressing)
  {
    pressCount++;
  }
  else
  {
    switch (selectRow)
    {
    case kRowStateEx:
      selectColumn = (int8_t)selectColumn + inc < 0 ? 0 : selectColumn + inc;
      break;
    case kRowStateInFront:
      if (selectColumn == 0 && inc != 0)
      {
        selectTab = (int8_t)selectTab + inc < 0 ? 0 : selectTab + inc;
        selectTab = CLAMP(selectTab, kTabA, kTabB);
      }
      break;
    case kRowStateInBack:
      if (selectColumn == 0 && inc != 0)
      {
        switch (selectTab)
        {
        case kTabA:
          selectAppA = (int8_t)selectAppA + inc < 0 ? 0 : selectAppA + inc;
          selectAppA = CLAMP(selectAppA ,kAppTypeBlank, kNumAppType - 1);
          appA.ReloadApp(selectAppA, patch);
          break;
        case kTabB:
          selectAppB = (int8_t)selectAppB + inc < 0 ? 0 : selectAppB + inc;
          selectAppB = CLAMP(selectAppB ,kAppTypeBlank, kNumAppType - 1);
          appB.ReloadApp(selectAppB, patch);
          break;
        default:
          break;
        }
      }
      break;
    default:
      break;
    } 
  }

  appA.app->UpdateAutoControls(ctrlVal);
  appB.app->UpdateAutoControls(ctrlVal);

  switch (selectTab)
  {
  case kTabA:
    appA.app->UpdateFixedControls(selectRow, selectColumn, inc);
    break;
  case kTabB:
    appB.app->UpdateFixedControls(selectRow, selectColumn, inc);
  default:
    break;
  }

}

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
  updateControls();

  appA.app->AudioTickCallback(in, out, size);
  appB.app->AudioTickCallback(in, out, size);
}

void UpdateOled()
{
  patch.display.Fill(false);
  const char* select = (selectColumn == 0) ? ">" : " ";
  sprintf(buf,"%s", select);
  patch.display.SetCursor(0,0);
  patch.display.WriteString(buf, Font_7x10, true);

  patch.display.SetCursor(7 * 1, 0);
  switch (selectTab)
  {
  case kTabA:
    sprintf(buf, "%s", appA.app->GetName().c_str());
    break;
  case kTabB:
    sprintf(buf, "%s", appB.app->GetName().c_str());
    break;
  default:
    break;
  }
  patch.display.WriteString(
    buf,
    Font_7x10,
    !(selectColumn == 0 && selectRow == kRowStateInBack)
  );

  patch.display.SetCursor(7 * 17, 0);
  sprintf(buf, "%s", selectTab == kTabA ? "A" : "B");
  patch.display.WriteString(
    buf,
    Font_7x10,
    !(selectColumn == 0 && selectRow == kRowStateInFront)
  );

  switch (selectTab)
  {
  case kTabA:
    appA.app->UpdateOled(selectRow, selectColumn);
    break;
  case kTabB:
    appB.app->UpdateOled(selectRow, selectColumn);
    break;
  default:
    break;
  }

  if (selectColumn == 0 && selectRow == kRowStateInFront)
  {
    patch.display.DrawRect(0, 0, patch.display.Width()-1, patch.display.Height()-1, true);
  }

  patch.display.Update();
}

int main(void)
{
	patch.Init();

  std::string str = "DaisyHlcApps";
  char* cstr = &str[0];
  patch.display.WriteString(cstr, Font_7x10, true);
  patch.display.Update();
  patch.DelayMs(1000);

	patch.StartAdc();

  delayLine.Init();

	SdmmcHandler::Config sd_cfg;
	sd_cfg.Defaults();
	sd_cfg.speed = SdmmcHandler::Speed::STANDARD;
  sd_handler.Init(sd_cfg);

  fsi.Init(FatFSInterface::Config::MEDIA_SD);
	f_mount(&fsi.GetSDFileSystem(), "/", 1);

	sampleHolder.Init(fsi.GetSDPath());

  patch.SetAudioBlockSize(4);
	patch.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	patch.StartAudio(AudioCallback);

  appA.ReloadApp(kAppTypeDualVca, patch);
  appB.ReloadApp(kAppTypeDualVca, patch);

  while(1)
	{
	  patch.DelayMs(1);

    appA.app->MainLoopCallback();
    appB.app->MainLoopCallback();

    static int skip = 0;
    if (skip++>20)
    {
      skip=0;
      UpdateOled();
    }
  }
}
