#ifndef __EXAMPLE_H__
#define __EXAMPLE_H__

#include "uart.h"

extern const uartConfig_t uart_conf_battery;
extern const uartConfig_t uart_conf_gateway;

void example(void);
void encode_decode_simple(uint8_t* pkt, uint32_t pkt_len, bool encode);

#define P_UART_CONF_BATTERY &uart_conf_battery
#define P_UART_CONF_GATEWAY &uart_conf_gateway

#endif // __EXAMPLE_H__
