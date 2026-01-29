#ifndef AHT10_H
#define AHT10_H

#include <stdbool.h>
#include <stdint.h>
#include "hardware/i2c.h"

// Endereço I2C padrão do AHT10
#define AHT10_I2C_ADDRESS  0x38

// Comandos AHT10
#define AHT10_CMD_INITIALIZE  0xE1
#define AHT10_CMD_MEASURE     0xAC
#define AHT10_CMD_RESET       0xBA

// Driver básico: sem handle, apenas funções globais
bool aht10_init_simple(void);
bool aht10_read_simple(float *temperature, float *humidity);

#endif // AHT10_H
