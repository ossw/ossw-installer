srec_cat ossw-installer.hex -intel -offset - -minimum-addr ossw-installer.hex -intel -o ossw-installer.bin -binary
srec_cat ossw-installer.bin -binary -offset 0x36000 -o ossw-installer_at_0x36000.hex -intel

srec_cat s120_nrf51_2.0.0_softdevice.hex -intel -offset - -minimum-addr s120_nrf51_2.0.0_softdevice.hex -intel -o s120_nrf51_2.0.0_softdevice.bin -binary
srec_cat s120_nrf51_2.0.0_softdevice.bin -binary -offset 0x19000 -o s120_at_0x19000.hex -intel

srec_cat ossw-bootloader-s120.hex -intel -exclude 0x10001000 0x100010fc -offset - -minimum-addr ossw-bootloader-s120.hex -intel -o ossw-bootloader-s120.bin -binary
srec_cat ossw-bootloader-s120.bin -binary -offset 0x15000 -o ossw-bootloader-s120_at_0x15000.hex -intel

srec_cat ossw-jump.hex -intel ossw-bootloader-s120_at_0x15000.hex -intel s120_at_0x19000.hex -intel ossw-installer_at_0x36000.hex -intel -o combined.hex -intel
srec_cat combined.hex -intel -offset - -minimum-addr combined.hex -intel -o ossw-combined.bin -binary