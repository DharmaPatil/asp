
#include "LCD.h"
#include "LCD_cfg.h"
#include <stdbool.h>


#define LCD_BYTES_X  			30 			//��ʾ�����
#define TEXT_HOME_ADDRESS 		0x0000 		//�ı���ʾ���׵�ַ
#define GRAPHIC_HOME_ADDRESS 	0x01E0 		//ͼ����ʾ���׵�ַ

#define	GPIO_SET(x)		{LCD_##x##_PORT -> BSRRL = LCD_##x##_PIN;}
#define	GPIO_RESET(x)	{LCD_##x##_PORT -> BSRRH = LCD_##x##_PIN;}

#define	LCD_WRITE_DATA(data)	LCD_DATA1_PORT -> BSRRH = 0x00FF;LCD_DATA1_PORT -> BSRRL = (data) ;		//д����
#define	LCD_SET_DATA_OUT()		LCD_DATA1_PORT->MODER |= 0x00005555; 									//����������Ϊ���ģʽ
#define	LCD_SET_DATA_IN()	    LCD_DATA1_PORT->MODER &= 0xffff0000; 									//����������Ϊ����ģʽ

#define SdCmd	LCD_WriteCommand
#define SdData	LCD_WriteData

#define	RdData

// ����XΪRAM��ַ���꣬X��ַ����Ϊ3������
// ����YΪ���ص�����
// *pstrΪ�����ַ��������飬��Ӧ�ֿ�ΪCCTAB
const unsigned char abcde[32] = {
0x00,0x04,0x7F,0xFE,0x40,0x24,0x5F,0xF4,
      0x41,0x04,0x41,0x04,0x41,0x44,0x4F,0xE4,
      0x41,0x04,0x41,0x44,0x41,0x24,0x41,0x04,
      0x5F,0xF4,0x40,0x04,0x7F,0xFC,0x40,0x04	
};
/****************************************************************************
* ��	�ƣ�u8 fLCD_ReadLEDstate(void)
* ��	�ܣ�������״̬
* ��ڲ�����
* ���ڲ�����u8	1:�����Ѵ򿪣�	0�������ѹر�
* ˵	������
****************************************************************************/
u8 fLCD_ReadLEDstate(void)
{
	return GPIO_ReadOutputDataBit(LCD_LED_PORT , LCD_LED_PIN);
}
/****************************************************************************
* ��	�ƣ�void fLCD_LEDON(void)
* ��	�ܣ���LCD����
* ��ڲ�����
* ���ڲ�����
* ˵	������
****************************************************************************/
void fLCD_LEDON(void)
{
	GPIO_SET(LED);
}
/****************************************************************************
* ��	�ƣ�void fLCD_LEDOFF(void)
* ��	�ܣ��ر�LCD����
* ��ڲ�����
* ���ڲ�����
* ˵	������
****************************************************************************/
void fLCD_LEDOFF(void)
{
	GPIO_RESET(LED);
}


static void WhileDly(u32 nDly)
{
	while(nDly--);
}

static void LCD_PortInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);		//��������ʱ��G
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);		//��������ʱ��G

 	//���ö�д����Ϊ�������
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;  
	GPIO_InitStructure.GPIO_Pin   = LCD_WR_PIN|LCD_RD_PIN|LCD_CE_PIN|LCD_CD_PIN|LCD_RST_PIN|LCD_FS_PIN|LCD_LED_PIN/
									LCD_DATA1_PIN|LCD_DATA2_PIN|LCD_DATA3_PIN|LCD_DATA4_PIN|LCD_DATA5_PIN|LCD_DATA6_PIN|LCD_DATA7_PIN|LCD_DATA8_PIN;		//LCD_WR
	GPIO_Init(LCD_WR_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = LCD_AD_CTRL_PIN ;					
	GPIO_Init(LCD_AD_CTRL_PORT, &GPIO_InitStructure);	
					
	GPIO_RESET(RST);
	
	GPIO_SET(CE);
	GPIO_SET(WR);
	GPIO_SET(AD_CTRL);
	GPIO_SET(RD);
	GPIO_SET(CD);
	GPIO_RESET(FS);		//ʹ��8*8ģʽ
	GPIO_SET(LED);		//�򿪱���
}

