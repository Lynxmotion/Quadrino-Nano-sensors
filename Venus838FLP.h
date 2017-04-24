#include <Arduino.h>
#include <stdint.h>

/* ENable this for using GPS simulator (NMEA only)*/
//#define GPS_SIMULATOR

/* GPS using a SERIAL port
 if enabled, define here the Arduino Serial port number and the UART speed
 note: only the RX PIN is used in case of NMEA mode, the GPS is not configured by multiwii
 in NMEA mode the GPS must be configured to output GGA and RMC NMEA sentences (which is generally the default conf for most GPS devices)
 at least 5Hz update rate. uncomment the first line to select the GPS serial port of the arduino */
 
#define GPS_SERIAL 2// should be 2 for flyduino v2. It's the serial port number on arduino MEGA
                             // must be 0 for PRO_MINI (ex GPS_PRO_MINI)
                             // note: Now a GPS can share MSP on the same port. The only constrain is to not use it simultaneously, and use the same port speed.

// avoid using 115200 baud because with 16MHz arduino the 115200 baudrate have more than 2% speed error (57600 have 0.8% error)
#define GPS_BAUD   57600// GPS_BAUD will override SERIALx_COM_SPEED for the selected port

// The Venus GPS chip will send location updates at this rate (in Hz)
// Venus6 supports 1, 5, 10, 20, 40
// Venus8 supports Venus6 rates plus 50
#define VENUS_UPDATE_RATE  40


/* GPS protocol 
 NMEA  - Standard NMEA protocol GGA, GSA and RMC  sentences are needed
 UBLOX - U-Blox binary protocol, use the ublox config file (u-blox-config.ublox.txt) from the source tree 
 MTK_BINARY16 and MTK_BINARY19 - MTK3329 chipset based GPS with DIYDrones binary firmware (v1.6 or v1.9)
 With UBLOX and MTK_BINARY you don't have to use GPS_FILTERING in multiwii code !!! */

#ifndef __VENUS6_H
#define __VENUS6_H


typedef unsigned char byte;

//#define VenusSerial Serial2

// enable for Venus8 chipsets, otherwise defaults to Venus6
#define VENUS8

// specify the GPS 3v regulators EN pin that is connected to the Atmel (or undefine if not connected)
#define VenusPowerPin  36

// the default update rate
#ifndef VENUS_UPDATE_RATE
#define VENUS_UPDATE_RATE  20
#endif

// enable to get a lot of output in NMEA format (uses GPREM for remark)
//#define VENUS_DEBUG 1

// the amount of time to wait for a response from the VenusGPS chipset. This is only
// relevant on startup during configuration and no sync waits are performed during normal
// gps loop.
#define VENUS_DEFAULT_TIMEOUT    3000
#define VENUS_FAST_TIMEOUT    500      // when checking baud rate and certain actions expected to return fast

// the maximum number of bytes that can be received in one message (size of the receive buffer)
#define VENUS_MAX_PAYLOAD        60

// maximum and minimum baudrate
#define VENUS_MAX_BAUDRATE       115200 // warning: 115200 has 2% error on atmel, ideal is 57600 @ 0.8%
#define VENUS_MIN_BAUDRATE       9600

// after writing a setting to flash we must wait a small delay (in millis)
// before the GPS to recovers.
#define VENUS_DELAY_AFTER_FLASH  50

/* Venus6 message codes */
        // Input System Messages
#define VENUS_SYS_RESTART                 0x01
#define VENUS_QUERY_SW_VERSION            0x02
#define VENUS_QUERY_SW_CRC                0x03
#define VENUS_SET_FACTORY_DEFAULTS        0x04
#define VENUS_CONFIG_SERIAL_PORT          0x05
#define VENUS_CONFIG_NMEA                 0x08
#define VENUS_CONFIG_OUTPUT_MSG_FORMAT    0x09
#define VENUS_CONFIG_POWER_MODE           0x0C
#define VENUS_CONFIG_GPS_UPDATE_RATE      0x0E
#define VENUS_QUERY_GPS_UPDATE_RATE       0x10
#define VENUS_QUERY_POWER_MODE            0x15
        // Input GPS Messages
#define VENUS_CONFIG_DATUM                0x29
#define VENUS_QUERY_DATUM                 0x2D
#define VENUS_GET_EPHEMERIS               0x30
#define VENUS_SET_EPHEMERIS               0x31
#define VENUS_CONFIG_WAAS                 0x37
#define VENUS_QUERY_WAAS                  0x38
#define VENUS_CONFIG_GPS_PINNING          0x39
#define VENUS_QUERY_GPS_PINNING           0x3A
#define VENUS_CONFIG_GPS_PINNING_PARAMS   0x3B
#define VENUS_CONFIG_NAV_MODE             0x3C
#define VENUS_QUERY_NAV_MODE              0x3D
#define VENUS_CONFIG_1PPS_MODE            0x3E
#define VENUS_QUERY_1PPS_MODE             0x3F
        // Output System Messages
