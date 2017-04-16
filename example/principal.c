
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "edm.h"

int main (int argv, char** argc)
{
    p_cm_t cmEdm = NULL;
    uint16_t crc16 = 0;
    uint8_t vet[] = {"123456789"};
    uint8_t sizeVet = sizeof(vet);
    uint32_t crc32 = 0;
    int ret = 0;
    
    cmEdm = libEdmInitCRC32();
    ret = libEdmGetCrc32_r(cmEdm, vet, sizeVet, &crc32);    
    printf("CRC32: %.4x\n", crc32);
//    
//    printf("Versao: %s\n", libEdmVersion(0));
//    
//    cmEdm = libEdmInitCRC16CITT();
//    libEdmInfoCrc(cmEdm);
//    ret = libEdmGetCrc16CITT_r(cmEdm, vet, sizeVet, &crc16);    
//    printf("ret: %d\n", ret);
//    printf("CITT CRC: %.4x\n", crc16);
//    
//    cmEdm = libEdmInitXModem();
//    crc16 = 0;
//    libEdmInfoCrc(cmEdm);
//    ret = libGetCrcXModem_r(cmEdm, vet, sizeVet, &crc16);    
//    printf("ret: %d\n", ret);
//    printf("XMODEM CRC: %.4x\n", crc16);
//    
//    cmEdm = libEdmInitArc();
//    libEdmInfoCrc(cmEdm);
//    crc16 = 0;
//    ret = libEdmGetCrcArc_r(cmEdm, vet, sizeVet, &crc16);    
//    printf("ret: %d\n", ret);
//    printf("ARC CRC: %.4x\n", crc16);
    
    cmEdm = libEdmInitModbus();
    crc16 = 0;
    ret = libEdmGetCrcModbus_r(cmEdm, vet, sizeVet, &crc16);   
    printf("MODBUS CRC: %.4x\n", crc16);
    
    exit(0);
}

/******************************************************************************/
/*                             End of example.c                              */
/******************************************************************************/
