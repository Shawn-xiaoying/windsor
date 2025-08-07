#ifndef APP_FSTORGE_H
#define APP_FSTORGE_H

void fstorge_init(void);
uint32_t fstorge_read_device_code(void);
bool fstorge_write_device_code(uint32_t device_code);

#endif