#define VENUS_REPORT_SW_VERSION           0x80
#define VENUS_REPORT_SW_CRC               0x81
#define VENUS_REPORT_GPS_UPDATE_RATE      0x86
#define VENUS_REPORT_EPHEMERIS            0xB1
        // Output GPS Messages
#define VENUS_GPS_LOCATION                0xA8
#define VENUS_GPS_DATUM                   0xAE
#define VENUS_GPS_WAAS_STATUS             0xB3
#define VENUS_GPS_PINNING_STATUS          0xB4
#define VENUS_GPS_NAV_MODE                0xB5
#define VENUS_GPS_1PPS_MODE               0xB6
#define VENUS_GPS_POWER_MODE              0xB9

        // Request Acknowledge
#define VENUS_ACK                         0x83
#define VENUS_NACK                        0x84

#ifdef VENUS8
  // command messages
#define VENUS8_EXT2                       0x62
#define VENUS8_EXT3                       0x63
#define VENUS8_EXT4                       0x64
#define VENUS8_CONFIG_NAV_MODE            0x17  // w/EXT4
#define VENUS8_QUERY_NAV_MODE             0x18  // w/EXT4
#define VENUS8_CONFIG_CONSTELLATION_TYPE  0x19  // w/EXT4
#define VENUS8_QUERY_CONSTELLATION_TYPE   0x1a  // w/EXT4
#define VENUS8_CONFIG_SAEE                0x01  // w/EXT3
#define VENUS8_QUERY_SAEE                 0x02  // w/EXT3
  // Response codes
#define VENUS8_REPORT_NAV_MODE            0x8b  // w/EXT4
#define VENUS8_REPORT_CONSTELLATION_TYPE  0x8c  // w/EXT4
#define VENUS8_REPORT_SAEE                0x80  // w/EXT3
#endif


// Read/write errors (will be char)
#define VENUS_OK           0  // read successful
#define VENUS_TIMEOUT     -1  // no data received within timeout period
#define VENUS_MORE        -2  // more data remaining
#define VENUS_NACKED      -3  // message reply was a nack
#define VENUS_ACK_NOREPLY -4  // message was incomplete
#define VENUS_BADCR       -5  // checksum failure
#define VENUS_INCOMPLETE  -6  // message was incomplete
#define VENUS_CLIPPED     -7  // message buffer too small, response payload was clipped

// Startup error codes
#define STARTUP_NORESPONSE         1
#define STARTUP_BAUDRATE           2
#define STARTUP_MSGMODE            4
#define STARTUP_UPDATERATE         8
#define STARTUP_QUERYOPTIONS      16
#define STARTUP_NAVMODE           32
#define STARTUP_POSPINNING        64
#define STARTUP_POWERMODE        128
#define STARTUP_NOSAEE           256

// NAVIGATION MODES
#define VENUS_NAVMODE_AUTO                0    // defaults to car
#define VENUS_NAVMODE_PEDESTRIAN          1
#ifdef VENUS8
#define VENUS8_NAVMODE_CAR                 2
#define VENUS8_NAVMODE_MARINE              3
#define VENUS8_NAVMODE_BALLON              4    // is this balloon?
#define VENUS8_NAVMODE_AIRBORNE            5
#define VENUS8_NAVMODE_SURVEY_AND_MAPPING  6
#endif

// POSITION PINNING
#ifdef VENUS8
#define VENUS_POSPINNING_AUTO     0
#define VENUS_POSPINNING_ENABLE   1
#define VENUS_POSPINNING_DISABLE  2
#else
#define VENUS_POSPINNING_AUTO     0
#define VENUS_POSPINNING_ENABLE   1
#define VENUS_POSPINNING_DISABLE  0
#endif

// POWER MODES
#define VENUS_POWERMODE_NORMAL    0
#define VENUS_POWERMODE_POWERSAVE 1

// SAEE MODES
#define VENUS8_SAEE_DEFAULT       0
#define VENUS8_SAEE_ENABLE        1
#define VENUS8_SAEE_DISABLE       2

// typical SRAM or FLASH attribute
#define VENUS_SRAM            0
#define VENUS_FLASH           1

// MESSAGE OUTPUT MODES
#define VENUS_OUTPUT_NONE        0
#define VENUS_OUTPUT_NMEA        1
#define VENUS_OUTPUT_BINARY      2

#if !defined(VENUS_OUTPUT_MODE)
#if defined(NMEA)
#define VENUS_OUTPUT_MODE VENUS_OUTPUT_NMEA
#else
#define VENUS_OUTPUT_MODE VENUS_OUTPUT_BINARY
#endif
#endif

