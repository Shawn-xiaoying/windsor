#ifndef AIR_CONDITIONE_COMMUNICATION_H
#define AIR_CONDITIONE_COMMUNICATION_H

bool  uart_send_set_air_conditioner_OnOff(bool set_cmd);
void  uart_send_read_air_conditioner_state(void);
bool  uart_send_set_air_conditioner_temper(int16_t set_valu);
void  uart_send_read_air_conditioner_temper(void);
void  uart_send_read_air_conditioner_Measured_temper(void);
bool  air_conditioner_cmd_hanlder(uint8_t *pdata,uint8_t len);

#endif
