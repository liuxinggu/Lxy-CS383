void main (void)
{
	u8 key;
	wendu=check_wendu();		  //初始化时调用温度读取函数 防止开机85°C
	Init1602();			  //调用初始化显示函数
	LCD_Write_String(0,0,"SET T:00   E:000");  //开机界面
	LCD_Write_String(1,0,"NOW T:00.0 E:000");  
	delay_ms(1000);
	wendu=check_wendu();		  //初始化时调用温度读取函数 防止开机85°C
	while (1)        					//主循环
	{
		key=Key_Scan();					//按键扫描
		yanwu=Adc0832(0);				//读取烟雾值
		wendu=check_wendu();	  //读取温度值
		
		if(key==KEY_SET)
		{
			Mode++;
		}
		
		switch(Mode)						//判断模式的值
		{
			case 0:								//监控模式
			{
				Display_1602(yushe_wendu,yushe_yanwu,wendu,yanwu);  //显示预设温度，预设烟雾，温度值，烟雾值
				if(yanwu>=yushe_yanwu)	  //烟雾值大于等于预设值时
				{
					Led_Reg=0;		  			//烟雾指示灯亮
					Fan=0;
					Buzzer=0;			  			//蜂鸣器报警
				}
				else					  					//烟雾值小于预设值时
				{
					Led_Reg=1;		  			//关掉报警灯
					Fan=1;
				}
				if(wendu>=(yushe_wendu*10))	  //温度大于等于预设温度值时（为什么是大于预设值*10：因为我们要显示的温度是有小数点后一位，是一个3位数，25.9°C时实际读的数是259，所以判断预设值时将预设值*10）
				{
					Buzzer=0;			  			//打开蜂鸣器报警
					Led_Yellow=0;		  			//打开温度报警灯
				}
				else					  					//温度值小于预设值时
				{
					Led_Yellow=1;		  			//关闭报警灯
				}
				if((yanwu<yushe_yanwu)&&(wendu<(yushe_wendu*10)))	  //当烟雾小于预设值并且温度也小于预设值时 （&&：逻辑与，左右两边的表达式都成立（都为真，也就是1）时，该if语句才成立）
				{
					Buzzer=1;			  			//停止报警
				}
				break;
			}
			case 1://预设温度模式
			{
				SelectPosition(0,5) ;					//指定位置
	   		write_com(0x0d);							//阴影闪烁
				if(key==KEY_ADD)							//加键按下
				{
					yushe_wendu++;					    //预设温度值（阀值）加1
					if(yushe_wendu>=99)			 		//当阀值加到大于等于99时
					yushe_wendu=99;					 		//阀值固定为99
					LCD_Write_Char(0,6,yushe_wendu,2) ;//显示预设温度
				}
				if(key==KEY_MINUS)				 		//减键按下
				{
					if(yushe_wendu<=1)					//当温度上限值减小到1时
					yushe_wendu=1;          		//固定为1
					yushe_wendu--;							//预设温度值减一,最小为0
					LCD_Write_Char(0,6,yushe_wendu,2) ;//显示预设温度
				}
				break;			  								//执行后跳出switch
			}
			case 2:				//预设烟雾模式
			{
				SelectPosition(0,12) ;				//指定位置	
	   		write_com(0x0d);							//打开显示 无光标 光标闪烁
				if(key==KEY_ADD)							//加键按下
				{
					if(yushe_yanwu>=255)        //当阀值加到大于等于255时
					yushe_yanwu=254;            //阀值固定为254
					yushe_yanwu++;					    //预设烟雾值（阀值）加1,最大为255
					LCD_Write_Char(0,13,yushe_yanwu,3) ;//显示预设烟雾
				}
				if(key==KEY_MINUS)						//减键按下
				{
					if(yushe_yanwu<=1)					//当烟雾上限值减小到1时
						yushe_yanwu=1;          	//固定为1
					yushe_yanwu--;							//预设温度值减一,最小为0	  
					LCD_Write_Char(0,13,yushe_yanwu,3) ;//显示预设烟雾
				}
				break;
			}
			default	:	
			{
				write_com(0x38);//屏幕初始化
				write_com(0x0c);//打开显示 无光标 无光标闪烁
				Mode=0;			//恢复正常模式
				break;
			}
		}
		
	}

}
int Buzzer=8;   //定义数字口8为Buzzer（蜂鸣器）
int Led=7;
void setup()
{
  pinMode(Buzzer,OUTPUT); //定义数字口8为输出模式
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
        digitalWrite(Buzzer,HIGH); //蜂鸣器不响
        digitalWrite(Led,HIGH);
        val=analogRead(0);   
        Serial.println(val,DEC);
      }
  digitalWrite(Buzzer,LOW); //蜂鸣器响
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
      debugSerial.print("get fired， val = ");
      debugSerial.println(val);
    #endif
    }
  }
  
  delay(NORMAL_SAMPLE_INTERVAL);
}


