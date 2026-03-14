# 🔬 Research Phase — Bare-Metal RP2040 (Legacy Baseline)

> **Fase 0 do Projeto P1 — Validação Arquitetural em Baixo Nível**

Esta branch documenta a fase inicial de engenharia reversa do **RP2040**, onde toda a stack de inicialização foi construída manualmente, **sem uso do Pico SDK**.

O objetivo desta etapa foi validar domínio completo sobre:

- Cadeia de boot do RP2040 (**BootROM → boot2 → aplicação**)
- Layout de memória (FLASH/RAM, **VMA vs LMA**)
- Runtime mínimo (crt0 manual)
- Configuração de clocks
- Drivers via MMIO direto (UART/GPIO)
- Pipeline de build customizada (linker script + geração de binário)

🔗 O projeto ativo (**Sonda Forense SWD**) está na branch `main`.

---

## 📂 Estrutura do Projeto

| Diretório | Descrição |
|------------|------------|
| `src/` | Código C e Assembly: `start.s` (Reset Handler), `main.c` (drivers MMIO), `boot2_w25q080.S` (estágio 2 do boot). |
| `linker/` | `memmap.ld` — definição explícita de FLASH/RAM, símbolos do runtime e posicionamento fixo das seções. |
| `tools/` | `maker.py` — geração determinística do `boot2` (padding + CRC32) e ferramentas auxiliares de teste. |

---

## 🧠 Objetivo Técnico da Fase 0

Esta etapa não tinha como foco “funcionar rápido”, mas sim ser **estruturalmente correta**.

### Critérios de Validação Arquitetural

- `.boot2` fixo em `0x10000000`, com exatamente **256 bytes**
- `.text` iniciando exatamente em `0x10000100`
- `.data` em RAM com **LMA em FLASH (`AT > FLASH`)**
- Implementação manual do `crt0`:
  - inicialização do stack pointer
  - cópia de `.data`
  - zeragem de `.bss`
  - chamada explícita de `main`
- Geração do `firmware.bin` a partir do ELF final, validando offsets reais
- Comunicação UART funcional via MMIO direto (sem SDK)

---

## ⚙️ Funcionalidades Implementadas

### 1️⃣ Boot e Runtime Manual

- Vetor mínimo definido em Assembly
- `Reset_Handler` responsável por:
  - configurar stack
  - copiar `.data` (FLASH → RAM)
  - zerar `.bss`
  - transferir controle para `main`
- Nenhuma dependência de bibliotecas padrão

---

### 2️⃣ Clock Tree Manual

- Inicialização do XOSC (12 MHz)
- Seleção manual de `clk_ref` e `clk_sys`
- Escrita direta em registradores do bloco `CLOCKS`

---

### 3️⃣ Drivers MMIO Diretos

- Controle de GPIO via SIO
- Configuração de UART0 via escrita direta em registradores:
  - reset de periféricos
  - mux de pinos
  - divisores `IBRD/FBRD`
  - ativação TX/RX

Sem HAL.  
Sem SDK.  
Sem abstrações intermediárias.

---

## 🧪 Metodologia de Teste

Os testes foram conduzidos incrementalmente:

- ✔️ Teste de inicialização (LED via GPIO)
- ✔️ Validação de layout de memória via `readelf` e `objdump`
- ✔️ Verificação matemática de VMA/LMA
- ✔️ Geração determinística do `boot2` com CRC32
- 🔄 Teste de UART realizado por etapas (clocks, reset, mux, baud)

> O código pode conter trechos temporariamente comentados durante validações isoladas de cada subsistema.

---

## 📉 Limitações Identificadas

Embora funcional, a abordagem bare-metal pura via UART apresenta limitações estruturais para o caso de uso final.

### 🚧 Gargalo de Exfiltração

UART 8N1 transmite **10 bits por byte efetivo**.

Exemplo prático:

- 2 MiB = 2.097.152 bytes  
- 115200 bps → 11.520 bytes/s efetivos  
- Tempo aproximado: ~182 segundos (~3 minutos)

Mesmo aumentando o baud rate, o uso de linhas não diferenciais (jumpers) introduz ruído e perda de integridade, exigindo protocolos adicionais de correção.

---

### ⚠️ Complexidade Arquitetural

O diferencial técnico da ferramenta está na implementação segura do protocolo **SWD (Safe-Read)**.

Implementar uma stack USB bare-metal desviaria o foco da engenharia reversa do alvo e aumentaria a superfície de risco.

---

## 🔄 Migração para Modelo Híbrido

A partir das limitações observadas, o projeto evoluiu para uma arquitetura híbrida na branch `main`:

- Pico SDK utilizado exclusivamente para transporte (**TinyUSB / USB CDC**)
- Implementação bare-metal preservada na interface SWD (bit-banging controlado)

Essa abordagem mantém:

- Velocidade de exfiltração adequada
- Integridade de comunicação com o host
- Controle total do protocolo SWD
- Preservação da cadeia de evidência

---

## 🎯 Papel Atual desta Branch

Esta branch representa:

- Baseline arquitetural validado
- Prova de domínio do silício RP2040
- Referência educacional e técnica
- Fundamento estrutural do projeto principal

> A fundação sobre a qual a arquitetura híbrida foi construída.