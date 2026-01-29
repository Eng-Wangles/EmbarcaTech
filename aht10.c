#include "aht10.h"
#include "pico/stdlib.h"

static bool aht10_write(const uint8_t *data, size_t len) {
    int res = i2c_write_blocking(i2c0, AHT10_I2C_ADDRESS, data, len, false);
    return res == (int)len;
}

static bool aht10_read_bytes(uint8_t *data, size_t len) {
    int res = i2c_read_blocking(i2c0, AHT10_I2C_ADDRESS, data, len, false);
    return res == (int)len;
}

bool aht10_init_simple(void) {
    // Soft reset
    uint8_t reset_cmd = AHT10_CMD_RESET;
    if (!aht10_write(&reset_cmd, 1)) {
        return false;
    }
    sleep_ms(40);

    // Init: 0xE1 0x08 0x00
    uint8_t init_cmd[3] = { AHT10_CMD_INITIALIZE, 0x08, 0x00 };
    if (!aht10_write(init_cmd, 3)) {
        return false;
    }
    sleep_ms(20);

    // Lê 1 byte de status apenas para confirmar comunicação
    uint8_t status = 0;
    if (!aht10_read_bytes(&status, 1)) {
        return false;
    }

    // Não trava em bit de calibração; só garante que respondeu
    return true;
}

bool aht10_read_simple(float *temperature, float *humidity) {
    if (!temperature || !humidity) return false;

    // Comando de medição: 0xAC 0x33 0x00
    uint8_t meas_cmd[3] = { AHT10_CMD_MEASURE, 0x33, 0x00 };
    if (!aht10_write(meas_cmd, 3)) {
        return false;
    }

    sleep_ms(100);

    uint8_t raw[6];
    if (!aht10_read_bytes(raw, 6)) {
        return false;
    }

    // Se ainda estiver ocupado, falha
    if (raw[0] & 0x80) {
        return false;
    }

    uint32_t raw_hum  = ((uint32_t)raw[1] << 12) | ((uint32_t)raw[2] << 4) | (raw[3] >> 4);
    uint32_t raw_temp = (((uint32_t)(raw[3] & 0x0F) << 16) | ((uint32_t)raw[4] << 8) | raw[5]);

    *humidity    = (raw_hum  * 100.0f) / 1048576.0f;
    *temperature = (raw_temp * 200.0f) / 1048576.0f - 50.0f;

    return true;
}
