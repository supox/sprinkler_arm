#include "I2CRoutines.h"
#include "text_buffer.h"

/* I2C ADD0 mask */
#define OAR1_ADD0_Set           ((uint16_t)0x0001)
#define OAR1_ADD0_Reset         ((uint16_t)0xFFFE)

#define I2C_IT_BUF                      ((uint16_t)0x0400)
#define I2C_IT_EVT                      ((uint16_t)0x0200)
#define I2C_IT_ERR                      ((uint16_t)0x0100)

typedef enum
{
	I2C_DIRECTION_TX,
	I2C_DIRECTION_RX
} eI2CDirection;

/* I2C SPE mask */
#define CR1_PE_Set              ((uint16_t)0x0001)
#define CR1_PE_Reset            ((uint16_t)0xFFFE)

/* I2C START mask */
#define CR1_START_Set           ((uint16_t)0x0100)
#define CR1_START_Reset         ((uint16_t)0xFEFF)

#define CR1_POS_Set           ((uint16_t)0x0800)
#define CR1_POS_Reset         ((uint16_t)0xF7FF)

/* I2C STOP mask */
#define CR1_STOP_Set            ((uint16_t)0x0200)
#define CR1_STOP_Reset          ((uint16_t)0xFDFF)

/* I2C ACK mask */
#define CR1_ACK_Set             ((uint16_t)0x0400)
#define CR1_ACK_Reset           ((uint16_t)0xFBFF)


__IO eI2CDirection I2CDirection = I2C_DIRECTION_TX;
__IO uint32_t NumberOfBytesToRead = 0;
__IO uint32_t TotalNumberOfBytes = 0;
__IO uint8_t I2CAddress = 0;
static TextBuffer I2C_ReadBuffer;
static TextBuffer I2C_WriteBuffer;
static bool bI2cInit = false;
void I2C1_EV_IRQHandler(void);
void I2C1_ER_IRQHandler(void);

TextBuffer* I2C_GetReadingBuffer(void)
{
	return &I2C_ReadBuffer;
}

I2CStatus I2C_Master_Read(const unsigned char NumByteToRead, unsigned char SlaveAddress)
{
	if(!bI2cInit)
		I2C_LowLevel_Init();

	while(I2C_IsBusy());

	TextBufferClear(&I2C_ReadBuffer);
	
	/* Enable I2C errors interrupts (used in all modes: Polling, DMA and Interrupts */
	I2C1->CR2 |= I2C_IT_ERR;

	/* Enable EVT IT*/
	I2C1->CR2 |= I2C_IT_EVT;
	/* Enable BUF IT */
	I2C1->CR2 |= I2C_IT_BUF;
	/* Set the I2C direction to reception */
	I2CDirection = I2C_DIRECTION_RX;
	SlaveAddress |= OAR1_ADD0_Set;
	I2CAddress = SlaveAddress;
		
	TotalNumberOfBytes = NumberOfBytesToRead = NumByteToRead;
	
	/* Send START condition */
	I2C1->CR1 |= CR1_START_Set;
	/* Wait until the START condition is generated on the bus: START bit is cleared by hardware */
	while ((I2C1->CR1&0x100) == 0x100);
	/* Wait until BUSY flag is reset (until a STOP is generated) */
	while (I2C_IsBusy());
	/* Enable Acknowledgement to be ready for another reception */
	I2C1->CR1 |= CR1_ACK_Set;

	return Success;
}

I2CStatus I2C_Master_Write( const unsigned char* message, const unsigned char length, uint8_t SlaveAddress )
{
	if(!bI2cInit)
		I2C_LowLevel_Init();

	while(I2C_IsBusy());
	
	TextBufferWrite(&I2C_WriteBuffer, message, length);
	
	/* Enable Error IT (used in all modes: DMA, Polling and Interrupts */
	I2C1->CR2 |= I2C_IT_ERR;

	/* Enable EVT IT*/
	I2C1->CR2 |= I2C_IT_EVT;
	/* Enable BUF IT */
	I2C1->CR2 |= I2C_IT_BUF;
	/* Set the I2C direction to Transmission */
	I2CDirection = I2C_DIRECTION_TX;
	SlaveAddress &= OAR1_ADD0_Reset;
	I2CAddress = SlaveAddress;
	/* Send START condition */
	I2C1->CR1 |= CR1_START_Set;
	/* Wait until the START condition is generated on the bus: the START bit is cleared by hardware */
	while ((I2C1->CR1&0x100) == 0x100);
	/* Wait until BUSY flag is reset: a STOP has been generated on the bus signaling the end
	of transmission */
	while (I2C_IsBusy());
	return Success;
}

