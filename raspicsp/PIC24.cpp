#include "PIC24.h"
#include "Logger.h"

uint32_t PIC24::upper8(uint32_t data) {
    return (data >> 16) & 0xffu;
}

uint32_t PIC24::lower16(uint32_t data) {
    return data & 0xffffu;
}

uint32_t PIC24::STO(REG reg, uint32_t addr) {
    return 0x880000u | ((addr & 0xfffeu) << 3) | reg;
}

uint32_t PIC24::RET(uint32_t addr, REG reg) {
    return 0x800000u | ((addr & 0xfffeu) << 3) | reg;
}

uint32_t PIC24::LDI(uint32_t data, REG reg) {
    return 0x200000u | (data << 4) | reg;
}

uint32_t PIC24::JMP(uint32_t addr) {
    return 0x040000u | addr;
}

uint32_t PIC24::BSET(uint32_t addr, uint8_t bit) {
    return 0xA80000u | (addr & 0x1ffeu) | ((bit & 0xeu) << 12) | (bit & 0x1u);
}

uint32_t PIC24::TBLRDL(REG src, TBL_MODE src_mode, REG dest, TBL_MODE dest_mode) {
    return 0xBA0000u | (dest_mode << 11) | (dest << 7) | (src_mode << 4) | src;
}

uint32_t PIC24::TBLRDH(REG src, TBL_MODE src_mode, REG dest, TBL_MODE dest_mode) {
    return 0xBA8000u | (dest_mode << 11) | (dest << 7) | (src_mode << 4) | src;
}

uint32_t PIC24::TBLWTL(REG src, TBL_MODE src_mode, REG dest, TBL_MODE dest_mode) {
    return 0xBB0000u | (dest_mode << 11) | (dest << 7) | (src_mode << 4) | src;
}

uint32_t PIC24::TBLWTH(REG src, TBL_MODE src_mode, REG dest, TBL_MODE dest_mode) {
    return 0xBB8000u | (dest_mode << 11) | (dest << 7) | (src_mode << 4) | src;
}

uint32_t PIC24::TBLWTHB(REG src, TBL_MODE src_mode, REG dest, TBL_MODE dest_mode) {
    return 0xBBC000u | (dest_mode << 11) | (dest << 7) | (src_mode << 4) | src;
}


PIC24::PIC24(HAL &hal, const DEVICE &device) : icsp(ICSP(hal, device)), device(device) {
    icsp
    << NOP
    << JMP(device.START_ADDR)
    << NOP;
}

void PIC24::read_device_id(uint16_t &higher, uint16_t &lower) {
    icsp
    << NOP
    << JMP(device.START_ADDR)
    << NOP
    << LDI(upper8(device.DEVICE_ID_ADDR), W0)
    << STO(W0, device.TBLPAG_ADDR)
    << LDI(lower16(device.DEVICE_ID_ADDR), W6)
    << LDI(device.VISI_ADDR, W7)
    << NOP
    << TBLRDL(W6, INDIRECT_POST_INC, W7, INDIRECT)
    << NOP
    << NOP
    >> higher
    << NOP
    << TBLRDL(W6, INDIRECT, W7, INDIRECT)
    << NOP
    << NOP
    >> lower
    << NOP;
}

uint32_t PIC24::read_word(uint32_t addr) {
    uint16_t higher, lower;
    icsp
    << NOP
    << JMP(device.START_ADDR)
    << NOP
    << LDI(upper8(addr), W0)
    << STO(W0, device.TBLPAG_ADDR)
    << LDI(lower16(addr), W6)
    << LDI(device.VISI_ADDR, W7)
    << NOP
    << TBLRDL(W6, INDIRECT, W7, INDIRECT)
    << NOP
    << NOP
    >> lower
    << NOP
    << TBLRDH(W6, INDIRECT, W7, INDIRECT)
    << NOP
    << NOP
    >> higher
    << NOP;

    return ((higher & 0xffu) << 16) | (lower & 0xffffu);
}

void PIC24::erase_chip() {
    icsp
    << NOP
    << JMP(device.START_ADDR)
    << NOP
    << LDI(device.NVMCON_ERASE_ALL, W10)
    << STO(W10, device.NVMCON_ADDR)
    << LDI(0x0, W0)
    << STO(W0, device.TBLPAG_ADDR)
    << LDI(0x0, W0)
    << TBLWTL(W0, DIRECT, W0, INDIRECT)
    << NOP
    << NOP
    << BSET(device.NVMCON_ADDR, NVMCOM_WR_BIT)
    << NOP
    << NOP;

    while (1) {
        uint16_t visi = 0;

        icsp
        << JMP(device.START_ADDR)
        << NOP
        << RET(device.NVMCON_ADDR, W2)
        << STO(W2, device.VISI_ADDR)
        << NOP
        >> visi
        << NOP;

        if (!(visi & device.NVMCON_WRITING)) {
            return;
        }
    }
}