#define VENUS_INT16(h,l)   ((h<<8)|l)

typedef struct {
    int32_t x;
  int32_t y;
  int32_t z;
} xyz32_t;

typedef struct {
  uint8_t  fixmode;
  uint8_t  sv_count;  // satellites
  uint16_t gps_week;
  uint32_t gps_tow;
  int32_t latitude;
  int32_t longitude;
  uint32_t ellipsoid_alt;
  uint32_t sealevel_alt;
  uint16_t gdop, pdop, hdop, vdop, tdop;
  xyz32_t ecef;
  xyz32_t vel;
} venus_location;

typedef struct _venus_message {
  unsigned char length;       // payload length;
  byte id;    // message id
  union {
    byte body[VENUS_MAX_PAYLOAD];
    venus_location location;
  };
} venus_message;



// Query the Venus
short VenusQuery(int timeout);
short VenusRead(int timeout); 
short VenusReadAndPrint(int timeout); 
void VenusDispatchMessage(int result);

// initializes the GPS module and sets up defaults for the best airborne performance
// this also scans for the GPS baud rate. You really only need to call this function.
bool GPSModuleInit();

// seeks out the GPS chipset by scanning baudrates, will also setup the
// desired update rate and message format. (called by GPSModuleInit)
bool VenusScan(unsigned long desiredBaud=57600, byte desiredMessageFormat=VENUS_OUTPUT_BINARY);

// process a byte of input, the return value indicates if venus_message struct holds
// a valid message.
short VenusProcessInput(int c);

// configure the default GPS settings (as per config defines)
// already done as part of the GPSModuleInit()
bool GPSConfigureDefaults();

// get the value of a single-byte VENUS_QUERY_xxxx option
byte VenusGetOption(byte msgid);

// set the single-byte value of a VENUS_CONFIG_xxxx option
short VenusSetOption(byte option, byte value, bool flash);

#ifdef VENUS8
// get the value of a single-byte VENUS8_QUERY_xxxxx option (extended option)
byte Venus8GetExtendedOption(byte ext_number, byte msgid);

// set the single-byte value of a VENUS8_CONFIG_xxxx option (extended option)
// these extended messages are under accessed under 1 of 3 extensions (62,63,64) though they may later add (60-6f)
short Venus8SetExtendedOption(byte ext_number, byte option, byte value, bool flash);
#endif

/* Convenience functions 
 *
 * These functions are inline and simply call the Venus(Get/Set)Option or Venus8(Get/Set)ExtendedOption.
 *
 */
 
// Set how often the location should be updated/sent from the GPS receiver
inline byte VenusQueryUpdateRate() { return VenusGetOption(VENUS_QUERY_GPS_UPDATE_RATE); }
inline char VenusSetUpdateRate(byte updaterate, bool flash) { return VenusSetOption(VENUS_CONFIG_GPS_UPDATE_RATE, updaterate, flash); }

// Set the output of GPS location message to Binary or NMEA or None
// (this does not affect other communication, you can still send/receive binary config/query messages)
inline char VenusSetOutput(byte outputmode, bool flash) { return VenusSetOption(VENUS_CONFIG_OUTPUT_MSG_FORMAT, outputmode, flash); }

// GPS Wide Area Augmentation System
inline byte VenusQueryWAAS() { return VenusGetOption(VENUS_QUERY_WAAS); }
inline char VenusSetWAAS(char enable, bool flash) { return VenusSetOption(VENUS_CONFIG_WAAS, enable, flash); }

// Set power mode to POWER SAVE or NORMAL
inline byte VenusQueryPowerMode() { return VenusGetOption(VENUS_QUERY_POWER_MODE); }
inline char VenusSetPowerMode(char enable, bool flash) { return VenusSetOption(VENUS_CONFIG_POWER_MODE, enable, flash); }

// position pinning snaps GPS location to prevent it from constantly moving
// (not a good thing for our airborne copters!)
inline byte VenusQueryPositionPinning() { return VenusGetOption(VENUS_QUERY_GPS_PINNING); }
inline char VenusSetPositionPinning(char enable, bool flash) { return VenusSetOption(VENUS_CONFIG_GPS_PINNING, enable, flash); }

// Self-Aided Ephemeris Estimation (SAEE) is a GNSS feature to aide in the power-on gps lock
// This feature cannot be enabled while the update rate is more than once a second so we disable it
inline byte VenusQuerySAEE() { return Venus8GetExtendedOption(VENUS8_EXT3, VENUS8_QUERY_SAEE); }
inline char VenusSetSAEE(uint32_t value, bool flash) { return Venus8SetExtendedOption(VENUS8_EXT3, VENUS8_CONFIG_SAEE, value, flash); }

