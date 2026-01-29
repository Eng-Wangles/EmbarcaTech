#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

// ==== I2C0 – Sensores ambientais (conector I2C0) ====
#define I2C0_SDA_GPIO          0     // GPIO0 - SDA I2C0
#define I2C0_SCL_GPIO          1     // GPIO1 - SCL I2C0

// Endereços I2C típicos dos sensores (podem ser ajustados conforme módulo)
#define AHT10_I2C_ADDR         0x38
#define BH1750_I2C_ADDR        0x23
#define TCS34725_I2C_ADDR      0x29

// ==== Servo SG90 contínuo (conector IDC) ====
#define SERVO_GPIO             18    // GPIO18 - sinal PWM via IDC
#define SERVO_PWM_SLICE        3     // slice correspondente ao GPIO18 no RP2040
#define SERVO_PWM_CHANNEL      PWM_CH_A

// ==== Buzzer de sinalização ====
#define BUZZER_GPIO            21    // Buzzer A onboard (via transistor)

// ==== LED RGB discreto onboard ====
#define LED_R_GPIO             13
#define LED_G_GPIO             11
#define LED_B_GPIO             12

// ==== Matriz WS2812 5x5 (opcional) ====
#define NEOPIXEL_GPIO          7
#define NEOPIXEL_COUNT         25

// ==== Botões ====
#define BUTTON_A_GPIO          5
#define BUTTON_B_GPIO          6

typedef struct {
    float temp_open;      // acima disso abre
    float temp_close;     // abaixo disso fecha
    float hum_open;       // acima disso abre (se quiser usar umidade)
    float hum_close;      // abaixo disso fecha
} terrario_limits_t;


#endif // HARDWARE_CONFIG_H
