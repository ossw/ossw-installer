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
#include "nrf.h"

__asm void StartApplication(uint32_t start_addr)
{
    LDR   R2, [R0]               ; Get App MSP.
    MSR   MSP, R2                ; Set the main stack pointer to the applications MSP.
    LDR   R3, [R0, #0x00000004]  ; Get application reset vector address.
    BX    R3                     ; No return - stack code is now activated only through SVC and plain interrupts.
    ALIGN
}

void clone_to_memory(uint32_t *src, uint32_t *dest, uint32_t length) {
	  uint32_t end_dest_addr = (uint32_t)dest + length;
	  while((uint32_t)dest < end_dest_addr) {
			  *dest++ = *src++;
		}
}
	
/**
 * @brief Function for application main entry.
 */
int main(void)
{
	  // TODO: verify image checksum
	
    uint32_t *inst_src_addr = (uint32_t *)0x36000;
    uint32_t *inst_dest_addr = (uint32_t *)0x20003000;
	  uint32_t inst_size = 0x900;
	
	  clone_to_memory(inst_src_addr, inst_dest_addr, inst_size);
	
	  StartApplication((uint32_t)inst_dest_addr);
}


/** @} */
