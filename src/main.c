#include <stdint.h>

int main(void)
{
    volatile uint32_t *p = (uint32_t *)0x20000000;
    *p = 0xDEADBEEF;
    while (1);
}
