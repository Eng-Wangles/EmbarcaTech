#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include "web_server.h"

// Variáveis globais definidas em Terrario.c
extern float g_temp_c;
extern float g_hum_rh;
extern float g_lux;
extern bool  g_servo_aberto;
extern bool  g_alarme;
extern uint16_t g_color_r;
extern uint16_t g_color_g;
extern uint16_t g_color_b;
extern const char *g_color_name;

// Buffer da resposta HTTP
static char http_response[512];

// Envia página principal
static err_t http_server_send(struct tcp_pcb *tpcb) {
    int len = snprintf(http_response, sizeof(http_response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<html><head><title>Terrario IoT</title>"
        "<meta http-equiv='refresh' content='5' />"
        "</head><body>"
        "<h1>Terrario IoT</h1>"
        "<p>Temperatura: %.1f &deg;C</p>"
        "<p>Umidade: %.1f %%</p>"
        "<p>Luminosidade: %.0f lx</p>"
        "<p>Cor: R=%u G=%u B=%u</p>"
        "<p>%s</p>"
        "<p>Servo: %s</p>"
        "<p>Alarme: %s</p>"
        "</body></html>",
        g_temp_c, g_hum_rh, g_lux,
        g_color_r, g_color_g, g_color_b,
        g_color_name,
        g_servo_aberto ? "aberto" : "fechado",
        g_alarme ? "ATIVO" : "normal"
    );

    if (len < 0) return ERR_OK;
    if (len > (int)sizeof(http_response)) len = sizeof(http_response);

    err_t err = tcp_write(tpcb, http_response, len, TCP_WRITE_FLAG_COPY);
    if (err == ERR_OK) {
        tcp_output(tpcb);
    }
    return err;
}

// ----------------- Autenticação Basic -----------------

static err_t http_server_recv(void *arg, struct tcp_pcb *tpcb,
                              struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    // Copia o payload para análise simples
    char *req = (char *)p->payload;

    bool authorized = false;

    // Procura header Authorization: Basic ...
    // Credenciais: "admin:terrario123" -> Base64: YWRtaW46dGVycmFyaW8xMjM=
    if (strstr(req, "Authorization: Basic YWRtaW46dGVycmFyaW8xMjM=")) {
        authorized = true;
    }

    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);

    if (!authorized) {
        const char *unauth =
            "HTTP/1.1 401 Unauthorized\r\n"
            "WWW-Authenticate: Basic realm=\"Terrario\"\r\n"
            "Connection: close\r\n"
            "\r\n";
        tcp_write(tpcb, unauth, strlen(unauth), TCP_WRITE_FLAG_COPY);
        tcp_close(tpcb);
        return ERR_OK;
    }

    // Autorizado → envia página
    http_server_send(tpcb);
    tcp_close(tpcb);
    return ERR_OK;
}

// ------------------------------------------------------

static err_t http_server_accept(void *arg, struct tcp_pcb *new_pcb, err_t err) {
    tcp_recv(new_pcb, http_server_recv);
    return ERR_OK;
}

void web_server_init(void) {
    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (!pcb) return;

    if (tcp_bind(pcb, IP_ANY_TYPE, 80) != ERR_OK) {
        tcp_close(pcb);
        return;
    }

    pcb = tcp_listen(pcb);
    tcp_accept(pcb, http_server_accept);
}
