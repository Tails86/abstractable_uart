#include "example.h"
#include "uart.h"

const uartConfig_t uart_conf_battery = {1, encode_decode_simple};
const uartConfig_t uart_conf_gateway = {2, NULL};

void example(void)
{
    uart_init(P_UART_CONF_BATTERY, 9600);
    uart_init(P_UART_CONF_GATEWAY, 9600);
    
    char msg[] = "This is E102 Gamma";
    uart_transmit(P_UART_CONF_BATTERY, (uint8_t*)msg, sizeof(msg));
    
    char buffer[100] = {};
    uint32_t len = uart_receive(P_UART_CONF_BATTERY, (uint8_t*)buffer, sizeof(buffer) - 1);
    if (len > sizeof(buffer) - 1)
    {
        // Handle overflow
    }
}

void encode_decode_simple(uint8_t* pkt, uint32_t pkt_len, bool encode)
{
    if (encode)
    {
        for (; pkt_len > 0; --pkt_len, ++pkt)
        {
            *pkt = 100; // encode here
        }
    }
    else
    {
        for (; pkt_len > 0; --pkt_len, ++pkt)
        {
            *pkt = 200; // decode here
        }
    }
}

const uartConfig_t* get_uart_conf_battery(void)
{
    return &uart_conf_battery;
}

const uartConfig_t* get_uart_conf_gateway(void)
{
    return &uart_conf_gateway;
}
