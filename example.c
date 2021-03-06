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
    uint8_t rcv_errs = 0;
    uint32_t len = uart_receive(P_UART_CONF_BATTERY, (uint8_t*)buffer, sizeof(buffer) - 1, &rcv_errs);
    if (rcv_errs != 0)
    {
        // Handle receive errors
    }
    else if (len > sizeof(buffer) - 1)
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
            *pkt += 26;
        }
    }
    else
    {
        for (; pkt_len > 0; --pkt_len, ++pkt)
        {
            *pkt -= 26;
        }
    }
}
