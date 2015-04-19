/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#ifndef BOARDS_H
#define BOARDS_H

#include "nrf_gpio.h"

#define BUTTON_UP      20
#define BUTTON_SELECT  18
#define BUTTON_DOWN    19
#define BUTTON_BACK    17
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define SPI_OPERATING_FREQUENCY (0x02000000uL << (uint32_t)Freq_1Mbps)  /**< Slave clock frequency. */

// SPI0. 
#define SPI0_SCK     11u                                     /**< SPI clock GPIO pin number. */
#define SPI0_MOSI    12u                                     /**< SPI Master Out Slave In GPIO pin number. */
#define SPI0_MISO    9u                                      /**< SPI Master In Slave Out GPIO pin number. */
#define SPI0_SS0     8u                                      /**< SPI Slave Select GPIO pin number for device 0 */
#define SPI0_SS1     23u                                     /**< SPI Slave Select GPIO pin number for device 1 */

// SPI1. 
#define SPI1_SCK     31u        /* 15u //dev board */
#define SPI1_MOSI    28u        /* 16u //dev board */
#define SPI1_MISO    4u                                      
#define SPI1_SS0     30u   

#define LCD_VOLTAGE_REG   13u
#define LCD_ENABLE       6u
#define LCD_BACKLIGHT    7u


#define EXT_FLASH_SPI_SS        SPI0_SS0

#define TIMEOUT_COUNTER         0x3000uL                     /**< Timeout for SPI transaction in units of loop iterations. */

extern uint32_t * p_spi0_base_address;
extern uint32_t * p_spi1_base_address;

#endif
