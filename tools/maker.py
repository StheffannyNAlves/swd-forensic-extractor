# gera os blocos do uf2


import struct


def rp2040_crc32(data):
    crc = 0xFFFFFFFF
    for byte in data:
        crc ^= (byte << 24)
        for _ in range(8):
            if crc & 0x80000000:
                crc = (crc << 1) ^ 0x04C11DB7
            else:
                crc = crc << 1
    return crc & 0xFFFFFFFF

def gera_boot2(raw_bin, output_s):
    with open(raw_bin, "rb") as f:
        data = f.read()

    if len(data) < 64:
        raise ValueError(f"Erro na compilação: o arquivo tem boot2.bin tem apenas {len(data)} bytes")
    if len(data) > 252:
        raise ValueError("Estouro de memória: o código do boot2.bin ultrapasssou 252 bytes")
    

    data = data.ljust(252, b'\x00')


    crc = rp2040_crc32(data)
    final = data + struct.pack("<I", crc)
    print(f"CRC calculado: {hex(crc)}")

    with open(output_s, "w") as f:
        f.write('.section .boot2, "ax"\n')
        f.write('.global boot2\n')
        f.write('boot2:\n')

        for i in range(0, 256, 16):
            chunk = final[i:i+16]
            bytes_str = ", ".join(f"0x{b:02x}" for b in chunk)
            f.write(f"    .byte {bytes_str}\n")

if __name__ == "__main__":
    gera_boot2("boot2.bin", "boot2_gerado.S")
    
  