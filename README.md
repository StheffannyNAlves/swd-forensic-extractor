# P1: Aquisição Forense Bare-Metal de FLASH via SWD (RP2040)

[![Status](https://img.shields.io/badge/Status-Em_Desenvolvimento-orange)]()
[![Plataforma](https://img.shields.io/badge/Plataforma-RP2040%20\(Bare--Metal\)-green)]()
[![Licença](https://img.shields.io/badge/Licença-MIT-blue)](LICENSE)

Projeto pessoal de engenharia reversa e forense de hardware, desenvolvido para estudo aprofundado de bare-metal no RP2040 e implementação de mecanismos de extração de memória em baixo nível.


Este README reflete **o estado real do desenvolvimento**, documentando o que já foi implementado, validado e os próximos passos técnicos planejados.

---

## 🎯 Objetivo Técnico do Projeto

Desenvolver uma ferramenta forense capaz de:

* compreender e controlar o processo de boot do RP2040;
* executar firmware bare-metal sem dependência de SDK;
* manipular registradores via **MMIO**;
* estabelecer um método confiável de extração de memória;
* evoluir para uma abordagem correta de extração forense via **SWD**.

O projeto segue uma abordagem **incremental**, começando pela fundação bare-metal e avançando para o protocolo de debug.

---

## 🧱 Fundações Implementadas (Estado Atual)

### 1. Ambiente Bare-Metal Funcional

Foi implementado um ambiente completo de compilação e boot para o RP2040:

* Toolchain `arm-none-eabi-gcc` configurada
* Binário próprio, sem Pico SDK
* Boot via USB utilizando formato **UF2**

#### Maker.py

Script Python responsável por:

* conversão de `.bin` para `.uf2`;
* inserção do `boot2` obrigatório do RP2040;
* empacotamento em blocos UF2 de 512 bytes;
* cálculo de CRC conforme especificação UF2.

Esse script permite gravação direta via BOOTSEL, sem ferramentas externas.

---

### 2. Linker Script Personalizado (`memmap.ld`)

Mapa de memória configurado manualmente:

* FLASH iniciando em `0x10000100` (offset do boot2);
* RAM em `0x20000000` (264 KB);
* definição explícita das seções:

  * `.text`
  * `.data`
  * `.bss`

Símbolos como `_etext`, `_sdata`, `_edata` são utilizados para inicialização correta da RAM.

---

### 3. Startup Assembly (`start.s`)

Implementação manual do código de inicialização:

* tabela de vetores (Stack Pointer + Reset Handler);
* rotina de limpeza da seção `.bss`;
* chamada explícita da função `main()`;
* compreensão prática do fluxo de boot do Cortex-M0+.

---

## ⚙️ Firmware Bare-Metal Desenvolvido

### Definição Manual de Registradores

Todos os periféricos são acessados via MMIO:

* CLOCKS
* RESETS
* IO_BANK0
* PADS_BANK0
* UART0
* SIO

Cada registrador é definido como `volatile uint32_t*`, garantindo acesso direto ao hardware.

---

### Inicialização de Clock (XOSC)

* Configuração do cristal externo de 12 MHz;
* uso do valor de desbloqueio `0xFABAA000`;
* espera explícita pelo bit de estabilidade;
* comutação do clock do sistema para o XOSC.

---

### Driver UART Bare-Metal

Implementação manual da UART:

* liberação dos periféricos via `RESETS`;
* configuração dos GPIOs (TX/RX);
* cálculo de baud rate para 115200 bps;
* configuração dos registradores IBRD/FBRD;
* envio bloqueante via FIFO (`uart_putc()`).

O firmware atual:

* transmite caracteres continuamente;
* utiliza GPIO25 para indicação visual (LED);
* emprega delays simples por loop de CPU.

---

## 🔌 Montagem de Hardware Realizada

### Configuração Atual

* **Alvo**: RP2040 executando firmware bare-metal próprio;
* **Sonda**: RP2040 rodando `debugprobe_on_pico.uf2`.

Conexões básicas testadas:

* Alimentação compartilhada (VSYS/GND);
* tentativa de comunicação UART entre placas.

---

## 🧪 Testes Executados e Resultados

* Firmware executa corretamente (LED pisca);
* Clock e inicialização confirmados;
* UART configurada e funcional em teoria;
* Comunicação entre placas **não funcional** no setup atual.

### Diagnóstico Técnico

Foi identificado que:

* `debugprobe_on_pico.uf2` transforma o RP2040 em **CMSIS-DAP**;
* os GPIOs são usados internamente pelo firmware da probe;
* a Sonda **não atua como bridge UART genérica**;
* portanto, a falha não está no firmware do Alvo, mas na arquitetura do teste.

---

## 🔍 Reavaliação Técnica do Escopo

Com base nos testes, ficou claro que:

* UART não é adequada como método primário de extração forense;
* o objetivo real exige acesso direto à memória;
* o protocolo nativo para isso é **SWD**;
* a arquitetura correta é **Sonda–Alvo**, sem firmware ativo no Alvo.

Essa constatação levou à redefinição técnica do projeto.

---

## 📌 Estado Atual do Projeto

### Implementado

* Ambiente bare-metal completo
* Processo de boot compreendido e funcional
* Tooling próprio (UF2, linker, startup)
* Drivers básicos (clock, GPIO, UART)

### Não Implementado (Ainda)

* Comunicação funcional entre placas
* Protocolo SWD
* Leitura de memória via DAP/MEM-AP
* Extração real de FLASH

---

## 🛣️ Próximos Passos Técnicos Planejados

1. Testar UART do firmware com adaptador USB-UART dedicado
2. Implementar controle de reset do Alvo
3. Implementar SWD físico (bit-banging)
4. Leitura do IDCODE do RP2040
5. Navegação DAP / MEM-AP
6. Extração incremental de memória

---

## 🧠 Escopo Atual

Este projeto está na **fase de fundação técnica**.

O foco atual é:

* domínio do hardware;
* entendimento profundo do boot e periféricos;
* preparação da base necessária para SWD.

A extração forense completa é o objetivo final, mas depende dessas etapas fundamentais.

---

## 📜 Licença

MIT License

---

> Projeto P1 — Engenharia de Computação / Engenharia Reversa
> README alinhado ao progresso técnico real do desenvolvimento.
