#include <stdint.h>
#include "tcs34725.h"
#include "pico/stdlib.h"

static bool tcs_write8(uint8_t reg, uint8_t value) {
    uint8_t buf[2] = { TCS34725_COMMAND_BIT | reg, value };
    int res = i2c_write_blocking(i2c0, TCS34725_I2C_ADDR, buf, 2, false);
    return res == 2;
}

static bool tcs_read16(uint8_t reg, uint16_t *value) {
    uint8_t cmd = TCS34725_COMMAND_BIT | reg;
    uint8_t buf[2];

    if (i2c_write_blocking(i2c0, TCS34725_I2C_ADDR, &cmd, 1, true) != 1)
        return false;

    if (i2c_read_blocking(i2c0, TCS34725_I2C_ADDR, buf, 2, false) != 2)
        return false;

    *value = ((uint16_t)buf[1] << 8) | buf[0];  // LSB primeiro
    return true;
}

bool tcs34725_init_simple(void) {
    // Opcional: ler ID e checar se é 0x44/0x10 conforme datasheet
    uint8_t cmd = TCS34725_COMMAND_BIT | TCS34725_REG_ID;
    uint8_t id = 0;

    if (i2c_write_blocking(i2c0, TCS34725_I2C_ADDR, &cmd, 1, true) != 1)
        return false;
    if (i2c_read_blocking(i2c0, TCS34725_I2C_ADDR, &id, 1, false) != 1)
        return false;

    // Configura tempo de integração e ganho padrão
    // ATIME = 0xEB (~50 ms), ganho x4 (0x01) é um bom compromisso.
    if (!tcs_write8(TCS34725_REG_ATIME, 0xEB)) return false;
    if (!tcs_write8(TCS34725_REG_CONTROL, 0x01)) return false;

    // Power on
    if (!tcs_write8(TCS34725_REG_ENABLE, TCS34725_ENABLE_PON)) return false;
    sleep_ms(3);  // >= 2.4 ms

    // Habilita ADC
    if (!tcs_write8(TCS34725_REG_ENABLE,
                    TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN))
        return false;

    // Espera pelo menos um ciclo de integração
    sleep_ms(60);

    return true;
}

bool tcs34725_read_rgbc(uint16_t *c, uint16_t *r, uint16_t *g, uint16_t *b) {
    if (!c || !r || !g || !b) return false;

    uint16_t c_raw, r_raw, g_raw, b_raw;

    if (!tcs_read16(TCS34725_REG_CDATAL, &c_raw)) return false;
    if (!tcs_read16(TCS34725_REG_CDATAL + 2, &r_raw)) return false;
    if (!tcs_read16(TCS34725_REG_CDATAL + 4, &g_raw)) return false;
    if (!tcs_read16(TCS34725_REG_CDATAL + 6, &b_raw)) return false;

    *c = c_raw;
    *r = r_raw;
    *g = g_raw;
    *b = b_raw;

    return true;
}