/****************************************************************************
* ��	�ƣ�	void LCD_WriteCommand(uint8 nCmd)
* ��	�ܣ�	дLCD����
* ��ڲ�����u8	����
* ���ڲ�������
* ˵	����	��
****************************************************************************/
void LCD_WriteCommand(unsigned char nCmd)
{
	LCD_SET_DATA_OUT()				/* �������������Ϊ�����ʽ */
	GPIO_RESET(CE);
	GPIO_RESET(CD);
	GPIO_RESET(WR);
	LCD_WRITE_DATA(nCmd);			//��MCUд�����ݣ�׼����LCD�������
	WhileDly(1);
	GPIO_SET(WR);
	GPIO_SET(CE);
	LCD_SET_DATA_IN();				/* �������������Ϊ���뷽ʽ */
}

void LCD_WriteData(unsigned char nData)
{
	LCD_SET_DATA_OUT();				/* �������������Ϊ�����ʽ */
	GPIO_RESET(CE);					//GPIO_RESET(CE);                               
	GPIO_SET(CD);                   //GPIO_RESET(CD);                   
	GPIO_RESET(WR);                 //GPIO_RESET(WR);                        
	LCD_WRITE_DATA(nData);		    //LCD_WRITE_DATA(nCmd);	
	WhileDly(1);         			//WhileDly(NS100_DLY(4));                          
	GPIO_SET(WR);                   //GPIO_SET(WR);
	GPIO_SET(CE);
	LCD_SET_DATA_IN();				/* �������������Ϊ���뷽ʽ */
}


unsigned char Rd_Data(void)  
{
	unsigned char Ddata;
	
	LCD_SET_DATA_IN();
	GPIO_SET(CD); 
	GPIO_RESET(CE);
	GPIO_RESET(RD);
	Ddata = GPIO_ReadInputData(GPIOG);
	GPIO_SET(RD);
	GPIO_SET(CE);
	
    return (Ddata);
}

//-----��㺯��--------------------------
void Draw_Dot(int x, int y)
// ����(x,y)��xΪˮƽ�������ص��У�yΪ��ֱ�������ص���
{
	int k1,k2,m;
	
   m=x/3;
   SdCmd(0xf4); SdCmd(m);                        // ���ò���������߽�
   SdCmd(0xf5); SdCmd(y);                        // ���ò��������ϱ߽�
   SdCmd(0xf6); SdCmd(m);                        // ���ò��������ұ߽�
   SdCmd(0xf7); SdCmd(y);                        // ���ò��������±߽�
   SdCmd(0xf8);			  	         			 // ���ô��ڲ���ʹ��
   SdCmd(0xd6);					 				 // ��������λΪ16λ
   SdCmd(m&0x0f); SdCmd(0x10|(m>>4));            // ������ʼ�е�ַ
   SdCmd(0x60+(y&0x0f)); SdCmd(0x70+(y>>4));     // ������ʼ�е�ַ
   k1 = Rd_Data();                                  // �ն�
   k1 = Rd_Data();                                  // ������
   k2 = Rd_Data();                                  // ������
	
   m=x%3;
   switch (m) {
       case 0:   SdData(k1|0xf8);SdData(k2);		break;   // д��
       case 1:   SdData(k1|0x07);SdData(k2|0xe0);	break;
       case 2:   SdData(k1);SdData(k2|0x1f);		break;
      }
   SdCmd(0xd5);					  // �ָ���������λΪ12λ
}

