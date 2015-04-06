/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
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

/** @file
 * @defgroup flashwrite_example_main main.c
 * @{
 * @ingroup flashwrite_example
 *
 * @brief This file contains the source code for a sample application using the Flash Write Application.
 *
 *
 * @image html example_board_setup_a.jpg "Use board setup A for this example."
 */

#include <stdbool.h>
#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "spi.h"
#include "boards.h"

static uint32_t * p_spi0_base_address;

/** @brief Function for erasing a page in flash.
 *
 * @param page_address Address of the first word in the page to be erased.
 */
static void flash_page_erase(uint32_t *page_address)
{
  // Turn on flash erase enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos);
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
    
    // Erase page:
    NRF_NVMC->ERASEPAGE = (uint32_t)page_address;
    
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
    
    // Turn off flash erase enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
    
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
}

static void flash_uicr_erase()
{
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos);
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
    
    // Erase page:
    //NRF_NVMC->ERASEUICR = 1;
		NRF_NVMC->ERASEALL = 1;
    
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
    
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
    
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
}


/** @brief Function for filling a page in flash with a value.
 *
 * @param[in] address Address of the first word in the page to be filled.
 * @param[in] value Value to be written to flash.
 */
static void flash_word_write(uint32_t *address, uint32_t value)
{
    // Turn on flash write enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);
    
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
  
    *address = value;
  
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
  
    // Turn off flash write enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
  
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
}

/*static void clone_data(uint32_t *src, uint32_t *dest, uint32_t length) {
    uint32_t pg_size = NRF_FICR->CODEPAGESIZE;
 	  uint32_t *clean_address = dest; 
	  uint32_t dest_end_address = (uint32_t)dest + length; 
	
	  // clear pages
		while ((uint32_t)clean_address < dest_end_address) {
		    flash_page_erase(clean_address);
			  clean_address += pg_size;
		}
		
		// clone data	
	  uint32_t *read_address = src; 	
	  uint32_t *write_address = dest; 	
		uint32_t data;
		while ((uint32_t)write_address < dest_end_address) {
			  data = (uint32_t)*read_address++;
			  flash_word_write(write_address++, (uint32_t)data);
		}
}*/

/**@brief  Function for configuring the spi
 */
static void spi_init(void)
{
    p_spi0_base_address = spi_master_init(SPI0, SPI_MODE0, false);
}

static uint8_t ext_flash_read_status(uint8_t *status) {
		uint8_t command[] = {0x05};
	  return spi_master_rx_data(p_spi0_base_address, EXT_FLASH_SPI_SS, command, 1, status, 1);
}

static bool ext_flash_read_data(uint32_t ext_flash_address, uint8_t *buffer, uint32_t data_size) {
	  uint8_t command[] = {0x03, 0xFF, 0xFF, 0xFF};
		command[1] = ext_flash_address >> 16 & 0xFF;
		command[2] = ext_flash_address >> 8 & 0xFF;
		command[3] = ext_flash_address & 0xFF;
	  return spi_master_rx_data(p_spi0_base_address, EXT_FLASH_SPI_SS, command, 4, buffer, data_size);
}

static bool ext_flash_write_enable() {
	  uint8_t command[] = {0x06};
		return spi_master_tx(p_spi0_base_address, EXT_FLASH_SPI_SS, command, 1);
}

static bool ext_flash_wait_until_ready() {
		uint8_t status = 0;
	  ext_flash_read_status(&status);
		if (status & 0x01 == 0) {
			   return true;
		}
		// first wait 10us, second 100us, third and following 1000us
	  uint32t wait_time = 10;
	  do {
			  nrf_delay_us(wait_time); //wait 1 ms
	      ext_flash_read_status(&status);
			  if (wait_time < 1000) {
					  wait_time *= 10;
				}
		} while (status & 0x01);
		return true;
}

static bool ext_flash_erase_page(uint32_t page_address) {
	  bool success = ext_flash_write_enable();
	  if (success) {
			  uint8_t command[] = {0x20, 0xFF, 0xFF, 0x00};
				command[1] = page_address >> 16 & 0xFF;
				command[2] = page_address >> 8 & 0xFF;
		    success = spi_master_tx(p_spi0_base_address, EXT_FLASH_SPI_SS, command, 4);
		}
		if (success) {
			  success = ext_flash_wait_until_ready();
		}
	  return success;
}

