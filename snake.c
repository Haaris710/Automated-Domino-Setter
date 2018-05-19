#include "EV3ButtonPressed.c"

//FUNCTION PROTOTYPES
bool loadMore();
int refillDominoes();
void placeDomino();
void resetBlock();
void motorOFF();
int snake();
void placeRightHalfArc(int & numDom, int & flag);
void placeLeftArc(int & numDom, int & flag);
void placeStraight(int & numDom, int & flag);
void placeRightArc(int & numDom, int & flag);

void placeRightArc(int & numDom, int & flag)
{
	resetMotorEncoder(motorC);
	while(getMotorEncoder(motorC) < 1200 && flag != 1)
	{
		placeDomino();
		numDom--;
		wait1Msec(300);

		motor[motorB] = motor[motorC] = 30;
		wait1Msec(200);
		motorOFF();

		motor[motorC] = 30;
		motor[motorB] = 20;

		wait1Msec(320);
		motorOFF();
		wait1Msec(300);

		if(numDom == 0)
		{
			resetBlock();
			if(loadMore())
				numDom = refillDominoes();
			else
				flag = 1;
		}
	}
	resetBlock();
}

void placeStraight(int & numDom, int & flag)
{
	resetMotorEncoder(motorB);
	while(getMotorEncoder(motorB) < 700 && flag != 1)
	{
		placeDomino();
		numDom--;
		wait1Msec(300);
		motor[motorB] = motor[motorC] = 30;
		wait1Msec(320);
		motorOFF();
		wait1Msec(300);

		if(numDom == 0)
		{
			resetBlock();
			if(loadMore())
				numDom = refillDominoes();
			else
			{
				eraseDisplay();
				displayString(0, "Program Terminated");
				flag = 1;
			}
		}
	}
	resetBlock();
}

void placeLeftArc(int & numDom, int & flag)
{
	resetMotorEncoder(motorB);
	while(getMotorEncoder(motorB) < 1200 && flag != 1)
	{
		placeDomino();
		numDom--;
		wait1Msec(300);

		motor[motorB] = motor[motorC] = 30;
		wait1Msec(240);

		motor[motorC] = 20;
		motor[motorB] = 40;

		wait1Msec(180);
		motorOFF();
		wait1Msec(300);

		if(numDom == 0)
		{
			resetBlock();
			if(loadMore())
			{
				numDom = refillDominoes();
			}
			else
			{
				flag = 1;
			}
		}
	}
	resetBlock();
}

void placeRightHalfArc(int & numDom, int & flag)
{
	resetMotorEncoder(motorC);
	while(getMotorEncoder(motorC) < 600 && flag != 1)
	{
		placeDomino();
		numDom--;
		wait1Msec(300);

		motor[motorB] = motor[motorC] = 30;
		wait1Msec(240);

		motor[motorC] = 40;
		motor[motorB] = 20;

		wait1Msec(180);
		motorOFF();
		wait1Msec(300);

		if(numDom == 0)
		{
			resetBlock();
			if(loadMore())
			{
				numDom = refillDominoes();
			}
			else
			{
				flag = 1;
			}
		}
	}
	resetBlock();
}

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

//Motor off function
void motorOFF()
{
	motor[motorB] = motor[motorC] = 0;
}

int snake()
{
	int numDom = 0;
	//Checking if user wants to load dominoes
	if(loadMore())
	{
		numDom = refillDominoes();
	}
	else
	{
		eraseDisplay();
		displayString(0, "Program Terminated");
		return 1;
	}

	int flag = 0;

	//Turning right
	placeRightHalfArc(numDom, flag);
	/*
	resetMotorEncoder(motorC);
	while(getMotorEncoder(motorC) < 600 && flag != 1)
	{
		placeDomino();
		numDom--;
		wait1Msec(300);

		motor[motorC] = 80;
		motor[motorB] = 30;

		wait1Msec(240);
		motorOFF();
		wait1Msec(300);

		if(numDom == 0)
		{
			resetBlock();
			if(loadMore())
			{
				numDom = refillDominoes();
			}
			else
			{
				flag = 1;
			}
		}
	}
	resetBlock();
	*/

	time1[T1] = 0;
	while(time1[T1] < 120000 && flag != 1)
	{
		//Turning left
		placeLeftArc(numDom, flag);
		/*
		resetMotorEncoder(motorB);
		while(getMotorEncoder(motorB) < 1200 && flag != 1)
		{
			placeDomino();
			numDom--;
			wait1Msec(300);

			motor[motorC] = 30;
			motor[motorB] = 80;

			wait1Msec(240);
			motorOFF();
			wait1Msec(300);

			if(numDom == 0)
			{
				resetBlock();
				if(loadMore())
				{
					numDom = refillDominoes();
				}
				else
				{
					flag = 1;
				}
			}
		}
		resetBlock();
		*/

		//Driving straight
		placeStraight(numDom, flag);
		/*
		resetMotorEncoder(motorB);
		while(getMotorEncoder(motorB) < 700 && flag != 1)
		{
			placeDomino();
			numDom--;
			wait1Msec(300);
			motor[motorB] = motor[motorC] = 30;
			wait1Msec(240);
			motorOFF();
			wait1Msec(300);

			if(numDom == 0)
			{
				resetBlock();
				if(loadMore())
					numDom = refillDominoes();
				else
				{
					eraseDisplay();
					displayString(0, "Program Terminated");
					flag = 1;
				}
			}
		}
		resetBlock();
		*/

		//Turning right
		placeRightArc(numDom, flag);
		/*
		resetMotorEncoder(motorC);
		while(getMotorEncoder(motorC) < 1200 && flag != 1)
		{
			placeDomino();
			numDom--;
			wait1Msec(300);

			motor[motorC] = 80;
			motor[motorB] = 30;

			wait1Msec(240);
			motorOFF();
			wait1Msec(300);

			if(numDom == 0)
			{
				resetBlock();
				if(loadMore())
					numDom = refillDominoes();
				else
					flag = 1;
			}
		}
		resetBlock();
		*/

		//Driving straight
		placeStraight(numDom, flag);
		/*
		resetMotorEncoder(motorB);
		while(getMotorEncoder(motorB) < 700 && flag != 1)
		{
			placeDomino();
			numDom--;
			wait1Msec(300);
			motor[motorB] = motor[motorC] = 30;
			wait1Msec(240);
			motorOFF();
			wait1Msec(300);

			if(numDom == 0)
			{
				resetBlock();
				if(loadMore())
					numDom = refillDominoes();
				else
				{
					eraseDisplay();
					displayString(0, "Program Terminated");
					flag = 1;
				}
			}
		}
		resetBlock();
		*/
	}
	return 0;
}


task main()
{
	snake();
}
