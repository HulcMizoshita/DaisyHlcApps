#include <cstring>
#include "SampleHolder.h"

using namespace daisy;
using namespace hlc;

void SampleHolder::Init(const char *search_path)
{
    FRESULT result = FR_OK;
    FILINFO fno;
    DIR     dir;
    char *  fn;
    file_sel[0] = 0;
    file_sel[1] = 0;
    file_cnt_ = 0;

    for (size_t i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        sampleBuffer[0][i] = 0;
        sampleBuffer[1][i] = 0;
    }
    

    // Open Dir and scan for files.
    if(f_opendir(&dir, search_path) != FR_OK)
    {
        return;
    }
    do
    {
        result = f_readdir(&dir, &fno);
        // Exit if bad read or NULL fname
        if(result != FR_OK || fno.fname[0] == 0)
            break;
        // Skip if its a directory or a hidden file.
        if(fno.fattrib & (AM_HID | AM_DIR))
            continue;
        // Now we'll check if its .wav and add to the list.
        fn = fno.fname;
        if(file_cnt_ < kMaxFiles - 1)
        {
            if(strstr(fn, ".wav") || strstr(fn, ".WAV"))
            {
                strcpy(file_info_[file_cnt_].name, fn);
                strcpy(file_info_[file_cnt_].path, search_path);
                strcat(file_info_[file_cnt_].path, fn);
                file_cnt_++;
                // For now lets break anyway to test.
                //                break;
            }
        }
        else
        {
            break;
        }
    } while(result == FR_OK);
    f_closedir(&dir);

    // Now we'll go through each file and load the WavInfo.
    for(size_t i = 0; i < file_cnt_; i++)
    {
        size_t bytesread;
        if(f_open(&fil_, file_info_[i].path, (FA_OPEN_EXISTING | FA_READ))
           == FR_OK)
        {
            // Populate the WAV Info
            if(f_read(&fil_,
                      (void *)&file_info_[i].raw_data,
                      sizeof(WAV_FormatTypeDef),
                      &bytesread)
               != FR_OK)
            {
                // Maybe add return type
                return;
            }
            
            size_t offset = sizeof(WAV_FormatTypeDef) + file_info_[i].raw_data.SubChunk1Size;
            file_info_[i].sample_size = 
                (f_size(&fil_) - offset) / sizeof(sampleBuffer[0][0]) / 2;
            
            f_close(&fil_);
        }
    }
}


int SampleHolder::Open(size_t file_index)
{
    return f_open(
        &fil_, file_info_[file_index].path, (FA_OPEN_EXISTING | FA_READ));
}

int SampleHolder::Close()
{
    return f_close(&fil_);
}

void SampleHolder::PrepareBuffer(bool aorb, size_t file_index)
{
    if (file_sel[aorb] == file_index) return;

    buff_state[aorb] = BUFFER_STATE_PREPARE;

    if(Open(file_index) != FR_OK) return;
    
    file_sel[aorb] = file_index;
    size_t bytesread, rxsize;
    bytesread = 0;
    rxsize    = kBufferSize * sizeof(sampleBuffer[0][0]);
    f_lseek(
        &fil_, 
        sizeof(WAV_FormatTypeDef) + file_info_[file_sel[aorb]].raw_data.SubChunk1Size
    );
    f_read(&fil_, &sampleBuffer[aorb][0], rxsize, &bytesread);
    Close();
    buff_state[aorb] = BUFFER_STATE_IDLE;
}

int16_t SampleHolder::GetSample(bool aorb, size_t sample_index)
{
    int16_t samp = 0;
    sample_index %= kBufferSize;
    if (buff_state[aorb] == BUFFER_STATE_IDLE)
    {
        samp = sampleBuffer[aorb][sample_index];
    }

    return samp;
}
