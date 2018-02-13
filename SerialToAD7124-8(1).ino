/*
  Serial Call and Response in ASCII
  Language: Wiring/Arduino

  This program sends an ASCII A (byte of value 65) on startup and repeats that
  until it gets some data in. Then it waits for a byte in the serial port, and
  sends three ASCII-encoded, comma-separated sensor values, truncated by a
  linefeed and carriage return, whenever it gets a byte in.

  The circuit:
  - potentiometers attached to analog inputs 0 and 1
  - pushbutton attached to digital I/O 2

  created 26 Sep 2005
  by Tom Igoe
  modified 24 Apr 2012
  by Tom Igoe and Scott Fitzgerald
  Thanks to Greg Shakar and Scott Fitzgerald for the improvements

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/SerialCallResponseASCII
*/

// include the SPI library:
#include <SPI.h>
#include "AD7124-8.h"
#include "RTDSetup_AD7124-8.h"

//based on teensy 3.2 documentation
#define PWMPin 3
#define CSPin 10
#define CLKPin 13
#define MOSIPin 11
#define MISOPin 12

String InMessage; //Holds the most recent message

long Freq = 488; //PWM frequency
long Duty = 0; //PWM Duty cycle

bool Connected = false; //flips when connected
//Message format:
//
//Prefix+' '+value or RequestChar+Terminator
//Example:
//"Freq 100;"
//
//Whitespace and linefeeds are ignored but safer
//
const String ConnectString = "Hi PWM!\n";
const String ID = "20170915_AD7124_8_JBM";

const String IDPrefix = "ID";
const String AD_IDPrefix = "AD_ID";
const String ValPrefix = "Val";
const String AllValsPrefix = "AllVals";
const String AddrPrefix = "Addr";
const String ResetPrefix = "Reset";
const String ReadPrefix = "Read";
const String CurrentPrefix = "Current";
const String ChannelPrefix = "Channel";
const String IntCalibationPrefix = "IntCalibrate";

const String ErrorPrefix = "Error";

const char RequestChar = '?';
const char Terminator = ';';
const char Delimiter = ',';

////Pin that current will come out of
//const AD7124_8_Pins AD_IOUT1Pin = AIN8;
//list of data input pins
const AD7124::AD7124_8_Pins Default_AD_0ChP_Pin = AD7124::AIN0;
const AD7124::AD7124_8_Pins Default_AD_0ChN_Pin = AD7124::AIN1;
const AD7124::AD7124_8_Pins Default_AD_1ChP_Pin = AD7124::AIN2;
const AD7124::AD7124_8_Pins Default_AD_1ChN_Pin = AD7124::AIN3;
const AD7124::AD7124_8_Pins Default_AD_2ChP_Pin = AD7124::AIN4;
const AD7124::AD7124_8_Pins Default_AD_2ChN_Pin = AD7124::AIN5;
const AD7124::AD7124_8_Pins Default_AD_3ChP_Pin = AD7124::AIN6;
const AD7124::AD7124_8_Pins Default_AD_3ChN_Pin = AD7124::AIN7;

const int NumDefaultChannels = 4;
AD7124::AD7124_Ch Default_AD_Channels[NumDefaultChannels] = {{0, Default_AD_0ChP_Pin, Default_AD_0ChN_Pin, true, 0},
  {1, Default_AD_1ChP_Pin, Default_AD_1ChN_Pin, true, 0},
  {2, Default_AD_2ChP_Pin, Default_AD_2ChN_Pin, true, 0},
  {3, Default_AD_3ChP_Pin, Default_AD_3ChN_Pin, true, 0}
};

//ad7124_st_reg ad7124_regs[57];
const SPISettings settings(1000000, MSBFIRST, SPI_MODE0);
AD7124 AD7124_RTDs = AD7124(CSPin, settings);

int t;

void setup() {
  // start serial port at 115200 bps and wait for port to open:
  Serial.begin(115200);//baud is always 2M on teensy
  Serial.setTimeout(20);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Connected...");

  pinMode(PWMPin, OUTPUT);   // digital sensor is on digital pin PWM
  //establishContact();  // send a byte to establish contact until receiver responds

  RTDSetupAD7124(&AD7124_RTDs);
  //int32_t i =ad7124_regs_init[0].addr;
  //uint32_t ID = AD7124_RTDs.ReadDeviceRegister(AD7124::AD7124_ADC_Control); //ID = Transmit(0x45000000);
  //Serial.println(ID, HEX);
  //  AD7124_RTDs=AD7124(CSPin);

}

