#include <plib.h>

#include "uart_port.h"
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
    struct pic32_periph_uart_mode_s 
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

    struct pic32_periph_uart_sta_s 
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
    struct pic32_periph_uart_mode_s mode;
    mode.reg = 0;
    struct pic32_periph_uart_sta_s sta;
    sta.reg = 0;
    // Ensure that channel is valid
    assert(channel > 0);
    assert(channel <= UART_MAX_CHANNEL);
    // Get a pointer to the UART
    pic32UartPeriph_t *uart_ptr = uart_peripherals[channel - 1];
    // Initialize regs
    uart_ptr->mode.reg = 0;
    uart_ptr->sta.reg = 0;
    // Calculate and set baud rate
    uart_ptr->brg.reg = ((PERIPH_CLK_FREQ / baud + 8) / 16UL) - 1UL;
    // Peripheral on
    mode.bits.ON = 1;
    uart_ptr->mode.reg = mode.reg;
    // Finally enable TX and RX
    sta.bits.UTXEN = 1;
    sta.bits.URXEN = 1;
    uart_ptr->sta.reg = sta.reg;
}

void uart_port_transmit(uint8_t channel, const uint8_t *pkt, uint32_t pkt_len) 
{
    // Ensure that channel is valid
    assert(channel > 0);
    assert(channel <= UART_MAX_CHANNEL);
    // Get a pointer to the UART
    pic32UartPeriph_t *uart_ptr = uart_peripherals[channel - 1];
    // Put one character at a time
    for (; pkt_len > 0; --pkt_len, ++pkt) 
    {
        // Wait for last byte to send
        while (uart_ptr->sta.bits.UTXBF); // intentionally empty body
        // Put next character TX reg
        uart_ptr->txreg.reg = *pkt;
    }
}

void uart_port_receive(uint8_t channel, uint8_t* p_buffer, uint32_t buffer_len) 
{
    // Ensure that channel is valid
    assert(channel > 0);
    assert(channel <= UART_MAX_CHANNEL);
    // Get a pointer to the UART
    pic32UartPeriph_t *uart_ptr = uart_peripherals[channel - 1];
    // Read one character at a time
    for (; buffer_len > 0; --buffer_len) 
    {
        // Wait for data to be available
        while (!uart_ptr->sta.bits.URXDA) 
        {
            // Correct for overrun
            if (uart_ptr->sta.bits.OERR) 
            {
                // Handle it; in this case, just clear the error
                uart_ptr->sta.clr = OVERRUN_CLEAR_VALUE;
            }
        }
        if (p_buffer != NULL) 
        {
            *p_buffer++ = uart_ptr->rxreg.reg;
        }
    }
}
