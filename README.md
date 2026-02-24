# 🔬 Research Phase: Bare-Metal RP2040 (Legacy)

> **Branch de Arquivo Legado - Fase 0 do Projeto P1**

Este branch contém o código-fonte original desenvolvido durante a fase de **Engenharia Reversa** e estudo de baixo nível do RP2040. Todo o código aqui presente foi construído "do zero" (*scratch*), sem utilização do Pico SDK, para validar o entendimento da arquitetura do chip.

🔗 **O projeto ativo (Sonda Forense SWD) está na branch [`main`](../../tree/main).**

## 📂 Estrutura do Projeto

| Diretório | Descrição |
| :--- | :--- |
| `src/` | **Código Fonte C e Assembly.** Contém o `main.c` (driver UART/GPIO via MMIO), o `start.s` (Reset Handler e Boot) e o `boot2_w25q080.S` (Estágio 2 do Boot).  |
| `linker/` | **Mapeamento de Memória.** O arquivo `memmap.ld` define manualmente as regiões de FLASH e RAM para o linker. |
| `tools/` | **Ferramentas de Build.** Scripts Python proprietários para gerar arquivos `.uf2` (`maker.py`) e testar a comunicação serial (`test_uart.py`). |

## ⚙️ Funcionalidades Implementadas

1.  **Bootloader Customizado:** Inicialização do processador Cortex-M0+ via Assembly puro (`start.s`).
2.  **Clock Tree Manual:** Configuração do cristal oscilador (XOSC) de 12MHz escrevendo diretamente nos registradores.
3.  **Drivers MMIO:** Controle de periféricos (UART0 e GPIO) via ponteiros voláteis, sem camadas de abstração.

## 🔄 Decisão Arquitetural: Pivô de Bare-Metal Puro para Híbrido

Esta branch documenta a Fase 0 da pesquisa, onde toda a stack (Bootloader, Clocks, GPIO, UART) foi desenvolvida estritamente via MMIO (Memory-Mapped I/O) sem uso do Pico SDK. O objetivo foi validar o domínio da arquitetura ARM Cortex-M0+ e do silício do RP2040.

No entanto, para o caso de uso final (Extração Forense de 2MB de memória), a abordagem bare-metal pura com comunicação UART demonstrou limitações matemáticas e físicas inaceitáveis:

1. **Gargalo de Exfiltração:** A comunicação UART (8N1), mesmo configurada em altas taxas de transmissão, exige um overhead de 10 bits no fio para cada 1 byte de payload. A extração de 2MB via serial impõe um tempo de espera incompatível com a agilidade exigida em operações de campo.
2. **Integridade Física do Sinal:** Aumentar o baud rate da UART sobre fios não-diferenciais (jumpers) introduz ruído e perda de pacotes, exigindo a implementação de protocolos de software custosos para retransmissão e checksum.
3. **Gerenciamento de Complexidade:** O diferencial desta ferramenta reside no controle de baixo nível do protocolo SWD para garantir a preservação da evidência (Safe-Read). Implementar uma stack USB em bare-metal desviaria o foco da engenharia reversa do alvo.

**A Solução (Branch `main`):**
O projeto migrou para uma Arquitetura Híbrida. Utilizo o Pico SDK estritamente para o transporte (TinyUSB via USB CDC para comunicação confiável e de alta velocidade com o PC Host) e mantenho a implementação *bare-metal* rigorosa (bit-banging) apenas na interface física do SWD que se conecta ao chip alvo, garantindo assim velocidade máxima sem abrir mão da segurança forense.

---
*Este código é mantido apenas para fins históricos e de referência educacional.*