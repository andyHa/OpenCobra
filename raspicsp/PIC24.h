//
// Contains the programmer to write a program to a PIC24 device
//

#ifndef RASPICSP_PIC24_H
#define RASPICSP_PIC24_H

#include <list>
#include "HAL.h"
#include "devices.h"
#include "ICSP.h"
#include "HexFile.h"

/*
 * Enumerates all working registers supported by a PIC24
 */
enum REG {
    W0 = 0,
    W1 = 1,
    W2 = 2,
    W3 = 3,
    W4 = 4,
    W5 = 5,
    W6 = 6,
    W7 = 7,
    W8 = 8,
    W9 = 9,
    W10 = 10,
    W11 = 11,
    W12 = 12,
    W13 = 13,
    W14 = 14,
    W15 = 15,
};

/*
 * Enumerates the modes when accessing registers in TBLRD* and TBLWT* instructions
 *
 * The 16-bit MCU and DSC Programmer’s Reference Manual (DS70157F) gets this wrong!
 * This seems more accurate: https://en.wikipedia.org/wiki/PIC_instruction_listings
 */
enum TBL_MODE {
    DIRECT = 0,
    INDIRECT = 1,
    INDIRECT_POST_DEC = 2,
    INDIRECT_POST_INC = 3,
    INDIRECT_PRE_DEC = 4,
    INDIRECT_PRE_INC = 5
};

/*
 * Programs a given set of memory location to a connected device my emitting
 * appropriate op codes
 */
class PIC24 {
private:

    /*
     * Represent the NOP instruction
     */
    static const uint32_t NOP = 0;

    /*
     * Contains the bit index of the WR bit which is set to initiate a write to the flash memory
     */
    static const uint32_t NVMCOM_WR_BIT = 15;

    const DEVICE &device;
    ICSP icsp;

    /*
     * Returns the upper 8 bits of a 24 bit word
     */
    uint32_t upper8(uint32_t data);

    /*
     * Returns the lower 16 bits of a 24 bit word
     */
    uint32_t lower16(uint32_t data);

    /*
     * Creates a STO instruction which writes the value of the register to the given address
     */
    uint32_t STO(REG reg, uint32_t addr);

    /*
     * Creates a RET instruction which writes the value at the given address into the given register
     */
    uint32_t RET(uint32_t addr, REG reg);

    /*
     * Creates a LDI instruction which loads the given data into the given register
     */
    uint32_t LDI(uint32_t data, REG reg);

    /*
     * Sets the given bit at the given address
     */
    uint32_t BSET(uint32_t addr, uint8_t bit);

    /*
     * Creates a TBLRDL instruction which transfers the lower 16 bits of the given memory source
     * to the given destination register. The addressing modes are determined by src_mode and dest_mode.
     */
    uint32_t TBLRDL(REG src, TBL_MODE src_mode, REG dest, TBL_MODE dest_mode);

    /*
     * Creates a TBLRDL instruction which transfers the upper 8 bits of the given memory source
     * to the given destination register. The addressing modes are determined by src_mode and dest_mode.
     */
    uint32_t TBLRDH(REG src, TBL_MODE src_mode, REG dest, TBL_MODE dest_mode);

    /*
     * Creates a TBLRDL instruction which transfers the lower 16 bits of the given source register
     * to the given memory destination. The addressing modes are determined by src_mode and dest_mode.
     */
    uint32_t TBLWTL(REG src, TBL_MODE src_mode, REG dest, TBL_MODE dest_mode);

    /*
     * Creates a TBLWTH instruction which transfers the upper 8 bits of the given source register
     * to the given memory destination. The addressing modes are determined by src_mode and dest_mode.
     */
    uint32_t TBLWTH(REG src, TBL_MODE src_mode, REG dest, TBL_MODE dest_mode);

    /*
     * Creates a TBLWTHB instruction which transfers the upper 8 bits of the given source register
     * to the given memory destination - in byte mode. The addressing modes are determined by src_mode and dest_mode.
     */
    uint32_t TBLWTHB(REG src, TBL_MODE src_mode, REG dest, TBL_MODE dest_mode);

    /*
     * Creates a JMP instruction which which basically updates the program counter.
     */
    uint32_t JMP(uint32_t addr);

    /*
     * Reads the given memory location.
     */
    uint32_t read_word(uint32_t addr);

    /*
     * Writes up to 128 words at once.
     */
    uint32_t write_128words(uint32_t addr,
                            std::vector<uint32_t>::const_iterator &iter,
                            std::vector<uint32_t>::iterator end);

    /*
     * Fetches the next value to write. Defaults to 0 (NOP) if the end of the iterator is reached.
     */
    uint32_t fetch_next(std::vector<uint32_t>::const_iterator &iter,
                        const std::vector<uint32_t>::iterator &end);


    /*
     * Writes the config words (which have to be written as single words)
     */
    void write_code_words(std::vector<uint32_t> &data);

    /*
     * Writes a single config word at the given adress
     */
    void write_config_word(uint32_t addr, uint32_t data);

    /*
     * Splits the given memory into program code and config words
     */
    void prepare_program(std::list<MemoryWord> &memory, std::vector<uint32_t> &code,
                         std::vector<MemoryWord> &configWords);

public:

    /*
     * Creates a new programmer for the given HAL and device.
     */
    PIC24(HAL &hal, const DEVICE &device);

    /*
     * Reads the device id
     */
    void read_device_id(uint16_t &higher, uint16_t &lower);

    /*
     * Erases the complete program memory
     */
    void erase_chip();

    /*
     * Writes the given memory contents to the device
     */
    void program(std::list<MemoryWord> &memory);

    /*
     * Verifies the contents on the chip against the given memory contents
     */
    void verify(std::list<MemoryWord> &memory);

};

#endif //RASPICSP_PIC24_H
