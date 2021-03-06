#include <stdio.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "meta_adc.h"
#include "WM2Linux.h"

#define TEST_PMIC_PRINT 	_IO('k', 0)
#define PMIC_READ 				_IOW('k', 1, int)
#define PMIC_WRITE 				_IOW('k', 2, int)
#define SET_PMIC_LCDBK 		_IOW('k', 3, int)
#define ADC_CHANNEL_READ 	_IOW('k', 4, int)

int meta_adc_fd=0;

/*Input : ChannelNUm, Counts*/ 
/*Output : Sum, Result (0:success, 1:failed)*/
int adc_in_data[2] = {1,1}; 

int adc_out_data[2] = {1,1}; 

void Meta_AUXADC_OP(AUXADC_REQ *req, char *peer_buff, unsigned short peer_len)
{
	AUXADC_CNF ADCMetaReturn;
	int ret;

	memset(&ADCMetaReturn, 0, sizeof(ADCMetaReturn));

	ADCMetaReturn.header.id=req->header.id+1;
	ADCMetaReturn.header.token=req->header.token;
	ADCMetaReturn.status=META_SUCCESS;
	ADCMetaReturn.ADCStatus=TRUE;
	
	/* open file */
	meta_adc_fd = open("/dev/mtk-adc-cali",O_RDWR, 0);
	if (meta_adc_fd == -1) {
		printf("Open /dev/mtk-adc-cali : ERROR \n");
		META_LOG("Open /dev/mtk-adc-cali : ERROR \n");
		ADCMetaReturn.status=META_FAILED;
		goto ADC_Finish;
	}
	
	adc_in_data[0] = req->dwChannel;
	adc_in_data[1] = req->dwCount;
	ret = ioctl(meta_adc_fd, ADC_CHANNEL_READ, adc_in_data);
	if (ret == -1)
	{
		ADCMetaReturn.dwData = 0;
		ADCMetaReturn.status = META_FAILED;
	}
	else
	{
		if (adc_in_data[1]==0) { 
			ADCMetaReturn.dwData = adc_in_data[0];
			ADCMetaReturn.status = META_SUCCESS;
		}
		else {
			ADCMetaReturn.dwData = 0;
			ADCMetaReturn.status = META_FAILED;
		}
	}

	printf("Meta_AUXADC_OP : CH[%d] in %d times : %d\n", req->dwChannel, req->dwCount, adc_in_data[0]);	
	META_LOG("Meta_AUXADC_OP : CH[%d] in %d times : %d\n", req->dwChannel, req->dwCount, adc_in_data[0]);	
	META_LOG("Meta_AUXADC_OP : CH[%d] in %d times : %d : dwdata\n", req->dwChannel, req->dwCount, ADCMetaReturn.dwData);
	
	close(meta_adc_fd);

ADC_Finish:
	if (false == WriteDataToPC(&ADCMetaReturn,sizeof(ADCMetaReturn),NULL,0)) {
        printf("Meta_AUXADC_OP : WriteDataToPC() fail 2\n");
		META_LOG("Meta_AUXADC_OP : WriteDataToPC() fail 2\n");
    }
	printf("Meta_AUXADC_OP : Finish !\n");
	META_LOG("Meta_AUXADC_OP : Finish !\n");
	
}

BOOL Meta_AUXADC_Init(void)
{
	return true;
}

BOOL Meta_AUXADC_Deinit(void)
{
	return true;
}

