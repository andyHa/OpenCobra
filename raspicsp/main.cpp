#include <iostream>
#include <fstream>
#include <iomanip>
#include "HAL.h"
#include "PIC24.h"
#include "Logger.h"

#define MCRL_PIN 2
#define PGC_PIN 3
#define PGD_PIN 4

using namespace std;

HAL hal(MCRL_PIN, PGD_PIN, PGC_PIN);


/**
 * Tries to find a device with the given name
 */
int findDevice(char *name, DEVICE &dev) {
    for (int i = 0; i < NUM_DEVICES; i++) {
        if (strcmp(name, DEVICES[i].NAME)) {
            dev = DEVICES[i];
            return 1;
        }
    }
    return 0;
}

void readHexFile(const char *name, std::list<MemoryWord> &mem) {
    HexFile file;
    std::fstream in;
    in.open(name, std::fstream::in);
    file.parse(in);
    file.compileTo16BitWords(mem);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: raspicsp <device> <hexfile>\n");
        return 1;
    }

    DEVICE dev;
    if (!findDevice(argv[0], dev)) {
        printf("Unknown device: %s\n\nKnown devices:\n", argv[0]);
        for (int i = 0; i < NUM_DEVICES; i++) {
            printf(" * %s\n", DEVICES[i].NAME);
        }
        return 2;
    }

    PIC24 pgm(hal, dev);
    std::list<MemoryWord> mem;
    readHexFile(argv[1], mem);

    uint16_t lo, hi;
    pgm.read_device_id(lo, hi);
    Logger::log("main", "Device ID is: 0x%04x 0x%04x", lo, hi);

    Logger::log("main", "Erasing all program memory...");
    pgm.erase_chip();

    Logger::log("main", "Programming device...");
    pgm.program(mem);

    Logger::log("main", "Verifying memory...");
    pgm.verify(mem);

    return 0;
}