void loop() {
  // if there is a message read and parse it.
  if (Serial.available() > 0) {
    //t=millis();
    if (ParseMessage())
    {

    }
  }
}

void establishContact() {//not used right now

  while (!Connected) {//Waits for someone to call

    if (Serial.available() <= 0)
    {
      InMessage = Serial.readString();
      if (InMessage == ConnectString)
      {
        Connected = true;
      }
    }

  }
}

bool ParseMessage()
{
  bool done = false;

  InMessage = Serial.readString();



  int FirstPos = InMessage.indexOf(Terminator);
  if (FirstPos == -1) //Check to see if it is a message at all
  {
    return false;
  }
  //Serial.println(String(millis()-t));
  while (!done)
  {
    String TempStr = InMessage.substring(0, FirstPos);
    //Serial.println("got: " + InMessage);
    if (TempStr.startsWith(IDPrefix))
    {
      TempStr.remove(0, IDPrefix.length());
      TempStr.trim();
      if (TempStr.indexOf(RequestChar) != -1) //contains '?' else do nothing
      {
        Serial.println(ID);
      }
    }
    else if (TempStr.startsWith(AD_IDPrefix))
    {
      TempStr.remove(0, AD_IDPrefix.length());
      TempStr.trim();
      if (TempStr.indexOf(RequestChar) != -1) //contains '?' else do nothing
      {
        GetID();
        //Serial.println(ID);
      }
    }
    else if (TempStr.startsWith(IntCalibationPrefix))
    {

    }
    else if (TempStr.startsWith(AddrPrefix))
    {
      TempStr.remove(0, AddrPrefix.length());
      TempStr.trim();
      if (TempStr.indexOf(RequestChar) != -1) //contains '?' else do nothing
      {
        TempStr.remove(TempStr.indexOf(RequestChar));
        GetAddr(HexStr2num(TempStr));


        //Serial.println(ID);
      }
      else
      {
        if (TempStr.length() >= 4)
        {
          String addr = TempStr.substring(0, 4);
          //Serial.println("Now: " + TempStr);
          //Serial.println(addr);
          int pl_size = (int)((TempStr.length() - 6) / 2); //4 char for addr 0xXX and two for 0x start of num
          //Serial.println(pl_size);
          if (pl_size > 0)
          {
            uint8_t payload[pl_size];
            //int i=0;
            uint32_t com_payload = 0;
            for (int i = 0; i < pl_size; i++)
            {
              payload[i] = HexStr2num(TempStr.substring(7 + i * 2, 7 + i * 2 + 2));
              //Serial.println(':' + TempStr.substring(7 + i * 2, 7 + i * 2 + 2));
              com_payload |= (payload[i] << ((pl_size - i - 1) * 8));
              Serial.println(com_payload, HEX);
            }
            Serial.println(com_payload, HEX);
            uint32_t ID = AD7124_RTDs.WriteDeviceRegister(static_cast<AD7124::ad7124_registers>(HexStr2num(addr)), com_payload); //ID = Transmit(0x45000000);
          }
        }

        //SetAddr(HexStr2num(addr), payload, pl_size);
      }
    }
    else if (TempStr.startsWith(ResetPrefix))
    {
      Serial.println(AD7124_RTDs.Reset());

      RTDSetupAD7124(&AD7124_RTDs);
      Serial.println("Reset...:");
    }
    else if (TempStr.startsWith(ChannelPrefix))
    {
      /*Expected format:
         "Channel" ChanelNumber[Required], EnableVal[Optional; use "on" or num>0 to turn on], PosTerminal[Optional], NegTerminal[Optional], SetupRegister[Optional];
      */
      TempStr.remove(0, CurrentPrefix.length());
      TempStr.trim();

      int channelnum = 0;
      AD7124::AD7124_Ch channel;
      int valpos = TempStr.indexOf(Delimiter);
      if (valpos != -1)
      {
        channelnum = (TempStr.substring(0, valpos)).toInt();
        TempStr.remove(0, valpos + 1);
      }
      else
      {
        Serial.println(ErrorPrefix + " for channel command");
      }
      //Serial.println("cn"+TempStr+String(channelnum));

      if ((channelnum >= 0) && (channelnum < NumDefaultChannels))
      {
        channel = Default_AD_Channels[channelnum];
      }
      else
      {
        channel.ChNum = channelnum;
      }

      valpos = TempStr.indexOf(Delimiter);
      if (valpos != -1)
      {
        channel.Enabled = (((TempStr.substring(0, valpos)).toLowerCase()).startsWith("on") ||
                           (TempStr.substring(0, valpos)).toInt() > 0);
        TempStr.remove(0, valpos + 1);
        //Serial.println("b"+TempStr+String( channel.Enabled));
        valpos = TempStr.indexOf(Delimiter);
        if (valpos != -1)
        {
          channel.Pos = static_cast<AD7124::AD7124_8_Pins>((TempStr.substring(0, valpos)).toInt());
          TempStr.remove(0, valpos + 1);
          //Serial.println("a"+String( (int)(channel.Pos)));

          valpos = TempStr.indexOf(Delimiter);
          if (valpos != -1)
          {
            channel.Neg = static_cast<AD7124::AD7124_8_Pins>((TempStr.substring(0, valpos)).toInt());
            TempStr.remove(0, valpos + 1);
            //Serial.println("n"+String((int) channel.Neg));

            if (TempStr.length() >= 1) //make sure there is still stuff
            {
              channel.SetupNum = ((TempStr).toInt());
              //Serial.println("s"+String( (int)(channel.SetupNum)));

            }
          }
        }

      }
      bool suc = SetChannel(&AD7124_RTDs, channel);
      //Serial.println("Set channel "+ String(suc));
    }
    else if (TempStr.startsWith(ReadPrefix))
    {
      //Serial.println(String(millis()-t));
      //uint32_t time = millis();
      ReadAll(&AD7124_RTDs);
      //Serial.println(millis()-t);

    }
    else if (TempStr.startsWith(CurrentPrefix))
    {
      TempStr.remove(0, CurrentPrefix.length());
      TempStr.trim();
      if (TempStr.indexOf(RequestChar) != -1) //contains '?' else write
      {
        //TempStr.remove(TempStr.indexOf(RequestChar));
        //GetAddr(HexStr2num(TempStr));
        //Serial.println(ID);
      }
      else
      {

        Serial.println(CurrentPrefix + ": " + String(SetBestCurrent(&AD7124_RTDs, TempStr.toInt())));
      }
    }
    else
    {
      Serial.println(ErrorPrefix + ": unrecognized message ");
    }

    //Check for multiple messages
    InMessage.remove(0, FirstPos + 1);
    FirstPos = InMessage.indexOf(Terminator);
    if (FirstPos == -1) //Check to see if it is a message at all
    {
      done = true;
    }
    //delay(1000);

  }
  return done;
}

