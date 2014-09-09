#include "WS2811.h"
#include "discoveryLEDs.h"
#include "graphics.h"
#include "freeRTOS/include/FreeRTOS.h"
#include "freeRTOS/include/task.h"


void wait(uint32_t cycles){
	for(uint32_t i = 0; i < cycles; i++);
}

void showLED(){
	updateLED();
	wait(10000);
	//vTaskDelay(100);
}

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{
	//flash a error light ift stack overflow occures
}

void animation_task (void *paramters){
	 while(1)
	    {
	    	//try knightrider animation
	    	//startup
//	    	setLED(2, 8,0,0);
//	    	setLED(1, 64,0,0);
//	    	setLED(0, 255, 0, 0);
//	    	showLED();
//
//	    	shiftRight(1);
//	    	setLED(0, 64,0,0);
//	    	showLED();
//
//	    	shiftRight(1);
//	    	setLED(0, 8,0,0);
//	    	showLED();
//
//	    	for(uint8_t i = 3; i< NR_LEDS; i++){
//	    		shiftRight(1);
//	    		showLED();
//	    	}
//
//	    	shiftLeft(1);
//			setLED(NR_LEDS-1, 64,0,0);
//			showLED();
//
//			shiftLeft(1);
//			setLED(NR_LEDS-1, 8,0,0);
//			showLED();
//
//			for(uint8_t i = 3; i< NR_LEDS; i++){
//				shiftLeft(1);
//				showLED();
//			}

			for(uint8_t i = 1; i>0; i++){
				setAllLED(i,i,i);
				showLED();
			}
			for(uint8_t i = 255; i>0; i--){
				setAllLED(i,i,i);
				showLED();
			}


			for(uint8_t i = 1; i>0; i++){
				setAllLED(i,0,0);
				showLED();
			}
			for(uint8_t i = 255; i>0; i--){
				setAllLED(i,0,0);
				showLED();
			}


			for(uint8_t i = 1; i>0; i++){
				setAllLED(0,i,0);
				showLED();
			}
			for(uint8_t i = 255; i>0; i--){
				setAllLED(0,i,0);
				showLED();
			}


			for(uint8_t i = 1; i>0; i++){
				setAllLED(0,0,i);
				showLED();
			}
			for(uint8_t i = 255; i>0; i--){
				setAllLED(0,0,i);
				showLED();
			}


			for(uint8_t i = 1; i>0; i++){
				setAllLED(i,i,0);
				showLED();
			}
			for(uint8_t i = 255; i>0; i--){
				setAllLED(i,i,0);
				showLED();
			}


			for(uint8_t i = 1; i>0; i++){
				setAllLED(0,i,i);
				showLED();
			}
			for(uint8_t i = 255; i>0; i--){
				setAllLED(0,i,i);
				showLED();
			}


			for(uint8_t i = 1; i>0; i++){
				setAllLED(i,0,i);
				showLED();
			}
			for(uint8_t i = 255; i>0; i--){
				setAllLED(i,0,i);
				showLED();
			}

	    }
}

void boardLEDs(void *paramters){

}

int main(void)
{
	LEDS_init();
	LEDon(LED_BLUE);
	ws2811_init();
	clearAllLED();

	//xTaskCreate(animation_task, (signed char*)"animation", 16, 0, 1, 0);
	//vTaskStartScheduler();
	animation_task((void *) 0);
    return -1;
}

//will be call from IDLE thread
void vApplicationIdleHook( void ){

	//just show im alive
	static uint8_t led = 0;
	if(led)
		LEDon(LED_GREEN);
	else
		LEDoff(LED_GREEN);

	led = !led;
}
