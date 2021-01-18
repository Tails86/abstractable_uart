#include <stdio.h>
#include "uart.h"
#include "uart_port.h"

void uart_init(const uartConfig_t* p_config, uint32_t baud)
{
    // Initialize this UART through the defined port function
    uart_port_init(p_config->channel, baud);
}

void uart_transmit(const uartConfig_t* p_config, uint8_t* p_pkt, uint32_t pkt_len)
{
    // If encode function is defined for this config, encode the data
    if (p_config->p_encode_decode != NULL)
    {
        p_config->p_encode_decode(p_pkt, pkt_len, true);
    }
    // Transmit packet length
    // Note: endianness is assumed to be the same from transmitter to receiver
    uart_port_transmit(p_config->channel, (uint8_t*)&pkt_len, sizeof(pkt_len));
    // Transmit data over ported function
    uart_port_transmit(p_config->channel, p_pkt, pkt_len);
}

uint32_t uart_receive(const uartConfig_t* p_config, uint8_t* p_buffer, uint32_t buffer_len, uint8_t* p_rcv_errs)
{
    // Reset error flags if it has been given
    if (p_rcv_errs != NULL)
    {
        *p_rcv_errs = 0;
    }
    // Receive the length of the packet first (blocking receive)
    // Note: endianness is assumed to be the same from transmitter to receiver
    uint32_t pkt_len = 0;
    uart_port_receive(p_config->channel, (uint8_t*)&pkt_len, sizeof(pkt_len), p_rcv_errs);
    // Receive data (blocking receive)
    uint32_t rcv_len = pkt_len > buffer_len ? buffer_len : pkt_len;
    uart_port_receive(p_config->channel, p_buffer, rcv_len, p_rcv_errs);
    // Dump extra bytes if buffer overflow detected
    if (pkt_len > rcv_len)
    {
        uart_port_receive(p_config->channel, NULL, pkt_len - rcv_len, p_rcv_errs);
    }
    // If decode function is defined for this config, decode the data
    if (p_config->p_encode_decode != NULL)
    {
        p_config->p_encode_decode(p_buffer, rcv_len, false);
    }
    return pkt_len;
}