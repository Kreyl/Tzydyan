/*
 * Sequences.h
 *
 *  Created on: 09 ���. 2015 �.
 *      Author: Kreyl
 */

#pragma once

#include "ChunkTypes.h"

#define COLOR_IDLE      {9, 0, 9}
#define COLOR_POWERED   {255, 0, 255}


const LedRGBChunk_t lsqStart[] = {
        {csSetup, 0, clRed},
        {csWait, 207},
        {csSetup, 0, clGreen},
        {csWait, 207},
        {csSetup, 0, clBlue},
        {csWait, 207},
        {csSetup, 0, COLOR_IDLE},
        {csEnd},
};

const LedRGBChunk_t lsqBlinking[] = {
        {csSetup, 207, COLOR_POWERED},
        {csSetup, 207, COLOR_IDLE},
        {csGoto, 0},
};

const LedRGBChunk_t lsqIdle[] = {
        {csSetup, 270, COLOR_IDLE},
        {csEnd}
};

const LedRGBChunk_t lsqPowered[] = {
        {csSetup, 270, COLOR_POWERED},
        {csEnd}
};

const LedRGBChunk_t lsqFailure[] = {
        {csSetup, 0, clRed},
        {csWait, 99},
        {csSetup, 0, clBlack},
        {csWait, 99},
        {csSetup, 0, clRed},
        {csWait, 99},
        {csSetup, 0, clBlack},
        {csWait, 99},
        {csSetup, 0, clRed},
        {csWait, 99},
        {csSetup, 0, clBlack},
        {csEnd}
};