void I2C_Slave_BufferReadWrite(void)
{
	if(!bI2cInit)
		I2C_LowLevel_Init();
	
	/* Enable Event IT needed for ADDR and STOPF events ITs */
	I2C1->CR2 |= I2C_IT_EVT ;
	/* Enable Error IT */
	I2C1->CR2 |= I2C_IT_ERR;

	/* Enable Buffer IT (TXE and RXNE ITs) */
	I2C1->CR2 |= I2C_IT_BUF;
}

void I2C_LowLevel_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	I2C_InitTypeDef  I2C_InitStructure;

	TextBufferInit(&I2C_WriteBuffer);
	TextBufferInit(&I2C_ReadBuffer);
	
	/* 1 bit for pre-emption priority, 3 bits for subpriority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	NVIC_SetPriority(I2C1_EV_IRQn, 0x00); 
	NVIC_EnableIRQ(I2C1_EV_IRQn);

	NVIC_SetPriority(I2C1_ER_IRQn, 0x01); 
	NVIC_EnableIRQ(I2C1_ER_IRQn);
	
	/* GPIOB clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	/* I2C1 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	/* I2C1 SDA and SCL configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Enable I2C1 reset state */
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
	/* Release I2C1 from reset state */
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);

	/* I2C1 and I2C2 configuration */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = OwnAddress1;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = I2CClockSpeed;
	I2C_Init(I2C1, &I2C_InitStructure);
	
	bI2cInit = true;
}

bool I2C_IsBusy(void)
{
	return (I2C1->SR2 & 0x2) != 0 ;
}

