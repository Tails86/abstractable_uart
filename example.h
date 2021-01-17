#ifndef __EXAMPLE_H__
#define __EXAMPLE_H__

#include "uart.h"

void example(void);
void encode_decode_simple(uint8_t* pkt, uint32_t pkt_len, bool encode);
const uartConfig_t* get_uart_conf_battery(void);
const uartConfig_t* get_uart_conf_gateway(void);

#define P_UART_CONF_BATTERY get_uart_conf_battery()
#define P_UART_CONF_GATEWAY get_uart_conf_gateway()

#endif // __EXAMPLE_H__
