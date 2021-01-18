#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>
#include <stdbool.h>
#include "uart_port.h"

//! Encode function pointer typedef
//! @param[in,out] pkt  The packet to encode or decode (written in place)
//! @param[in] pkt_len  The length of pkt
//! @param[in] encode  Set to true to encode or false to decode
typedef void (*fnEncodeDecode_t)(uint8_t* pkt, uint32_t pkt_len, bool encode);
//! Structure definition of UART configuration
typedef struct uartConfig_s
{
    //! The hardware channel of this UART
    uint8_t channel;
    //! Pointer to the appropriate encode/decode function or NULL if no encoding is necessary.
    fnEncodeDecode_t p_encode_decode;
} uartConfig_t;

//! Initialize a UART configuration
//! @param[in] p_config  Pointer to the UART configuration
//! @param[in] baud  The desired BAUD rate for this connection
void uart_init(const uartConfig_t* p_config, uint32_t baud);

//! Transmit a packet over a UART (blocking)
//! @param[in] p_config  Pointer to the UART configuration
//! @param[in,out] pkt  The package data to tranmit (data will be encoded in place)
//! @param[in] pkt_len  The number of elements in pkt
void uart_transmit(const uartConfig_t* p_config, uint8_t* p_pkt, uint32_t pkt_len);

//! Receive a packet from a UART (blocking)
//! @param[in] p_config  Pointer to the UART configuration
//! @param[out] buffer  Where the received data is to be written
//! @param[in] pkt_len  The maximum number of bytes in buffer
//! @param[out] p_rcv_errs  Outputs the receive errors (0 if ok, see uart_defs otherwise)
//! @returns the number of bytes in the packet which may exceed the value of buffer_len
uint32_t uart_receive(const uartConfig_t* p_config, uint8_t* p_buffer, uint32_t buffer_len, uint8_t* p_rcv_errs);

#endif // __UART_H__
