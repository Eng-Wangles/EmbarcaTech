#ifndef TCS34725_H
#define TCS34725_H

#include <stdint.h>
#include <stdbool.h>
#include "hardware/i2c.h"

// Endereço I2C padrão
#define TCS34725_I2C_ADDR   0x29

// Comando base (bit 7 = 1)
#define TCS34725_COMMAND_BIT   0x80

// Registradores
#define TCS34725_REG_ENABLE    0x00
#define TCS34725_REG_ATIME     0x01
#define TCS34725_REG_CONTROL   0x0F
#define TCS34725_REG_ID        0x12
#define TCS34725_REG_CDATAL    0x14  // clear + R/G/B seguem em pares

// Bits de ENABLE
#define TCS34725_ENABLE_PON    0x01
#define TCS34725_ENABLE_AEN    0x02

// Protótipos
bool tcs34725_init_simple(void);
bool tcs34725_read_rgbc(uint16_t *c, uint16_t *r, uint16_t *g, uint16_t *b);

#endif
