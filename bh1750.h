#ifndef BH1750_H
#define BH1750_H

#include <stdbool.h>
#include <stdint.h>
#include "hardware/i2c.h"
#include "hardware_config.h"   // usa BH1750_I2C_ADDR definido lá

// Comandos principais
#define BH1750_POWER_ON       0x01
#define BH1750_RESET          0x07
#define BH1750_CONT_HIRES     0x10  // Continuous high resolution mode

bool bh1750_init_simple(void);
bool bh1750_read_simple(float *lux);

#endif // BH1750_H
