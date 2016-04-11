//
// Reads a hex file which contains program code.
//

#ifndef RASPICSP_HEXFILE_H
#define RASPICSP_HEXFILE_H

#include <inttypes.h>
#include <list>
#include <istream>
#include <ostream>
#include <vector>

/*
 * Represents one line of a hex file
 */
class HexEntry {
public:
    /*
     * Contains the number of bytes in this entry
     */
    uint8_t numBytes;

    /*
     * Contains byte-address of the first byte in this entry
     */
    uint32_t addr;

    /*
     * Contains the type of the record
     */
    uint8_t type;

    /*
     * Contains the byte-data
     */
    std::vector<uint8_t> data;

    /*
     * Contains a checksum for the byte data
     */
    uint32_t checksum;
};

/*
 * Represents a 16 bit word of memory
 */
class MemoryWord {
public:
    uint32_t address;
    uint32_t data;
};

/*
 * Used to reads a hex file into a list of memory locations
 */
class HexFile {
public:

    /*
     * Parses the given stream. Returns the number of lines read or a negative number
     * to indicate the first line which contained invalid data.
     */
    int parse(std::istream &input);

    /*
     * Compiles the content of the file into a list of memory locations
     */
    void compileTo16BitWords(std::list<MemoryWord> &memory);

private:
    std::list<HexEntry> entries;
};

#endif //RASPICSP_HEXFILE_H