// Nav Mode to Auto, Car, Pedestrian, *Airborne*
#ifdef VENUS8
inline byte VenusQueryNavMode() { return Venus8GetExtendedOption(VENUS8_EXT4, VENUS8_QUERY_NAV_MODE); }
inline char VenusSetNavMode(char navmode, bool flash) { return Venus8SetExtendedOption(VENUS8_EXT4, VENUS8_CONFIG_NAV_MODE, navmode, flash); }
#else
inline byte VenusQueryNavMode() { return VenusGetOption(VENUS_QUERY_NAV_MODE); }
inline char VenusSetNavMode(char navmode, bool flash) { return VenusSetOption(VENUS_CONFIG_NAV_MODE, navmode ? 1 : 0 , flash); }
#endif


#endif




#define VENUS_SERIAL_OPEN(x) Serial2.begin(x)
#define VENUS_SERIAL_WRITE(x) Serial2.write(x)
#define VENUS_SERIAL_AVAILABLE() Serial2.available()
#define VENUS_SERIAL_READ() Serial2.read()


venus_message venus_ctx;

#ifdef VENUS_DEBUG
void VenusDebugWriteMsg(const char* desc, bool crlf=true)
{
  Serial.print("$GPREM,");
  Serial.print(desc);
  Serial.print(",L");
  Serial.print(venus_ctx.length);
  Serial.print(',');
  Serial.print((short)venus_ctx.id);
  for(char i=0; i<venus_ctx.length; i++) {
      Serial.print(',');
      Serial.print((short)venus_ctx.body[i]);
  }
  if(crlf)
    Serial.println();  
}
#else
#define VenusDebugWriteMsg(desc) {}
#endif

void VenusWriteImmediate()
{
  int pls=0;
  byte cs=venus_ctx.id;
  while(pls<venus_ctx.length)
    cs = cs ^ venus_ctx.body[pls++];
  pls++;
  VENUS_SERIAL_WRITE(0xA0);
  VENUS_SERIAL_WRITE(0xA1);
  VENUS_SERIAL_WRITE((pls>>8)&0xff);
  VENUS_SERIAL_WRITE(pls&0xff);
  VENUS_SERIAL_WRITE(venus_ctx.id);
  for(pls=0; pls<venus_ctx.length; pls++)
    VENUS_SERIAL_WRITE(venus_ctx.body[pls]);
  //Serial.write(body, length);
  VENUS_SERIAL_WRITE(cs);
  VENUS_SERIAL_WRITE(0x0D);
  VENUS_SERIAL_WRITE(0x0A);
  VenusDebugWriteMsg("OUT");
}

void VenusWriteNull()
{
  VENUS_SERIAL_WRITE(0x0D);
  VENUS_SERIAL_WRITE(0x0A);
}

//inline void VenusWriteImmediate(byte msgid, byte a) { venus_ctx.length=1; venus_ctx.id=msgid; venus_ctx.body[0]=a; VenusWriteImmediate(); }
//inline void VenusWriteImmediate(byte msgid, byte a, byte b) { venus_ctx.length=1; venus_ctx.id=msgid; venus_ctx.body[0]=a; venus_ctx.body[1]=b; VenusWriteImmediate(); }



#if 0
#define SWAP16(x) ((x&0xff)<<8 | (x>>8))
#define SWAP32(x) ((x&0xff)<<24 | ((x&0xff00)<<8) | ((x&0xff0000)>>8) | ((x&0xff000000)>>24))
#else
uint16_t SWAP16(uint16_t x) { return ((x&0xff)<<8 | (x>>8)); }
uint32_t SWAP32(uint32_t x) { return ((x&0xff)<<24 | ((x&0xff00)<<8) | ((x&0xff0000)>>8) | ((x&0xff000000)>>24)); }
#endif

void VenusFixLocationEndianess()
{
  venus_ctx.location.gps_week=SWAP16(venus_ctx.location.gps_week);
  venus_ctx.location.gps_tow = SWAP32(venus_ctx.location.gps_tow);
  venus_ctx.location.latitude = SWAP32(venus_ctx.location.latitude);
  venus_ctx.location.longitude = SWAP32(venus_ctx.location.longitude);
  venus_ctx.location.ellipsoid_alt = SWAP32(venus_ctx.location.ellipsoid_alt);
  venus_ctx.location.sealevel_alt = SWAP32(venus_ctx.location.sealevel_alt);
  venus_ctx.location.gdop = SWAP16(venus_ctx.location.gdop);
  venus_ctx.location.pdop = SWAP16(venus_ctx.location.pdop);
  venus_ctx.location.hdop = SWAP16(venus_ctx.location.hdop);
  venus_ctx.location.vdop = SWAP16(venus_ctx.location.vdop);
  venus_ctx.location.tdop = SWAP16(venus_ctx.location.tdop);
  venus_ctx.location.ecef.x = SWAP32(venus_ctx.location.ecef.x);
  venus_ctx.location.ecef.y = SWAP32(venus_ctx.location.ecef.y);
  venus_ctx.location.ecef.z = SWAP32(venus_ctx.location.ecef.z);
  venus_ctx.location.vel.x = SWAP32(venus_ctx.location.vel.x);
  venus_ctx.location.vel.y = SWAP32(venus_ctx.location.vel.y);
  venus_ctx.location.vel.z = SWAP32(venus_ctx.location.vel.z);
}

