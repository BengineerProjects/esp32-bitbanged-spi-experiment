# esp32-bitbanged-spi-experiment
This project is an SPI (Serial Peripheral Interface) driver for an ESP32 using ESP-IDF. It includes all four SPI modes and does not require specialized SPI hardware. 
This was project was an experiment and was built for learning purposes and probably limited real world use. This project was designed with controlling an 74hc595 shift register in mind. 

References: 
- overview of SPI https://www.analog.com/en/resources/analog-dialogue/articles/introduction-to-spi-interface.html
- SPI timing https://www.ti.com.cn/content/dam/videos/external-videos/en-us/8/3816841626001/6246789954001.mp4/subassets/adcs-spi-communications-timing-presentation.pdf

Limitations:
- Not as fast, reliable, and uses the CPU more compared to dedicated SPI hardware
- Does not support multiple subnode/peripherals on the same bus
- Does not have right shifts
- Only has byte operations
- Only mainnode/controller implementation
