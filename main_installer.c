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
#include "mlcd.h"
#include "ext_flash.h"

uint32_t * p_spi0_base_address;
uint32_t * p_spi1_base_address;

uint8_t last_progress;

/** @brief Function for erasing a page in flash.
 *
 * @param page_address Address of the first word in the page to be erased.
 */
void flash_page_erase(uint32_t *page_address)
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

void flash_uicr_erase()
{
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos);
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
    
    // Erase uicr:
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
void flash_word_write(uint32_t *address, uint32_t value)
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
    p_spi1_base_address = spi_master_init(SPI1, SPI_MODE0, false);
}

static bool restore_from_ext_flash(uint32_t *address, uint32_t size) {
	  // force that size if multiplication of 4
	  if (size % 4 > 0) {
			  size += (4 - (size % 4));
		}
	  uint32_t end_address = (uint32_t)address + size; 
	
	   // clear pages (not required when global erase)
  /*  uint32_t pg_size = NRF_FICR->CODEPAGESIZE;
 	  uint32_t *clean_address = address; 
		while ((uint32_t)clean_address < end_address) {
		    flash_page_erase(clean_address);
			  clean_address += pg_size;
		}*/
		
		uint32_t tmp_buff[32];
	  int32_t part_size;
		while ((uint32_t)address < end_address) {
			  part_size = end_address - (uint32_t)address > 128 ? 128 : end_address - (uint32_t)address;
		    if (!ext_flash_read_data((uint32_t)address, (uint8_t *)tmp_buff, part_size)){
		    	  return false;
		    }
				
				uint32_t word_number = part_size/4;
				for (uint32_t i = 0; i < word_number; i++) {
					  flash_word_write(address++, tmp_buff[i]);
				}
		}
		return true;
}

static uint8_t progress_bg_draw_func(uint8_t x, uint8_t y)
{
	  uint32_t d = (72-x)*(72-x) + (84-y)*(84-y);
    if(d>2800 && d < 4100){
        return 1;
    }
		return 0;
}

static uint8_t progress_draw_func(uint8_t x, uint8_t y)
{
	  uint32_t d = (72-x)*(72-x) + (84-y)*(84-y);
    if(d>2800 && d < 4100){
        return 1;
    } else if (y<=125 && y>77 && x>56 && x<88) {
				if (x>64 && x<80 && y<117) {
						return 0;
				} else {
						return 1;
				}
		} else if (x+y > 110 && x-y < 34 && y<85 && !(x+y > 121 && x-y < 23 && y<78)) {
				return 1;
		}
		return 0;
}

static uint8_t error_draw_func(uint8_t x, uint8_t y)
{
		uint32_t d = (72-x)*(72-x) + (84-y)*(84-y);
    if(d>2800 && d < 4100){
        return 1;
    } else if (x+y > 146 && x+y<166 && x>30 && x < 120 && y > 40 && y<128) {
        return 1;
    }
    return 0;
}

void init_progress_notification() {
	  last_progress = 0;
	  mlcd_set_screen_with_func(progress_bg_draw_func);
	
	  mlcd_display_on();
		mlcd_backlight_on();
}

void update_progress_notification(uint8_t progress) {
	  if(progress > last_progress) {
			  mlcd_set_lines_with_func(progress_draw_func, 132u-progress, progress-last_progress);
			
				last_progress = progress;
		}
}

static void handleError(uint32_t error_code) {
	  mlcd_set_screen_with_func(error_draw_func);
	  do{}while(true);
}


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t *sd_dest_addr = (uint32_t *)0;
    uint32_t *sd_src_addr = (uint32_t *)0x19000;
	  uint32_t sd_size = 0x1D000;
    uint32_t *bl_dest_addr = (uint32_t *)0x3C000;
    uint32_t *bl_src_addr = (uint32_t *)0x15000;
	  uint32_t bl_size = 0x4000;
	
    spi_init();
    mlcd_init();
	  mlcd_power_on();
	
	  nrf_delay_us(100000);
	
	  init_progress_notification();

	  // erase all data in external flash
 	  if(!ext_flash_erase_chip()){
			 handleError(0x01);
		}
		update_progress_notification(15);
 
  	// backup bootloader in external flash
		if (!ext_flash_write_data_block((uint32_t)bl_dest_addr, (uint8_t *)bl_src_addr, bl_size)){
			  handleError(0x11);
		}
		
		update_progress_notification(25);
		
		// backup softdevice in external flash
		if (!ext_flash_write_data_block((uint32_t)sd_dest_addr, (uint8_t *)sd_src_addr, sd_size)){
			  handleError(0x12);
		}
		
		update_progress_notification(45);
		
		// verify bootloader data
		if (!ext_flash_check_equal((uint32_t)bl_dest_addr, (uint8_t *)bl_src_addr, bl_size)) {
			  handleError(0x21);
		}
		
		update_progress_notification(50);
		
		// verify softdevice data
		if (!ext_flash_check_equal((uint32_t)sd_dest_addr, (uint8_t *)sd_src_addr, sd_size)) {
			  handleError(0x22);
		}
	
		update_progress_notification(65);
		
	  // erase UICR and set new bootloader address
	  flash_uicr_erase();
		uint32_t *uicr_bl = (uint32_t *)0x10001014;
	  flash_word_write(uicr_bl, (uint32_t)bl_dest_addr);
	
		update_progress_notification(70);
		
		// restore softdevice from external cache
		restore_from_ext_flash(sd_dest_addr, sd_size);
		
		update_progress_notification(90);
	
		// restore bootloader from external cache
		restore_from_ext_flash(bl_dest_addr, bl_size);
	
		
		update_progress_notification(100);
		
	  NVIC_SystemReset();
}


/** @} */
