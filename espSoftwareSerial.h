/*example:
#include "espSoftwareSerial.h"

using namespace eSSEnv;

void setup() {
  Serial.begin(115200);

  eSS1.uartInit(2, 3);
  eSS2.uartInit(4, 5);
}

void loop() {
  returnData RTN = receiveDataTask();
  //Serial.println("LOOP OK!");
  if (RTN.whichUart != 0) {
    Serial.print(RTN.whichUart);
    Serial.print(" ");
    Serial.println(RTN.data);
  }
  sendData(UART1, "RTN.data")
}
*/



#ifndef espSoftwareSerial_H
#define espSoftwareSerial_H

#include <Arduino.h>

namespace eSSC{
	typedef unsigned int CountType;
	typedef uint8_t pinType;
	#define Baud 10000
	const uint32_t BaudMicroSecend = 1000000/Baud;
	const uint8_t UART1 = 1;
	const uint8_t UART2 = 2;
}
namespace eSSVar{
	String receiveStr;
	uint8_t waitflag = 0;
	uint8_t uartNum = 0;
	
}
namespace eSSStruct{
	struct returnData{
		uint8_t whichUart;
		String data;
	};
}


namespace eSSNamespace{
	using namespace eSSC;
	using namespace eSSVar;
	using namespace eSSStruct;
	

	void sendDataTask1(void *);
	returnData reUart1(void);
	returnData reUart2(void);
	
	class eSSModel{
		public:
			pinType TXD, RXD;
			String sendStr;
			void uartInit(pinType RXDt, pinType TXDt){
				TXD = TXDt;
				RXD = RXDt;
				pinMode(RXD, INPUT_PULLUP);
				pinMode(TXD, OUTPUT);
				digitalWrite(TXD, HIGH);
				//xTaskCreatePinnedToCore(receiveDataTask1, "receiveDataTask1", 4096, NULL, 3, &xHandle, 0);
			}
			void sendData(uint8_t whichUart, String str){
				sendStr = str;
				switch (whichUart){
					case 1:
						//Serial.println("Task OK!");
						xTaskCreatePinnedToCore(sendDataTask1, "sendDataTask1", 4096, NULL, 3, &xHandle, 0);
						break;
					case 2:
						/*xTaskCreatePinnedToCore(sendDataTask2, "sendDataTask2", 4096, NULL, 3, &xHandle, 1);*/
						break;
				}
			}
			/*void receiveData(){
				//String strTemp;
				static uint8_t i = 0;
				if (digitalRead(RXD) == LOW) {
					sendFlag = 0;
					ets_delay_us(BaudMicroSecend);  // 延迟一个位的时间
					char data = 0;
					
					// 接收每个比特位
					for (int i = 0; i < 8; i++) {
						data |= digitalRead(RXD) << i;  // 读取当前位并设置到相应的位置
						ets_delay_us(BaudMicroSecend);       // 延迟一个位的时间
					}
					receiveStr.concat((char)data);
					ets_delay_us(BaudMicroSecend); 
					//Serial.println("receiveDataTask1 OK!");
					i+=3;
				}else{
					if(i>0){
						i--;
						if(i == 0){
							reflag = 1;
							sendFlag = 1;
							//Serial.println("receiveDataTask1 reflag OK!");
						}
						ets_delay_us(BaudMicroSecend/2);
					}
				}
			}*/
			
		private:
			TaskHandle_t xHandle;
			
	};
	eSSModel eSS1, eSS2;
	
