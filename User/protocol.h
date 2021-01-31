#ifndef __PROTOCOL_H
#define __PROTOCOL_H
#include "main.h"

#define MY_ADDRESS 1    //������ַ
#define UART_RX_LEN 1024

//У�鷽ʽ�궨��
#define M_FRAME_CHECK_SUM	0		//У���
#define M_FRAME_CHECK_XOR	1		//���У��
#define M_FRAME_CHECK_CRC8	2		//CRC8У��
#define M_FRAME_CHECK_CRC16	3		//CRC16У��

//���ؽ�����������Ͷ���
typedef enum
{
	MR_OK=0,						//����
	MR_FRAME_FORMAT_ERR = 1,		//֡��ʽ����	 
	MR_FRAME_CHECK_ERR = 2,			//У��ֵ��λ 
	MR_FUNC_ERR = 3,				//�ڴ���� 
	MR_TIMEOUT = 4,				//ͨ�ų�ʱ
}m_result;

//֡��ʽ����
__packed typedef struct
{ 
	//uint8_t head[2];					//֡ͷ
	uint8_t address;					//�豸��ַ��1~255
	uint8_t function;				//�����룬0~255
	uint8_t count;						//֡���
	uint8_t datalen;					//��Ч���ݳ���
	uint8_t data[UART_RX_LEN];	//���ݴ洢��
	uint16_t chkval;					//У��ֵ 
	//uint8_t tail;						//֡β
}m_frame_typedef;

//extern m_protocol_dev_typedef m_ctrl_dev;			//����֡
extern uint8_t COUNT; //����֡������

m_result my_unpack_frame(m_frame_typedef *fx);
void my_packsend_frame(m_frame_typedef *fx);
m_result my_deal_frame(m_frame_typedef *fx);
void My_Func_1(void);//������1

#endif
