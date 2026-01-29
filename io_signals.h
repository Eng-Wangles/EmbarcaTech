#ifndef IO_SIGNALS_H
#define IO_SIGNALS_H

#include <stdbool.h>
#include <stdint.h>

// Inicializa LEDs e buzzer (GPIO + PWM do buzzer)
void io_init(void);

// Controle simples do LED RGB discreto
void led_set_rgb(bool r, bool g, bool b);

// Piscar LED RGB N vezes (bloqueante, para testes)
void led_blink_rgb(bool r, bool g, bool b,
                   uint32_t on_ms, uint32_t off_ms,
                   uint32_t count);

// Beep simples no buzzer (bloqueante)
void buzzer_beep(uint32_t freq_hz, uint32_t duration_ms);

#endif // IO_SIGNALS_H
