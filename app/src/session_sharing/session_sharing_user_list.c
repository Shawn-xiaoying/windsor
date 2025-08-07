#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "sdk_config.h"
#include "mt_cst_protocol.h"
#include "session_sharing_user_list.h"

#define NRF_LOG_MODULE_NAME session_sharing_user_list_module
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

uint8_t session_sharing_user_list[NRF_SDH_BLE_PERIPHERAL_LINK_COUNT][sizeof(device_infor.ShortUserID)]={0x00} ;

bool  insert_session_sharing_user(uint8_t* userid)
{
	 uint8_t (*p)[sizeof(device_infor.ShortUserID)]= session_sharing_user_list;
	 uint8_t  zero_value[sizeof(device_infor.ShortUserID)]={0x00};
	 
	 NRF_LOG_INFO("insert_session_sharing_user");	
	 if(find_session_sharing_user(userid) )
	 return true ;
	 
	 for (uint8_t i = 0; i < NRF_SDH_BLE_PERIPHERAL_LINK_COUNT; i++)
	 {
	   if( memcmp(p+i,zero_value,sizeof(device_infor.ShortUserID)) ==0 )
		 {
			   memcpy(p+i,userid,sizeof(device_infor.ShortUserID)) ;
			   return true ;
		 }
	 }
   return false ;	
}

bool  find_session_sharing_user(uint8_t* userid)
{
	 uint8_t (*p)[sizeof(device_infor.ShortUserID)]= session_sharing_user_list;
	 for (uint8_t i = 0; i < NRF_SDH_BLE_PERIPHERAL_LINK_COUNT; i++)
	 {
	   if( memcmp(p+i,userid,sizeof(device_infor.ShortUserID)) ==0 )
		 {
			   return true ;
		 }
	 }
   return false ;	
	
	
}

bool  clear_session_sharing_user(uint8_t* userid)
{
	// uint8_t (*p)[sizeof(device_infor.ShortUserID)]= session_sharing_user_list;
	//  NRF_LOG_INFO("clear_session_all_sharing_user");	
	// uint8_t  zero_value[sizeof(device_infor.ShortUserID)]={0x00};
	 for (uint8_t i = 0; i < NRF_SDH_BLE_PERIPHERAL_LINK_COUNT; i++)
	 {
		
	   if( memcmp(&session_sharing_user_list[i][0],userid,sizeof(device_infor.ShortUserID)) ==0 )
		 {
		//	     NRF_LOG_INFO("clear_session_sharing_user");	
			     memset(&session_sharing_user_list[i][0],0x00,sizeof(device_infor.ShortUserID)) ;
			
			     return true ;
		 }
	 }
   return false ;	
	
	
}

void  clear_all_sharing_user(void)
{
	uint8_t (*p)[sizeof(device_infor.ShortUserID)]= session_sharing_user_list;
	for (uint8_t i = 0; i < NRF_SDH_BLE_PERIPHERAL_LINK_COUNT; i++)
	 {
      memset(p+i,0x00,sizeof(device_infor.ShortUserID)) ;
	
	 }
	
}

void  clear_session_all_sharing_user(void)
{
	 uint8_t (*p)[sizeof(device_infor.ShortUserID)]= session_sharing_user_list;
	 for (uint8_t i = 0; i < NRF_SDH_BLE_PERIPHERAL_LINK_COUNT; i++)
	 {
	   if( memcmp(p+i,device_infor.ShortUserID,sizeof(device_infor.ShortUserID)) !=0 )
		 {
			     memset(p+i,0x00,sizeof(device_infor.ShortUserID)) ;
			    
		 }
	 }
    NRF_LOG_INFO("clear_session_all_sharing_user");	
	
}

void  print_all_sharing_user(void)
{
	 
	 for (uint8_t i = 0; i < NRF_SDH_BLE_PERIPHERAL_LINK_COUNT; i++)
	 {
	//	 NRF_LOG_INFO("userid %d",i);
	   NRF_LOG_HEXDUMP_INFO(&session_sharing_user_list[i][0], sizeof(device_infor.ShortUserID));
	 }
	
}

