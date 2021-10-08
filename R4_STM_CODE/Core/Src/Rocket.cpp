#include "Rocket.hh"

Rocket::Rocket(std::shared_ptr<Motor> _FillMotor, std::shared_ptr<Motor> _DeprMotor,
				std::shared_ptr<Motor> _QDMotor, std::shared_ptr<Igniter> _igniter,
				std::shared_ptr<HX711> _RocketWeight, std::shared_ptr<HX711> _TankWeight,  std::shared_ptr<Motor> _PQDMotor){
	FillMotor =_FillMotor;
	DeprMotor =_DeprMotor;
	QDMotor =_QDMotor;
	PQDMotor =_PQDMotor;
	igniter = _igniter;
	RocketWeight = _RocketWeight;
	TankWeight = _TankWeight;
	currState = RocketStateInit;
}
	
void Rocket::setCurrState(uint8_t newState){
	if(newState < RocketStateNumOfStates)
		currState = (state)newState;
	else
		currState = RocketStateIdle;

	if(currState == RocketStateIdle){
		FillMotor->close();
	}
	else if(currState == RocketStateAbort){
		FillMotor->close();
		DeprMotor->open();
	}
}

uint8_t Rocket::getCurrState() const{
	return currState;
}

template <typename cString>
void Rocket::comandHandler(const cString & Input){
	std::string_view comand(Input);
	float tempNumber = 0;
	if(std::isdigit(comand[5]))
		tempNumber = std::stof(comand.data() + 5);
	//std::from_chars(comand.data() + 5, comand.data() + comand.size(), tempNumber); //need to find compiler settings for this
	if(comand.substr(0, 4) == "STAT"){ // state'y
		setCurrState((uint8_t)(comand[7] - '0'));
	}
	else if(comand.substr(0, 4) == "DSTA" && currState == RocketStateCountdown)  //FIRE
		igniter->FIRE();
	else if(comand.substr(0, 2) == "DW"){	//wagi
		if(comand[2] == 'R')
			RocketWeight->handleComand(comand[3], tempNumber);
		else if(comand[2] == 'T')
			TankWeight->handleComand(comand[3], tempNumber);
	}
	else if(comand.substr(0, 2) == "DZ"){	//zawory
		if(comand[2] == 'T')
			FillMotor->handleComand(comand[3]);
		else if(comand[2] == 'O')
			DeprMotor->handleComand(comand[3]);
		else if(comand[2] == 'Q')
			QDMotor->handleComand(comand[3]);
		else if(comand[2] == 'D')
			PQDMotor->handleComand(comand[3]);
	}
}

std::string Rocket::getInfo() const{
	char bufx[15];
	std::string tmp(std::to_string(currState) + ";");
	//tmp.resize(50);
	tmp.append(std::to_string(igniter->isConnected()) + ";");
	tmp.append(std::to_string(FillMotor->getStatus()) + ";");
	tmp.append(std::to_string(DeprMotor->getStatus()) + ";");
	tmp.append(std::to_string(QDMotor->getStatus()) + ";");
	tmp.append(std::to_string(PQDMotor->getStatus()) + ";");
	std::sprintf(bufx, "%.2f", RocketWeight->getWeigthInKilogramsWithOffset());
	tmp.append(bufx);
	tmp.append(";");
	std::sprintf(bufx, "%.2f", TankWeight->getWeigthInKilogramsWithOffset());
	tmp.append(bufx);
	return tmp;
}

template void Rocket::comandHandler(const std::string &);
template void Rocket::comandHandler(const std::string_view &);
