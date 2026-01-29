#include <stdint.h>

#define IO_BANK0_BASE 0x40014000
#define SIO_BASE      0xd0000000

#define GPIO25_CTRL   *(volatile uint32_t *) (IO_BANK0_BASE + 0x0CC)
#define GPIO_OE_SET   *(volatile uint32_t *) (SIO_BASE + 0x024)
#define GPIO_OUT_SET  *(volatile uint32_t *) (SIO_BASE + 0x014)
#define GPIO_OUT_CLR  *(volatile uint32_t *) (SIO_BASE + 0x018)

#define FUNC_SIO      5
#define LED           25

void delay(void) {
    for (volatile int i = 0; i < 200000; i++); 
}

int main(void) {
    
    GPIO25_CTRL = FUNC_SIO;
    
    GPIO_OE_SET = (1 << LED);
    
    while (1) {
        GPIO_OUT_SET = (1 << LED); 
        delay();
        GPIO_OUT_CLR = (1 << LED); 
        delay();
    }
}