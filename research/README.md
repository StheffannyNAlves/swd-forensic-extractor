# 🔬 Research Phase: Bare-Metal RP2040 (Fase 0)

> **Artefatos de Estudo de Baixo Nível & Engenharia Reversa**

Este diretório contém a base fundamental desenvolvida para compreender a arquitetura do RP2040 "na unha". Todo o código aqui foi escrito sem dependência do Pico SDK, utilizando acesso direto à memória (MMIO) e Assembly manual.

Embora o projeto principal tenha evoluído para uma abordagem híbrida (SWD + TinyUSB), esta fase foi crucial para validar o entendimento do Bootrom, Clock Tree e periféricos do chip.

## 📂 Estrutura do Diretório

A organização reflete o pipeline de compilação manual (Cross-Compilation):

| Diretório | Arquivo | Descrição Técnica |
| :--- | :--- | :--- |
| **`/src`** | `main.c` | **Lógica MMIO.** Driver UART e GPIO escrito via ponteiros diretos para endereços físicos (ex: `0x40034000`). Configura clocks e periféricos manualmente. |
| | `start.s` | **Startup Code (Assembly).** Responsável pela Tabela de Vetores, configuração do Stack Pointer (`SP`) e limpeza da seção `.bss` antes de chamar o `main`. |
| **`/linker`** | `memmap.ld` | **Linker Script.** Define o layout físico da memória, mapeando `.text` na Flash (`0x10000000`) e `.data/.bss` na RAM (`0x20000000`). |
| **`/tools`** | `maker.py` | **Build Tool.** Ferramenta Python proprietária que insere o *Boot Block* (CRC32) obrigatório e converte o binário cru em formato **UF2** para gravação. |
| | `test_uart.py` | **Validação.** Script para teste de loopback serial (PC <-> Pico). |

## ⚙️ Detalhes da Implementação

### 1. Boot Manual
Diferente do SDK padrão que esconde a inicialização, aqui controlamos:
1.  **Reset Handler:** O primeiro código executado pelo Cortex-M0+.
2.  **XOSC Init:** Ativação manual do cristal de 12MHz e comutação do PLL.
3.  **BSS Zeroing:** Loop em Assembly para limpar a memória RAM não inicializada.

### 2. Drivers MMIO
Os drivers não utilizam abstrações. Exemplo de configuração da UART via registrador:
```c
// Definição direta do endereço físico
#define UART0_CR *(volatile uint32_t *) (0x40034000 + 0x030)
// Escrita direta de bits de controle
UART0_CR = (1 << 0) | (1 << 8) | (1 << 9);
```
## Pivô de Arquitetura (Post-Mortem UART)

Durante a validação deste driver UART (src/main.c), identificou-se que a UART não é viável como vetor primário de extração forense, motivando o pivô para o protocolo SWD no projeto principal.

Motivos Técnicos:

    1. **Dependência de Agente:** A UART exige que um firmware colaborativo esteja rodando no alvo para enviar dados. Em forense, não podemos garantir execução de código no dispositivo analisado.

    2. **Limitação de Transporte:** O código bare-metal desenvolvido não possui Stack USB CDC. Para funcionar, exige hardware adicional (ponte USB-Serial ou outra Pico atuando como Debug Probe), o que complexifica o setup de campo.

    3. **Integridade:** A injeção de um firmware de dump via UART altera excessivamente o estado da RAM (evidência).

Conclusão: O código UART aqui presente serve como referência acadêmica de controle de hardware, mas a ferramenta final (P1) utilizará SWD (Serial Wire Debug) para controle externo e USB Bulk para exfiltração de dados.

> Este diretório é um arquivo morto de pesquisa. O desenvolvimento ativo da ferramenta forense ocorre na raiz do repositório.