void I2C1_EV_IRQHandler(void)
{

	/* Read the I2C1 SR1 and SR2 status registers */
	__IO uint32_t SR1Register = I2C1->SR1;
	__IO uint32_t SR2Register = I2C1->SR2;

	/* If I2C1 is slave (MSL flag = 0) */
	if ((SR2Register &0x0001) != 0x0001)
	{
		/* If ADDR = 1: EV1 */
		if ((SR1Register & 0x0002) == 0x0002)
		{
				/* Clear SR1Register and SR2Register variables to prepare for next IT */
				SR1Register = 0;
				SR2Register = 0;
		}
		/* If TXE = 1: EV3 */
		if ((SR1Register & 0x0080) == 0x0080)
		{
				/* Write data in data register */
				I2C1->DR = TextBufferGetChar(&I2C_WriteBuffer);
				SR1Register = 0;
				SR2Register = 0;
		}
		/* If RXNE = 1: EV2 */
		if ((SR1Register & 0x0040) == 0x0040)
		{
				/* Read data from data register */
				TextBufferPutChar(&I2C_ReadBuffer, I2C1->DR);
				
				SR1Register = 0;
				SR2Register = 0;

		}
		/* If STOPF =1: EV4 (Slave has detected a STOP condition on the bus */
		if (( SR1Register & 0x0010) == 0x0010)
		{
				I2C1->CR1 |= CR1_PE_Set;
				SR1Register = 0;
				SR2Register = 0;

		}
	} /* End slave mode */

	/* If SB = 1, I2C1 master sent a START on the bus: EV5) */
	if ((SR1Register &0x0001) == 0x0001)
	{
		/* Send the slave address for transmssion or for reception (according to the configured value
				in the write master write routine */
		I2C1->DR = I2CAddress;
		SR1Register = 0;
		SR2Register = 0;
	}
	/* If I2C1 is Master (MSL flag = 1) */

	if ((SR2Register &0x0001) == 0x0001)
	{
		/* If ADDR = 1, EV6 */
		if ((SR1Register &0x0002) == 0x0002)
		{
				/* Write the first data in case the Master is Transmitter */
				if (I2CDirection == I2C_DIRECTION_TX)
				{
						/* Initialize the Transmit counter */
						/* Write the first data in the data register */
						I2C1->DR = TextBufferGetChar(&I2C_WriteBuffer);
						/* If no further data to be sent, disable the I2C BUF IT
						in order to not have a TxE  interrupt */
						if (TextBufferNumberOfAvailableBytes(&I2C_WriteBuffer) == 0)
						{
								I2C1->CR2 &= (uint16_t)~I2C_IT_BUF;
						}
				}
				/* Master Receiver */
				else
				{
						/* Initialize Receive counter */
						/* At this stage, ADDR is cleared because both SR1 and SR2 were read.*/
						/* EV6_1: used for single byte reception. The ACK disable and the STOP
						Programming should be done just after ADDR is cleared. */
						if(TotalNumberOfBytes == 2) {
								/* Clear ACK */
								I2C1->CR1 &= CR1_ACK_Reset;							
						}
						if (TotalNumberOfBytes == 1)
						{
								/* Clear ACK */
								I2C1->CR1 &= CR1_ACK_Reset;
								/* Program the STOP */
								I2C1->CR1 |= CR1_STOP_Set;
						}
				}
				SR1Register = 0;
				SR2Register = 0;

		}
		/* Master transmits the remaing data: from data2 until the last one.  */
		/* If TXE is set */
		if ((SR1Register &0x0084) == 0x0080)
		{
				/* If there is still data to write */
				if (TextBufferNumberOfAvailableBytes(&I2C_WriteBuffer)!=0)
				{
						/* Write the data in DR register */
						I2C1->DR = TextBufferGetChar(&I2C_WriteBuffer);
						/* If  no data remains to write, disable the BUF IT in order
						to not have again a TxE interrupt. */
						if (TextBufferNumberOfAvailableBytes(&I2C_WriteBuffer) == 0)
						{
								/* Disable the BUF IT */
								I2C1->CR2 &= (uint16_t)~I2C_IT_BUF;
						}
				}
				SR1Register = 0;
				SR2Register = 0;
		}
		/* If BTF and TXE are set (EV8_2), program the STOP */
		if ((SR1Register &0x0084) == 0x0084)
		{

				/* Program the STOP */
				I2C1->CR1 |= CR1_STOP_Set;
				/* Disable EVT IT In order to not have again a BTF IT */
				I2C1->CR2 &= (uint16_t)~I2C_IT_EVT;
				SR1Register = 0;
				SR2Register = 0;
		}
		/* If RXNE is set */
		if ((SR1Register &0x0040) == 0x0040)
		{
				if(TotalNumberOfBytes == 2) {
					unsigned char i;
					/* Clear ACK */
					I2C1->CR1 &= CR1_ACK_Reset;
					/* Program the STOP */
					I2C1->CR1 |= CR1_STOP_Set;
					
					// Read the two byte
					for(i = 0 ; i < 2 ; i++)
					{
						unsigned char received_byte = I2C1->DR;				
						TextBufferPutChar(&I2C_ReadBuffer, received_byte);
				
						NumberOfBytesToRead--;
					}
					return;
				}
				else {
					/* Read the data register */
					unsigned char received_byte = I2C1->DR;				
					TextBufferPutChar(&I2C_ReadBuffer, received_byte);
								
					/* Decrement the number of bytes to be read */
					NumberOfBytesToRead--;
					/* If it remains only one byte to read, disable ACK and program the STOP (EV7_1) */
					if (NumberOfBytesToRead == 1)
					{
							/* Clear ACK */
							I2C1->CR1 &= CR1_ACK_Reset;
							/* Program the STOP */
							I2C1->CR1 |= CR1_STOP_Set;
					}
					SR1Register = 0;
					SR2Register = 0;
				}
		}

	}
}

void I2C1_ER_IRQHandler(void)
{
	__IO uint32_t SR1Register =0;

	/* Read the I2C1 status register */
	SR1Register = I2C1->SR1;
	/* If AF = 1 */
	if ((SR1Register & 0x0400) == 0x0400)
	{
			I2C1->SR1 &= 0xFBFF;
			SR1Register = 0;
	}
	/* If ARLO = 1 */
	if ((SR1Register & 0x0200) == 0x0200)
	{
			I2C1->SR1 &= 0xFBFF;
			SR1Register = 0;
	}
	/* If BERR = 1 */
	if ((SR1Register & 0x0100) == 0x0100)
	{
			I2C1->SR1 &= 0xFEFF;
			SR1Register = 0;
	}

	/* If OVR = 1 */
	if ((SR1Register & 0x0800) == 0x0800)
	{
			I2C1->SR1 &= 0xF7FF;
			SR1Register = 0;
	}
}
