# só imprime o que chega


import serial 
import sys

PORTA = '/dev/ttyACM0' 


try:
    porta = sys.argv[1] if len(sys.argv) > 1 else PORTA
    
    print(f"[*] Abrindo porta {porta} a 115200 baud...")
    ser = serial.Serial(porta, 115200, timeout=1)
    
    print("[*] Conectado! Aguardando dados (Ctrl+C para sair)...")
    
    while True:
        dado = ser.read(1)
        
        if dado:
            # Mostra o byte cru (Hex) e a representação ASCII
            print(f"Byte: {dado.hex().upper()} | ASCII: {dado}")
            
except serial.SerialException as e:
    print(f"[!] Erro ao abrir serial: {e}")
    print("verifique se a porta está certa e se o Pico está plugado.")
except KeyboardInterrupt:
    print("\n[*] Encerrando teste.")