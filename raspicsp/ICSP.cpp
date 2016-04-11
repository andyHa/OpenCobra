#include <unistd.h>
#include "ICSP.h"
#include "Logger.h"

void ICSP::enter_ICSP() {
    Logger::log("ICSP", "Entering ICSP mode");
    Logger::trace("ICSP", "Entering Konami Code: 0x%08x (%d bits)", device.ICSP_CODE, device.ICSP_CODE_LENGTH);

    hal.mclr_up();
    usleep(100);
    hal.mclr_down();
    usleep(100);

    uint32_t bit = 1u << (device.ICSP_CODE_LENGTH - 1);
    for (uint8_t i = 0; i < device.ICSP_CODE_LENGTH; i++) {
        hal.write_bit((device.ICSP_CODE & bit) > 0 ? 1 : 0);
        bit = bit >> 1;
    }

    usleep(20000);
    hal.mclr_up();
    usleep(50000);

    hal.write_bit(0);
    hal.write_bit(0);
    hal.write_bit(0);
    hal.write_bit(0);
    hal.write_bit(0);
}

void ICSP::write_SIX(uint32_t op_code) {
    if (Logger::is_tracing()) {
        Logger::trace("ICSP", "<< 0x%06x", op_code);
    }
    hal.write_bit(0);
    hal.write_bit(0);
    hal.write_bit(0);
    hal.write_bit(0);
    uint32_t bit = 1;
    for (uint8_t i = 0; i < 24; i++) {
        hal.write_bit((op_code & bit) > 0 ? 1 : 0);
        bit = bit << 1;
    }
}

uint16_t ICSP::read_VISI() {
    hal.write_bit(1);
    hal.write_bit(0);
    hal.write_bit(0);
    hal.write_bit(0);

    hal.write_bit(0);
    hal.write_bit(0);
    hal.write_bit(0);
    hal.write_bit(0);
    hal.write_bit(0);
    hal.write_bit(0);
    hal.write_bit(0);
    hal.write_bit(0);

    hal.read_mode();

    uint16_t result = 0;
    uint16_t bit = 1;
    for (uint8_t i = 0; i < 16; i++) {
        if (hal.read_bit()) {
            result |= bit;
        }
        bit = bit << 1;
    }

    hal.write_mode();

    if (Logger::is_tracing()) {
        Logger::trace("ICSP", ">> 0x%04x", result);
    }

    return result;
}

ICSP::ICSP(HAL &hal, const DEVICE &device) : hal(hal), device(device) {
    enter_ICSP();
}

ICSP &ICSP::operator<<(uint32_t op_code) {
    write_SIX(op_code);
    return *this;
}

ICSP &ICSP::operator>>(uint16_t &visi_contents) {
    visi_contents = read_VISI();
    return *this;
}

ICSP::~ICSP() {
    hal.mclr_down();
    usleep(5000);
    hal.mclr_up();
}


