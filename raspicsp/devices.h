//
// Defines properties of devices or device families like location of specific registers
// etc.
//

#ifndef RASPICSP_DEVICES_H
#define RASPICSP_DEVICES_H

#include <stdint.h>
#include <cstring>

/*
 * Describes the properties of a device.
 */
typedef struct {

    /*
     * Contains the name of the device
     */
    const char *NAME;

    /*
     * Describes the code-sequence required to enter ICSP mode
     */
    uint32_t ICSP_CODE;

    /*
     * Contains the length of the code in bits
     */
    uint8_t ICSP_CODE_LENGTH;

    /*
     * Contains the address of the TBLPAG register
     */
    uint32_t TBLPAG_ADDR;

    /*
     * Contains the address of the NVMCON register
     */
    uint32_t NVMCON_ADDR;

    /*
     * Contains the address of the VISI register
     */
    uint32_t VISI_ADDR;

    /*
     * Contains the effective start-address. The PC of a device is
     * regularly reset to this address to keep it from overflowing
     */
    uint32_t START_ADDR;

    /*
     * Contains the address of the device id register
     */
    uint32_t DEVICE_ID_ADDR;

    /*
     * Contains the bit-pattern written to NVMCON to erase all memory
     */
    uint32_t NVMCON_ERASE_ALL;

    /*
     * Contains the bit-pattern written to NVMCON to write a row of data
     */
    uint32_t NVMCON_WRITE_ROW;

    /*
     * Contains the bit-pattern written to NVMCON to write a single 24 bit word of data
     */
    uint32_t NVMCON_WRITE_WORD;

    /*
     * Contains the bit-pattern which is use to check agains NVMCON to determine if the device
     * is still writing to its flash storage
     */
    uint32_t NVMCON_WRITING;

    /*
     * Contains the address of the first (lowest) config word
     */
    uint32_t CONFIG_WORDS_START_ADDR;

    /*
     * Contains the number of config words
     */
    uint8_t NO_CONFIG_WORDS;
} DEVICE;

/*
 * Describes a PIC24FJ32GB0XX device
 */
static const DEVICE PIC24FJ32GB0XX = {
        "PIC24FJ32GB0XX",
        0x4D434851,
        32,
        0x32,
        0x760,
        0x784,
        0x200,
        0xFF0000,
        0x404F,
        0x4001,
        0x4003,
        0x8000,
        0x0057F8,
        4
};

/*
 * Describes a PIC24FJ64GB0XX device
 */
static const DEVICE PIC24FJ64GB0XX = {
        "PIC24FJ64GB0XX",
        0x4D434851,
        32,
        0x32,
        0x760,
        0x784,
        0x200,
        0xFF0000,
        0x404F,
        0x4001,
        0x4003,
        0x8000,
        0x00ABF8,
        4
};

/*
 * Enumerates all devices
 */
static const DEVICE DEVICES[] = {PIC24FJ32GB0XX, PIC24FJ64GB0XX};

/*
 * Contains the number of devices in the DEVICES array
 */
static const int NUM_DEVICES = 2;


#endif //RASPICSP_DEVICES_H