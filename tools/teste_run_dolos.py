#!/usr/bin/env python3
"""Teste manual do controle de RUN da Pico alvo pelo Dolos.

Requisitos: pip install pyserial
Uso: python3 teste_run_dolos.py [porta]
Ex.: python3 teste_run_dolos.py /dev/ttyACM0

Meça o pino RUN do alvo em relação ao GND com o multímetro.
O script envia RELEASE no encerramento para não deixar o alvo em reset.
"""

import sys
import serial

PORTA = sys.argv[1] if len(sys.argv) > 1 else "/dev/ttyACM0"
MAGIC = 0xAA
CMD_HOLD = 0x02
CMD_RELEASE = 0x03


def enviar(ser: serial.Serial, cmd: int) -> bool:
    pacote = bytes((MAGIC, cmd, 0, MAGIC ^ cmd))
    ser.write(pacote)
    ser.flush()
    resposta = ser.readline().decode(errors="replace").strip()
    print(f"Comando 0x{cmd:02X} -> {resposta or '(sem resposta)'}")
    return resposta == "C000"


def main() -> int:
    try:
        with serial.Serial(PORTA, 115200, timeout=3) as ser:
            print(f"Sonda Dolos: {PORTA}")
            print("Meça sempre RUN no alvo em relação ao GND.")
            try:
                print("\n1) Enviando HOLD: RUN deve ficar próximo de 0 V.")
                if not enviar(ser, CMD_HOLD):
                    print("O HOLD não foi confirmado. Não interprete a tensão como validada.")
                    return 1
                input("Anote a tensão do RUN em LOW e pressione ENTER para liberar... ")
            finally:
                print("\n2) Enviando RELEASE: RUN deve voltar a aproximadamente 3,3 V.")
                liberou = enviar(ser, CMD_RELEASE)
                if not liberou:
                    print("ATENÇÃO: liberação não confirmada. Verifique o RUN com o multímetro.")

            input("Anote a tensão do RUN em HIGH e pressione ENTER para encerrar... ")
            print("Teste concluído. Compare as duas medições.")
            return 0 if liberou else 1
    except (KeyboardInterrupt, EOFError):
        print("\nTeste interrompido. Confira o nível do RUN antes de desconectar.")
        return 130
    except serial.SerialException as exc:
        print(f"Erro de comunicação serial: {exc}")
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
