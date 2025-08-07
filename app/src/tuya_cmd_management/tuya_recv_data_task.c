// #define SYSTEM_GLOBAL
// #include "mcu.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "tuya_cmd_send_task.h"
#include "upload_up_cmd.h"

#define NRF_LOG_MODULE_NAME tuya_recv_data_task
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

static func_rev_cb_prt recv_ack_callback = NULL;
static func_recv_cb_error_ptr recv_ack_error_callback = NULL;
static func_dpid_upload_cb_ptr dpid_upload_callback = proactively_upload_callback;

extern const DOWNLOAD_CMD_S upload_cmd[];

void set_recv_callback(func_rev_cb_prt fun)
{
    recv_ack_callback = fun;
}

void set_recv_error_callback(func_recv_cb_error_ptr fun)
{
    recv_ack_error_callback = fun;
}

void set_dpid_upload_callback(func_dpid_upload_cb_ptr fun)
{
    dpid_upload_callback = fun;
}
void proactively_upload_error_callback()
{
}
#ifdef TUYA_DPID_TWO_BYTE 

static unsigned char get_upload_dpid_index(unsigned short dpid)
#else 

static unsigned char get_upload_dpid_index(unsigned char dpid)
#endif 
{
    unsigned char index;
    unsigned char total = get_upload_cmd_total();

    //	  NRF_LOG_INFO("upload cmd total=%d",total);
    for (index = 0; index < total; index++)
    {
        if (upload_cmd[index].dp_id == dpid)
        {
            break;
        }
    }
    //    NRF_LOG_INFO("get upload cmd index=%d",index);
    return index;
}

static unsigned char data_point_handle(const unsigned char value[])
{
	
	#ifdef TUYA_DPID_TWO_BYTE 
	  unsigned short dp_id;
		unsigned char	index;
    unsigned char dp_type;
    unsigned char ret;
    unsigned short dp_len;
	
    dp_id   = value[0]*0x100+value[1];
    dp_type = value[2];
    dp_len =  value[3] * 0x100;
    dp_len += value[4];

//    index = get_upload_dpid_index(dp_id);

//    if (dp_type != upload_cmd[index].dp_type)
//    {
//  //      NRF_LOG_INFO("non-existen upload cmd: id= %d  dp_type=%d", dp_id, dp_type);
//        return SUCCESS;
//    }
//    else
//    {

        dpid_upload_callback(&ao_tuya_cmd_send_task, dp_id, value + 5, dp_len);
        ret = dp_upload_handle_have_cb(dp_id,dp_type,value + 5, dp_len);
 //   }

  #else 	
    unsigned char dp_id, index;
    unsigned char dp_type;
    unsigned char ret;
    unsigned short dp_len;
	
    dp_id = value[0];
    dp_type = value[1];
    dp_len = value[2] * 0x100;
    dp_len += value[3];

    index = get_upload_dpid_index(dp_id);

    if (dp_type != upload_cmd[index].dp_type)
    {
  //      NRF_LOG_INFO("non-existen upload cmd: id= %d  dp_type=%d", dp_id, dp_type);
        return SUCCESS;
    }
    else
    {

        dpid_upload_callback(&ao_tuya_cmd_send_task, dp_id, value + 4, dp_len);
        ret = dp_upload_handle_have_cb(dp_id, value + 4, dp_len);
    }
   #endif 
		
    return ret;
}

static void data_handle(unsigned short offset)
{

    unsigned short dp_len;

    unsigned char ret;
    unsigned short i, total_len;
    unsigned char cmd_type = bluetooth_data_process_buf[offset + FRAME_TYPE];

    switch (cmd_type)
    {
    case GET_BLUETOOTH_STATUS_CMD:
        send_bluetooth_status_to_mcu();
        break;
    case STATE_UPLOAD_CMD:
        total_len = (bluetooth_data_process_buf[offset + LENGTH_HIGH] << 8) | bluetooth_data_process_buf[offset + LENGTH_LOW];
			//	NRF_LOG_INFO("ata_handle total len=%d", total_len);
        for (i = 0; i < total_len;)
        {
					#ifdef TUYA_DPID_TWO_BYTE 
					  dp_len = bluetooth_data_process_buf[offset + DATA_START + i + 3] * 0x100;
            dp_len += bluetooth_data_process_buf[offset + DATA_START + i + 4];
					
				//	  NRF_LOG_INFO("ata_handle dp_len=%d", dp_len);
					  ret = data_point_handle((unsigned char *)bluetooth_data_process_buf + offset + DATA_START + i);

            if (SUCCESS == ret)
            {
            }
            else
            {
            }

            i += (dp_len + 5);
					#else 
            dp_len = bluetooth_data_process_buf[offset + DATA_START + i + 2] * 0x100;
            dp_len += bluetooth_data_process_buf[offset + DATA_START + i + 3];
					  ret = data_point_handle((unsigned char *)bluetooth_data_process_buf + offset + DATA_START + i);

            if (SUCCESS == ret)
            {
            }
            else
            {
            }

            i += (dp_len + 4);
					#endif 

           
        }
        break;
    default:
        if (recv_ack_callback != NULL)
            recv_ack_callback(&ao_tuya_cmd_send_task, cmd_type, offset, (unsigned char *)bluetooth_data_process_buf);
        break;
    }
}

void tuya_recv_data_task(void)
{
    static unsigned short rx_in = 0;
    unsigned short offset = 0;
    unsigned short rx_value_len = 0;

    while ((rx_in < sizeof(bluetooth_data_process_buf)) && with_data_rxbuff() > 0)
    {
        bluetooth_data_process_buf[rx_in++] = take_byte_rxbuff();
    }

    if (rx_in < PROTOCOL_HEAD)
        return;

    while ((rx_in - offset) >= PROTOCOL_HEAD)
    {

        if (bluetooth_data_process_buf[offset + HEAD_FIRST] != FRAME_FIRST)
        {
            offset++;

            continue;
        }

        if (bluetooth_data_process_buf[offset + HEAD_SECOND] != FRAME_SECOND)
        {
            offset++;

            continue;
        }

        if (bluetooth_data_process_buf[offset + PROTOCOL_VERSION] != BLUETOOTH_RX_VER)
        {
            offset += 2;
            continue;
        }

        rx_value_len = bluetooth_data_process_buf[offset + LENGTH_HIGH] * 0x100;
        rx_value_len += (bluetooth_data_process_buf[offset + LENGTH_LOW] + PROTOCOL_HEAD);
        if (rx_value_len > sizeof(bluetooth_data_process_buf) + PROTOCOL_HEAD)
        {
            offset += 3;

            continue;
        }

        if ((rx_in - offset) < rx_value_len)
        {
            break;
        }

        if (get_check_sum((unsigned char *)bluetooth_data_process_buf + offset, rx_value_len - 1) != bluetooth_data_process_buf[offset + rx_value_len - 1])
        {
            offset += 3;
            if (recv_ack_error_callback != NULL)
                recv_ack_error_callback(&ao_tuya_cmd_send_task);
            else
                proactively_upload_error_callback();

            rx_in = 0;
            return;
            //  continue;
        }

        data_handle(offset);
        offset += rx_value_len;
    } // end while

    rx_in -= offset;
    if (rx_in > 0)
    {
        my_memcpy((char *)bluetooth_data_process_buf, (const char *)bluetooth_data_process_buf + offset, rx_in);
    }
}

void clear_recv_callbak(void)
{
    set_recv_callback(NULL);
    set_recv_error_callback(NULL);
    set_dpid_upload_callback(proactively_upload_callback);
}
