#include "dht11.h"
#include "main.h"
#include "tim.h"

//uint8_t Data[5]={0x00,0x00,0x00,0x00,0x00};   //Data�洢��ȡ����ʪ����Ϣ

void my_delay_us(uint32_t us)
{
    __HAL_TIM_SET_COUNTER(&htim1, 0);
    
    HAL_TIM_Base_Start(&htim1);
    
    while (__HAL_TIM_GET_COUNTER(&htim1) != us);
    
    HAL_TIM_Base_Stop(&htim1);
}

/**
 * ************************************************************************
 * @brief ��DHT11����Ϊ�������ģʽ
 * ************************************************************************
 */
static void DHT11_PP_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = DHT11_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;	
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

/**
 * ************************************************************************
 * @brief ��DHT11����Ϊ��������ģʽ
 * ************************************************************************
 */
static void DHT11_UP_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = DHT11_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;	//����
	HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

/**
 * ************************************************************************
 * @brief ��ȡ�ֽ�
 * @return temp
 * ************************************************************************
 */
uint8_t DHT11_ReadByte(void)
{
	uint8_t i, temp = 0;

	for (i = 0; i < 8; i++)
	{
		while (DHT11_ReadPin == 0);		// �ȴ��͵�ƽ����
		
		my_delay_us(40);			//	��ʱ 40 ΢��
		
		if (DHT11_ReadPin == 1)
		{
			while (DHT11_ReadPin == 1);	// �ȴ��ߵ�ƽ����
			
			temp |= (uint8_t)(0X01 << (7 - i));			// �ȷ��͸�λ
		}
		else
		{
			temp &= (uint8_t)~(0X01 << (7 - i));
		}
	}
	return temp;
}

/**
 * ************************************************************************
 * @brief ��ȡһ������
 * @param[in] DHT11_Data  ����Ľṹ�����
 * @return 0��1������У���Ƿ�ɹ���
 * @note ��������DHT11���������źţ�Ȼ��ȴ�DHT11��Ӧ�����DHT11��ȷӦ��
 * 		 �������ȡʪ��������ʪ��С�����¶��������¶�С����У������ݣ�
 * 		 ������У����Խ�������У��
 * ************************************************************************
 */
uint8_t DHT11_ReadData(DHT11_Data_TypeDef *DHT11_Data)
{
	DHT11_PP_OUT();			// �����������������
	DHT11_PULL_0;	
	HAL_Delay(18);				// ��ʱ 18 ms
	
	DHT11_PULL_1;					// �������ߣ���ʱ 30 us
	my_delay_us(30);	

	DHT11_UP_IN();				// �������룬��ȡ DHT11 ����
	
	if (DHT11_ReadPin == 0)				// �յ��ӻ�Ӧ��
	{
		while (DHT11_ReadPin == 0);		// �ȴ��ӻ�Ӧ��ĵ͵�ƽ����
		
		while (DHT11_ReadPin == 1);		// �ȴ��ӻ�Ӧ��ĸߵ�ƽ����
		
		/*��ʼ��������*/   
		DHT11_Data->humi_int  = DHT11_ReadByte();
		DHT11_Data->humi_dec = DHT11_ReadByte();
		DHT11_Data->temp_int  = DHT11_ReadByte();
		DHT11_Data->temp_dec = DHT11_ReadByte();
		DHT11_Data->check_sum = DHT11_ReadByte();
		
		DHT11_PP_OUT();		// ��ȡ��������������
		DHT11_PULL_1;	
		
		// ����У��
		if (DHT11_Data->check_sum == DHT11_Data->humi_int + DHT11_Data->humi_dec + DHT11_Data->temp_int + DHT11_Data->temp_dec)	
		{
			return 1;
		}		
		else
		{
			return 0;
		}
	}
	else		// δ�յ��ӻ�Ӧ��
	{
		return 0;
	}
}
