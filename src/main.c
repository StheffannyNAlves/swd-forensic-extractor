#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// endereços base
#define RESETS_BASE (0x4000c000)
#define UART0_BASE (0x40034000)
#define IO_BANK0 *(uint32_t *) (0x40014000 + 0x0CC)
#define SSIO_BASE (0x4003c000)

// mapeamento dos registradores 
#define UART_FR *(uint32_t *) (UART0_BASE + 0x18) // Status
#define UART_DR *(uint32_t *) (UART0_BASE + 0x00) // Dados
#define RESETS_RESET *(uint32_t *) (RESETS_BASE + 0x00)
#define RESETS_RESET_DONE *(uint32_t *) (RESETS_BASE + 0x08) // pra zerar o bit
#define GPIO6_CTRL *(uint32_t *) (IO_BANK0 + 0) // Configuração do pino Tx
#define SSI_CR0  *(uint32_t *) (SSIO_BASE + 0x00) // control register 0, define o formato(clock, modo SPI)
#define SSI_CR1  *(uint32_t *) (SSIO_BASE + 0x04) // ctrl reg 1, habilita/desabilita o SSI para cada leitura
#define SSI_DR0  *(uint32_t *) (SSIO_BASE + 0x08) // data reg, envio e recebimento de dados da FLASH


// Valores escritos nos registradores, mascaras
#define TXFF_RST_BIT (1 << 5) // isola o bit 5(buffer/FIFO já cheio)
#define UART0_RST_BIT (1 << 22) 
#define FUNC_UART0 (1 << 0)
#define SSI0_RST_BIT (1<< 16) // mascara pra sair do reset

#define SSI_EN_BIT (1 << 0)




void uart_putc(char data) // funcao de envio de bits
{
   // enquanto o bit 5 no flag register(UART_FR) for 1, espere
   while (UART_FR & TXFF_RST_BIT)
   {
    // a cpu espera 
   }

   UART_DR = data; // escreva o byte de dado diretamente no Data Register

}
   


void ssi_init(void)
{
   RESETS_RESET &= ~SSI0_RST_BIT;
   while (RESETS_RESET_DONE & SSI0_RST_BIT)
   {

   }   
   
   SSI_CR1 &= ~SSI_EN_BIT; // SSI desabilitado


   // configuração dos pinos
}
