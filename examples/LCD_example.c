#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "SOFTWARE_SPI_DRIVER.h"

#define SPI_OUT_GPIO 13  //SPI Controller output
#define SPI_IN_GPIO 23  //SPI input read from peripheral
#define SPI_CLK_GPIO 19  //SPI Controller clock
#define SPI_CS_GPIO 5  //SPI chip select

#define LCD_RS 0x80
#define LCD_E  0x40

/*
takes data pin values to write characters
ensures RS pin is high for writes
clocks E pin
*/
void LCD_write(controller *SPI_connection, uint8_t data){


    //MS nibble
    uint8_t send_nibble = (data >> 4);
    send_nibble |= LCD_RS | LCD_E;
    spi_send_byte(SPI_connection, send_nibble);
    send_nibble &= ~LCD_E;
    spi_send_byte(SPI_connection, send_nibble);

    //LS nibble
    send_nibble = (data & 0x0F);
    send_nibble |= LCD_RS | LCD_E;
    spi_send_byte(SPI_connection, send_nibble);
    send_nibble &= ~LCD_E;
    spi_send_byte(SPI_connection, send_nibble);


}
/*
takes data pin values to write commands
ensures RS pin is low for commands
clocks E pin
*/
void LCD_command(controller *SPI_connection, uint8_t data, int delay){

    uint8_t send_nibble = (data >> 4);
    send_nibble |= LCD_E;
    spi_send_byte(SPI_connection,send_nibble);
    send_nibble &= ~LCD_E;
    spi_send_byte(SPI_connection,send_nibble);


    send_nibble = (data & 0x0F);
    send_nibble |= LCD_E;
    spi_send_byte(SPI_connection,send_nibble);
    send_nibble &= ~LCD_E;
    spi_send_byte(SPI_connection,send_nibble);
    vTaskDelay(delay/portTICK_PERIOD_MS);

}

/*basically sends all initialization commands to set it in 4 bit mode*/
void LCD_init(controller *SPI_connection){

    //for information on commands check LCD 1602A documentation

    vTaskDelay(50/portTICK_PERIOD_MS);
    LCD_command(SPI_connection, 0x33, 5);
    LCD_command(SPI_connection, 0x32, 5);
    LCD_command(SPI_connection, 0x28, 1);
    LCD_command(SPI_connection, 0x08, 1);
    LCD_command(SPI_connection, 0x01, 10);
    LCD_command(SPI_connection, 0x06, 1);
    LCD_command(SPI_connection, 0x0C, 1);

}

void app_main(void){


    controller SPI_connection = {
    .clock_gpio      = SPI_CLK_GPIO,
    .serial_out_gpio = SPI_OUT_GPIO,
    .serial_in_gpio  = SPI_IN_GPIO,
    .chip_select_gpio = SPI_CS_GPIO,
    .CPOL            = 0,
    .CPHA            = 0,
    .half_period_us  = 1,
    };

    spi_initialize(&SPI_connection);    //configures ports and sets SPI clock to idle

    LCD_init(&SPI_connection);

    LCD_command(&SPI_connection, 0x80, 1);
    LCD_write(&SPI_connection, 'c');
    LCD_write(&SPI_connection, 'a');
    LCD_write(&SPI_connection, 't');

}
