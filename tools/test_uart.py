import serial
import time

try:
    # Abre a porta bruta. 
    ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1) 
    print("Conexão estabelecida. Disparando...")
    
    while True:
        ser.write(b'U') 
        
        resposta = ser.read(1) 
        if resposta:
            print(f"Resposta: {resposta.hex().upper()}")
        else:
            print("Microcontrolador mudo...")
            
        time.sleep(0.5) # Atraso para você conseguir ler a tela
        
except Exception as e:
    print(f"Falha de ambiente operacional: {e}")