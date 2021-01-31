#include "main.h"

uint8_t COUNT; //����֡������

extern UART_HandleTypeDef huart1;

uint8_t checkmode=M_FRAME_CHECK_SUM; //����У�鷽ʽ

extern uint8_t UART_RX_BUF[UART_RX_LEN];
extern __IO uint16_t UART_RX_STA;

//���һ֡����
//�����ݷֱ�������ṹ���У�
//�����ں��洦���ʱ��Ϳ��Էǳ�����Ľ���������Ч����
m_result my_unpack_frame(m_frame_typedef *fx)
{
    uint16_t rxchkval=0;                //���յ���У��ֵ
    uint16_t calchkval=0;            //����õ���У��ֵ
    uint8_t datalen=0;                 //��Ч���ݳ���
    datalen=UART_RX_STA & 0X7FFF;
    
    if(datalen<5) //��֡����С��5��ʱ��(��ַ1λ��������1λ��֡���1λ�����ݳ���1λ��У����1λ)��Ϊ��һ֡���������⣬ֱ�ӷ���֡��ʽ����
    {
        UART_RX_STA=0;
        return MR_FRAME_FORMAT_ERR;  
    }
    switch(checkmode)
    {
        case M_FRAME_CHECK_SUM:                            //У���
            calchkval=mc_check_sum(UART_RX_BUF,datalen-1);
            rxchkval=UART_RX_BUF[datalen-1];
            break;
        case M_FRAME_CHECK_XOR:                            //���У��
            calchkval=mc_check_xor(UART_RX_BUF,datalen-1);
            rxchkval=UART_RX_BUF[datalen-1];
            break;
        case M_FRAME_CHECK_CRC8:                        //CRC8У��
            calchkval=mc_check_crc8(UART_RX_BUF,datalen-1);
            rxchkval=UART_RX_BUF[datalen-1];
            break;
        case M_FRAME_CHECK_CRC16:                        //CRC16У��
            calchkval=mc_check_crc16(UART_RX_BUF,datalen-2);
            rxchkval=((uint16_t)UART_RX_BUF[datalen-2]<<8)+UART_RX_BUF[datalen-1];
            break;
    }
    
    if(calchkval==rxchkval)        //У������
    {
        fx->address=UART_RX_BUF[0];
        fx->function=UART_RX_BUF[1];
        fx->count=UART_RX_BUF[2];
        fx->datalen=UART_RX_BUF[3];
        if(fx->datalen)
        {
            for(datalen=0;datalen<fx->datalen;datalen++)
            {
                fx->data[datalen]=UART_RX_BUF[4+datalen];        //��������
            }
        }
        fx->chkval=rxchkval;    //��¼У��ֵ
    }else 
    {
        UART_RX_STA=0;
        return MR_FRAME_CHECK_ERR;
    }
    UART_RX_STA=0;      //Ӧ���ڲ�����֮���������־λ
    return MR_OK;
}

//���һ֡���ݣ�������
//fx��ָ����Ҫ�����֡
void my_packsend_frame(m_frame_typedef *fx)
{
    uint16_t i;
    uint16_t calchkval=0;            //����õ���У��ֵ
    uint16_t framelen=0;                //������֡����
    uint8_t sendbuf[UART_RX_LEN];                //���ͻ�����

    if(checkmode==M_FRAME_CHECK_CRC16)framelen=6+fx->datalen;
    else framelen=5+fx->datalen;
    sendbuf[0]=fx->address;
    sendbuf[1]=fx->function;
    sendbuf[2]=fx->count;
    sendbuf[3]=fx->datalen;
    for(i=0;i<fx->datalen;i++)
    {
        sendbuf[4+i]=fx->data[i];
    }
    switch(checkmode)
    {
        case M_FRAME_CHECK_SUM:                            //У���
            calchkval=mc_check_sum(sendbuf,fx->datalen+4);
            break;
        case M_FRAME_CHECK_XOR:                            //���У��
            calchkval=mc_check_xor(sendbuf,fx->datalen+4);
            break;
        case M_FRAME_CHECK_CRC8:                        //CRC8У��
            calchkval=mc_check_crc8(sendbuf,fx->datalen+4);
            break;
        case M_FRAME_CHECK_CRC16:                        //CRC16У��
            calchkval=mc_check_crc16(sendbuf,fx->datalen+4);
            break;
    }

    if(checkmode==M_FRAME_CHECK_CRC16)        //�����CRC16,����2���ֽڵ�CRC
    {
        sendbuf[4+fx->datalen]=(calchkval>>8)&0XFF;     //���ֽ���ǰ
        sendbuf[5+fx->datalen]=calchkval&0XFF;            //���ֽ��ں�
    }else sendbuf[4+fx->datalen]=calchkval&0XFF;
    HAL_UART_Transmit_DMA(&huart1, sendbuf, framelen);     //DMA������һ֡����
}

m_result my_deal_frame(m_frame_typedef *fx)
{
    if(fx->address == MY_ADDRESS)
    {
        switch(fx->function)
        {
            case 1:
            {
                My_Func_1();
            }break;
            case 2:
            {
                //My_Func_2(fx);
            }break;
            case 3:
            {
                //My_Func_3(fx);
            }break;
            case 4:
            {
                //My_Func_4(fx);
            }break;
            case 5:
            {
                //My_Func_5(fx);
            }break;
            case 6:
            {
                //My_Func_6(fx);
            }break;
            case 7:
            {
                //My_Func_7(fx);
            }break;
            case 8:
            {
                //My_Func_8(fx);
            }break;
            default:
                return MR_FUNC_ERR;
        }
    }return MR_OK;
}

void My_Func_1(void)
{
    m_frame_typedef txbuff;
    txbuff.address=MY_ADDRESS;      //�����ַ��
    txbuff.function=1;          //�����뷵��
    txbuff.count=(COUNT++)%255; //֡��ż�һ
    txbuff.datalen=3;           //��㷢��3λ����
    txbuff.data[0]=0x01;
    txbuff.data[1]=0x02;
    txbuff.data[2]=0x03;
    my_packsend_frame(&txbuff); //�����ݴ��������
}