short VenusProcessInput(int c)
{
  static byte state=0;
  static unsigned char n=0;
  static int cr=0;
  
  switch(state) {
    case 0: if(c==0xA0) state++; break;
    case 1: if(c==0xA1) state++; else state=0; break;
    case 2: venus_ctx.length=c<<8; state++; break;  // read payload length (2 bytes)
    case 3: venus_ctx.length|=c; state++; break;
    case 4: 
      venus_ctx.id=cr=c; n=0; 
      if(--venus_ctx.length>0) state++; else state=6; // if no payload then skip next state
      break;
    case 5: // read bytes of the payload
      if(n<VENUS_MAX_PAYLOAD)
        venus_ctx.body[n]=(char)c;
      n++;
      cr ^= c;  // adjust checksum
      if(n>=venus_ctx.length) state++;
      break;
    case 6: 
      if(c==cr) 
        state++; 
      else {
        state=0; 
        return VENUS_BADCR; 
      } break; // check checksum, abort if not-equal
    case 7: if(c==0x0d) state++; break;
    case 8: 
      state=0;
      if(venus_ctx.id==VENUS_GPS_LOCATION)
        VenusFixLocationEndianess();
      return (c==0x0A) ? ((n<=VENUS_MAX_PAYLOAD)?VENUS_OK:VENUS_CLIPPED): VENUS_INCOMPLETE; 
    default: 
      state=0; 
      return VENUS_INCOMPLETE;
  }
  return VENUS_MORE;
}

// reads the next binary message
short VenusRead(int timeout)
{
  short result;
  // read with timeout or infinite
  unsigned long long stopat = millis() + timeout;
  while(timeout==0 || millis()<stopat) {
    if (VENUS_SERIAL_AVAILABLE() && ((result=VenusProcessInput(VENUS_SERIAL_READ()))==VENUS_OK || result==VENUS_CLIPPED)) {
        // a complete message was received
      #ifdef VENUS_DEBUG
        VenusDebugWriteMsg("IN",false);
        switch(result) {
          case VENUS_OK:          Serial.println(",OK"); break;
          case VENUS_TIMEOUT:     Serial.println(",TO"); break;
          case VENUS_MORE:        Serial.println(",MORE"); break;
          case VENUS_NACKED:      Serial.println(",NACKED"); break;
          case VENUS_ACK_NOREPLY: Serial.println(",NOREPLY"); break;
          case VENUS_BADCR:       Serial.println(",BADCR"); break;
          case VENUS_INCOMPLETE:  Serial.println(",INC"); break;
          case VENUS_CLIPPED:     Serial.println(",CLIP"); break;
          default:
            Serial.print(",R");
            Serial.println(result);
        }
      #endif
        
        return result;
    }
  }
#ifdef VENUS_DEBUG
  Serial.println("$GPREM,IN,TIMEOUT");
#endif
  return VENUS_TIMEOUT;
}  
short VenusRead(int timeout); 




#if 0
short VenusAsyncRead()
{
  short result;
  if (VENUS_SERIAL_AVAILABLE() && (result=VenusProcessInput(VENUS_SERIAL_READ()))==VENUS_OK || result==VENUS_CLIPPED)
        // a complete message was received
        return result;
  return VENUS_MORE;
}
#endif

short VenusWrite(int timeout)
{
  byte msgid = venus_ctx.id;
  unsigned long long stopat = millis() + timeout;
  VenusWriteImmediate();      // write msg
  while(millis() < stopat) {    // now wait for ack
    VenusRead(stopat - millis());
    if(venus_ctx.id==VENUS_ACK && venus_ctx.body[0]==msgid)
      return VENUS_OK;
    else if(venus_ctx.id==VENUS_NACK && venus_ctx.body[0]==msgid)
      return VENUS_NACKED;
    //else
    //  VenusDispatchMessage();  // dispatch this message through the normal channels
  }
  return VENUS_TIMEOUT;
}

/* conflicts with VenusWrite(timeout)
bool VenusWrite(byte queryid)
{
    venus_ctx.id = queryid;
    venus_ctx.length = 0;
    return VenusWrite(1000);
}*/

