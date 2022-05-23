#include "../include/tasks/tasks.h"



void stateTask(void *arg){
  StateMachine stateMachine(stm.stateTask);
  
  while(1){

    if(ulTaskNotifyTake(pdTRUE, false)){
      switch(stateMachine.getRequestedState()){
        case IDLE:
          stateMachine.changeStateConfirmation();
          break;
           
        case FUELING:
          if((digitalRead(IGN_TEST_CON_1)==HIGH ||
           digitalRead(IGN_TEST_CON_2)==HIGH) &&
           digitalRead(ARM_PIN)==LOW &&
           digitalRead(FIRE1)==LOW &&
           digitalRead(FIRE2)==LOW )
           
            stateMachine.changeStateConfirmation();
          else
            stateMachine.changeStateRejection();
          break;

        case ARMED:
          if((digitalRead(IGN_TEST_CON_1)==HIGH ||
           digitalRead(IGN_TEST_CON_2)==HIGH) &&
           digitalRead(ARM_PIN)==LOW &&
           digitalRead(FIRE1)==LOW &&
           digitalRead(FIRE2)==LOW )

            stateMachine.changeStateConfirmation();
          else
            stateMachine.changeStateRejection();          
          break;

        case RDY_TO_LAUNCH:
          if((digitalRead(IGN_TEST_CON_1)==HIGH ||
           digitalRead(IGN_TEST_CON_2)==HIGH) &&
           digitalRead(ARM_PIN)==HIGH &&
           digitalRead(FIRE1)==LOW &&
           digitalRead(FIRE2)==LOW)
            stateMachine.changeStateConfirmation();
          else
            stateMachine.changeStateRejection();
          break;

        case COUNTDOWN:
           if((digitalRead(IGN_TEST_CON_1)==HIGH ||
           digitalRead(IGN_TEST_CON_2)==HIGH) &&
           digitalRead(ARM_PIN)==HIGH &&
           digitalRead(FIRE1)==LOW &&
           digitalRead(FIRE2)==LOW)
            stateMachine.changeStateConfirmation();
          else
            stateMachine.changeStateRejection();

          break;

        case HOLD:
          // piekny przyklad uzycia klasy uniwersalnej -- nie dla debili jak ja ~ molon
          //TxData motorMsg{0,0}; predefined at the beginning of a file
          // motorMsg = {.command = MOTOR_FILL, .commandValue = CLOSE_VALVE};
          // pwrCom.sendCommand(&motorMsg);
          // xSemaphoreTake(stm.i2cMutex, pdTRUE);
          // pwrCom.sendCommandMotor(MOTOR_FILL, CLOSE_VALVE);
          // xSemaphoreGive(stm.i2cMutex);

          stateMachine.changeStateConfirmation();
          break;

        case ABORT:
          stateMachine.changeStateConfirmation();
          break;

        default: 
          stateMachine.changeStateRejection();
          break;
      }
    }

    //state loop
    switch(StateMachine::getCurrentState()){
      case IDLE:
        //Idle state means nothing is going on

        vTaskDelay(500 / portTICK_PERIOD_MS);
        break;
    
      
          
      case FUELING:
        //CAN GO FROM ARMED
        //ALLOW IF INGITERS HAVE CONTINUITY (HARDWARE ARMED)
        //ALLOW FOR VALE MANIPULATION, NO ACCESS TO IGNITER

        digitalWrite(ARM_PIN, LOW);
        digitalWrite(FIRE1, LOW);
        digitalWrite(FIRE2, LOW);
        //send msg to close valves?
        // motorMsg.command = MOTOR_FILL;
        // motorMsg.commandValue = CLOSE_VALVE;

        xSemaphoreTake(stm.i2cMutex, pdTRUE);
        pwrCom.sendCommandMotor(MOTOR_FILL, CLOSE_VALVE);
        pwrCom.sendCommandMotor(MOTOR_DEPR, CLOSE_VALVE);
        xSemaphoreGive(stm.i2cMutex);

        break;


      case ARMED:
        //CHECK THE CONTINUITY OF IGNITERS
        //if armed 
       // soft arm
        digitalWrite(ARM_PIN, HIGH);
        digitalWrite(FIRE1, LOW);
        digitalWrite(FIRE2, LOW);

        
        break;

      case RDY_TO_LAUNCH:
        //
        //Do not allow valve manipulation, do not give access to the igniter
        digitalWrite(ARM_PIN, HIGH);
        digitalWrite(FIRE1, LOW);
        digitalWrite(FIRE2, LOW);

        break;

      case COUNTDOWN:
        //ALLOW TO STOP AND GO BACK TO RDY_TO_LAUNCH
        //CAN GO IN IF ARMED AND SOFTWARMED AND IGNITERS HAVE CONTINUITY
        //FIRE THE IGNITER AFTER COUNTDOWN
        digitalWrite(ARM_PIN, HIGH);
        digitalWrite(FIRE1, LOW);
        digitalWrite(FIRE2, LOW);

        break;

      case HOLD:
        //ALLOW TO GO FROM ANY STATE
        //SOFTWARE DISARM, NO ACCESS TO VALVES
        digitalWrite(ARM_PIN, LOW);
        digitalWrite(FIRE1, LOW);
        digitalWrite(FIRE2, LOW);
        //CLOSE VALVES MSG
        xSemaphoreTake(stm.i2cMutex, pdTRUE);
        pwrCom.sendCommandMotor(MOTOR_FILL, CLOSE_VALVE);
        pwrCom.sendCommandMotor(MOTOR_DEPR, CLOSE_VALVE);
        xSemaphoreGive(stm.i2cMutex);

        break;

      case ABORT:
        //ALLOW TO GO FROM ANY STATE
        //CAN BE CAUSED EITHER BY SOFTWARE OR SAFETY SWITCH ON THE CASE
        //SOFTWARE DISARM IF ARMED, CLOSE FILL AND OPEN DEPR (DISCONNECT QUICK DISCONN?)
        //IF POSSIBLE GIVE MANUAL CONTROL OVER BOARD STEERING THE VENT AND MAIN VALVE
        digitalWrite(ARM_PIN, LOW);
        digitalWrite(FIRE1, LOW);
        digitalWrite(FIRE2, LOW);
        //CLOSE FILL, OPEN DEPR, OPEN VENT
        xSemaphoreTake(stm.i2cMutex, pdTRUE);
        pwrCom.sendCommandMotor(MOTOR_FILL, CLOSE_VALVE);
        pwrCom.sendCommandMotor(MOTOR_DEPR, OPEN_VALVE);
        // pwrCom.sendCommandMotor(MOTOR_VENT, OPEN_VALVE);
        xSemaphoreGive(stm.i2cMutex);

        break;

      default: 
        break;
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}