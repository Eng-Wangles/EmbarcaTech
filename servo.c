#include "servo.h"
#include "pico/stdlib.h"
#include <stdint.h>

static unsigned int slice_num;

// 50 Hz => período de 20.000 us.
// Usando clock padrão 125 MHz e wrap de 24999, cada tick = 1 us.
void servo_init(void) {
    gpio_set_function(SERVO_GPIO, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(SERVO_GPIO);

    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv(&cfg, 125.0f);   // 125 MHz / 125 = 1 MHz (1 us por tick)
    pwm_config_set_wrap(&cfg, 20000 - 1);  // 20.000 ticks => 20 ms
    pwm_init(slice_num, &cfg, true);

    servo_stop();  // inicia parado
}

void servo_write_us(uint16_t pulse_us) {
    if (pulse_us < 500)  pulse_us = 500;
    if (pulse_us > 2500) pulse_us = 2500;
    pwm_set_gpio_level(SERVO_GPIO, pulse_us);
}

void servo_stop(void) {
    servo_write_us(1500);   // neutro: tenta parar o contínuo
}

void servo_open(void) {
    servo_write_us(2000);   // sentido 1 (ajuste fino depois)
}

void servo_close(void) {
    servo_write_us(1000);   // sentido 2
}
