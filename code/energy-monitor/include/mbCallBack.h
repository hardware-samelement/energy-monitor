#ifndef mbCallBack_h
#define mbCallBack_h

#include <stdint.h>
#include "EnergyMonitor_global.h"
// extern mbConf;
// extern ModbusRTU mb;

int serialConfig(int _dataLength, int _parity, int _stopbit);
uint16_t cb_baudrate(TRegister* reg, uint16_t val);
uint16_t cb_dataLength(TRegister* reg, uint16_t val);
uint16_t cb_parity(TRegister* reg, uint16_t val);
uint16_t cb_stopbit(TRegister* reg, uint16_t val);
uint16_t cb_address(TRegister* reg, uint16_t val);
uint16_t cb_cal_v1(TRegister* reg, uint16_t val);
uint16_t cb_cal_v2(TRegister* reg, uint16_t val);
uint16_t cb_cal_v3(TRegister* reg, uint16_t val);
uint16_t cb_cal_i1(TRegister* reg, uint16_t val);
uint16_t cb_cal_i2(TRegister* reg, uint16_t val);
uint16_t cb_cal_i3(TRegister* reg, uint16_t val);
#endif