//-----��㺯��--------------------------
void Draw_ClrDot(int x, int y)
// ����(x,y)��xΪˮƽ�������ص��У�yΪ��ֱ�������ص���
{
	int k1,k2,m;
	
   m=x/3;
   SdCmd(0xf4); SdCmd(m);                        // ���ò���������߽�
   SdCmd(0xf5); SdCmd(y);                        // ���ò��������ϱ߽�
   SdCmd(0xf6); SdCmd(m);                        // ���ò��������ұ߽�
   SdCmd(0xf7); SdCmd(y);                        // ���ò��������±߽�
   SdCmd(0xf8);			  	         			 // ���ô��ڲ���ʹ��
   SdCmd(0xd6);					 				 // ��������λΪ16λ
   SdCmd(m&0x0f); SdCmd(0x10|(m>>4));            // ������ʼ�е�ַ
   SdCmd(0x60+(y&0x0f)); SdCmd(0x70+(y>>4));     // ������ʼ�е�ַ
   k1 = Rd_Data();                                  // �ն�
   k1 = Rd_Data();                                  // ������
   k2 = Rd_Data();                                  // ������
	
   m=x%3;
   switch (m) {
       case 0:   SdData(k1&0x07);SdData(k2);		break;   // д��
       case 1:   SdData(k1&0xF8);SdData(k2&0x1F);	break;
       case 2:   SdData(k1);SdData(k2&0xe0);		break;
      }
   SdCmd(0xd5);					  // �ָ���������λΪ12λ
}
void LCD_DrawLine(int x1,int y1,int x2,int y2)
//  xΪˮƽ���������У�yΪ��ֱ�������ص���
//  ����(x1,y1)Ϊ����ʼ��ַ���ꣻ����(x2,y2)Ϊ����ֹ��ַ���ꡣ
{
  u32 temp;
  int dalt_x,dalt_y,err=0;
	
 if (y1>y2)
    {
    temp=x1;
    x1=x2;
    x2=temp;
    temp=y1;
    y1=y2;
    y2=temp;
    }
 Draw_Dot(x1,y1);
 dalt_x=x2-x1;
 dalt_y=y2-y1;
 if(dalt_x>=0)
     {
      if(dalt_y>dalt_x)//k>1
        {
         while(y1<y2)
         {
          if(err<0)
          {
           x1=x1+1;
           y1=y1+1;
           err=err+dalt_y-dalt_x;
           }
           else
           {
           y1=y1+1;
           err=err-dalt_x;
           }
          Draw_Dot(x1,y1);
         }
       }
     else  // 0<=k=<1
      {
       if (dalt_y==0)
          y1=y1-1;
       while(x1<x2)
         {
          if(err<0)
          {
           x1=x1+1;
           err=err+dalt_y;
           }
           else
           {
           y1=y1+1;
           x1=x1+1;
           err=err+dalt_y-dalt_x;
           }
          Draw_Dot(x1,y1);
         }
     }
     
  }
else
  { 
   dalt_x=x1-x2;  
    if(dalt_y>dalt_x)//k<-1
     {
      while(y1<y2)
       {
        if(err<0)
         {
          x1=x1-1;
          y1=y1+1;
          err=err+dalt_y-dalt_x;
         }
        else
         {
          y1=y1+1;
          err=err-dalt_x;
         }
        Draw_Dot(x1,y1);
      }
    }
   else       //0>k>=-1
    {
      if (dalt_y==0)
          y1=y1-1; 
    while(x1>x2)
      {
       if(err<0)
        {
         x1=x1-1;
         err=err+dalt_y;
        }
       else
        {
         x1=x1-1;
         y1=y1+1;
         err=err+dalt_y-dalt_x;
        }
       Draw_Dot(x1,y1);
      }
    } 
  }
     
}
void LCD_ShowBMP(int x, int y, int width, int high, const unsigned char bmp[])
{  
// ����XΪ�����п�0-79�п飬3���ص�����/�п�
// ����YΪ���ص�����
// ͼ�ο��widthΪˮƽ�����������Ҫ���ֵΪ6�ı���
// ͼ�θ߶�highΪ��ֱ���������
// ͼ������bmp[]Ϊ��Ҫд���ͼ�����ݣ���1bpp��8dots/byte)��ˮƽ���и�ʽ��ʾ
// д��ͼ��ǰ��Ҫ��������ʾǰ��ɫ�ͱ���ɫ 
	int p; 
	int i,j,k,m;
 
	SdCmd(0xf4); SdCmd(x);                   		// ���ò���������߽�
	SdCmd(0xf5); SdCmd(y);                   		// ���ò��������ϱ߽� 
	SdCmd(0xf6); SdCmd(width/3);             		// ���ò��������ұ߽�
	SdCmd(0xf7); SdCmd(high-1);              		// ���ò��������±߽�
	SdCmd(0xf8);		                    		// ���ô��ڲ���ʹ��
	SdCmd(x&0x0f);SdCmd(0x10|(x>>4));        		// ������ʼ�е�ַ
	SdCmd(0x60+(y&0x0f)); SdCmd(0x70+(y>>4));		// ������ʼ�е�ַ
	p = 0;                                     		// ����ָ���ʼ��
	for (i=0; i<high; i++) {                      	// ѭ����������
		for (j=0; j<width/8; j++) {              	// ѭ���ֽ���/��
			k = bmp[p++];                 			// ȡͼ������
			for (m=0; m<8; m++) {              		// 1�ֽ�ת����4���ֽ�����д��
				if (k & 0x80) {
					Draw_Dot(x+j*8+m, y+i);
				} else {
					Draw_ClrDot(x+j*8+m, y+i);
				}
				k = (k<<1);
			}
		}
	} 
}

