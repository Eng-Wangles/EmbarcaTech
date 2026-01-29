# EmbarcaTech
O Projeto Final tem como objetivo aplicar os conhecimentos adquiridos na residência para desenvolver um sistema IoT completo.

***

# 🌿 Terrário IoT com RP2040 e FreeRTOS

Este projeto consiste em um sistema embarcado de monitoramento e controle para terrários, desenvolvido com o microcontrolador **RP2040 (Raspberry Pi Pico W)**. O sistema utiliza **FreeRTOS** para gerenciamento multitarefa, **lwIP** para servidor web e integra diversos sensores e atuadores.

  

## 📋 Funcionalidades

### 🔍 Monitoramento
- **Temperatura e Umidade (AHT10)**: Monitoramento ambiental preciso.
- **Luminosidade (BH1750)**: Medição da intensidade de luz em Lux.
- **Cor (TCS34725)**: Identificação da cor predominante (R/G/B).

### ⚙️ Atuação e Controle
- **Controle de Servo Motor**: Abertura/fechamento automático da tampa baseado em histerese (T > 27°C ou U > 80%).
- **Feedback Visual (OLED SSD1306)**: Exibição local de dados, IP e status.
- **Feedback Luminoso e Sonoro**: LED RGB e Buzzer para alertas e status de conexão.

### 🌐 Interface Web
- **Dashboard Responsivo**: Visualização em tempo real de todos os sensores.
- **Segurança**: Autenticação via **Basic Auth** (`admin` / `terrario123`).
- **Conectividade**: Servidor HTTP robusto usando lwIP Raw API.

***

## 🛠️ Arquitetura do Firmware

O projeto utiliza **FreeRTOS** para dividir a complexidade em tarefas independentes, comunicando-se via **Queues** thread-safe.

### Estrutura de Tasks

| Task | Prioridade | Descrição |
| :--- | :---: | :--- |
| **vTaskSensores** | 3 (Alta) | Lê AHT10, BH1750, TCS34725 a cada 1s e envia dados para a fila `queue_sensor_data`. |
| **vTaskAtuadores**| 2 (Média)| Consome dados da fila, atualiza o display OLED e executa a lógica de controle do Servo (abrir/fechar). |
| **vTaskWeb** | 1 (Baixa)| Mantém a conexão Wi-Fi ativa e gerencia o servidor HTTP em background. |

### Fluxo de Inicialização
1. Inicialização de Hardware (I2C, GPIO).
2. Teste visual de todos os sensores no OLED.
3. Conexão Wi-Fi com feedback visual.
4. Inicialização do Scheduler FreeRTOS.

***

## 🔌 Hardware e Pinout

O projeto foi desenvolvido na placa de desenvolvimento **BitDogLab** (ou similar com RP2040).

| Componente | Protocolo | Pinos (RP2040) |
| :--- | :--- | :--- |
| **AHT10 / BH1750 / TCS34725** | I2C0 | **SDA:** GP0 / **SCL:** GP1 |
| **Display OLED SSD1306** | I2C1 | **SDA:** GP14 / **SCL:** GP15 |
| **Servo Motor** | PWM | **Sinal:** GP12 |
| **LED RGB** | GPIO | **R:** GP13, **G:** GP11, **B:** GP10 |
| **Buzzer** | PWM | **Sinal:** GP21 |

***

## 🚀 Como Compilar e Rodar

### Pré-requisitos
- [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk) instalado e configurado.
- Compilador `arm-none-eabi-gcc`.
- CMake e Make/Ninja.
- VS Code (recomendado).

### Passos
1. **Clone o repositório:**
   ```bash
   git clone https://github.com/seu-usuario/terrario-iot.git
   cd terrario-iot
   ```

2. **Configure o Wi-Fi:**
   Edite o arquivo `Terrario.c` e insira suas credenciais:
   ```c
   #define WIFI_SSID     "SUA_REDE"
   #define WIFI_PASSWORD "SUA_SENHA"
   ```

3. **Compile:**
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

4. **Upload:**
   - Conecte o Pico W segurando o botão `BOOTSEL`.
   - Copie o arquivo `Terrario.uf2` gerado para a unidade USB montada.

***

## 📸 Interface Web

Acesse pelo navegador usando o IP exibido no OLED (ex: `http://192.168.1.155`).

**Credenciais Padrão:**
- **Usuário:** `admin`
- **Senha:** `terrario123`

***

## 📚 Tecnologias Utilizadas

- **Linguagem C** (Padrão C99)
- **FreeRTOS Kernel** (SMP para RP2040)
- **lwIP** (Lightweight IP Stack - Raw API)
- **Pico SDK** (Hardware APIs)

***

## 📝 Autor

Desenvolvido como projeto de referência para sistemas embarcados.
**Prof. Wangles** - *2026*

---
