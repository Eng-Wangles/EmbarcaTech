#include <stdint.h>
#include <stdbool.h>

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware_config.h"
#include "io_signals.h"

static uint32_t pwm_slice_buzzer;

// ==== Inicialização ====

void io_init(void)
{
    // LED RGB: saídas digitais
    gpio_init(LED_R_GPIO);
    gpio_set_dir(LED_R_GPIO, GPIO_OUT);
    gpio_put(LED_R_GPIO, 0);

    gpio_init(LED_G_GPIO);
    gpio_set_dir(LED_G_GPIO, GPIO_OUT);
    gpio_put(LED_G_GPIO, 0);

    gpio_init(LED_B_GPIO);
    gpio_set_dir(LED_B_GPIO, GPIO_OUT);
    gpio_put(LED_B_GPIO, 0);

    // Buzzer em PWM
    gpio_set_function(BUZZER_GPIO, GPIO_FUNC_PWM);
    pwm_slice_buzzer = pwm_gpio_to_slice_num(BUZZER_GPIO);

    pwm_config cfg = pwm_get_default_config();
    // Deixa o TOP padrão (65535); frequência será ajustada por set_clkdiv
    pwm_init(pwm_slice_buzzer, &cfg, false);
}

// ==== LED RGB ====

void led_set_rgb(bool r, bool g, bool b)
{
    gpio_put(LED_R_GPIO, r ? 1 : 0);
    gpio_put(LED_G_GPIO, g ? 1 : 0);
    gpio_put(LED_B_GPIO, b ? 1 : 0);
}

void led_blink_rgb(bool r, bool g, bool b,
                   uint32_t on_ms, uint32_t off_ms,
                   uint32_t count)
{
    for (uint32_t i = 0; i < count; i++) {
        led_set_rgb(r, g, b);
        sleep_ms(on_ms);
        led_set_rgb(false, false, false);
        sleep_ms(off_ms);
    }
}

// ==== Buzzer ====

void buzzer_beep(uint32_t freq_hz, uint32_t duration_ms)
{
    if (freq_hz == 0) {
        pwm_set_enabled(pwm_slice_buzzer, false);
        return;
    }

    // F = clock / (wrap + 1) / div
    // clock padrão: 125 MHz
    const uint32_t sys_clk = 125000000u;
    uint32_t wrap = 1000; // resolução razoável
    float div = (float)sys_clk / ((wrap + 1u) * freq_hz);

    pwm_set_wrap(pwm_slice_buzzer, wrap);
    pwm_set_clkdiv(pwm_slice_buzzer, div);

    // duty ~50%
    pwm_set_gpio_level(BUZZER_GPIO, wrap / 2);
    pwm_set_enabled(pwm_slice_buzzer, true);

    sleep_ms(duration_ms);

    pwm_set_enabled(pwm_slice_buzzer, false);
}
