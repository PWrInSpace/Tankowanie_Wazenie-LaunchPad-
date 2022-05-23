#include "../include/com/internalCommunication.h"

template <typename rxType, typename txType>
InternalI2C<rxType, txType>::InternalI2C(TwoWire *_wire, uint8_t _address):
  i2c(_wire),
  address(_address)
{}

/**********************************************************************************************/

template <typename rxType, typename txType>
bool InternalI2C<rxType, txType>::sendCommand(txType *_data){
  size_t writeStatus = 0;
  i2c->beginTransmission(address);
  writeStatus = i2c->write((uint8_t*) _data, sizeof(txType));
  if(i2c->endTransmission() != I2C_ERROR_OK){
    return false;
  }
  
  return writeStatus != 0 ? true : false; 
}


/**********************************************************************************************/

template <typename rxType, typename txType>
bool InternalI2C<rxType, txType>::getData(rxType* _data){
  
  i2c->requestFrom(address, sizeof(rxType));
  if (i2c->available()) {
    if (!i2c->readBytes((uint8_t*)_data, sizeof(rxType))) {
      return false;
    }
  }

  return true;

}


/**********************************************************************************************/

template <typename rxType, typename txType>
bool InternalI2C<rxType, txType>::sendCommandMotor(uint8_t _command_valve, uint8_t _command_state, uint16_t _commandValue){
  size_t writeStatus = 0;
  TxData motorMsg{00,0};
  motorMsg.command = _command_valve*10+_command_state;// combinig command valve with state (33,300) - vent vale open_timed for 300ms
  motorMsg.commandValue = _commandValue;
  txType *_data = &motorMsg;
  i2c->beginTransmission(address);
  writeStatus = i2c->write((uint8_t*) _data, sizeof(txType));
  if(i2c->endTransmission() != I2C_ERROR_OK){
    return false;
  }
  
  return writeStatus != 0 ? true : false; 
}
//EXAMPLE OF RESET:
//obj.sendCommandMotor(0,RESET_COMMAND);

/**********************************************************************************************/