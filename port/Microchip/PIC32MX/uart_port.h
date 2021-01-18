#ifndef __UART_PORT_H__
#define __UART_PORT_H__

#include <stdint.h>
#include <stdbool.h>
#include <plib.h>

// Select the right number of UART channels based on hardware
#ifdef _UART6_BASE_ADDRESS
    #define UART_MAX_CHANNEL 6
#else
    #ifdef _UART5_BASE_ADDRESS
        #define UART_MAX_CHANNEL 5
    #else
        #ifdef _UART4_BASE_ADDRESS
            #define UART_MAX_CHANNEL 4
        #else
            #ifdef _UART3_BASE_ADDRESS
                #define UART_MAX_CHANNEL 3
            #else
                #define UART_MAX_CHANNEL 2
            #endif
        #endif
    #endif
#endif

//! Initialize UART for this hardware port
//! @param[in] channel  The UART channel number
//! @param[in] baud  The BAUD rate to communicate with
void uart_port_init(uint8_t channel, uint32_t baud);

//! Transmit raw bytes over UART for this hardware port
//! @param[in] channel  The UART channel number
//! @param[in] pkt  The data to transmit
//! @param[in] pkt_len  The number of bytes to transmit
void uart_port_transmit(uint8_t channel, const uint8_t *pkt, uint32_t pkt_len);

//! Receive raw bytes from UART for this hardware port (blocking)
//! @param[in] channel  The UART channel number
//! @param[out] p_buffer  Where to write data to or NULL to dump data
//! @param[in] buffer_len  The number of bytes to receive
//! @param[out] p_rcv_errs  Outputs the receive errors (0 if ok, see uart_defs otherwise)
void uart_port_receive(uint8_t channel, uint8_t* p_buffer, uint32_t buffer_len, uint8_t* p_rcv_errs);

#endif // __UART_PORT_H__
