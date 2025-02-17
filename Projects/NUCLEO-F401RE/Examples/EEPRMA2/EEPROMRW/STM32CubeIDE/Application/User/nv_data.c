
#include <stdio.h>
#include "nv_data.h"
#include "eeprma2_m95.h"
#include "stm32f4xx_nucleo_errno.h"

// Array to store image headers
ImageHeader imageHeaders[MAX_IMAGES];

// Initialize EEPROM Metadata
/**
 * @brief Initializes EEPROM with a magic number and CRC checksum.
 */
void InitializeEEPROM() {
    uint32_t magic = MAGIC_NUMBER;
    EEPRMA2_M95_WriteData(EEPRMA2_M95M04_0,(uint8_t*)&magic, 0, sizeof(magic));
    uint32_t crc = 0x0000;
    EEPRMA2_M95_WriteData(EEPRMA2_M95M04_0, (uint8_t*)&crc, 4, sizeof(crc));
}

// Load Headers from EEPROM
/**
 * @brief Loads image headers from EEPROM into memory.
 */
void LoadImageHeaders() {
    EEPRMA2_M95_ReadData(EEPRMA2_M95M04_0, (uint8_t*)imageHeaders, 8, HEADER_AREA_SIZE);
}

// Find Free Header Slot
/**
 * @brief Finds an available slot for a new image header.
 * @return Index of available slot, or -1 if full.
 */
uint32_t FindFreeHeaderSlot() {
    for (int i = 0; i < MAX_IMAGES; i++) {
        if (imageHeaders[i].id == 0xFFFF || imageHeaders[i].id == 0x0000) {
            return i;
        }
    }
    return -1; // No space
}

// Find Free Space in Data Area
/**
 * @brief Finds a free space in the EEPROM data area.
 * @param size Required size in bytes.
 * @return Address of free space, or 0xFFFFFFFF if no space.
 */
uint32_t FindFreeDataSpace(uint32_t size) {
    uint32_t address = DATA_START_ADDR;
    while (address + size <= EEPROM_SIZE) {
        uint8_t used = 0;
        for (int i = 0; i < MAX_IMAGES; i++) {
            if (imageHeaders[i].id != 0xFFFF && imageHeaders[i].id != 0x0000 &&
                imageHeaders[i].address == address) {
                used = 1;
                address += MAX_IMAGE_SIZE; // Skip to next slot
                break;
            }
        }
        if (!used) return address;
    }
    return 0xFFFFFFFF; // No space
}

// Save Image
/**
 * @brief Saves an image to EEPROM.
 * @param id Image identifier.
 * @param width Image width in pixels.
 * @param height Image height in pixels.
 * @param data Pointer to image data buffer.
 * @return 1 if successful, 0 otherwise.
 */
uint8_t EEPROM_SaveImage(uint16_t id, uint16_t width, uint16_t height, uint8_t* data) {
    uint32_t size = width * height;
    if (size > MAX_IMAGE_SIZE) return 0;

    int header_index = FindFreeHeaderSlot();
    if (header_index == -1) return 0; // No header slot available

    uint32_t address = FindFreeDataSpace(size);
    if (address == 0xFFFFFFFF) return 0; // No space

    ImageHeader header = {id, width, height, address};
    int32_t write_data_status = EEPRMA2_M95_WriteData(EEPRMA2_M95M04_0, data, address, size);
    imageHeaders[header_index] = header;
    int32_t update_header_status = EEPRMA2_M95_WriteData(EEPRMA2_M95M04_0, (uint8_t*)imageHeaders, 8, HEADER_AREA_SIZE); // Update headers

    if(write_data_status != BSP_ERROR_NONE || update_header_status != BSP_ERROR_NONE)
    {
        printf("Error while saving image into EEPROM\r");
    }
//    uint32_t crc = CalculateCRC(EEPROM + 8, EEPROM_SIZE - 8);
//    EEPRMA2_M95_WriteData(EEPRMA2_M95_ReadData, (uint8_t*)&crc, 4, sizeof(crc));
    
    return 1;
}

// Read Image
/**
 * @brief Reads an image from EEPROM.
 * @param id Image identifier.
 * @param buffer Destination buffer.
 * @param width Pointer to store image width.
 * @param height Pointer to store image height.
 * @return 1 if image found, 0 otherwise.
 */
uint8_t EEPROM_ReadImage(uint16_t id, uint8_t* buffer, uint16_t* width, uint16_t* height) {
    for (int i = 0; i < MAX_IMAGES; i++) {
        if (imageHeaders[i].id == id) {
            *width = imageHeaders[i].width;
            *height = imageHeaders[i].height;
            uint32_t size = (*width) * (*height);
            EEPRMA2_M95_ReadData(EEPRMA2_M95M04_0, buffer, imageHeaders[i].address, size);
            return 1;
        }
    }
    return 0; // Not found
}

// Main Function
//int main(void) {
//    InitializeEEPROM();
//    LoadImageHeaders();
//
//    uint8_t image_data[MAX_IMAGE_SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
//    uint8_t read_buffer[MAX_IMAGE_SIZE];
//    uint16_t width, height;
//
//    if (EEPROM_SaveImage(0x0001, 2, 8, image_data)) {
//        if (EEPROM_ReadImage(1, read_buffer, &width, &height)) {
//            printf("Read Back Image: ");
//            for (uint32_t i = 0; i < width * height; i++) {
//                printf("%u,", read_buffer[i]);
//                if (read_buffer[i] != image_data[i]) {
//                    printf("Error: Data mismatch\n");
//                    return -1;
//                }
//            }
//        }
//    }
//    return 0;
//}
