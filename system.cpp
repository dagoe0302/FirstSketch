#include "Arduino.h"
#include "system.h"

static const unsigned long notify_mask[32] = {
  0x00000001,
  0x00000002,
  0x00000004,
  0x00000008,
  0x00000010,
  0x00000020,
  0x00000040,
  0x00000080,
  0x00000100,
  0x00000200,
  0x00000400,
  0x00000800,
  0x00001000,
  0x00002000,
  0x00004000,
  0x00008000,
  0x00010000,
  0x00020000,
  0x00040000,
  0x00080000,
  0x00100000,
  0x00200000,
  0x00400000,
  0x00800000,
  0x01000000,
  0x02000000,
  0x04000000,
  0x08000000,
  0x10000000,
  0x20000000,
  0x40000000,
  0x80000000
};

static NOTIFY_LIST notify_list;

void System::ClearNotify(NOTIFY_ELEMENT_TYPE notify_element)
/*--------------------------------------------------------------
Input    : -
Output   : -             See SPC-file
Function : -
--------------------------------------------------------------*/
{
  //if((read_ccr() & 0x40) != 0)
  //{
    /* Called from high prio */
    //notify_list.val &= ~notify_element;
  //}
  //else if((read_ccr() & 0x80) != 0)
  //{
    /* Called from low prio */
    //set_interrupt_mask(DISABLE_ALL_IRQ);
    //notify_list.val &= ~notify_element;
    //set_interrupt_mask(DISABLE_LOW_IRQ);
  //}
  //else
  //{
    //set_interrupt_mask(DISABLE_ALL_IRQ);
    notify_list.val &= ~notify_element;
    //set_interrupt_mask(ENABLE_ALL_IRQ);
  //}
}

void System::Notify(NOTIFY_ELEMENT_TYPE notify_element)
/*--------------------------------------------------------------
Input    : -
Output   : -             See SPC-file
Function : -
--------------------------------------------------------------*/
{
 // if((read_ccr() & 0x40) != 0)
 // {
    /* Called from high prio */
 //   notify_list.val |= notify_element;
  //}
  //else if((read_ccr() & 0x80) != 0)
 // {
    /* Called from low prio */
  //  set_interrupt_mask(DISABLE_ALL_IRQ);
 //   notify_list.val |= notify_element;
 //   set_interrupt_mask(DISABLE_LOW_IRQ);
 // }
 // else
//  {
 //   set_interrupt_mask(DISABLE_ALL_IRQ);
    notify_list.val |= notify_element;
//    set_interrupt_mask(ENABLE_ALL_IRQ);
//  }
}

NOTIFY_ELEMENT_TYPE System::GetNotifyList(void)
/*--------------------------------------------------------------
Input    : -
Output   : -             See SPC-file
Function : -
--------------------------------------------------------------*/
{
  uint8_t i;

  for(i=0; i<=31; i++)
  {
    if(notify_list.val & notify_mask[i])
    {
     // set_interrupt_mask(DISABLE_ALL_IRQ);
      notify_list.val &= ~notify_mask[i];
    //  set_interrupt_mask(ENABLE_ALL_IRQ);
      return (notify_mask[i]);
    }
  }
  return (0);
}

void System::DisableInterrupts(void)
{
    cli();
}

void System::EnableInterrupts(void)
{
    sei();
}