void PIC24::write_code_words(std::vector<uint32_t> &data) {
    std::vector<uint32_t>::const_iterator iter = data.begin();
    uint32_t addr = 0;
    while (iter != data.end()) {
        addr = write_128words(addr, iter, data.end());
    }
}

uint32_t PIC24::write_128words(uint32_t addr,
                               std::vector<uint32_t>::const_iterator &iter,
                               std::vector<uint32_t>::iterator end) {
    icsp
    << NOP
    << JMP(device.START_ADDR)
    << NOP
    << LDI(device.NVMCON_WRITE_ROW, W10)
    << STO(W10, device.NVMCON_ADDR)
    << LDI(upper8(addr), W0)
    << STO(W0, device.TBLPAG_ADDR)
    << LDI(lower16(addr), W7);

    for (uint8_t i = 0; i < 16; i++) {
        uint32_t data1 = fetch_next(iter, end);
        uint32_t data2 = fetch_next(iter, end);
        uint32_t data3 = fetch_next(iter, end);
        uint32_t data4 = fetch_next(iter, end);
        uint32_t data5 = fetch_next(iter, end);
        uint32_t data6 = fetch_next(iter, end);
        uint32_t data7 = fetch_next(iter, end);
        uint32_t data8 = fetch_next(iter, end);
        if (Logger::is_tracing()) {
            Logger::trace("PIC24",
                          "Writing (0x%06x, 0x%06x, 0x%06x, 0x%06x, 0x%06x, 0x%06x, 0x%06x, 0x%06x) to: 0x%06x", data1,
                          data2, data3, data4, data5, data6, data7, data8,
                          addr + (i * 8));
        }

        uint32_t word1 = data1;
        uint32_t word2 = ((data4 & 0xffu) << 8) | (data2 & 0xffu);
        uint32_t word3 = data3;
        uint32_t word4 = data5;
        uint32_t word5 = ((data8 & 0xffu) << 8) | (data6 & 0xffu);
        uint32_t word6 = data7;

        icsp
        << NOP
        << JMP(device.START_ADDR)
        << NOP
        << LDI(0, W6)
        << LDI(word1, W0)
        << LDI(word2, W1)
        << LDI(word3, W2)
        << LDI(word4, W3)
        << LDI(word5, W4)
        << LDI(word6, W5)
        << TBLWTL(W6, INDIRECT_POST_INC, W7, INDIRECT)
        << NOP
        << NOP
        << TBLWTHB(W6, INDIRECT_POST_INC, W7, INDIRECT_POST_INC)
        << NOP
        << NOP
        << TBLWTHB(W6, INDIRECT_POST_INC, W7, INDIRECT_PRE_INC)
        << NOP
        << NOP
        << TBLWTL(W6, INDIRECT_POST_INC, W7, INDIRECT_POST_INC)
        << NOP
        << NOP
        << TBLWTL(W6, INDIRECT_POST_INC, W7, INDIRECT)
        << NOP
        << NOP
        << TBLWTHB(W6, INDIRECT_POST_INC, W7, INDIRECT_POST_INC)
        << NOP
        << NOP
        << TBLWTHB(W6, INDIRECT_POST_INC, W7, INDIRECT_PRE_INC)
        << NOP
        << NOP
        << TBLWTL(W6, INDIRECT_POST_INC, W7, INDIRECT_POST_INC)
        << NOP
        << NOP;
    }

    icsp
    << BSET(device.NVMCON_ADDR, NVMCOM_WR_BIT)
    << NOP
    << NOP;

    while (1) {
        uint16_t visi = 0;

        icsp
        << JMP(device.START_ADDR)
        << NOP
        << RET(device.NVMCON_ADDR, W2)
        << STO(W2, device.VISI_ADDR)
        << NOP
        >> visi
        << NOP;

        if (!(visi & device.NVMCON_WRITING)) {
            break;
        }
    }

    icsp
    << JMP(device.START_ADDR)
    << NOP;

    return addr + 128;
}

uint32_t PIC24::fetch_next(std::vector<uint32_t>::const_iterator &iter,
                           const std::vector<uint32_t>::iterator &end) {
    uint32_t result = 0;
    if (iter != end) {
        result = *iter;
        iter++;
    }
    return result;
}