// return the response msgid for a given query msgid
byte VenusWhatIsResponseMsgIdOf(byte msgid)
{
  if(msgid>0x60 && msgid<0x6f)
    return msgid;
  switch(msgid) {
         case VENUS_QUERY_SW_VERSION: return VENUS_REPORT_SW_VERSION; 
    case VENUS_QUERY_GPS_UPDATE_RATE: return VENUS_REPORT_GPS_UPDATE_RATE; 
            case VENUS_GET_EPHEMERIS: return VENUS_REPORT_EPHEMERIS; 
              case VENUS_QUERY_DATUM: return VENUS_GPS_DATUM; 
               case VENUS_QUERY_WAAS: return VENUS_GPS_WAAS_STATUS; 
        case VENUS_QUERY_GPS_PINNING: return VENUS_GPS_PINNING_STATUS; 
           case VENUS_QUERY_NAV_MODE: return VENUS_GPS_NAV_MODE; 
          case VENUS_QUERY_1PPS_MODE: return VENUS_GPS_1PPS_MODE; 
         case VENUS_QUERY_POWER_MODE: return VENUS_GPS_POWER_MODE;
          default: return 0;
  }
}

byte Venus8WhatIsResponseMsgIdOf(byte msgid)
{
  switch(msgid) {
             case VENUS8_QUERY_NAV_MODE: return VENUS8_REPORT_NAV_MODE;
   case VENUS8_QUERY_CONSTELLATION_TYPE: return VENUS8_REPORT_CONSTELLATION_TYPE;
                 case VENUS8_QUERY_SAEE: return VENUS8_REPORT_SAEE;
    default: return 0;
  }
}

// send a query to the GPS module and expect/waitfor a reply
short VenusQuery(int timeout)
{
  int orig_timeout = timeout;
  byte msgid=venus_ctx.id, reply_msgid=VenusWhatIsResponseMsgIdOf(venus_ctx.id);
  bool isExtMsg = msgid>0x60 && msgid<0x6f;
  
  // venus8 extension has two id fields
  byte msgid8 =0, reply_msgid8 =0;
  if(isExtMsg) {
   msgid8 = venus_ctx.body[0];
   reply_msgid8 = Venus8WhatIsResponseMsgIdOf(msgid8);
  }
  
  unsigned long long stopat = millis() + timeout;
  bool acked=false;
  short result;
  VenusWriteImmediate();      // write msg
  while(millis() < stopat) {    // now wait for ack
    result=VenusRead(stopat - millis());
    if(result == VENUS_TIMEOUT)
      goto timedout;
    else if(venus_ctx.id==VENUS_ACK && venus_ctx.body[0]==msgid) {
      if(!isExtMsg || venus_ctx.body[1] == msgid8) {
        acked=true;  // got the ack, now the response
        timeout = orig_timeout;
        //Serial.println("$GPSREM,ACK");
        //VenusDispatchMessage(result);  // dispatch this message through the normal channels
      }
    } else if(venus_ctx.id==VENUS_NACK && venus_ctx.body[0]==msgid) {
      if(!isExtMsg || venus_ctx.body[1] == msgid8) {
        //Serial.println("$GPSREM,NACK");
        return VENUS_NACKED;
      }
    } else if(venus_ctx.id==reply_msgid) {
      if(!isExtMsg || venus_ctx.body[0] == reply_msgid8) {
        //Serial.println("$GPSREM,SUCCESS");
        //VenusDispatchMessage(result);  // dispatch this message through the normal channels
        return result;
      }
    } //else
      
  }
timedout:
  return acked ? VENUS_ACK_NOREPLY : VENUS_TIMEOUT;
}

byte VenusGetOption(byte msgid)
{
  venus_ctx.length = 0;
  venus_ctx.id = msgid;
  return (VenusQuery(VENUS_DEFAULT_TIMEOUT)==VENUS_OK)
    ? venus_ctx.body[0]
    : 0;
}

short VenusSetOption(byte option, byte value, bool flash)
{
    venus_ctx.id = option;
    venus_ctx.length = 2;
    venus_ctx.body[0] = value;
    venus_ctx.body[1] = flash?VENUS_FLASH:VENUS_SRAM;
    short result=VenusWrite(VENUS_DEFAULT_TIMEOUT);
    if(result!=VENUS_TIMEOUT && flash)
      delay(VENUS_DELAY_AFTER_FLASH);  // slight delay for writing to flash
    return result;
}

