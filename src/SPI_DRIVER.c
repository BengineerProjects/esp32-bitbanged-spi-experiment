#include "esp_rom_sys.h"
#include "driver/gpio.h"
#include "SOFTWARE_SPI_DRIVER.h"
#include <stdio.h>

#define MSB 0x80
#define LSB 0x01

/*initializes the GPIO pins and clock idle state*/
void spi_initialize(controller *SPI_controller){

    //initialize controller clock pin
    gpio_reset_pin(SPI_controller->clock_gpio);
    gpio_set_direction(SPI_controller->clock_gpio, GPIO_MODE_OUTPUT);
    gpio_set_level(SPI_controller->clock_gpio, SPI_controller->CPOL);

    //initialize controller output pin
    gpio_reset_pin(SPI_controller->serial_out_gpio);
    gpio_set_direction(SPI_controller->serial_out_gpio, GPIO_MODE_OUTPUT);

    //initialize chip select gpio pin (optional)
    if (SPI_controller->chip_select_gpio != -1){
        gpio_reset_pin(SPI_controller->chip_select_gpio);
        gpio_set_direction(SPI_controller->chip_select_gpio, GPIO_MODE_OUTPUT);
        gpio_set_level(SPI_controller->chip_select_gpio, 1);
    }
    //initialize peripheral output/controller input gpio pin (optional)
    if (SPI_controller->serial_in_gpio != -1){
        gpio_reset_pin(SPI_controller->serial_in_gpio);
        gpio_set_direction(SPI_controller->serial_in_gpio, GPIO_MODE_INPUT);
    }
    

}

/*helper function that sends bytes in mode 0 and 2*/
static uint8_t send_byte_CPHA0(controller *SPI_controller, uint8_t data){

    for(int i = 0; i < 8; i++){
        gpio_set_level(SPI_controller->serial_out_gpio, (data & MSB) != 0);    //start value change
        data <<= 1; //shift register
        esp_rom_delay_us(SPI_controller->half_period_us);  //setup time
        //read data. reads the moment before the clock edge to ensure the peripheral does not change states
        if (SPI_controller->serial_in_gpio != -1){
            data |= (gpio_get_level(SPI_controller->serial_in_gpio) & LSB);  //read peripheral data into lsb of controller register
        }
        gpio_set_level(SPI_controller->clock_gpio, !SPI_controller->CPOL);  //sampling edge
        esp_rom_delay_us(SPI_controller->half_period_us);  //hold time
        gpio_set_level(SPI_controller->clock_gpio, SPI_controller->CPOL);   //non-sampling edge
    }
    return data;

}


/*helper function that sends bytes in mode 1 and 3*/
static uint8_t send_byte_CPHA1(controller *SPI_controller, uint8_t data){

    for(int i = 0; i < 8; i++){
        
        esp_rom_delay_us(SPI_controller->half_period_us);  //hold time
        gpio_set_level(SPI_controller->serial_out_gpio, (data & MSB) != 0);    //start value change
        data <<= 1;
        gpio_set_level(SPI_controller->clock_gpio, SPI_controller->CPOL);   //non-sampling edge
        esp_rom_delay_us(SPI_controller->half_period_us);  //setup time
        if (SPI_controller->serial_in_gpio != -1){
            data |= (gpio_get_level(SPI_controller->serial_in_gpio) & LSB);  //read peripheral data
        }
        gpio_set_level(SPI_controller->clock_gpio, !SPI_controller->CPOL);  //sampling edge
        
    }
    return data;
}


/*
sends a byte in all 4 SPI modes
SPI_controller should be initialized with spi_initialize

Note that data operates like a rotating shift register
controller MSB is shifted out and peripheral MSB is shifted in 
*/
uint8_t spi_send_byte(controller *SPI_controller, uint8_t data){

    //set chip select pin low (enabled)
    if (SPI_controller->chip_select_gpio != -1){
        gpio_set_level(SPI_controller->chip_select_gpio, 0);
    }

    if (SPI_controller->CPHA == 0)
        data = send_byte_CPHA0(SPI_controller, data);
    else
        data = send_byte_CPHA1(SPI_controller, data);
   
    //set chip select pin high (disabled)
    if (SPI_controller->chip_select_gpio != -1){
        gpio_set_level(SPI_controller->chip_select_gpio, 1);
    }

    return data;
}