void PIC24::write_config_word(uint32_t addr, uint32_t data) {
    icsp
    << NOP
    << JMP(device.START_ADDR)
    << NOP
    << LDI(lower16(addr), W7)
    << LDI(device.NVMCON_WRITE_WORD, W10)
    << STO(W10, device.NVMCON_ADDR)
    << LDI(upper8(addr), W0)
    << STO(W0, device.TBLPAG_ADDR)
    << LDI(data & 0xffffu, W6)
    << NOP
    << TBLWTL(W6, DIRECT, W7, INDIRECT_POST_INC)
    << NOP
    << NOP
    << BSET(device.NVMCON_ADDR, NVMCOM_WR_BIT)
    << NOP
    << NOP;

    while (1) {
        uint16_t visi = 0;

        icsp
        << JMP(device.START_ADDR)
        << NOP
        << RET(device.NVMCON_ADDR, W2)
        << STO(W2, device.VISI_ADDR)
        << NOP
        >> visi
        << NOP;

        if (!(visi & device.NVMCON_WRITING)) {
            break;
        }
    }

    icsp
    << JMP(device.START_ADDR)
    << NOP;
}

void PIC24::prepare_program(std::list<MemoryWord> &memory, std::vector<uint32_t> &code,
                            std::vector<MemoryWord> &configWords) {
    // We can only program rows of 128 words. So we have to stay away from the row containing the config registers.
    uint32_t upper_memory_limit = device.CONFIG_WORDS_START_ADDR - (device.CONFIG_WORDS_START_ADDR % 128);
    uint32_t max_memory_location = 0;
    std::list<MemoryWord>::const_iterator iter = memory.begin();
    while (iter != memory.end()) {
        if (iter->address < upper_memory_limit) {
            if (iter->address > max_memory_location) {
                max_memory_location = iter->address;
            }
        } else if (iter->address < device.CONFIG_WORDS_START_ADDR) {
            Logger::log("PIC24F",
                        "Warning: Cannot program at 0x%06x! This within less than 128 words of the config registers! Verification will most probably fail.",
                        iter->address);
        }
        iter++;
    }

    // Fill all available and programmable code with 0x000000 which is NOP
    for (uint32_t i = 0; i <= max_memory_location; i++) {
        code.push_back(0);
    }

    for (int i = 0; i < device.NO_CONFIG_WORDS; i++) {
        MemoryWord configWord;
        configWord.address = device.CONFIG_WORDS_START_ADDR + i * 2;
        configWord.data = 0;
        configWords.push_back(configWord);
    }

    // Fill code and config registers as given in the hex file...
    iter = memory.begin();
    while (iter != memory.end()) {
        if (iter->address >= device.CONFIG_WORDS_START_ADDR &&
            iter->address <= (device.CONFIG_WORDS_START_ADDR + 2 * device.NO_CONFIG_WORDS)) {
            uint32_t offset = iter->address - device.CONFIG_WORDS_START_ADDR;
            uint32_t data = iter->data;
            if (offset % 2 == 0) {
                configWords[offset >> 1].data |= (data & 0xffffu);
            } else {
                // This is most probably not used, as the config registers only use the lower 16 bits...
                configWords[offset >> 1].data |= (data & 0xffffu) << 16;
            }
        } else {
            code[iter->address] = iter->data;
        }
        iter++;
    }
}

void PIC24::program(std::list<MemoryWord> &memory) {
    std::vector<MemoryWord> configWords;
    std::vector<uint32_t> code;

    prepare_program(memory, code, configWords);

    Logger::log("PIC24", "Programming device (%i code words and %i config words)...", code.size(), configWords.size());
    write_code_words(code);

    for (int i = 0; i < configWords.size(); i++) {
        Logger::log("PIC24", "Config word 0x%06x: 0x%06x", configWords[i].address, configWords[i].data);
        write_config_word(configWords[i].address, configWords[i].data);
    }
}

void PIC24::verify(std::list<MemoryWord> &memory) {
    Logger::log("PIC24", "Verifying %i words of memory...", memory.size());
    std::list<MemoryWord>::const_iterator iter = memory.begin();
    uint32_t current_address = 0;
    uint32_t current_data = 0;

    while (iter != memory.end()) {
        uint32_t data = 0;
        if (iter->address % 2 == 1) {
            if (iter != memory.begin() && iter->address - 1 == current_address) {
                data = (current_data >> 16) & 0xffu;
            } else {
                data = (read_word(iter->address - 1) >> 16) & 0xffu;
            }
        } else {
            current_address = iter->address;
            current_data = data = read_word(current_address);
        }

        if ((data & 0xffffu) != iter->data) {
            Logger::log("PIC24",
                        "Warning, memory does not match expected value!. Address: 0x%06x, Expected: 0x%04x, Read: 0x%04x",
                        iter->address, iter->data, data & 0xffffu);
        }
        iter++;
    }

    Logger::log("PIC24", "Verification Completed...");
}











