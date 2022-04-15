void main (void)
{
	u8 key;
	wendu=check_wendu();		  //��ʼ��ʱ�����¶ȶ�ȡ���� ��ֹ����85��C
	Init1602();			  //���ó�ʼ����ʾ����
	LCD_Write_String(0,0,"SET T:00   E:000");  //��������
	LCD_Write_String(1,0,"NOW T:00.0 E:000");  
	delay_ms(1000);
	wendu=check_wendu();		  //��ʼ��ʱ�����¶ȶ�ȡ���� ��ֹ����85��C
	while (1)        					//��ѭ��
	{
		key=Key_Scan();					//����ɨ��
		yanwu=Adc0832(0);				//��ȡ����ֵ
		wendu=check_wendu();	  //��ȡ�¶�ֵ
		
		if(key==KEY_SET)
		{
			Mode++;
		}
		
		switch(Mode)						//�ж�ģʽ��ֵ
		{
			case 0:								//���ģʽ
			{
				Display_1602(yushe_wendu,yushe_yanwu,wendu,yanwu);  //��ʾԤ���¶ȣ�Ԥ�������¶�ֵ������ֵ
				if(yanwu>=yushe_yanwu)	  //����ֵ���ڵ���Ԥ��ֵʱ
				{
					Led_Reg=0;		  			//����ָʾ����
					Fan=0;
					Buzzer=0;			  			//����������
				}
				else					  					//����ֵС��Ԥ��ֵʱ
				{
					Led_Reg=1;		  			//�ص�������
					Fan=1;
				}
				if(wendu>=(yushe_wendu*10))	  //�¶ȴ��ڵ���Ԥ���¶�ֵʱ��Ϊʲô�Ǵ���Ԥ��ֵ*10����Ϊ����Ҫ��ʾ���¶�����С�����һλ����һ��3λ����25.9��Cʱʵ�ʶ�������259�������ж�Ԥ��ֵʱ��Ԥ��ֵ*10��
				{
					Buzzer=0;			  			//�򿪷���������
					Led_Yellow=0;		  			//���¶ȱ�����
				}
				else					  					//�¶�ֵС��Ԥ��ֵʱ
				{
					Led_Yellow=1;		  			//�رձ�����
				}
				if((yanwu<yushe_yanwu)&&(wendu<(yushe_wendu*10)))	  //������С��Ԥ��ֵ�����¶�ҲС��Ԥ��ֵʱ ��&&���߼��룬�������ߵı��ʽ����������Ϊ�棬Ҳ����1��ʱ����if���ų�����
				{
					Buzzer=1;			  			//ֹͣ����
				}
				break;
			}
			case 1://Ԥ���¶�ģʽ
			{
				SelectPosition(0,5) ;					//ָ��λ��
	   		write_com(0x0d);							//��Ӱ��˸
				if(key==KEY_ADD)							//�Ӽ�����
				{
					yushe_wendu++;					    //Ԥ���¶�ֵ����ֵ����1
					if(yushe_wendu>=99)			 		//����ֵ�ӵ����ڵ���99ʱ
					yushe_wendu=99;					 		//��ֵ�̶�Ϊ99
					LCD_Write_Char(0,6,yushe_wendu,2) ;//��ʾԤ���¶�
				}
				if(key==KEY_MINUS)				 		//��������
				{
					if(yushe_wendu<=1)					//���¶�����ֵ��С��1ʱ
					yushe_wendu=1;          		//�̶�Ϊ1
					yushe_wendu--;							//Ԥ���¶�ֵ��һ,��СΪ0
					LCD_Write_Char(0,6,yushe_wendu,2) ;//��ʾԤ���¶�
				}
				break;			  								//ִ�к�����switch
			}
			case 2:				//Ԥ������ģʽ
			{
				SelectPosition(0,12) ;				//ָ��λ��	
	   		write_com(0x0d);							//����ʾ �޹�� �����˸
				if(key==KEY_ADD)							//�Ӽ�����
				{
					if(yushe_yanwu>=255)        //����ֵ�ӵ����ڵ���255ʱ
					yushe_yanwu=254;            //��ֵ�̶�Ϊ254
					yushe_yanwu++;					    //Ԥ������ֵ����ֵ����1,���Ϊ255
					LCD_Write_Char(0,13,yushe_yanwu,3) ;//��ʾԤ������
				}
				if(key==KEY_MINUS)						//��������
				{
					if(yushe_yanwu<=1)					//����������ֵ��С��1ʱ
						yushe_yanwu=1;          	//�̶�Ϊ1
					yushe_yanwu--;							//Ԥ���¶�ֵ��һ,��СΪ0	  
					LCD_Write_Char(0,13,yushe_yanwu,3) ;//��ʾԤ������
				}
				break;
			}
			default	:	
			{
				write_com(0x38);//��Ļ��ʼ��
				write_com(0x0c);//����ʾ �޹�� �޹����˸
				Mode=0;			//�ָ�����ģʽ
				break;
			}
		}
		
	}

}
int Buzzer=8;   //�������ֿ�8ΪBuzzer����������
int Led=7;
void setup()
{
  pinMode(Buzzer,OUTPUT); //�������ֿ�8Ϊ���ģʽ
  pinMode(Led,OUTPUT);
  Serial.begin(9600);
}
void loop()
{
  int val;
  val=analogRead(0);
  Serial.println(val,DEC);
  while(val>125) 
     {
        digitalWrite(Buzzer,HIGH); //����������
        digitalWrite(Led,HIGH);
        val=analogRead(0);   
        Serial.println(val,DEC);
      }
  digitalWrite(Buzzer,LOW); //��������
  digitalWrite(Led,LOW);
}
lora LoRa;

const int buzzerPin = 13;
const int photodiodePin = 0;  // A0
const int smokeThreshold = 80;  // 0 <= no smoke < 80, 80 < thin smoke < 200, 200 < thick smoke <= 1023 

#define DEBUG    0  // 0=disable, 1=enable

#if DEBUG
#include <SoftwareSerial.h>
SoftwareSerial debugSerial(8, 9);  // 8=RX, 9=TX
#endif

void setup()
{
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(57600);  // for LoRa Node
#if DEBUG
  debugSerial.begin(9600); // for debug
#endif
}

void loop()
{
  // perfect interval for Arduino watchdog timer is: 15, 30, 60, 120, 250, 500, 1000, 2000, 4000, 8000
  #define FAST_SAMPLE_INTERVAL      120   // 120ms
  #define NORMAL_SAMPLE_INTERVAL    1000  // 1000ms
  #define SAMPLE_NUMBER             16
  
  int val, sum;
  int array[1];
  
  val = analogRead(photodiodePin);
#if DEBUG  
  debugSerial.println(val);
#endif  
  if (smokeThreshold < val)  // sentry observation
  {
    // Check fire danger after sentry alert
    sum = 0;
    for (int count = 0; count < SAMPLE_NUMBER; ++count)
    {
      val = analogRead(photodiodePin);
      sum += val;
      delay(FAST_SAMPLE_INTERVAL);
    }

    val = sum / SAMPLE_NUMBER;  // get the average value
    if (smokeThreshold < val)
    {
      array[0] = val;
      LoRa.write(array, sizeof(array));
      
      digitalWrite(buzzerPin, HIGH);
      delay(3000);
      digitalWrite(buzzerPin, LOW);

    #if DEBUG
      debugSerial.print("get fired�� val = ");
      debugSerial.println(val);
    #endif
    }
  }
  
  delay(NORMAL_SAMPLE_INTERVAL);
}