static bool ext_flash_erase_chip() {
	  bool success = ext_flash_write_enable();
	  if (success) {
			  uint8_t command[] = {0xC7};
		    success = spi_master_tx(p_spi0_base_address, EXT_FLASH_SPI_SS, command, 1);
		}
		if (success) {
			  success = ext_flash_wait_until_ready();
		}
	  return success;
}

static bool ext_flash_write_page(int32_t ext_flash_address, uint8_t *buffer, uint32_t data_size) {
	  bool success = ext_flash_write_enable();
	  if (success) {
			  uint8_t command[] = {0x02, 0xFF, 0xFF, 0xFF};
				command[1] = ext_flash_address >> 16 & 0xFF;
				command[2] = ext_flash_address >> 8 & 0xFF;
				command[3] = ext_flash_address & 0xFF;
		    success = spi_master_tx_data(p_spi0_base_address, EXT_FLASH_SPI_SS, command, 4, buffer, data_size);
		}
		if (success) {
			  success = ext_flash_wait_until_ready();
		}
	  return success;
}

#define EXT_FLASH_PAGE_SIZE 0x100

static bool ext_flash_write_data_block(int32_t ext_flash_address, uint8_t *buffer, uint32_t data_size) {
    int32_t end_address = ext_flash_address + data_size;
	  int32_t first_page_offset = ext_flash_address & (EXT_FLASH_PAGE_SIZE-1);
	  int32_t page_size = (data_size + first_page_offset > EXT_FLASH_PAGE_SIZE) ? EXT_FLASH_PAGE_SIZE - first_page_offset : data_size;
	  bool success;
	  success = ext_flash_write_page(ext_flash_address, buffer, page_size);
	  ext_flash_address = (0xFFFFFF00&ext_flash_address) + EXT_FLASH_PAGE_SIZE;
	  buffer+=page_size;
		while (ext_flash_address < end_address) {
			  if (!success) {
					  return false;
				}
			  page_size = ((end_address - ext_flash_address) > EXT_FLASH_PAGE_SIZE) ? EXT_FLASH_PAGE_SIZE : data_size;
			  success = ext_flash_write_page(ext_flash_address, buffer, page_size);
        ext_flash_address += EXT_FLASH_PAGE_SIZE;
			  buffer+=page_size;
		}
		return success;
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
	
    uint32_t *sd_dest_addr = (uint32_t *)0;
    uint32_t *bl_dest_addr = (uint32_t *)0x3C000;
    uint32_t *sd_src_addr = (uint32_t *)0x19000;
	  uint32_t sd_size = 0x1D000;
    uint32_t *bl_src_addr = (uint32_t *)0x15000;
	  uint32_t bl_size = 0x4000;
	
    spi_init();
	
	  ext_flash_erase_chip();
 
	 /* uint32_t test_addr = 0x3C000;
	  uint8_t test_data[] = {0x01, 0x02, 0x03, 0x04, 0xFB, 0xFC, 0xFD, 0xFE};
	  uint8_t rx_data[12];
		
		ext_flash_read_data(test_addr, rx_data, 12);
		ext_flash_erase_page(test_addr);
		ext_flash_read_data(test_addr, rx_data, 12);
	  ext_flash_write_page(test_addr, test_data, 8);
		ext_flash_read_data(test_addr, rx_data, 12);
	*/
  	// copy bootloader
  	// clone_data(bl_src_addr, bl_dest_addr, bl_size);
		//clone_to_external_flash(bl_src_addr, bl_dest_addr, bl_size);
		ext_flash_write_data_block((uint32_t)bl_dest_addr, (uint8_t *)bl_src_addr, bl_size);
	
  	// copy softdevice
  	//clone_to_external_flash(sd_src_addr, sd_dest_addr, sd_size);
	
	  // set new bootloader address
/*	  flash_uicr_erase();
		uint32_t *uicr_bl = (uint32_t *)0x10001014;
	  flash_word_write(uicr_bl, (uint32_t)bl_dest_addr);
*/	
	
	do{}while(true);
	 // NVIC_SystemReset();
}


/** @} */