	void sendDataTask1(void *arg){
		CountType length = eSS1.sendStr.length() + 1;
		char data[length];
		eSS1.sendStr.toCharArray(data, sizeof(data));
		/*Serial.println(length);
		Serial.println(data);*/
		for(int j = 0; j<length - 1; j++){
			digitalWrite(eSS1.TXD, LOW);    // 发送起始位（低电平）
			ets_delay_us(BaudMicroSecend);  // 延迟一个位的时间
			// 发送每个比特位
			for (int i = 0; i < 8; i++) {
				digitalWrite(eSS1.TXD, data[j] & 1);  // 发送当前位
				ets_delay_us(BaudMicroSecend);     // 延迟一个位的时间
				data[j] >>= 1;                   // 移位获取下一位
			}
			digitalWrite(eSS1.TXD, HIGH);   // 发送停止位（高电平）
			ets_delay_us(BaudMicroSecend);  // 延迟一个位的时间
		}
		
		Serial.println("sendDataTask1 OK!");
		vTaskDelete(NULL);
	}
	
	returnData receiveDataTask(){
		returnData RTN;
		switch(uartNum){
			case UART1:
				RTN = reUart1();
				return RTN;
				break;
			case UART2:
				RTN = reUart2();
				return RTN;
				break;
			default:
				if(digitalRead(eSS1.RXD) == LOW){
					uartNum = UART1;
					RTN = reUart1();
					return RTN;
				}
				if(digitalRead(eSS2.RXD) == LOW){
					uartNum = UART2;
					RTN = reUart2();
					return RTN;
				}
				break;
		}
		RTN.whichUart = 0;
		return RTN;
	}
	
	returnData reUart1(){
		returnData RTN;
		if(digitalRead(eSS1.RXD) == LOW) {
			ets_delay_us(BaudMicroSecend); 
			char data = 0;
			for (int i = 0; i < 8; i++) {
				data |= digitalRead(eSS1.RXD) << i;
				ets_delay_us(BaudMicroSecend);
			}
			receiveStr.concat((char)data);
			ets_delay_us(BaudMicroSecend); 
			waitflag+=3;
			RTN.whichUart = 0;
		}else{
			if(waitflag > 0) {
				waitflag--;
				if(waitflag == 0){
					uartNum = 0;
					RTN.whichUart = 1;
					RTN.data = receiveStr;
					receiveStr = "";
				}
				ets_delay_us(BaudMicroSecend/2);
			}
		}
		return RTN;
	}
	returnData reUart2(){
		returnData RTN;
		if(digitalRead(eSS2.RXD) == LOW) {
			ets_delay_us(BaudMicroSecend); 
			char data = 0;
			for (int i = 0; i < 8; i++) {
				data |= digitalRead(eSS2.RXD) << i;
				ets_delay_us(BaudMicroSecend);
			}
			receiveStr.concat((char)data);
			ets_delay_us(BaudMicroSecend); 
			waitflag+=3;
			RTN.whichUart = 0;
		}else{
			if(waitflag > 0) {
				waitflag--;
				if(waitflag == 0){
					uartNum = 0;
					RTN.whichUart = 2;
					RTN.data = receiveStr;
					receiveStr = "";
				}
				ets_delay_us(BaudMicroSecend/2);
			}
		}
		return RTN;
	}
}

namespace eSSEnv{
	using namespace eSSStruct;
	using namespace eSSC;
	using eSSNamespace::receiveDataTask;
	using eSSNamespace::eSS1;
	using eSSNamespace::eSS2;
}

#endif

/*if (digitalRead(eSS1.RXD) == LOW) {
	sendFlag = 0;
	ets_delay_us(BaudMicroSecend);  // 延迟一个位的时间
	char data = 0;
	
	// 接收每个比特位
	for (int i = 0; i < 8; i++) {
		data |= digitalRead(eSS1.RXD) << i;  // 读取当前位并设置到相应的位置
		ets_delay_us(BaudMicroSecend);       // 延迟一个位的时间
	}
	receiveStr.concat((char)data);
	ets_delay_us(BaudMicroSecend); 
	//Serial.println("receiveDataTask OK!");
	i+=3;
}else{
	if(i>0){
		i--;
		if(i == 0){
			reflag = 1;
			sendFlag = 1;
			//Serial.println("receiveDataTask reflag OK!");
		}
		ets_delay_us(BaudMicroSecend/2);
	}
*/