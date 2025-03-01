#ifndef lcd_display_h
#define lcd_display_h

#include "EnergyMonitor_global.h"

enum {
  display_voltage = 0,
  display_current,
  display_real,
  display_apparent,
  display_energy,
  display_pf,
  display_ch1,
  display_ch2,
  display_ch3,
  display_info,
  display_apmode

} display_page_t;

void lcd_init();
void lcd_show(int page);
void clearValue(void);
void clearValueCh(void);

#endif