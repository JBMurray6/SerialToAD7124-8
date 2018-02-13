#include "RTDSetup_AD7124-8.h"

const int NumRegToSet = 8;
const int InitialNumActiveChs = 2;
int NumActiveChs = InitialNumActiveChs;

int32_t AllChData[16];

uint16_t UsedPorts_BF = 0x0003; //bit field 0b0000 0000 0000 0011


#define AD7124_CFG_REG_BIPOLAR     (1 << 11)
#define AD7124_CFG_REG_BURNOUT(x)  (((x) & 0x3) << 9)
#define AD7124_CFG_REG_REF_BUFP    (1 << 8)
#define AD7124_CFG_REG_REF_BUFM    (1 << 7)
#define AD7124_CFG_REG_AIN_BUFP    (1 << 6)
#define AD7124_CFG_REG_AINN_BUFM   (1 << 5)
#define AD7124_CFG_REG_REF_SEL(x)  ((x) & 0x3) << 3
#define AD7124_CFG_REG_PGA(x)      (((x) & 0x7) << 0)

const AD7124::ad7124_st_reg ad7124_regs_Setup[NumRegToSet] = {
  //{0x00, 0x00,   1, 2}, /* AD7124_Status */
  {0x01, 0x0000 | AD7124_ADC_CTRL_REG_POWER_MODE(2) | AD7124_ADC_CTRL_REG_MODE(0) | AD7124_ADC_CTRL_REG_DATA_STATUS | AD7124_ADC_CTRL_REG_REF_EN, 2, 1}, /* AD7124_ADC_Control set to single conversion*/
  //{0x02, 0x0000, 3, 2}, /* AD7124_Data */
  {0x03, 0x0000 | AD7124_IO_CTRL1_REG_IOUT0(4) | AD7124_IO_CTRL1_REG_IOUT_CH0(6), 3, 1}, /* AD7124_IOCon1 */
  //  {0x04, 0x0000, 2, 1}, /* AD7124_IOCon2 */
  //  {0x05, 0x14,   1, 2}, /* AD7124_ID */
  //  {0x06, 0x0000, 3, 2}, /* AD7124_Error */
  //  {0x07, 0x0040, 3, 1}, /* AD7124_Error_En */
  //  {0x08, 0x00,   1, 2}, /* AD7124_Mclk_Count */
  {0x09, 0x8020 | AD7124_CH_MAP_REG_CH_ENABLE, 2, 1}, /* AD7124_Channel_0 */
  {0x0A, 0x0000 | AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_AINM(2) | AD7124_CH_MAP_REG_AINP(3), 2, 1}, /* AD7124_Channel_1 */
  //  {0x0B, 0x0000 | AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_AINM(6) | AD7124_CH_MAP_REG_AINP(5), 2, 1}, /* AD7124_Channel_2 */
  //  {0x0C, 0x0001 | AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_AINM(0) | AD7124_CH_MAP_REG_AINP(1), 2, 1},  /* AD7124_Channel_3 */
  //  {0x0D, 0x0001 | AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_AINM(3) | AD7124_CH_MAP_REG_AINP(4), 2, 1}, /* AD7124_Channel_4 */
  //  {0x0E, 0x0001, 2, 1}, /* AD7124_Channel_5 */
  //  {0x0F, 0x0001, 2, 1}, /* AD7124_Channel_6 */
  //  {0x10, 0x0001, 2, 1}, /* AD7124_Channel_7 */
  //  {0x11, 0x0001, 2, 1}, /* AD7124_Channel_8 */
  //  {0x12, 0x0001, 2, 1}, /* AD7124_Channel_9 */
  //  {0x13, 0x0001, 2, 1}, /* AD7124_Channel_10 */
  //  {0x14, 0x0001, 2, 1}, /* AD7124_Channel_11 */
  //  {0x15, 0x0001, 2, 1}, /* AD7124_Channel_12 */
  //  {0x16, 0x0001, 2, 1}, /* AD7124_Channel_13 */
  //  {0x17, 0x0001, 2, 1}, /* AD7124_Channel_14 */
  //  {0x18, 0x0001, 2, 1}, /* AD7124_Channel_15 */
  {0x19, 0x0860 | AD7124_CFG_REG_REF_BUFP | AD7124_CFG_REG_REF_BUFM, 2, 1}, /* AD7124_Config_0 */
  {0x1A, 0x0860 | AD7124_CFG_REG_REF_BUFP | AD7124_CFG_REG_REF_BUFM | AD7124_CFG_REG_REF_SEL(2), 2, 1}, /* AD7124_Config_1 */
  //  {0x1B, 0x0860, 2, 1}, /* AD7124_Config_2 */
  //  {0x1C, 0x0860, 2, 1}, /* AD7124_Config_3 */
  //  {0x1D, 0x0860, 2, 1}, /* AD7124_Config_4 */
  //  {0x1E, 0x0860, 2, 1}, /* AD7124_Config_5 */
  //  {0x1F, 0x0860, 2, 1}, /* AD7124_Config_6 */
  //  {0x20, 0x0860, 2, 1}, /* AD7124_Config_7 */
  {0x21, 0x060000 | AD7124_FILT_REG_FS(0x140) , 3, 1}, /* AD7124_Filter_0 set to 60 Hz on full power*/
    {0x22, 0x060000 | AD7124_FILT_REG_FS(0x140) , 3, 1} /* AD7124_Filter_1 */
  //  {0x23, 0x060180, 3, 1}, /* AD7124_Filter_2 */
  //  {0x24, 0x060180, 3, 1}, /* AD7124_Filter_3 */
  //  {0x25, 0x060180, 3, 1}, /* AD7124_Filter_4 */
  //  {0x26, 0x060180, 3, 1}, /* AD7124_Filter_5 */
  //  {0x27, 0x060180, 3, 1}, /* AD7124_Filter_6 */
  //  {0x28, 0x060180, 3, 1}, /* AD7124_Filter_7 */
  //  {0x29, 0x800000, 3, 1}, /* AD7124_Offset_0 */
  //  {0x2A, 0x800000, 3, 1}, /* AD7124_Offset_1 */
  //  {0x2B, 0x800000, 3, 1}, /* AD7124_Offset_2 */
  //  {0x2C, 0x800000, 3, 1}, /* AD7124_Offset_3 */
  //  {0x2D, 0x800000, 3, 1}, /* AD7124_Offset_4 */
  //  {0x2E, 0x800000, 3, 1}, /* AD7124_Offset_5 */
  //  {0x2F, 0x800000, 3, 1}, /* AD7124_Offset_6 */
  //  {0x30, 0x800000, 3, 1}, /* AD7124_Offset_7 */
  //  {0x31, 0x500000, 3, 1}, /* AD7124_Gain_0 */
  //  {0x32, 0x500000, 3, 1}, /* AD7124_Gain_1 */
  //  {0x33, 0x500000, 3, 1}, /* AD7124_Gain_2 */
  //  {0x34, 0x500000, 3, 1}, /* AD7124_Gain_3 */
  //  {0x35, 0x500000, 3, 1}, /* AD7124_Gain_4 */
  //  {0x36, 0x500000, 3, 1}, /* AD7124_Gain_5 */
  //  {0x37, 0x500000, 3, 1}, /* AD7124_Gain_6 */
  //  {0x38, 0x500000, 3, 1}, /* AD7124_Gain_7 */
};

