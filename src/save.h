#ifndef SAVE_H
#define SAVE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SAVE_APPVAR_NAME "PADDLZHS"
#define SAVE_FORMAT_VERSION 1
#define SAVE_DATA_SIZE 8

void encodeSaveData(unsigned int high_score, uint8_t data[SAVE_DATA_SIZE]);
bool decodeSaveData(const uint8_t* data, size_t size, unsigned int* high_score);
unsigned int loadHighScore(void);
bool saveHighScore(unsigned int high_score);

#endif
