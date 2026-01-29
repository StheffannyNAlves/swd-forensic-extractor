#include <stdint.h>

#define RESETS_BASE   0x4000c000
#define IO_BANK0_BASE 0x40014000
#define SIO_BASE      0xd0000000

#define RESETS_RESET      *(volatile uint32_t *) (RESETS_BASE + 0x00)
#define RESETS_RESET_DONE *(volatile uint32_t *) (RESETS_BASE + 0x08)

#define RST_IO_BANK0   (1 << 5)
#define RST_PADS_BANK0 (1 << 8)

#define GPIO25_CTRL   *(volatile uint32_t *) (IO_BANK0_BASE + 0x0CC)
#define GPIO_OE_SET   *(volatile uint32_t *) (SIO_BASE + 0x024)
#define GPIO_OUT_SET  *(volatile uint32_t *) (SIO_BASE + 0x014) // Usando SET em vez de XOR

#define FUNC_SIO      5
#define LED           25

int main(void) {
    uint32_t mascara_reset = RST_IO_BANK0 | RST_PADS_BANK0;
    RESETS_RESET &= ~mascara_reset;
    while ((RESETS_RESET_DONE & mascara_reset) != mascara_reset);

    GPIO25_CTRL = FUNC_SIO;
    
    GPIO_OE_SET = (1 << LED);  
    GPIO_OUT_SET = (1 << LED); 
    
    
    while (1);
}