byte Venus8GetExtendedOption(byte ext_number, byte msgid)
{
  venus_ctx.length = 1;
  venus_ctx.id = ext_number;
  venus_ctx.body[0] = msgid;
#ifndef VENUS_DEBUG
  return (VenusQuery(VENUS_DEFAULT_TIMEOUT)==VENUS_OK)
    ? venus_ctx.body[1]
    : 0;
#else
  short result=VenusQuery(VENUS_DEFAULT_TIMEOUT);
  Serial.print("$GPREM,GET,M");
  Serial.print((short)msgid);
  Serial.print(",R");
  Serial.print(result);
  Serial.print(',');
  Serial.print(venus_ctx.body[0]);
  Serial.print(',');
  Serial.println(venus_ctx.body[1]);
  return (result==VENUS_OK)
    ? venus_ctx.body[1]
    : 0;
 #endif
}

short Venus8SetExtendedOption(byte ext_number, byte option, byte value, bool flash)
{
    venus_ctx.id = ext_number;
    venus_ctx.length = 3;
    venus_ctx.body[0] = option;
    venus_ctx.body[1] = value;
    venus_ctx.body[2] = flash?VENUS_FLASH:VENUS_SRAM;
    short result=VenusWrite(VENUS_DEFAULT_TIMEOUT);
    if(result!=VENUS_TIMEOUT && flash)
      delay(VENUS_DELAY_AFTER_FLASH);  // slight delay for writing to flash
    return result;
}
  
uint32_t VenusGetBaudRate(byte n)
{
  switch(n) {
    case -1: return GPS_BAUD;  // the default speed
    //case 0: return 4800;
    case 1: return 9600;
//    case 2: return 19200;  // dont bother using these
//    case 3: return 28800;
    case 4: return 57600;
    case 5: return 115200;
    default: return 0;
  }
}

byte VenusGetBaudRateOrdinal(uint32_t baud)
{
  switch(baud) {
    case 4800: return 0;
    case 9600: return 1;
    case 19200: return 2;
    case 28800: return 3;
    case 57600: return 4;
    default: return 5;
  }
}

char VenusSetBaudRate(uint32_t baudrate, bool flash)
{
  char result;
  venus_ctx.id = VENUS_CONFIG_SERIAL_PORT;
  venus_ctx.length = 3;
  venus_ctx.body[0] = 0;  // Venus device's COM1
  venus_ctx.body[1] = VenusGetBaudRateOrdinal(baudrate);
  venus_ctx.body[2] = flash?VENUS_FLASH:VENUS_SRAM;
  if((result=VenusWrite(VENUS_DEFAULT_TIMEOUT)) == VENUS_OK) {
    VENUS_SERIAL_OPEN(baudrate);
    
    // wait until we can communicate
    short attempts = 5;
    short wait = 100;
    venus_ctx.length = 1;
    venus_ctx.id = VENUS_QUERY_SW_VERSION;
    venus_ctx.body[0] = 1;  // system code
    while(attempts-- >0 && VenusQuery(VENUS_DEFAULT_TIMEOUT)!=VENUS_OK) {
      delay(wait);
      wait <<=1;  // multiply by 2 so as to progressively delay longer each fail
    }
    
    return (attempts>0) ? VENUS_OK : VENUS_ACK_NOREPLY;
  } else {
    return result;
  }
}

#ifdef VenusPowerPin
void VenusPowerCycle()
{
  // enable the 3V3B regulator thus enabling the GPS
  pinMode(VenusPowerPin, OUTPUT); 
  digitalWrite(VenusPowerPin, 0);
  delay(50);
  digitalWrite(VenusPowerPin, 1);  
  delay(500); // lets try a seconds pause
}
#else
#define VenusPowerCycle() {}  // no GPS_PWR_EN connected
#endif



void VenusError(short code)
{
  if(code==0) 
    return;
#ifdef VENUS_DEBUG
  Serial.print("$GPERR,");
  Serial.println(code);
#endif


  while(code>=1)
  {
    code >>=1; // divide by 2
    
  }
}

bool VenusScan(unsigned long desiredBaud, byte desiredMessageFormat)
{
  // detect the baud rate
  char result;
  uint32_t baud;
  int attempts=2;
  char i = VenusGetBaudRateOrdinal(desiredBaud);
  
  while(attempts >0) {
    
    
    // try to communicate at this baud rate
    baud = VenusGetBaudRate(i);
    if(baud>0) {
#ifdef VENUS_DEBUG
      Serial.print("$BAUD,");
      Serial.println(baud);
#endif
      
      VENUS_SERIAL_OPEN(baud);
      
      // a few null messages to clear the air
      //VENUS_LED_ON
    delay(6);
      for(int i=0; i<5; i++) {
        VenusWriteNull();
        delay(6);
      }
      //VENUS_LED_OFF

      venus_ctx.id=VENUS_QUERY_GPS_UPDATE_RATE;
      venus_ctx.length=0;
      if((result=VenusQuery(VENUS_FAST_TIMEOUT)) == VENUS_OK) {
        VenusSetOutput(VENUS_OUTPUT_NONE, false);
  
        if(baud != desiredBaud) {
          if(VenusSetBaudRate(desiredBaud, true)!=VENUS_OK)
            VenusError(STARTUP_BAUDRATE);
        }
  
        if(VenusSetOutput(desiredMessageFormat, false)!=VENUS_OK)
          VenusError(STARTUP_MSGMODE);
  
        return true;  // successful connect
      }
    }
    
    // try next baud level
    if(--i <0) {
        i = VenusGetBaudRateOrdinal(VENUS_MAX_BAUDRATE);
        attempts--;
        VenusPowerCycle();
    }
  }
  VenusError(STARTUP_NORESPONSE);
  return false;
}

