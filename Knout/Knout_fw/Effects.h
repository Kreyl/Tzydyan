/*
 * Effects.h
 *
 *  Created on: 28 нояб. 2020 г.
 *      Author: layst
 */

#pragma once

#include "color.h"

struct Params_t {
    int32_t SpeedMin=180, SpeedMax=270;
    int32_t SizeMin=4, SizeMax=70;
    int32_t DelayMin=180, DelayMax=360;

    int32_t GetSpeed() { return Random::Generate(SpeedMin, SpeedMax); }
    int32_t GetSize()  { return Random::Generate(SizeMin,  SizeMax); }
    int32_t GetDelay() { return TIME_MS2I(Random::Generate(DelayMin, DelayMax)); }
};

extern Params_t Params;

namespace Effects {

void Init();
void PowerOn();
void PowerOff();
void BatteryIndication(uint32_t ABattery_mV);

};
