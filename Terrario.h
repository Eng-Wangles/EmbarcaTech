// Terrario.h - Estruturas e handles compartilhados
#ifndef TERRARIO_H
#define TERRARIO_H

#include "FreeRTOS.h"
#include "queue.h"

// Dados dos sensores
typedef struct {
    float temp_c;
    float hum_rh;
    float lux;
    uint16_t color_r, color_g, color_b;
    const char *color_name;
    bool alarme;
} SensorData_t;

// Comando web para atuadores
typedef enum {
    CMD_SERVO_OPEN,
    CMD_SERVO_CLOSE,
    CMD_NONE
} WebCommand_t;

// Handles das queues (definidos em Terrario.c)
extern QueueHandle_t queue_sensor_data;
extern QueueHandle_t queue_web_command;

// Protótipos das tasks
void vTaskSensores(void *pvParameters);
void vTaskAtuadores(void *pvParameters);
void vTaskWeb(void *pvParameters);

#endif
