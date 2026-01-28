# P1: SWD Forensic Extractor (RP2040)

![Status](https://img.shields.io/badge/Status-Sprint_1_(Foundation)-blue)
![Platform](https://img.shields.io/badge/Platform-RP2040-2b2a2a?logo=raspberry-pi)
![License](https://img.shields.io/badge/License-MIT-green)

> **Ferramenta de engenharia reversa e forense de hardware para extração de firmware via SWD.**

O **P1** é uma ferramenta de aquisição forense *bare-metal* (híbrida) projetada para extrair o conteúdo da memória FLASH externa de microcontroladores RP2040. Diferente de debuggers convencionais (CMSIS-DAP), este projeto foca em **integridade forense**, **performance de extração** e **controle de baixo nível** do protocolo SWD.

---

## 🎯 Objetivo Técnico

O objetivo final é transformar um Raspberry Pi Pico (Sonda) em um extrator capaz de realizar o dump de **2MB de Flash em < 60 segundos**, validando a integridade via SHA256, sem executar código residente no dispositivo Alvo.

### 🏗️ Arquitetura Híbrida
Para maximizar a eficiência de aprendizado e a performance profissional, o projeto adota uma arquitetura mista:

1. **Camada Física (Custom PHY):** Implementação manual ("bit-banging") do protocolo SWD para controle preciso dos tempos de sinal e manipulação do DAP (Debug Access Port).
2.  **Transporte (High-Level):** Utilização do **TinyUSB (CDC)** do Pico SDK para exfiltração de dados em alta velocidade para o host (PC).
3.  **Controle de Hardware:** Manipulação direta de GPIOs para *Target Reset* (Kill Switch) e *Power Glitching* (futuro).

---

## 🔌 Hardware & Setup

O sistema opera com duas unidades RP2040 conectadas fisicamente (Sonda e Alvo). A integridade elétrica é garantida por resistores de proteção nos sinais críticos.

### 📐 Esquema de Ligação (Wiring)

#### 1. Interface SWD & Kill Switch (Sonda ➔ Alvo)
Estas conexões permitem que o firmware da Sonda controle o Alvo.

| Sinal | Sonda (GPIO) | Componente Obrigatório | Alvo (Pino) | Função |
| :--- | :--- | :--- | :--- | :--- |
| **SWCLK** | `GP2` (Configurável) | Fio Direto | `SWCLK` | Clock do protocolo SWD. |
| **SWDIO** | `GP3` (Configurável) | Resistor **330Ω** (Série) | `SWDIO` | Dados bidirecionais. O resistor evita danos em conflitos de I/O. |
| **RESET** | `GP22` (Configurável) | Resistor **1kΩ** (Série) | `RUN` (Pino 30) | **Kill Switch:** Permite controle total do ciclo de vida do alvo via software. |
| **GND** | `GND` | Fio Direto | `GND` | Referência comum. **Crucial.** |

> **Nota:** Os GPIOs listados acima devem coincidir com as definições no `p1_config.h`.

#### 2. Botão de Reset da Sonda (Opcional - Dev Helper)
Hardware auxiliar para facilitar o desenvolvimento e reinicialização da ferramenta sem desconexão USB.

* **Componente:** Chave Táctil (Push-Button) N.O.
* **Conexão:** Entre o pino `RUN` (Pino 30) da própria **Sonda** e qualquer `GND`.
* **Ação:** Pressionar para reiniciar o firmware da Sonda (Reset físico).


---

## 🗺️ Roadmap de Desenvolvimento

O projeto segue cronograma de Sprints quinzenais.

### ✅ Fase 0: Pesquisa Bare-Metal (Concluído)
*Estudo prévio de engenharia reversa do bootrom e drivers manuais (arquivos disponíveis em `/research`).*
- [x] Bootloader UF2 customizado.
- [x] Linker script (`memmap.ld`) e Startup code (`start.s`) manuais.
- [x] Drivers MMIO para UART e GPIO.

### 🚧 Fase 1: Fundação da Sonda (Sprint Atual)
- [x] Setup do ambiente CMake + TinyUSB.
- [ ] **Kill Switch:** Controle físico do pino RUN do alvo.
- [ ] **USB Echo:** Validação do pipeline de comunicação PC <-> Sonda.
- [ ] **SWD PHY:** Implementação das primitivas `WriteBit`, `ReadBit` e `Turnaround`.

### 🔜 Próximos Passos (Sprints 2-5)
- Implementação da Máquina de Estados do Protocolo SWD.
- Leitura do registro `IDCODE` e Handshake.
- Inicialização remota da Flash QSPI via escrita em registradores.
- Pipeline de extração em massa (Bulk Read).

---

## 🛠️ Como Compilar e Rodar

### Pré-requisitos
* **Arm GNU Toolchain** (`arm-none-eabi-gcc`)
* **CMake** & **Make** (ou Ninja)
* **Pico SDK** configurado no `PATH`

### Build
```bash
# 1. Clone o repositório
git clone [https://github.com/StheffannyNAlves/swd-forensic-extractor.git](https://github.com/StheffannyNAlves/swd-forensic-extractor.git)
cd swd-forensic-extractor

# 2. Crie o diretório de build
mkdir build && cd build

# 3. Compile
cmake ..
make
```