void GetID()
{
  uint32_t ID = AD7124_RTDs.ReadDeviceRegister(AD7124::AD7124_ID); //ID = Transmit(0x45000000);
  Serial.println(ID, HEX);
}

uint8_t HexStr2num(String S)
{
  //uint8_t returnval;

  S.trim();
  if (S.startsWith("0x"))
  {

    S.remove(0, 2);

  }

  if (S.length() == 1)
  {
    return HexChar2num(S[0]);
  }
  else if (S.length() == 2)
  {
    return (HexChar2num(S[0]) << 4) | (HexChar2num(S[1]));
  }
  else
  {
    return 0;
  }

}

uint8_t HexChar2num(char c)
{
  if (c >= '0' && c <= '9')
  {
    return (uint8_t)(c - '0');
  }
  else
  {
    return (uint8_t)(c - 'A' + 10);
  }
}

void GetAddr(uint8_t addr)
{
  //uint32_t ID = Transmit(0x40000000 | addr<<24);
  uint32_t ID = AD7124_RTDs.ReadDeviceRegister(static_cast<AD7124::ad7124_registers>(addr));
  Serial.println(ID, HEX);
}

void SetAddr(uint8_t addr, uint8_t *payload, int pl_size)
{
  uint32_t ID = 0;
  for (int i = (pl_size - 1); i > 0; i--)
  {
    ID |= payload[i] << (8 * i);
  }

  ID = ID << (8 * (3 - pl_size));
  Serial.println(ID, HEX);
  ID = Transmit((addr << 24) | ID);
  Serial.println(ID, HEX);
}

//bool SetupAD7124()
//{
//  return true;
//}

uint32_t Transmit(uint32_t val)
{
  uint32_t returnval = 0;
  digitalWrite (CSPin, LOW);
  // only two command bytes then the rest is just to read
  returnval |= ((uint32_t)SPI.transfer((char)(val >> 24))) << 24;
  returnval |= ((uint32_t)SPI.transfer((char)(val >> 16))) << 16;
  returnval |= ((uint32_t)SPI.transfer((char)(val >> 8))) << 8;
  returnval |= ((uint32_t)SPI.transfer((char)(val >> 0))) << 0;
  digitalWrite (CSPin, HIGH);
  return returnval;
}


