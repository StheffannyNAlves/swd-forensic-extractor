#include <stdint.h>
#include <stdbool.h>
#include <stddef.h> 

#define CLOCKS_BASE   0x40008000
#define RESETS_BASE   0x4000c000
#define IO_BANK0_BASE 0x40014000
#define XOSC_BASE     0x40024000
#define UART0_BASE    0x40034000
#define SIO_BASE      0xd0000000 

#define XOSC_CTRL     *(volatile uint32_t *) (XOSC_BASE + 0x00)
#define XOSC_STATUS   *(volatile uint32_t *) (XOSC_BASE + 0x04)
#define XOSC_STARTUP  *(volatile uint32_t *) (XOSC_BASE + 0x0C)

// valores mágicos / senhas
#define XOSC_ENABLE   0xFABAA0
#define CMD_ENABLE    0xB
#define STABLE_BIT    (1 << 31)

#define CLK_REF_CTRL  *(volatile uint32_t *) (CLOCKS_BASE + 0x30)
#define CLK_SYS_CTRL  *(volatile uint32_t *) (CLOCKS_BASE + 0x3C)
#define CLK_PERI_CTRL *(volatile uint32_t *) (CLOCKS_BASE + 0x48)
#define SRC_XOSC      2 
#define SRC_AUX       0

#define LED 25

#define RESETS_RESET      *(volatile uint32_t *) (RESETS_BASE + 0x00)
#define RESETS_RESET_DONE *(volatile uint32_t *) (RESETS_BASE + 0x08)

// máscaras
#define RST_UART0      (1 << 22) 
#define RST_IO_BANK0   (1 << 5)
#define RST_PADS_BANK0 (1 << 8)

#define GPIO0_CTRL    *(volatile uint32_t *) (IO_BANK0_BASE + 0x004) // TX
#define GPIO1_CTRL    *(volatile uint32_t *) (IO_BANK0_BASE + 0x00c) // RX
#define GPIO25_CTRL   *(volatile uint32_t *) (IO_BANK0_BASE + 0x0CC) // LED
#define FUNC_UART     2
#define FUNC_SIO      5

#define GPIO_OUT_XOR  *(volatile uint32_t *) (SIO_BASE + 0x01C)
#define GPIO_OE_SET   *(volatile uint32_t *) (SIO_BASE + 0x024)

#define UART0_DR      *(volatile uint32_t *) (UART0_BASE + 0x000)
#define UART0_FR      *(volatile uint32_t *) (UART0_BASE + 0x018)
#define UART0_IBRD    *(volatile uint32_t *) (UART0_BASE + 0x024)
#define UART0_FBRD    *(volatile uint32_t *) (UART0_BASE + 0x028)
#define UART0_LCR_H   *(volatile uint32_t *) (UART0_BASE + 0x02c)
#define UART0_CR      *(volatile uint32_t *) (UART0_BASE + 0x030)

#define TXFF_BIT      (1 << 5) // TX FIFO cheio
#define RXFE_BIT      (1 << 4) // RX FIFO vazio

#define CLK_PERI  12000000
#define BAUD_RATE 115200

void delay(volatile uint32_t count)
{
    while (count--) {
        __asm volatile ("nop");
    }
}

// inicializa o cristal de 12 MHz e troca o clock do sistema
void xosc_init(void)
{
    XOSC_STARTUP = 47; 
    XOSC_CTRL = (XOSC_ENABLE | CMD_ENABLE);
    while (!(XOSC_STATUS & STABLE_BIT));

    CLK_REF_CTRL = SRC_XOSC; 
    CLK_SYS_CTRL = SRC_AUX; 

    // habilita clock periférico
    CLK_PERI_CTRL = (1 << 11);
}

void uart_init(void)
{
    RESETS_RESET &= ~RST_UART0;
    while (!(RESETS_RESET_DONE & RST_UART0)); 
   
    GPIO0_CTRL = FUNC_UART; // GPIO0 = UART0_TX
    GPIO1_CTRL = FUNC_UART; // GPIO1 = UART0_RX

    uint32_t div_x64 = ((CLK_PERI * 4) + (BAUD_RATE / 2)) / BAUD_RATE;

    UART0_CR = 0;                         // desabilita UART antes de configurar
    UART0_IBRD = div_x64 >> 6;            // parte inteira
    UART0_FBRD = div_x64 & 0x3F;          // parte fracionária
    UART0_LCR_H = (1 << 4) | (3 << 5);   // FIFO enable + 8 bits, sem paridade, 1 stop
    UART0_CR = (1 << 0) | (1 << 8) | (1 << 9); // UARTEN | TXE | RXE
}

char uart_getc(void)
{
    while (UART0_FR & RXFE_BIT);   // espera até haver dado no RX FIFO
    return (char)(UART0_DR & 0xFF);
}

void uart_putc(char data)
{
    while (UART0_FR & TXFF_BIT);   // espera enquanto TX FIFO estiver cheio
    UART0_DR = data;
}

void uart_puts(const char *s)
{
    while (*s) {
        uart_putc(*s++);
    }
}

int main(void)
{
    xosc_init();
    
    uint32_t rst = (RST_IO_BANK0 | RST_PADS_BANK0);
    RESETS_RESET &= ~rst;
    while ((RESETS_RESET_DONE & rst) != rst);

    GPIO25_CTRL = FUNC_SIO;
    GPIO_OE_SET = (1u << LED);

    // Pisca inicial para indicar boot
    for (int j = 0; j < 6; j++) {
        GPIO_OUT_XOR = (1u << LED);
        delay(1000000);
    }

    uart_init();

    // Mensagem inicial
    uart_puts("UART pronta\r\n");
    uart_puts("Envie 'S' para alternar o LED\r\n");

    while (1)
    {
        char c = uart_getc();   // espera um caractere chegar

        uart_putc(c);           // ecoa o que recebeu

        if (c == 'S' || c == 's') {
            GPIO_OUT_XOR = (1u << LED); // alterna LED
            uart_puts(" <- LED TOGGLED\r\n");
        }
    }

    return 0;
}