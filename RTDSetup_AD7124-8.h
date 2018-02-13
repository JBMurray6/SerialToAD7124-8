#ifndef RTDSETUP_AD7124_8_H
#define RTDSETUP_AD7124_8_H

#include "AD7124-8.h"

#define AD7124_ADC_CTRL_REG_POWER_MODE_MASK AD7124_ADC_CTRL_REG_POWER_MODE(3)
#define AD7124_ADC_CTRL_REG_MODE_MASK       AD7124_ADC_CTRL_REG_MODE(0xF)    
#define AD7124_IO_CTRL1_REG_IOUT0_MASK      AD7124_IO_CTRL1_REG_IOUT0(0x7)


void RTDSetupAD7124(AD7124 *devicetosetup);
int SetBestCurrent(AD7124 *devicetosetup, int setval);
bool SetChannel(AD7124 *devicetosetup, AD7124::AD7124_Ch channel);
uint32_t ReadAll(AD7124 *devicetosetup);

#endif
