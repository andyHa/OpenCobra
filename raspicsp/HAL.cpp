#include <sys/fcntl.h>
#include <sys/mman.h>
#include <stddef.h>
#include <unistd.h>
#include <exception>
#include <stdexcept>
#include <sys/time.h>
#include "HAL.h"
#include "Logger.h"

HAL::HAL(uint8_t mclr_pin, uint8_t pgd_pin, uint8_t pgc_pin) {
    Logger::log("HAL", "Starting HAL on pins %d (MCRL), %d (PGD) and %d (PGC)", mclr_pin, pgd_pin, pgc_pin);
    this->mclr_pin = mclr_pin;
    this->pgc_pin = pgc_pin;
    this->pgd_pin = pgd_pin;
#ifndef DRYRUN
    setup_io();
    setup_pins();
#endif
}

void HAL::setup_io() {
    Logger::trace("HAL", "Mapping %d bytes starting at 0x%08x into address space", BLOCK_SIZE, GPIO_BASE);
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        throw std::runtime_error("Cannot open /dev/mem");
    }

    void *gpio_map = mmap(
            NULL,
            BLOCK_SIZE,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            mem_fd,
            GPIO_BASE
    );

    close(mem_fd);

    if (gpio_map == MAP_FAILED) {
        throw std::runtime_error("Cannot map GPIO registers into local address space");
    }

    gpio = (volatile unsigned *) gpio_map;
}

void HAL::setup_pins() {
    Logger::trace("HAL", "Setting all pins as output");
    make_input(mclr_pin);
    make_output(mclr_pin);
    make_input(pgc_pin);
    make_output(pgc_pin);
    make_input(pgd_pin);
    make_output(pgd_pin);
}

/*
 * This code is taken from wirinPI (http://wiringpi.com/reference/timing/) as
 * usleep blocks up to 100us even when we only want to block 1us.
 */
void HAL::delayMicrosecondsHard(unsigned int howLong) {
    struct timeval tNow, tLong, tEnd;

    gettimeofday(&tNow, NULL);
    tLong.tv_sec = howLong / 1000000;
    tLong.tv_usec = howLong % 1000000;
    timeradd(&tNow, &tLong, &tEnd);

    while (timercmp(&tNow, &tEnd, <)) {
        gettimeofday(&tNow, NULL);
    }
}

void HAL::make_input(uint8_t pin) {
#ifndef DRYRUN
    *(gpio + (pin / 10)) &= ~(7 << ((pin % 10) * 3));
#endif
}

void HAL::make_output(uint8_t pin) {
#ifndef DRYRUN
    *(gpio + (pin / 10)) |= (1 << ((pin % 10) * 3));
#endif
}

void HAL::set_pin(uint8_t pin) {
#ifndef DRYRUN
    *(gpio + 7) = (uint8_t) (1 << pin);
#endif
}

void HAL::clear_pin(uint8_t pin) {
#ifndef DRYRUN
    *(gpio + 10) = (uint8_t) (1 << pin);
#endif
}

int HAL::read_pin(uint8_t pin) {
#ifndef DRYRUN
    return (*(gpio + 13) & (1 << pin)) ? 1 : 0;
#else
    return 0;
#endif
}

void HAL::mclr_up() {
    set_pin(mclr_pin);
}

void HAL::mclr_down() {
    clear_pin(mclr_pin);
}

void HAL::write_bit(int bit) {
    if (bit != 0) {
        set_pin(pgd_pin);
    } else {
        clear_pin(pgd_pin);
    }
    delayMicrosecondsHard(PERIOD_MICRO_SECONDS);
    set_pin(pgc_pin);
    delayMicrosecondsHard(PERIOD_MICRO_SECONDS);
    clear_pin(pgc_pin);
}

int HAL::read_bit() {
    set_pin(pgc_pin);
    delayMicrosecondsHard(PERIOD_MICRO_SECONDS);
    int result = read_pin(pgd_pin);
    clear_pin(pgc_pin);
    delayMicrosecondsHard(PERIOD_MICRO_SECONDS);

    return result;
}

void HAL::write_mode() {
    make_output(pgd_pin);
}

void HAL::read_mode() {
    make_input(pgd_pin);
}



