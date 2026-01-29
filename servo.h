#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>
#include <stdbool.h>
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware_config.h"   // deve definir SERVO_GPIO

// Inicializa PWM a 50 Hz para o servo
void servo_init(void);

// Define pulso em microssegundos (por exemplo 1000–2000 us)
void servo_write_us(uint16_t pulse_us);

// Atalhos úteis para SG90 contínuo
void servo_stop(void);      // ~1500 us
void servo_open(void);      // gira no sentido de abrir
void servo_close(void);     // gira no sentido de fechar

#endif
