#pragma once
#ifndef HLC_SAMPLE_HOLDER_H
#define HLC_SAMPLE_HOLDER_H

#include "daisy_patch.h"
#include "ff.h"

#define WAV_FILEPATH_MAX 256
#define SAMPLE_BUFFER_SIZE 960000

using namespace daisy;

extern int16_t DSY_SDRAM_BSS sampleBuffer[2][SAMPLE_BUFFER_SIZE];

namespace hlc{
struct SampleInfo
{
    WAV_FormatTypeDef raw_data;               /**< Raw wav data */
    char              path[WAV_FILEPATH_MAX]; /**< Wav filename */
    char              name[WAV_FILEPATH_MAX] = "None";
    size_t            sample_size = 0;
};

class SampleHolder
{
  public:
    SampleHolder() {}
    ~SampleHolder() {}

    void Init(const char* search_path);

    void PrepareBuffer(bool aorb, size_t file_index);

    inline size_t GetNumberFiles() const { return file_cnt_; }

    inline size_t GetCurrentFile(bool aorb) const { return file_sel[aorb]; }

    char* GetFileName(size_t file_index) { return file_info_[file_index].name; };

    uint32_t GetLength(size_t file_index) { return file_info_[file_index].sample_size; };

    int16_t GetSample(bool aorb, size_t sample_index);


  private:
    enum BufferState
    {
        BUFFER_STATE_IDLE,
        BUFFER_STATE_PREPARE
    };

    int Open(size_t file_index);

    int Close();

    static constexpr size_t kMaxFiles   = 8;
    static constexpr size_t kBufferSize = 480000;
    SampleInfo              file_info_[kMaxFiles];
    size_t                  file_cnt_;
    size_t                  file_sel[2] = {1, 1};
    BufferState             buff_state[2];
    FIL                     fil_;
};
} // namespace hlc
#endif
