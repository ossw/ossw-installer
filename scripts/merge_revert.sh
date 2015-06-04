srec_cat ossw-revert.hex -intel -offset - -minimum-addr ossw-revert.hex -intel -o ossw-revert.bin -binary
srec_cat ossw-revert.bin -binary -offset 0x3A000 -o ossw-revert_at_0x3A000.hex -intel

srec_cat chronos_fw.bin -binary -crop 0x0 0x14000 -o sd.bin -binary
srec_cat sd.bin -binary -offset 0x26000 -o sd_at_0x26000.hex -intel

srec_cat chronos_fw.bin -binary -crop 0x38000 0x40000 -offset -0x38000 -o bl.bin -binary
srec_cat bl.bin -binary -offset 0x1E000 -o bl_at_0x1E000.hex -intel

srec_cat ossw-revert-jump.hex -intel bl_at_0x1E000.hex -intel sd_at_0x26000.hex -intel ossw-revert_at_0x3A000.hex -intel -o ossw-revert-combined.hex -intel
srec_cat ossw-revert-combined.hex -intel -offset - -minimum-addr ossw-revert-combined.hex -intel -o ossw-revert-combined.bin -binary