void RTDSetupAD7124(AD7124 *devicetosetup)
{
  NumActiveChs=InitialNumActiveChs;
  devicetosetup->Setup();
  for (int i = 0; i < NumRegToSet; i++)
  {
    devicetosetup->WriteRegister(ad7124_regs_Setup[i]);
  }
}

int GetCurrent(AD7124 *devicetosetup)
{
  uint32_t val = devicetosetup->ReadDeviceRegister(static_cast<AD7124::ad7124_registers>(AD7124::AD7124_IOCon1));
  int index= (val>>8) & 0x7;//inverting  (((x) & 0x7) << 8);
  return devicetosetup->CurrentOptions[index];
}

int SetBestCurrent(AD7124 *devicetosetup, int setval)
{
  int bestval;
  int thiserror;
  int lasterror = abs(devicetosetup->CurrentOptions[0] - setval);
  for (int i = 1; i < 7; i++)
  {
    thiserror = abs(devicetosetup->CurrentOptions[i] - setval);
    //Serial.println(String(thiserror) + " " + String(lasterror)  + " " +String(devicetosetup->CurrentOptions[i]));
    
    if (lasterror < thiserror)
    {
      bestval = i - 1;
      break;
    }
    else
    { 
      
      bestval =i;
    }
    lasterror=thiserror;
  }

  uint32_t val = devicetosetup->ReadDeviceRegister(static_cast<AD7124::ad7124_registers>(AD7124::AD7124_IOCon1));
  val &= ~((uint32_t)AD7124_IO_CTRL1_REG_IOUT0_MASK); //clear the bits
  //Serial.println(val,HEX);
  val |= (AD7124_IO_CTRL1_REG_IOUT0(bestval)); //Single conversion mode

  devicetosetup->WriteDeviceRegister(static_cast<AD7124::ad7124_registers>(AD7124::AD7124_IOCon1), val);
  return (int)devicetosetup->CurrentOptions[bestval];
}

