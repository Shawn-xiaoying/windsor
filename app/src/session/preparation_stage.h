#ifndef _PREPARATON_STAGE_H_
#define _PREPARATON_STAGE_H_

typedef enum
{
    PREPARATION_STAGE_STOP = 0x00,
    PREPARATION_STAGE_START = 0x01,
    PREPARATION_STAGE_RUNNING = 0x02,
    PREPARATION_STAGE_NONE = 0xFF,
} preparation_stage_status;

void set_preparation_stage_status(preparation_stage_status status);
uint8_t get_preparation_features_pos(uint8_t *pos);
void preparation_stage_task(void);

#endif
