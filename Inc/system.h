#ifndef SYSTEM_H
#define SYSTEM_H

#include "Arduino.h"

#define A00 0x00000001
#define A01 0x00000002
#define A02 0x00000004
#define A03 0x00000008
#define A04 0x00000010
#define A05 0x00000020
#define A06 0x00000040
#define A07 0x00000080
#define A08 0x00000100
#define A09 0x00000200
#define A0A 0x00000400
#define A0B 0x00000800
#define A0C 0x00001000
#define A0D 0x00002000
#define A0E 0x00004000
#define A0F 0x00008000
#define A10 0x00010000
#define A11 0x00020000
#define A12 0x00040000
#define A13 0x00080000
#define A14 0x00100000
#define A15 0x00200000
#define A16 0x00400000
#define A17 0x00800000
#define A18 0x01000000
#define A19 0x02000000
#define A1A 0x04000000
#define A1B 0x08000000
#define A1C 0x10000000
#define A1D 0x20000000
#define A1E 0x40000000
#define A1F 0x80000000

typedef long NOTIFY_ELEMENT_TYPE;

typedef union
{
  struct
  {
    long b1f :1;
    long b1e :1;
    long b1d :1;
    long b1c :1;
    long b1b :1;
    long b1a :1;
    long b19 :1;
    long b18 :1;
    long b17 :1;
    long b16 :1;
    long b15 :1;
    long b14 :1;
    long b13 :1;
    long b12 :1;
    long b11 :1;
    long b10 :1;
    long b0f :1;
    long b0e :1;
    long b0d :1;
    long b0c :1;
    long b0b :1;
    long b0a :1;
    long b09 :1;
    long b08 :1;
    long b07 :1;
    long b06 :1;
    long b05 :1;
    long b04 :1;
    long b03 :1;
    long b02 :1;
    long b01 :1;
    long b00 :1;
  } field;
  long val;
} NOTIFY_LIST;

class System
{
  public:
    void ClearNotify(NOTIFY_ELEMENT_TYPE notify_element);
    void Notify(NOTIFY_ELEMENT_TYPE);
    NOTIFY_ELEMENT_TYPE GetNotifyList(void);
    void EnableInterrupts(void);
    void DisableInterrupts(void);
};

#endif