bool SetChannel(AD7124 *devicetosetup, AD7124::AD7124_Ch channel)
{

  AD7124::ad7124_st_reg reg;
  
  if (((int)channel.ChNum > 7))
  {
    return false;
  }
  reg=devicetosetup->ad7124_regs[(int)channel.ChNum+(int)AD7124::AD7124_Channel_0];
  //reg.addr = (int)channel.ChNum+(int)AD7124::AD7124_Channel_0;


  if (channel.Enabled)
  {
    reg.value = 0x0000 | AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(channel.SetupNum) | AD7124_CH_MAP_REG_AINM(channel.Neg) | AD7124_CH_MAP_REG_AINP(channel.Pos);
  }
  else
  {
    reg.value = 0x0000 | AD7124_CH_MAP_REG_SETUP(channel.SetupNum) | AD7124_CH_MAP_REG_AINM(channel.Neg) | AD7124_CH_MAP_REG_AINP(channel.Pos);
  }
//Channel 0,0,1,2,0;

  uint32_t current = devicetosetup->ReadDeviceRegister(static_cast<AD7124::ad7124_registers>(reg.addr));

  if((AD7124_CH_MAP_REG_CH_ENABLE&current)&&(reg.value&AD7124_CH_MAP_REG_CH_ENABLE))
  {
      
  }
  else if(AD7124_CH_MAP_REG_CH_ENABLE&current)
  {
    NumActiveChs--;
  }
  else if(reg.value&AD7124_CH_MAP_REG_CH_ENABLE)
  {
    NumActiveChs++;
  }
  else
  {
    
  }
  //Serial.println("Setting Ch: "+String(reg.value,HEX));
//       Serial.println("Setting Ch: "+String(channel.ChNum));
  //return false;
  //delay(5);
  devicetosetup->WriteRegister(reg);
     //current = devicetosetup->ReadDeviceRegister(static_cast<AD7124::ad7124_registers>(reg.addr));
     
  //return false;

  return true;

}

uint32_t ReadAll(AD7124 *devicetosetup)
{
  //uint8_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};


  uint32_t val = devicetosetup->ReadDeviceRegister(static_cast<AD7124::ad7124_registers>(AD7124::AD7124_ADC_Control));

  //Check if unit is in standby (mode 2) if so initialize single conversion.
  if (((val) &
       ((uint32_t)AD7124_ADC_CTRL_REG_MODE(2))) != 0)
  {
    //Serial.println(val,HEX);
    val &= ~((uint32_t)AD7124_ADC_CTRL_REG_MODE_MASK); //clear the bits
    //Serial.println(val,HEX);
    val |= (AD7124_ADC_CTRL_REG_MODE(0)); //cont conversion mode

    devicetosetup->WriteDeviceRegister(static_cast<AD7124::ad7124_registers>(AD7124::AD7124_ADC_Control), val);
    //Serial.println(val,HEX);
  }
  int32_t trashvar;
  devicetosetup->ReadData( &trashvar);
  uint32_t currentchannel;
  for (int i = 0; i < NumActiveChs; i++)
  {

    //uint32_t time = millis();
    if (TIMEOUT == devicetosetup->WaitForConvReady(1000))
    {
      //Serial.println("t");
    }
    
    currentchannel=0x0f&devicetosetup->ReadDeviceRegister(AD7124::AD7124_Status);
    devicetosetup->ReadData( &AllChData[i]);
    
    Serial.println("Ch "+String(currentchannel)+": "+String(AllChData[i] >> 8));
    //Serial.println(millis() - time);
  }
  return (uint32_t) 1;

}

