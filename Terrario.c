#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "pico/cyw43_arch.h"
#include "hardware_config.h"
#include "io_signals.h"
#include "ssd1306.h"
#include "aht10.h"
#include "bh1750.h"
#include "servo.h"
#include "web_server.h"
#include "tcs34725.h"
#include "Terrario.h"

// Credenciais WiFi
#define WIFI_SSID     "ITSelf"
#define WIFI_PASSWORD "code2020"

// ==========================================
// VARIÁVEIS GLOBAIS (Compartilhadas)
// ==========================================
float g_temp_c = 0.0f;
float g_hum_rh = 0.0f;
float g_lux = 0.0f;
bool g_servo_aberto = false;
bool g_alarme = false;
uint16_t g_color_r = 0;
uint16_t g_color_g = 0;
uint16_t g_color_b = 0;
const char *g_color_name = "aguardando...";

// Handles das Queues
QueueHandle_t queue_sensor_data = NULL;
QueueHandle_t queue_web_command = NULL;

// Limites
const struct {
    float temp_open, temp_close;
    float hum_open, hum_close;
    } limits = { .temp_open=27.0f,
                 .temp_close=26.0f,
                 .hum_open=80.0f,
                 .hum_close=70.0f };

// ==========================================
// TASK SENSORES (Loop de leitura)
// ==========================================
void vTaskSensores(void *pvParameters) {
    SensorData_t data;
    while (1) {
        // Leitura AHT10
        if (!aht10_read_simple(&data.temp_c, &data.hum_rh)) {
            data.temp_c = 0; data.hum_rh = 0;
        }

        // Leitura BH1750
        float temp_lux = 0.0f;
        bh1750_read_simple(&temp_lux);
        data.lux = temp_lux;

        // Leitura TCS34725
        uint16_t c;
        if (tcs34725_read_rgbc(&c, &data.color_r, &data.color_g, &data.color_b)) {
            if (data.color_r > data.color_g && data.color_r > data.color_b) 
                data.color_name = "A cor eh Vermelho";
            else if (data.color_g > data.color_r && data.color_g > data.color_b) 
                data.color_name = "A cor eh Verde   ";
            else if (data.color_b > data.color_r && data.color_b > data.color_g) 
                data.color_name = "A cor eh Azul    ";
            else data.color_name = "Normal";
        } else {
            data.color_name = "Erro Cor";
        }
        xQueueOverwrite(queue_sensor_data, &data);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// ==========================================
// TASK ATUADORES (OLED, Servo, Lógica)
// ==========================================
void vTaskAtuadores(void *pvParameters) {
    SensorData_t data;
    char buf[32];
    bool servo_estado_aberto = false;

    while (1) {
        if (xQueueReceive(queue_sensor_data, &data, portMAX_DELAY)) {
            // Atualiza Globais (Web)
            g_temp_c = data.temp_c;
            g_hum_rh = data.hum_rh; 
            g_lux = data.lux;
            g_color_r = data.color_r; 
            g_color_g = data.color_g; 
            g_color_b = data.color_b;
            g_color_name = data.color_name;
            g_servo_aberto = servo_estado_aberto;

            // Atualiza OLED (layout original mantido)
            ssd1306_clear();
            ssd1306_draw_string(0, 0,  "Terrario IoT");
            
            snprintf(buf, sizeof(buf), "Temperatura : %.1f C", data.temp_c);
            ssd1306_draw_string(0, 16, buf);
            
            snprintf(buf, sizeof(buf), "Umidade     : %.1f %%", data.hum_rh);
            ssd1306_draw_string(0, 24, buf);

            snprintf(buf, sizeof(buf), "Luminosidade: %.0f lux", data.lux);
            ssd1306_draw_string(0, 32, buf);
            
            if(data.color_name) ssd1306_draw_string(0, 48, (char*)data.color_name);

            // Lógica Servo
            bool deve_abrir = !servo_estado_aberto && (data.temp_c > limits.temp_open || data.hum_rh > limits.hum_open);
            bool deve_fechar = servo_estado_aberto && (data.temp_c < limits.temp_close && data.hum_rh < limits.hum_close);

            if (deve_abrir) {
                servo_open();
                vTaskDelay(pdMS_TO_TICKS(1500));
                servo_stop();
                servo_estado_aberto = true;
                g_alarme = true;
            } else if (deve_fechar) {
                servo_close();
                vTaskDelay(pdMS_TO_TICKS(1500));
                servo_stop();
                servo_estado_aberto = false;
                g_alarme = false;
            }
            g_servo_aberto = servo_estado_aberto;

            // Feedback
            if (g_alarme) {
                led_blink_rgb(true, false, false, 100, 100, 1);
                buzzer_beep(500, 50);
            } else {
                led_blink_rgb(false, true, false, 50, 2000, 1);
            }
            ssd1306_show();
        }
    }
}

// ==========================================
// TASK PRINCIPAL (Setup + Spawner)
// ==========================================
void terrario_task_init(void *pvParameters) {
    // 1. Inicializa IO e Drivers
    io_init();
    
    // OLED
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(14, GPIO_FUNC_I2C); 
    gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14); gpio_pull_up(15);
    ssd1306_init(i2c1);
    ssd1306_clear();
    ssd1306_draw_string(0, 0, "Terrario IoT");
    ssd1306_draw_string(0, 8, "Iniciando...");
    ssd1306_show();
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Sensores
    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(0, GPIO_FUNC_I2C); 
    gpio_set_function(1, GPIO_FUNC_I2C);
    gpio_pull_up(0); gpio_pull_up(1);
    servo_init(); servo_stop();

    // --- TESTE DE SENSORES (OLED) ---
    ssd1306_clear();
    ssd1306_draw_string(0, 0, "Check Sensores:");
    
    if (!aht10_init_simple()) ssd1306_draw_string(0, 16, "AHT10: OK");
    else ssd1306_draw_string(0, 16, "AHT10: ERRO");
    ssd1306_show(); vTaskDelay(pdMS_TO_TICKS(500));

    if (bh1750_init_simple()) ssd1306_draw_string(0, 24, "BH1750: OK");
    else ssd1306_draw_string(0, 24, "BH1750: ERRO");
    ssd1306_show(); vTaskDelay(pdMS_TO_TICKS(500));

    if (tcs34725_init_simple()) ssd1306_draw_string(0, 32, "TCS34725: OK");
    else ssd1306_draw_string(0, 32, "TCS34725: ERRO");
    ssd1306_show(); vTaskDelay(pdMS_TO_TICKS(1000));

    led_blink_rgb(true, false, false, 100, 100, 10); // 10 piscadas vermelhas (check)

    // --- CONEXÃO WIFI ---
    ssd1306_clear();
    ssd1306_draw_string(0, 0, "Wi-Fi:");
    ssd1306_draw_string(0, 16, "Conectando...");
    ssd1306_show();

    if (cyw43_arch_init()) {
        printf("WiFi init failed\n");
        vTaskDelete(NULL);
    }
    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        ssd1306_draw_string(0, 24, "Falha!");
        ssd1306_show();
        vTaskDelete(NULL);
    } else {
        ssd1306_draw_string(0, 24, "Conectado!");
        char ip_str[24];
        uint8_t *ip = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
        snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
        ssd1306_draw_string(0, 32, ip_str);
        ssd1306_show();
        
        led_blink_rgb(false, true, false, 100, 100, 10); // 10 piscadas verdes (OK)
        buzzer_beep(1000, 100);
        
        // Inicia Servidor Web
        web_server_init();
    }
    
    vTaskDelay(pdMS_TO_TICKS(3000)); // Deixa ver o IP por 3s

    // --- CRIA QUEUES E TASKS ---
    queue_sensor_data = xQueueCreate(1, sizeof(SensorData_t));
    queue_web_command = xQueueCreate(5, sizeof(WebCommand_t));

    if (queue_sensor_data && queue_web_command) {
        xTaskCreate(vTaskSensores,  "Sensores",  2048, NULL, 3, NULL);
        xTaskCreate(vTaskAtuadores, "Atuadores", 2048, NULL, 2, NULL);
        // vTaskWeb não precisa mais, o web_server já rodou no init
    }
    
    // Missão cumprida, deleta task de init
    vTaskDelete(NULL);
}

int main(void) {
    stdio_init_all();
    xTaskCreate(terrario_task_init, "Init", 4096, NULL, 1, NULL); // Stack maior para init
    vTaskStartScheduler();
    while (true);
}
