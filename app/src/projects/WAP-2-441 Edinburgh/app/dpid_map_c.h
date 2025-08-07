#ifndef DPID_MAP_H
#define DPID_MAP_H

typedef struct
{
    unsigned char feature_id;
    unsigned char dpid;
} FEATRUE_DPID_MAP_ST;

typedef struct
{
    unsigned char feature_id;
    void *value_adr;
} FEATRUE_SETVALUE_MAP_ST;

void feature_dpid_setValueAdr_map_init(void);
unsigned char get_dpid(unsigned char feature_id);
void *get_set_value_adr(unsigned char feature_id);
void *get_session_set_value_adr(unsigned char feature_id);

#endif
