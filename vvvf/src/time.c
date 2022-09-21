#include "time.h"
#include "vvvf.h"

float const time_step[6U] = {
    VVVF_WAVE_DUR,          VVVF_WAVE_DUR + 45.6F,
    VVVF_WAVE_DUR + 60.6F,  VVVF_WAVE_DUR + 75.8F,
    VVVF_WAVE_DUR + 106.0F, VVVF_WAVE_DUR + 106.0F + VVVF_WAVE_DUR,
};
