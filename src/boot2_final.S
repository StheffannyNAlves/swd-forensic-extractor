.section .boot2, "ax"
.global boot2
boot2:
    .incbin "../tools/boot2.bin"
    