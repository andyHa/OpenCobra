//
// Contains the execution engine for sending commands and reading memory of the
// devices meing programmed.
//

#ifndef RASPICSP_ICSP_H
#define RASPICSP_ICSP_H

#include "HAL.h"
#include "devices.h"

/*
 * Contains the execution engine for the In Circuit Serial Programmer.
 *
 * Its main purpose is to send op codes to the device (called SIX-commands) and to read
 * memory contents (via the VISI register).
 */
class ICSP {
private:
    HAL &hal;
    const DEVICE &device;

    /*
     * Enters the ICSP mode by sending a strictly defined bit pattern while holding MCLR low
     */
    void enter_ICSP();

    /*
     * Sends an op code to the device
     */
    void write_SIX(uint32_t op_code);

    /*
     * Reads the contents of the VISI register
     */
    uint16_t read_VISI();

public:

    /*
     * Creates a new ICSP engine for the given HAL and device
     */
    ICSP(HAL &hal, const DEVICE &device);

    /*
     * Resets the device to exit ICSP mode
     */
    ~ICSP();

    /*
     * Fancy way of sending a SIX command to the device
     */
    ICSP &operator<<(uint32_t op_code);

    /*
     * Fancy way of receiving contents of the VISI register
     */
    ICSP &operator>>(uint16_t &visi_contents);
};


#endif //RASPICSP_ICSP_H
