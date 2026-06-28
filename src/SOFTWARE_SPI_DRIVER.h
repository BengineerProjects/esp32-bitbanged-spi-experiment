#include <stdint.h>

#ifndef SOFTWARE_SPI_DRIVER_H
#define SOFTWARE_SPI_DRIVER_H



typedef struct {
    int clock_gpio;
    int serial_out_gpio;    //controller output gpio pin number
    int serial_in_gpio;     //controller input gpio pin number // -1 if unused
    int chip_select_gpio;   // -1 if unused
    bool CPOL;      //clock idle polarity
    bool CPHA;      // clock phase first edge (0) or second edge (1)
    int half_period_us;     //hold time and setup time in micro

} controller;

void spi_initialize(controller *SPI_controller);

uint8_t spi_send_byte(controller *SPI_controller, uint8_t data);

#endif