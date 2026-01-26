# 🔬 Research Phase: Bare-Metal RP2040 (Legacy)

> **Branch de Arquivo Morto - Fase 0 do Projeto P1**

Este branch contém o código-fonte original desenvolvido durante a fase de **Engenharia Reversa** e estudo de baixo nível do RP2040. Todo o código aqui presente foi construído "do zero" (*scratch*), sem utilização do Pico SDK, para validar o entendimento da arquitetura do chip.

🔗 **O projeto ativo (Sonda Forense SWD) está na branch [`main`](../../tree/main).**

## 📂 Estrutura do Projeto

| Diretório | Descrição |
| :--- | :--- |
| `src/` | **Código Fonte C e Assembly.** Contém o `main.c` (driver UART/GPIO via MMIO) e o `start.s` (Reset Handler e Boot). |
| `linker/` | **Mapeamento de Memória.** O arquivo `memmap.ld` define manualmente as regiões de FLASH e RAM para o linker. |
| `tools/` | **Ferramentas de Build.** Scripts Python proprietários para gerar arquivos `.uf2` (`maker.py`) e testar a comunicação serial (`test_uart.py`). |

## ⚙️ Funcionalidades Implementadas

1.  **Bootloader Customizado:** Inicialização do processador Cortex-M0+ via Assembly puro (`start.s`).
2.  **Clock Tree Manual:** Configuração do cristal oscilador (XOSC) de 12MHz escrevendo diretamente nos registradores.
3.  **Drivers MMIO:** Controle de periféricos (UART0 e GPIO) via ponteiros voláteis, sem camadas de abstração.

## 🛑 Status e Pivô

O desenvolvimento nesta branch foi encerrado após a validação do driver UART. Foi constatado que, para fins forenses, a abordagem Bare-Metal pura dificultava a implementação de uma stack USB robusta e segura.

**Decisão:** O projeto migrou para uma arquitetura híbrida (Bare-Metal + TinyUSB) na branch `main`, utilizando o protocolo **SWD** em vez de UART para extração de memória.

---
*Este código é mantido apenas para fins históricos e de referência educacional.*