void ClearScreen(void)
{
	int x = 0,y = 0;
	int width = 239;
	int high = 127;
	int i,j;
	
	SdCmd(0xf4); SdCmd(x);                   		// ���ò���������߽�
	SdCmd(0xf5); SdCmd(y);                   		// ���ò��������ϱ߽� 
	SdCmd(0xf6); SdCmd(width/3);             		// ���ò��������ұ߽�
	SdCmd(0xf7); SdCmd(high);              			// ���ò��������±߽�
	SdCmd(0xf8);		                    		// ���ô��ڲ���ʹ��
	SdCmd(x&0x0f);SdCmd(0x10|(x>>4));        		// ������ʼ�е�ַ
	SdCmd(0x60+(y&0x0f)); SdCmd(0x70+(y>>4));		// ������ʼ�е�ַ	
	for (i = 0; i < 128; i++) {
		for (j = 0; j < 120; j++) {
			SdData(0x00);
		}
	}	
}

void LCD_DrawGbChines(int x, int y, const unsigned char *pstr) 
{  
	char Ddata = 1;
	int m,ix,iy;

	SdCmd(0xf4); SdCmd(x);               		// ���ò���������߽�
	SdCmd(0xf5); SdCmd(y);           			// ���ò��������ϱ߽� 
	SdCmd(0xf6); SdCmd(x+16/3);         		// ���ò��������ұ߽�
	SdCmd(0xf7); SdCmd(y+14);           		// ���ò��������±߽�
	SdCmd(0xf8);		               			// ���ô��ڲ���ʹ��
	SdCmd(x&0x0f);SdCmd(0x10|(x>>4));        	// ������ʼ�е�ַ
	SdCmd(0x60+(y&0x0f)); SdCmd(0x70+(y>>4));	// ������ʼ�е�ַ	

	for (iy = 0; iy < 14; iy++) {
		for (ix = 0; ix < 2; ix++) {
			Ddata = *pstr++;
			for (m = 0; m < 8; m++) {
				if (Ddata&0x80) {
					Draw_Dot(x+ix*8+m, y+iy);
				} else {
					Draw_ClrDot(x+ix*8+m, y+iy);
				}
				Ddata <<= 1; 
			}
			
		}
	}
}
void LCD_DrawGbAsc(int x, int y, const unsigned char *pstr) 
{  
	char Ddata = 1;
	int m,ix,iy;

	SdCmd(0xf4); SdCmd(x);               		// ���ò���������߽�
	SdCmd(0xf5); SdCmd(y);           			// ���ò��������ϱ߽� 
	SdCmd(0xf6); SdCmd(x+8/3);         		// ���ò��������ұ߽�
	SdCmd(0xf7); SdCmd(y+14);           		// ���ò��������±߽�
	SdCmd(0xf8);		               			// ���ô��ڲ���ʹ��
	SdCmd(x&0x0f);SdCmd(0x10|(x>>4));        	// ������ʼ�е�ַ
	SdCmd(0x60+(y&0x0f)); SdCmd(0x70+(y>>4));	// ������ʼ�е�ַ	

	for (iy = 0; iy < 14; iy++) {
		for (ix = 0; ix < 1; ix++) {
			Ddata = *pstr++;
			for (m = 0; m < 8; m++) {
				if (Ddata&0x80) {
					Draw_Dot(x+ix*8+m, y+iy);
				} else {
					Draw_ClrDot(x+ix*8+m, y+iy);
				}
				Ddata <<= 1; 
			}
			
		}
	}
}
/****************************************************************************
* ��	�ƣ�void LCD_Init(void)
* ��	�ܣ���ʼ��LCD
* ��ڲ�������
* ���ڲ�����
* ˵	������
****************************************************************************/
void LCD_Init(void)
{
	//LCD�ܽų�ʼ��
	LCD_PortInit();	

    GPIO_RESET(RST); LCD_Delay(NS100_DLY(200)); 	//Ӳ����λ
    GPIO_SET(RST); 	LCD_Delay(NS100_DLY(80000)); 	//��λ���ӳ�800ms����ʱ��

    LCD_WriteCommand(0x25);                           	// �����¶Ȳ���ϵ��-0.05%/C
    LCD_WriteCommand(0x2b);                           	// �ڲ�DC-DC
	LCD_WriteCommand(0xc4);                          	// LCDӳ��MY=1��MX=0��LC0=0  c4
	LCD_WriteCommand(0xa3);                        		// ������ɨ��Ƶ��  fr=76.34hz a1
	LCD_WriteCommand(0xd1);                          	// ��ɫ���ݸ�ʽR-G-B
	LCD_WriteCommand(0xd5);								// ��������λΪ12λRRRR-GGGG-BBBB
	LCD_WriteCommand(0xc8);	LCD_WriteCommand(0x3f);    	// ����M�ź�Ϊ19�з�ת 11
	LCD_WriteCommand(0xec);                            	// ����ƫѹ��1/12
	LCD_WriteCommand(0xa6);                            	// ������ʾ
	LCD_WriteCommand(0xa4);                            	// ������ʾ
	LCD_WriteCommand(0x81);	LCD_WriteCommand(0x4f);   	//���öԱȶ�bf
	LCD_WriteCommand(0xd8);                            	// ����ɨ��ģʽd8

    LCD_WriteCommand(0xf1);	LCD_WriteCommand(0x9f);    	//������CEN,159
    LCD_WriteCommand(0xf2);	LCD_WriteCommand(0x20);    	//��ʼDST
    LCD_WriteCommand(0xf3);	LCD_WriteCommand(0x9f);    	//����DEN
	LCD_WriteCommand(0x84); 
	LCD_WriteCommand(0xa9); 
	
	LCD_Delay(NS100_DLY(800));// ����ʾad
	ClearScreen();
	
// 	LCD_ShowBMP(0,0,14,14,abcde);
// 	
// 	LCD_DrawGbChines(50,50,abcde);
	
}

