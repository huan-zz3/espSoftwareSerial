#ifndef espSoftwareSerial_H
#define espSoftwareSerial_H

#include <Arduino.h>

namespace eSSC{
	typedef unsigned int CountType;
	typedef uint8_t pinType;
	#define Baud 1000
	const uint32_t BaudMicroSecend = 1000000/Baud;
}
namespace eSSVar{
	
	
}


namespace eSSNamespace{
	using namespace eSSC;
	using namespace eSSVar;
	
	void sendDataTask1(void *);
	void delayMicroseconds(uint32_t);
	
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
			}
			void sendData(uint8_t whichUart, String str){
				sendStr = str;
				switch (whichUart){
					case 1:
						Serial.println("Task OK!");
						xTaskCreatePinnedToCore(sendDataTask1, "sendDataTask1", 4096, NULL, 3, &xHandle, 1);
						break;
					case 2:
						/*xTaskCreatePinnedToCore(sendDataTask2, "sendDataTask2", 4096, NULL, 3, &xHandle, 1);*/
						break;
				}
			}
			void receiveData(){
				
			}
		private:
			TaskHandle_t xHandle;
			
	};
	eSSModel eSS1, eSS2;
	void sendDataTask1(void *arg){
		CountType length = eSS1.sendStr.length() + 1;
		char data[length];
		eSS1.sendStr.toCharArray(data, sizeof(data));
		Serial.println(length);
		Serial.println(data);
		for(int j = 0; j<length - 1; j++){
			digitalWrite(eSS1.TXD, LOW);    // 发送起始位（低电平）
			delayMicroseconds(BaudMicroSecend);  // 延迟一个位的时间
			/*Serial.print("j:");
			Serial.println(j);*/
			// 发送每个比特位
			for (int i = 0; i < 8; i++) {
				digitalWrite(eSS1.TXD, data[j] & 1);  // 发送当前位
				delayMicroseconds(BaudMicroSecend);     // 延迟一个位的时间
				data[j] >>= 1;                   // 移位获取下一位
				/*Serial.print("i:");
				Serial.println(i);*/
			}
			digitalWrite(eSS1.TXD, HIGH);   // 发送停止位（高电平）
			delayMicroseconds(BaudMicroSecend);  // 延迟一个位的时间
		}
		
		Serial.println("sendDataTask1 OK!");
		vTaskDelete(NULL);
	}
	
	void delayMicroseconds(uint32_t us) {
		const TickType_t xDelay = pdMS_TO_TICKS(us / 1000);
		TickType_t xLastWakeTime = xTaskGetTickCount();
		vTaskDelayUntil(&xLastWakeTime, xDelay);
		
		/*TickType_t startTime = xTaskGetTickCount();
		const TickType_t delayTicks = pdMicrosecondsToTicks(us);
		vTaskDelayUntil(&startTime, delayTicks);*/
	}
}

namespace eSSEnv{
	using eSSNamespace::eSS1;
	using eSSNamespace::eSS2;
}

#endif