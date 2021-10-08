#include <L298.hh>

//make struct 'Motor', fill it, return pointer to it    (names for args start with '_' sign)
Motor::Motor(GPIO_TypeDef* _GPIO_PORT_IN1, uint16_t _PIN_IN1,
			 GPIO_TypeDef* _GPIO_PORT_IN2, uint16_t _PIN_IN2,
			 TIM_HandleTypeDef* _TIM_NR_EN, uint16_t _TIM_CHANNEL_EN, 		//TIMER not PORT (&htim3 = OK ; FILL_EN_PORT = NOT OK)
			 GPIO_TypeDef* _GPIO_PORT_LS_OPEN, uint16_t _PIN_LS_OPEN,
			 GPIO_TypeDef* _GPIO_PORT_LS_CLOSE, uint16_t _PIN_LS_CLOSE)
  :	GPIO_PORT_IN1(_GPIO_PORT_IN1), PIN_IN1(_PIN_IN1),
	GPIO_PORT_IN2(_GPIO_PORT_IN2), PIN_IN2 (_PIN_IN2),
	TIM_NR_EN(_TIM_NR_EN), TIM_CHANNEL_EN(_TIM_CHANNEL_EN),
	GPIO_PORT_LS_OPEN(_GPIO_PORT_LS_OPEN), PIN_LS_OPEN(_PIN_LS_OPEN),
	GPIO_PORT_LS_CLOSE(_GPIO_PORT_LS_CLOSE), PIN_LS_CLOSE(_PIN_LS_CLOSE),
	status(Status::MotorStateIDK)
{
	HAL_TIM_PWM_Start(TIM_NR_EN, TIM_CHANNEL_EN);
	stop();
}

Status Motor::getStatus(){
	return status;
}

void Motor::stop(){
	__HAL_TIM_SET_COMPARE(TIM_NR_EN, TIM_CHANNEL_EN, 0);
	HAL_GPIO_WritePin(GPIO_PORT_IN1, PIN_IN1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIO_PORT_IN2, PIN_IN2, GPIO_PIN_RESET);
}

void Motor::open(uint32_t milisecs){
	HAL_GPIO_WritePin(GPIO_PORT_IN1, PIN_IN1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIO_PORT_IN2, PIN_IN2, GPIO_PIN_RESET);
	__HAL_TIM_SET_COMPARE(TIM_NR_EN, TIM_CHANNEL_EN, 999);
	status = Status::MotorStateAttemptToOpen;
	if(GPIO_PORT_LS_OPEN != nullptr){ //if has limit switch
		for(uint32_t steps =  0; steps < milisecs/10 ; ++steps ){
			if(HAL_GPIO_ReadPin(GPIO_PORT_LS_OPEN, PIN_LS_OPEN) == GPIO_PIN_RESET){
				status = Status::MotorStateOpen;
				break;
			}
		HAL_Delay(10);
		}
	}
	else
		HAL_Delay(milisecs);
	stop();
}

void Motor::close(uint32_t milisecs){
	HAL_GPIO_WritePin(GPIO_PORT_IN1, PIN_IN1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIO_PORT_IN2, PIN_IN2, GPIO_PIN_SET);
	__HAL_TIM_SET_COMPARE(TIM_NR_EN, TIM_CHANNEL_EN, 999);
	status = Status::MotorStateAttemptToClose;
	if(GPIO_PORT_LS_CLOSE != nullptr){ //if has limit switch
		for(uint32_t steps =  0; steps < milisecs/10; ++steps){
			if(HAL_GPIO_ReadPin(GPIO_PORT_LS_CLOSE, PIN_LS_CLOSE) == GPIO_PIN_RESET){
				status = Status::MotorStateClose;
				break;
			}
			HAL_Delay(10);
		}
	}
	else
		HAL_Delay(milisecs);
	stop();
}

void Motor::test_open_close(){
	open();
	HAL_Delay(100);
	close();
	HAL_Delay(100);
	open();
	HAL_Delay(100);
	close();
	stop();
}

void Motor::handleComand(char command, uint32_t milisecs){
	if(command == 'O')
		open(milisecs);
	else if(command == 'Z')
		close(milisecs);
}
