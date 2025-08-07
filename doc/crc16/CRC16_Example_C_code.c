#include <stdio.h>

#define PLOY  0xA001

unsigned char Test_Array[] = {0xA5,0x20,0x01,0x11,0x22,0x33,0x44,0x55,0xaa,0xbb,0xcc,0xdd,0x00,0x00,0x00,0x00};

unsigned char AC_Request_0x0003[] =  {0x18,0x03,0x00,0x03,0x00,0x01,0x76,0x03};
unsigned char AC_Response_0x0003[] = {0x18,0x03,0x02,0xFB,0x2E,0xEA,0x26};

unsigned char AC_Request_0x0009[] =  {0x18,0x06,0x00,0x09,0x00,0x00,0x56,0x01};
unsigned char AC_Response_0x0009[] = {0x18,0x03,0x02,0x00,0x01,0xEA,0x26};
 
unsigned short crc16(char *data, unsigned int size, unsigned int poly)
{  
	unsigned short crc = 0xffff;
	int flag;
	int i = 0;
	int j = 0;
	char data_t ;

	if(data ==NULL){
		return 0;
	}

	for(j=0;j<size;j++){
		data_t = *data++;
		crc = (data_t ^ (crc));
		for(i=0;i<8;i++){
			if((crc&0x1)==1){
				crc = (crc>>1) ^ poly;
			}else{
				crc >>= 1;
			}
		}
	}	
	return crc;
}



void main(void)
{
    unsigned char len;
    //CRC16 example
    printf("\n\n\n");
    printf("AC MODBUS RTU COMMUNICATION TEST \n");

    //MODBUS AC request
    len = sizeof(Test_Array);
    printf("# of bytes in total : %d\n",len);
	printf("checksum : 0x%x\n",crc16(Test_Array,len-2,PLOY));
}