bool GPSModuleInit()
{
  VenusPowerCycle();
  return VenusScan(GPS_BAUD, VENUS_OUTPUT_MODE);
}

bool GPSConfigureDefaults()
{
  // query options state
  short result;
  unsigned char values[5];
  values[0] = VenusQueryNavMode();
  values[1] = VenusQueryPositionPinning();
  values[2] = VenusQuerySAEE();
  values[3] = VenusQueryPowerMode();
  values[4] = VenusQueryUpdateRate();
  
#if VENUS_DEBUG
  Serial.print("$GPOPT");  // not standard NMEA but it works and spits out in GPS Viewer
  for(char i=0; i<sizeof(values); i++) {
      Serial.print(',');
      Serial.print((short)values[i]);
  }
  Serial.println();
#endif

#if 1
  // configure NAV MODE
  if(values[0]!=VENUS8_NAVMODE_AIRBORNE && (result=VenusSetNavMode(VENUS8_NAVMODE_AIRBORNE, VENUS_FLASH))!=VENUS_OK)
    VenusError(STARTUP_NAVMODE);
  
  // disable POSITION PINNING
  if(values[1]!=VENUS_POSPINNING_DISABLE && (result=VenusSetPositionPinning(VENUS_POSPINNING_DISABLE, VENUS_FLASH))!=VENUS_OK)
    VenusError(STARTUP_POSPINNING);

  // disable SAEE
  if(values[2]!=VENUS8_SAEE_DISABLE && (result=VenusSetSAEE(VENUS8_SAEE_DISABLE, VENUS_FLASH))!=VENUS_OK)
    VenusError(STARTUP_NOSAEE);

  // NORMAL power mode
  if(values[3]!=VENUS_POWERMODE_NORMAL && (result=VenusSetPowerMode(VENUS_POWERMODE_NORMAL, VENUS_FLASH))!=VENUS_OK)
    VenusError(STARTUP_POWERMODE);

  // enable the update rate
  if(values[4]!=VENUS_UPDATE_RATE && VenusSetUpdateRate(VENUS_UPDATE_RATE, true)!=VENUS_OK)
    VenusError(STARTUP_UPDATERATE);
#endif
  
#ifdef VENUS_DEBUG
  Serial.println("$GPREM,SUCCESS");
#endif
  return true;
}

void VenusDispatchMessage(int result){
  //f.GPS_FIX                   = venus_ctx.location.fixmode >=2;
  Serial.print("Latitude: ");
  Serial.print(float(venus_ctx.location.latitude/10000000.000000),6);         // With 1.9 now we have real 10e7 precision
  Serial.print("\t Longitude: ");
  Serial.print(float(venus_ctx.location.longitude/10000000.000000),6);
  Serial.print("\t Altitude: ");
  Serial.print(float(venus_ctx.location.sealevel_alt /100));
  Serial.print("m \t Satelite Count: ");
  Serial.println(venus_ctx.location.sv_count);
  //GPS_coord[LON]              = venus_ctx.location.longitude;
  //GPS_altitude                = venus_ctx.location.sealevel_alt /100;    // altitude in meter
  //GPS_numSat                  = venus_ctx.location.sv_count;
  
  // note: the following vars are currently not used in nav code -- avoid retrieving it to save time
  // also, Venus8 doesnt provide these in a way that we can easily calculate without taking cpu cycles
  //GPS_speed                   = venus_ctx.location.ground_speed;     // in m/s * 100 == in cm/s
  //GPS_ground_course           = venus_ctx.location.ground_course/100;  //in degrees
}


// reads the next binary message and prints the result
short VenusReadAndPrint(int timeout)
{
  short result;
  // read with timeout or infinite
  unsigned long long stopat = millis() + timeout;
  while(timeout==0 || millis()<stopat) {
    if (VENUS_SERIAL_AVAILABLE() && ((result=VenusProcessInput(VENUS_SERIAL_READ()))==VENUS_OK || result==VENUS_CLIPPED)) {
        // a complete message was received
        VenusDispatchMessage(result);
        return result;
    }
  }
}  




