#include "HexFile.h"
#include <iomanip>

int HexFile::parse(std::istream &input) {
    int line_number = 0;
    while (!input.eof()) {
        std::string line;
        line_number++;
        std::getline(input, line);
        if (line.substr(0, 1) != ":") {
            if (line != "") {
                return -line_number;
            }
        } else {
            HexEntry entry;
            entry.numBytes = (uint8_t) std::stoul(line.substr(1, 2), 0, 16);
            entry.addr = (uint32_t) std::stoul(line.substr(3, 4), 0, 16);
            entry.type = (uint8_t) std::stoul(line.substr(7, 2), 0, 16);
            unsigned long position = 9;
            for (int i = 0; i < entry.numBytes; i++) {
                entry.data.push_back((uint8_t) std::stoul(line.substr(position, 2), 0, 16));
                position += 2;
            }
            entry.checksum = (uint32_t) std::stoul(line.substr(position, 2), 0, 16);
            entries.push_back(entry);
        }
    }

    return (int) entries.size();
}

bool compareMemoryEntry(const MemoryWord &left, const MemoryWord &right) {
    return left.address < right.address;
}

void HexFile::compileTo16BitWords(std::list<MemoryWord> &memory) {
    memory.clear();
    uint32_t current_upper = 0;
    std::list<HexEntry>::const_iterator iterator;
    for (iterator = entries.begin(); iterator != entries.end(); ++iterator) {
        HexEntry const entry = *iterator;

        if (entry.type == 4) {
            current_upper = ((entry.data[0] & 0xFFu) << 8) | (entry.data[1] & 0xFFu);
        } else if (entry.type == 0) {
            int idx = 0;
            uint32_t base_address = ((current_upper & 0xFFFFu) << 16 | entry.addr) >> 1;
            while (idx < entry.numBytes) {
                MemoryWord word;
                word.address = base_address;
                word.data = ((entry.data[idx + 1] & 0xFFu) << 8) | (entry.data[idx] & 0xFFu);
                idx += 2;
                base_address++;
                memory.push_back(word);
            }
        }
    }

    memory.sort(compareMemoryEntry);
}
