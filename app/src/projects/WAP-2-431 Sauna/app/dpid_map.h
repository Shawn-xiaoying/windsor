#ifndef DPID_MAP_H
#define DPID_MAP_H


#ifdef __cplusplus
extern "C" {
#endif
void  feature_dpid_setValueAdr_map_init(void);	
unsigned char get_dpid(unsigned char feature_id);
void* get_set_value_adr(unsigned char feature_id);
	
#ifdef __cplusplus
}
#endif


#endif
