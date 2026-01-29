#include "bh1750.h"
#include "pico/stdlib.h"

static bool bh1750_write(uint8_t cmd) {
    int res = i2c_write_blocking(i2c0, BH1750_I2C_ADDR, &cmd, 1, false);
    return res == 1;
}

static bool bh1750_read_raw(uint16_t *raw) {
    uint8_t buf[2];
    int res = i2c_read_blocking(i2c0, BH1750_I2C_ADDR, buf, 2, false);
    if (res != 2) return false;

    *raw = ((uint16_t)buf[0] << 8) | buf[1];
    return true;
}

bool bh1750_init_simple(void) {
    // Power on
    if (!bh1750_write(BH1750_POWER_ON)) {
        return false;
    }
    sleep_ms(10);

    // Reset (após power on)
    if (!bh1750_write(BH1750_RESET)) {
        // alguns módulos ignoram RESET, então não falha aqui
    }
    sleep_ms(10);

    // Continuous high resolution mode (~120 ms por conversão)
    if (!bh1750_write(BH1750_CONT_HIRES)) {
        return false;
    }

    // Tempo para primeira conversão
    sleep_ms(180);

    return true;
}

bool bh1750_read_simple(float *lux) {
    if (!lux) return false;

    uint16_t raw = 0;
    if (!bh1750_read_raw(&raw)) {
        return false;
    }

    // Conversão: lux = raw / 1.2 (datasheet)
    *lux = (float)raw / 1.2f;
    return true;
}
