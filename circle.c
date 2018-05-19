#include "EV3ButtonPressed.c"
//FUNCTION PROTOTYPES
void motorON();
void motorOFF();
void placeDomino();
int arc();
void resetBlock();
int refillDominoes();
bool loadmore();

bool loadMore()
{
	//Gets user input for whether they want to load more dominoes
	displayString(0, "Would you like to load more dominoes?");
	displayString(1, "Press left button for yes and right for no");

	while (eEV3ButtonPressed()!=4 && eEV3ButtonPressed()!=5)
	{}

	bool yes=eEV3ButtonPressed()-4;

	while (eEV3ButtonPressed()==yes+4)
	{}

	eraseDisplay();

	//Returns result
	if(yes) return 1;
	else return 0;
}

//Motor on function
void motorON()
{
	motor[motorC] = 60;
	motor[motorB] = 30;
}

//Motor off function
void motorOFF()
{
	motor[motorB] = motor[motorC] = 0;
}

//Universal place domino function
void placeDomino()
{
	motor[motorA]=100;
	while(getMotorEncoder(motorA)<0)
	{}

	motor[motorA]=0;

	wait1Msec(500);

	motor[motorA]=-100;
	while(getMotorEncoder(motorA)>-75)
	{}
	motor[motorA]=0;

	wait1Msec(500);
}

//Reset function for placement block
void resetBlock()
{
	motor[motorA]=100;
	while(getMotorEncoder(motorA)<0)
	{}

	motor[motorA]=0;
}

//Refill Dominoes
int refillDominoes(){
	eraseDisplay();
	displayString(0, "Out of dominoes");
	displayString(2, "Input dominoes loaded");
	displayString(4, "Left button to decrease");
	displayString(6, "Right button to increase");
	displayString(8, "Center button to enter");
	displayString(10,"------------------------");

	//Initial state of numDominoes
	int numDominoes = 0;
	displayString(12, "Dominoes loaded: %d", numDominoes);

	//Outputting current input every time a button is pressed
	while(eEV3ButtonPressed() != 2){
		if(eEV3ButtonPressed() == 4)
		{
			while(eEV3ButtonPressed() == 4)
			{}
			numDominoes ++;
			displayString(12, "Dominoes loaded: %d", numDominoes);
		}
		else if(eEV3ButtonPressed() == 5)
		{
			while(eEV3ButtonPressed() == 5)
			{}
			numDominoes --;
			if(numDominoes < 0)
				numDominoes = 0;

			displayString(12, "Dominoes loaded: %d",numDominoes);
		}
	}

	//Waiting for button release
	while(eEV3ButtonPressed() == 2)
	{}

	eraseDisplay();
	displayString(0, "New cartridge loaded");
	displayString(2,"-----------------------------");
	displayString(4, "Number of dominoes:");
	displayString(6, "%d", numDominoes);
	displayString(8, "Press center button to continue");

	while(eEV3ButtonPressed() != 2)
	{}

	while(eEV3ButtonPressed() == 2)
	{}
	eraseDisplay();
	return numDominoes;
}

//Scribe arc function
int arc()
{
	int iD = refillDominoes(); //Assuming empty cartridge when function called
	resetMotorEncoder(motorC);
	while(getMotorEncoder(motorC) < 8000) //While 'arc' not scribed
	{
		placeDomino();
		iD --;
		wait1Msec(300);
		motor[motorB] = motor[motorC] = 20
		wait1Msec(200);
		motorON();
		wait1Msec(240);
		motorOFF();
		wait1Msec(300);

		if(iD == 0)
		{
			resetBlock();
			if(loadMore())
				iD = refillDominoes();
			else
				return 0;
		}
	}
	resetBlock();
	return 0;
}


task main()
{
	arc();
}
