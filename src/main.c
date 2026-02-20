// prova que a comunicação uart funciona, não tá 100% certo

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h> 

#define CLOCKS_BASE   0x40008000
#define RESETS_BASE   0x4000c000
#define IO_BANK0_BASE 0x40014000
#define XOSC_BASE     0x40024000
#define UART0_BASE    0x40034000
#define SIO_BASE      0xd0000000 

//XOSC 
#define XOSC_CTRL     *(volatile uint32_t *) (XOSC_BASE + 0x00)
#define XOSC_STATUS   *(volatile uint32_t *) (XOSC_BASE + 0x04)
#define XOSC_STARTUP  *(volatile uint32_t *) (XOSC_BASE + 0x0C)

// valores magicos / senhas
#define XOSC_ENABLE   0xFABAA000
#define CMD_ENABLE    0xB
#define STABLE_BIT    (1 << 31)

// clocks 
#define CLK_REF_CTRL  *(volatile uint32_t *) (CLOCKS_BASE + 0x30)
#define CLK_SYS_CTRL  *(volatile uint32_t *) (CLOCKS_BASE + 0x3C)
#define SRC_XOSC      2 
#define SRC_AUX       0

#define LED 25

#define RESETS_RESET      *(volatile uint32_t *) (RESETS_BASE + 0x00)
#define RESETS_RESET_DONE *(volatile uint32_t *) (RESETS_BASE + 0x08)
// mascaras
#define RST_UART0      (1 << 22) 
#define RST_IO_BANK0   (1 << 5)
#define RST_PADS_BANK0 (1 << 8)

// GPIO & IO Bank
#define GPIO0_CTRL    *(volatile uint32_t *) (IO_BANK0_BASE + 0x004) // Tx
#define GPIO1_CTRL    *(volatile uint32_t *) (IO_BANK0_BASE + 0x00c) // Rx
#define GPIO25_CTRL   *(volatile uint32_t *) (IO_BANK0_BASE + 0x0CC) // LED
#define FUNC_UART     2
#define FUNC_SIO      5


#define GPIO_OUT_XOR  *(volatile uint32_t *) (SIO_BASE + 0x01C)
#define GPIO_OE_SET   *(volatile uint32_t *) (SIO_BASE + 0x024)

#define UART0_DR      *(volatile uint32_t *) (UART0_BASE + 0x000) // Dados
#define UART0_FR      *(volatile uint32_t *) (UART0_BASE + 0x018) // Status/Flags
#define UART0_IBRD    *(volatile uint32_t *) (UART0_BASE + 0x024) // Integer Baud
#define UART0_FBRD    *(volatile uint32_t *) (UART0_BASE + 0x028) // Fractional Baud
#define UART0_LCR_H   *(volatile uint32_t *) (UART0_BASE + 0x02c) // Line Control
#define UART0_CR      *(volatile uint32_t *) (UART0_BASE + 0x030) // Control
#define TXFF_BIT      (1 << 5) // buffer cheio

#define CLK_PERI  12000000
#define BAUD_RATE 115200

// inicializa o cristal de 12mhz e troca o clock do sistema
void xosc_init(void)
{
    XOSC_STARTUP = 47; 
    XOSC_CTRL = (XOSC_ENABLE | CMD_ENABLE);
    while (!(XOSC_STATUS & STABLE_BIT));
    CLK_REF_CTRL = SRC_XOSC; 
    CLK_SYS_CTRL = SRC_AUX; 
}

void uart_init(void)
{
    // tira do reset
    uint32_t rst_mascara = (RST_UART0 | RST_IO_BANK0 | RST_PADS_BANK0);
    RESETS_RESET &= ~rst_mascara;
    while ((RESETS_RESET_DONE & rst_mascara) != rst_mascara);

    // configura pinos (mux)
    GPIO0_CTRL = FUNC_UART;
    GPIO1_CTRL = FUNC_UART;
    
   

    uint32_t div_x64 = ((CLK_PERI * 4) + (BAUD_RATE / 2)) / BAUD_RATE;

    UART0_CR = 0; // desabilita pra configurar
    
    // ponto fixo
    UART0_IBRD = div_x64 >> 6;       
    UART0_FBRD = div_x64 & 0x3F;    

    // 8 bits, fifo habilitada
    UART0_LCR_H = (1 << 4) | (3 << 5); 

    // ativa uart, tx e rx
    UART0_CR = (1 << 0) | (1 << 8) | (1 << 9); 
}

// envio de char (bloqueante)
void uart_putc(char data)
{
    // espera se a fifo estiver cheia
    while (UART0_FR & TXFF_BIT); 
    UART0_DR = data;
}

int main(void)
{
    xosc_init();
    uart_init();
    
    // config led
    GPIO25_CTRL = FUNC_SIO;
    GPIO_OE_SET = (1 << LED); 

    while (1)
    {
        uart_putc('U');
        GPIO_OUT_XOR = (1 << LED); // pisca led
        for (volatile int i = 0; i < 500000; i++); 
    }
}
