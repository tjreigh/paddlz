#include <fileioc.h>

#include "save.h"

unsigned int loadHighScore(void)
{
    uint8_t data[SAVE_DATA_SIZE];
    uint8_t handle = ti_Open(SAVE_APPVAR_NAME, "r");
    if (handle == 0) {
        return 0;
    }

    size_t read_count = ti_Read(data, sizeof(data), 1, handle);
    ti_Close(handle);

    unsigned int high_score = 0;
    if (read_count != 1
        || !decodeSaveData(data, sizeof(data), &high_score)) {
        return 0;
    }

    return high_score;
}

bool saveHighScore(unsigned int high_score)
{
    uint8_t data[SAVE_DATA_SIZE];
    encodeSaveData(high_score, data);

    uint8_t handle = ti_Open(SAVE_APPVAR_NAME, "w");
    if (handle == 0) {
        return false;
    }

    bool success = ti_Write(data, sizeof(data), 1, handle) == 1;
    if (success) {
        ti_SetArchiveStatus(true, handle);
    }
    ti_Close(handle);
    return success;
}
