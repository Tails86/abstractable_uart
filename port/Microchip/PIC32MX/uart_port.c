#include <plib.h>

#include "uart_port.h"
#include "uart_defs.h"
#include "assert.h"
#include "target_clock.h"

// Value to write to clr value to clear overrun
#define OVERRUN_CLEAR_VALUE 0x0002

//! This structure was built around structures found in plib.h
typedef struct pic32UartPeriphReg_s 
{
    volatile uint32_t reg;
    volatile uint32_t clr;
    volatile uint32_t set;
    volatile uint32_t inv;
} pic32UartPeriphReg_t;

//! This structure was built around structures found in plib.h
typedef struct pic32UartPeriph_s 
{
    struct pic32UartPeriphMode_s 
    {
        union 
        {
            volatile uint32_t reg;
            volatile struct 
            {
                uint32_t STSEL : 1;
                uint32_t PDSEL : 2;
                uint32_t BRGH : 1;
                uint32_t RXINV : 1;
                uint32_t ABAUD : 1;
                uint32_t LPBACK : 1;
                uint32_t WAKE : 1;
                uint32_t UEN : 2;
                uint32_t:1;
                uint32_t RTSMD : 1;
                uint32_t IREN : 1;
                uint32_t SIDL : 1;
                uint32_t:1;
                uint32_t ON : 1;
            } bits;
        };
        volatile uint32_t clr;
        volatile uint32_t set;
        volatile uint32_t inv;
    } mode;

    struct pic32UartPeriphSta_s 
    {
        union 
        {
            volatile uint32_t reg;
            volatile struct 
            {
                uint32_t URXDA : 1;
                uint32_t OERR : 1;
                uint32_t FERR : 1;
                uint32_t PERR : 1;
                uint32_t RIDLE : 1;
                uint32_t ADDEN : 1;
                uint32_t URXISEL : 2;
                uint32_t TRMT : 1;
                uint32_t UTXBF : 1;
                uint32_t UTXEN : 1;
                uint32_t UTXBRK : 1;
                uint32_t URXEN : 1;
                uint32_t UTXINV : 1;
                uint32_t UTXISEL : 2;
                uint32_t ADDR : 8;
                uint32_t ADM_EN : 1;
            } bits;
        };
        volatile uint32_t clr;
        volatile uint32_t set;
        volatile uint32_t inv;
    } sta;

    volatile pic32UartPeriphReg_t txreg;
    volatile pic32UartPeriphReg_t rxreg;
    volatile pic32UartPeriphReg_t brg;
} pic32UartPeriph_t;

//! Peripheral lookup table for (channel - 1) to UART peripheral definition
pic32UartPeriph_t * const uart_peripherals[UART_MAX_CHANNEL] =
{
    (pic32UartPeriph_t *) _UART1_BASE_ADDRESS,
    (pic32UartPeriph_t *) _UART2_BASE_ADDRESS,
#ifdef _UART3_BASE_ADDRESS
    (pic32UartPeriph_t *) _UART3_BASE_ADDRESS,
#endif
#ifdef _UART4_BASE_ADDRESS
    (pic32UartPeriph_t *) _UART4_BASE_ADDRESS,
#endif
#ifdef _UART5_BASE_ADDRESS
    (pic32UartPeriph_t *) _UART5_BASE_ADDRESS,
#endif
#ifdef _UART6_BASE_ADDRESS
    (pic32UartPeriph_t *) _UART6_BASE_ADDRESS,
#endif
};

void uart_port_init(uint8_t channel, uint32_t baud) 
{
    struct pic32UartPeriphMode_s mode;
    mode.reg = 0;
    struct pic32UartPeriphSta_s sta;
    sta.reg = 0;
    // Ensure that channel is valid
    assert(channel > 0);
    assert(channel <= UART_MAX_CHANNEL);
    // Get a pointer to the UART
    pic32UartPeriph_t *p_uart = uart_peripherals[channel - 1];
    // Initialize regs
    p_uart->mode.reg = 0;
    p_uart->sta.reg = 0;
    // Calculate and set baud rate
    p_uart->brg.reg = ((PERIPH_CLK_FREQ / baud + 8) / 16UL) - 1UL;
    // Peripheral on
    mode.bits.ON = 1;
    p_uart->mode.reg = mode.reg;
    // Finally enable TX and RX
    sta.bits.UTXEN = 1;
    sta.bits.URXEN = 1;
    p_uart->sta.reg = sta.reg;
}

void uart_port_transmit(uint8_t channel, const uint8_t *pkt, uint32_t pkt_len) 
{
    // Ensure that channel is valid
    assert(channel > 0);
    assert(channel <= UART_MAX_CHANNEL);
    // Get a pointer to the UART
    pic32UartPeriph_t *p_uart = uart_peripherals[channel - 1];
    // Put one character at a time
    for (; pkt_len > 0; --pkt_len, ++pkt) 
    {
        // Wait for last byte to send
        // Note: This is for simplicity's sake; in production code, I'd make this interrupt based
        while (p_uart->sta.bits.UTXBF); // intentionally empty body
        // Put next character TX reg
        p_uart->txreg.reg = *pkt;
    }
}

void uart_port_receive(uint8_t channel, uint8_t* p_buffer, uint32_t buffer_len, uint8_t* p_rcv_errs) 
{
    // Ensure that channel is valid
    assert(channel > 0);
    assert(channel <= UART_MAX_CHANNEL);
    // Get a pointer to the UART
    pic32UartPeriph_t *p_uart = uart_peripherals[channel - 1];
    // Read one character at a time
    // NOTE: A timeout would be recommended for production code. We could otherwise get stuck here.
    for (; buffer_len > 0; --buffer_len) 
    {
        // Wait for data to be available
        // Note: This is for simplicity's sake; in production code, I'd make this interrupt based
        while (!p_uart->sta.bits.URXDA) 
        {
            // Correct for overrun
            if (p_uart->sta.bits.OERR) 
            {
                // Handle it; in this case, just clear the error
                // NOTE: This is obviously not the right thing to do for production code. I'd want
                // to kill this receive and flush the receive buffer before another receive is
                // expected.
                p_uart->sta.clr = OVERRUN_CLEAR_VALUE;
            }
        }
        if (p_uart->sta.bits.PERR)
        {
            // Parity error for the current byte
            if (p_rcv_errs != NULL)
            {
                *p_rcv_errs |= UART_ERR_MASK_PARITY;
            }
        }
        if (p_uart->sta.bits.FERR)
        {
            // Framing error for the current byte
            if (p_rcv_errs != NULL)
            {
                *p_rcv_errs |= UART_ERR_MASK_FRAMING;
            }
        }
        if (p_buffer != NULL) 
        {
            *p_buffer++ = p_uart->rxreg.reg;
        }
    }
}
