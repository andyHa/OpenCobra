//
// Defines the hardware abstraction layer.
//

#ifndef RASPICSP_HAL_H
#define RASPICSP_HAL_H

#include <stdint.h>

/*
 * The Hardware Abstraction Layer contains raspberry PI specific code to drive the
 * GPIO pins in order to behave as required for the pins MCLR (reset pin), PGC (clock), PGD (data).
 */
class HAL {
private:

    static const uint32_t BCM2708_PERI_BASE = 0x20000000;
    static const uint32_t GPIO_BASE = BCM2708_PERI_BASE + 0x200000;
    static const uint32_t BLOCK_SIZE = 4096;
    static const uint32_t PERIOD_MICRO_SECONDS = 1;

    uint8_t mclr_pin;
    uint8_t pgd_pin;
    uint8_t pgc_pin;
    volatile unsigned *gpio;

    /*
     * Maps the GPIO pins into the address space of our process to gain control
     */
    void setup_io();

    /*
     * Initializes the GPIO pins as required
     */
    void setup_pins();

    /*
     * Makes the given pin an input pin
     */
    void make_input(uint8_t pin);

    /*
     * Makes the given pin an output pin
     */
    void make_output(uint8_t pin);

    /*
     * Writes a 1 (high) to the given pin
     */
    void set_pin(uint8_t pin);

    /*
     * Writes a 0 (low) to the given pin
     */
    void clear_pin(uint8_t pin);

    /*
     * Reads the value of the given pin
     */
    int read_pin(uint8_t pin);

    /*
     * Delays the execution my the given number of microseconds
     */
    void delayMicrosecondsHard(unsigned int howLong);

public:

    /*
     * Creates a new instance which uses the given GPIOs as MCLR, PGD und PGC
     */
    HAL(uint8_t mclr_pin, uint8_t pgd_pin, uint8_t pgc_pin);

    /*
     * Raises the reset pin to 1
     */
    void mclr_up();

    /*
     * Lowers the reset pin to 0
     */
    void mclr_down();

    /*
     * Enables write mode (PCD is an output)
     */
    void write_mode();

    /*
     * Enables read mode (PGD is an input)
     */
    void read_mode();

    /*
     * Writes a bit (sets PGD to 0 or 1 depending on bit and emits a plus on PGC).
     */
    void write_bit(int bit);

    /*
     * Reads a bit (reads PCD while sending a pulse on PGC)
     */
    int read_bit();

};


#endif //RASPICSP_HAL_H
