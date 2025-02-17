#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H

#include <stdint.h>
#include <string.h>
#include "nv_data.h"

#define EEPROM_SIZE      524288       // 4Mb = 256KB
#define MAX_IMAGES       64           // Maximum number of image headers
#define HEADER_SIZE      sizeof(ImageHeader)
#define HEADER_AREA_SIZE (MAX_IMAGES * HEADER_SIZE)
#define DATA_START_ADDR  (HEADER_AREA_SIZE + MAX_IMAGES/8) // Data starts after headers, magic number, and CRC
#define MAX_IMAGE_SIZE   76800         // Example max image size
#define MAGIC_NUMBER     0x02072025  // Unique identifier

typedef struct {
    uint16_t id;
    uint16_t width;
    uint16_t height;
    uint32_t address;
} ImageHeader;

extern ImageHeader imageHeaders[MAX_IMAGES];

void EEPROM_Read(uint32_t address, uint8_t* buffer, uint32_t length);
void EEPROM_Write(uint32_t address, uint8_t* data, uint32_t length);
uint32_t CalculateCRC(uint8_t* data, uint32_t size);
void InitializeEEPROM();
void LoadImageHeaders();
uint32_t FindFreeHeaderSlot();
uint32_t FindFreeDataSpace(uint32_t size);
uint8_t EEPROM_SaveImage(uint16_t id, uint16_t width, uint16_t height, uint8_t* data);
uint8_t EEPROM_ReadImage(uint16_t id, uint8_t* buffer, uint16_t* width, uint16_t* height);

#endif // EEPROM_MANAGER_H
