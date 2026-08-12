#include "save.h"

static const uint8_t SAVE_MAGIC[4] = {'P', 'D', 'L', 'Z'};

void encodeSaveData(unsigned int high_score, uint8_t data[SAVE_DATA_SIZE])
{
    data[0] = SAVE_MAGIC[0];
    data[1] = SAVE_MAGIC[1];
    data[2] = SAVE_MAGIC[2];
    data[3] = SAVE_MAGIC[3];
    data[4] = SAVE_FORMAT_VERSION;
    data[5] = high_score & 0xFF;
    data[6] = (high_score >> 8) & 0xFF;
    data[7] = (high_score >> 16) & 0xFF;
}

bool decodeSaveData(const uint8_t* data, size_t size, unsigned int* high_score)
{
    if (size != SAVE_DATA_SIZE || high_score == NULL) {
        return false;
    }

    for (size_t index = 0; index < sizeof(SAVE_MAGIC); index++) {
        if (data[index] != SAVE_MAGIC[index]) {
            return false;
        }
    }

    if (data[4] != SAVE_FORMAT_VERSION) {
        return false;
    }

    *high_score = (unsigned int)data[5]
        | ((unsigned int)data[6] << 8)
        | ((unsigned int)data[7] << 16);
    return true;
}
