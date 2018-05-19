#include "EV3ButtonPressed.c"
#include "EV3_FileIO.c"

const int MOTORPOWER=10;
const int LINEEDGE=30;//change this value to make sure it stays on the line at correct spot
//need to measure the correct values for being on the line
//perhaps two diff constants for lineedge on each side
const float CORRECTFACTOR=0.08;//play with this constant
const float TURNCORRECTFACTOR=0.09;//alter this for turns
const float TURNPOWER=8;
//Left Motor is C
//Right Motor is B
bool keepGoing=true;


//Returns true if emergency stop not pressed and not falling off table
//Otherwise, sets all motors to zero, sets global flag variable to false and returns false
bool safe()
{
	if(!SensorValue[S2] && SensorValue[S3]>10)
		return true;
	else
	{
		motor[motorA]=motor[motorB]=motor[motorC]=0;
		keepGoing=false;
		return false;
	}
}


void knockOver()
{
	resetMotorEncoder(motorB);
	motor[motorC]=motor[motorB]=-100;
	while(getMotorEncoder(motorB)>-100 && keepGoing && safe())
	{}

	motor[motorC]=motor[motorB]=100;
	while(getMotorEncoder(motorB)<0)
	{}


	motor[motorC]=motor[motorB]=0;
}


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
			numDominoes ++; //Right button increase

			eraseDisplay();
			displayString(0, "Out of dominoes");
			displayString(2, "Input dominoes loaded");
			displayString(4, "Left button to decrease");
			displayString(6, "Right button to increase");
			displayString(8, "Center button to enter");
			displayString(10,"------------------------");
			displayString(12, "Dominoes loaded: %d", numDominoes);
		}
		else if(eEV3ButtonPressed() == 5)
		{
			while(eEV3ButtonPressed() == 5)
			{}
			numDominoes --; //Left button decrease
			if(numDominoes < 0)
				numDominoes = 0;

			eraseDisplay();
			displayString(0, "Out of dominoes");
			displayString(2, "Input dominoes loaded");
			displayString(4, "Left button to decrease");
			displayString(6, "Right button to increase");
			displayString(8, "Center button to enter");
			displayString(10,"------------------------");
			displayString(12, "Dominoes loaded: %d",numDominoes);
		}
	}


	//Waiting for button release
	while(eEV3ButtonPressed() == 2)
	{}

	//Outputting final number of dominoes stored in the robot as specified
	//by the user
	eraseDisplay();
	displayString(0, "New cartridge loaded");
	displayString(2,"-----------------------------");
	displayString(4, "Number of dominoes:");
	displayString(6, "%d", numDominoes);
	displayString(8, "Press center to continue");

	//Waiting for enter button press and release to continue
	while(eEV3ButtonPressed() != 2)
	{}


	while(eEV3ButtonPressed() == 2)
	{}


	eraseDisplay();


	return numDominoes;
}


void placeDomino()
{
	//Resets block to its starting position
	motor[motorA]=10;
	while(getMotorEncoder(motorA)<0 && keepGoing && safe())
	{}


	motor[motorA]=0;


	//Waits to allow domino to drop and settle
	wait1Msec(500);


	//Pulls block back quickly to release domino
	motor[motorA]=-100;
	while(getMotorEncoder(motorA)>-75 && keepGoing && safe())
	{}
	motor[motorA]=0;


	//Waits to allow domino to drop on table and settle
	wait1Msec(500);
}


void resetBlock()
{
	//Resets block to its starting position
	motor[motorA]=10;
	while(getMotorEncoder(motorA)<0)
	{}


	motor[motorA]=0;
}


//Motor on function for arcs
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


bool loadMore()
{
	//Gets user input for whether they want to load more dominoes
	displayString(0, "Would you like to load more");
	displayString(1, "dominoes?");
	displayString(3, "Press left button for yes");
	displayString(4, "or right button for no");


	while (eEV3ButtonPressed()!=4 && eEV3ButtonPressed()!=5)
	{}


	bool yes=eEV3ButtonPressed()-4;


	while (eEV3ButtonPressed()==yes+4)
	{}


	eraseDisplay();


	//Returns result
	if(yes) return true;
	else return false;
}


void storeMovements (TFileHandle & out, int leftMotorPower, int rightMotorPower, int timeMS)
{
	writeLongPC(out, leftMotorPower);
	writeEndlPC(out);
	writeLongPC(out, rightMotorPower);
	writeEndlPC(out);
	writeLongPC(out, timeMS);
	writeEndlPC(out);
}

//Placing and arc while driving towards the right
void placeRightArc(TFileHandle & fout, int & numDom, int & flag)
{
	resetMotorEncoder(motorC);
	while(getMotorEncoder(motorC) < 1200 && flag != 1 && keepGoing && safe())
	{
		placeDomino();
		numDom--; //Keeping track of the number of dominoes remaining

		time1[T2]=0;
		while(time1[T2]<300 && keepGoing && safe()){}

		//Driving straight after the placement of the domino before turning
		//to ensure that placed dominoes are not knocked over by the
		//turning robot
		motor[motorB] = motor[motorC] = 30;
		time1[T2]=0;
		while(time1[T2]<200 && keepGoing && safe()){}
		storeMovements(fout,30,30,200);
		motorOFF();

		//Driving in an arc
		motor[motorC] = 30;
		motor[motorB] = 20;


		time1[T2]=0;
		while(time1[T2]<320 && keepGoing && safe()){}
		storeMovements(fout,30,20,320);
		motorOFF();

		time1[T2]=0;
		while(time1[T2]<300 && keepGoing && safe()){}

		//Checking for an empty cartridge on each iteration and
		//calling the refill dominoes function if this was true
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

//Followed a similar pattern to the placeRightArc function
void placeStraight(TFileHandle & fout, int & numDom, int & flag)
{
	resetMotorEncoder(motorB);
	while(getMotorEncoder(motorB) < 700 && flag != 1)
	{
		placeDomino();
		numDom--;

		time1[T2]=0;
		while(time1[T2]<300 && keepGoing && safe()){}

		motor[motorB] = motor[motorC] = 30;

		time1[T2]=0;
		while(time1[T2]<320 && keepGoing && safe()){}
		storeMovements(fout,30,30,320);
		motorOFF();

		time1[T2]=0;
		while(time1[T2]<300 && keepGoing && safe()){}


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

//Followed a similar pattern to the placeRightArc function
void placeLeftArc(TFileHandle & fout, int & numDom, int & flag)
{
	resetMotorEncoder(motorB);
	while(getMotorEncoder(motorB) < 1200 && flag != 1)
	{
		placeDomino();
		numDom--;

		time1[T2]=0;
		while(time1[T2]<300 && keepGoing && safe()){}


		motor[motorB] = motor[motorC] = 30;
		time1[T2]=0;
		while(time1[T2]<210 && keepGoing && safe()){}
		storeMovements(fout,30,30,210);


		motor[motorC] = 20;
		motor[motorB] = 40;


		time1[T2]=0;
		while(time1[T2]<160 && keepGoing && safe()){}
		storeMovements(fout,20,40,160);
		motorOFF();

		time1[T2]=0;
		while(time1[T2]<300 && keepGoing && safe()){}


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

//Identical to the placeRightArc function except the duration of the function
//is half the duration of the placeRightArc function
void placeRightHalfArc(TFileHandle & fout, int & numDom, int & flag)
{
	resetMotorEncoder(motorC);
	while(getMotorEncoder(motorC) < 600 && flag != 1)
	{
		placeDomino();
		numDom--;

		time1[T2]=0;
		while(time1[T2]<300 && keepGoing && safe()){}


		motor[motorB] = motor[motorC] = 30;
		time1[T2]=0;
		while(time1[T2]<210 && keepGoing && safe()){}
		storeMovements(fout,30,30,210);


		motor[motorC] = 40;
		motor[motorB] = 20;


		time1[T2]=0;
		while(time1[T2]<180 && keepGoing && safe()){}
		storeMovements(fout,40,20,180);
		motorOFF();

		time1[T2]=0;
		while(time1[T2]<300 && keepGoing && safe()){}


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


int snake(TFileHandle & fout)
{
	int numDom = 0;


	numDom = refillDominoes();


	int flag = 0;


	//Turning right
	placeRightHalfArc(fout, numDom, flag);

	//Creating a pattern for a duration of approximately two minutes
	time1[T1] = 0;
	while(time1[T1] < 120000 && flag != 1 && keepGoing && safe())
	{
		//Turning left
		placeLeftArc(fout, numDom, flag);


		//Driving straight
		placeStraight(fout, numDom, flag);


		//Turning right
		placeRightArc(fout, numDom, flag);


		//Driving straight
		placeStraight(fout, numDom, flag);
	}
	return 0;
}


//Scribe arc function
int arc(TFileHandle & fout)
{
	int iD = refillDominoes(); //Assuming empty cartridge when function called
	resetMotorEncoder(motorC);
	while(getMotorEncoder(motorC) < 8000 && keepGoing && safe()) //While 'arc' not scribed
	{
		placeDomino();
		iD --;

		time1[T1]=0;
		while(time1[T1]<300 && keepGoing && safe()){}

		//Drive forward after placing domino, before turning to prevent rear
		//of robot from knocking over placed dominoes
		motor[motorB] = motor[motorC] = 20;
		time1[T1]=0;
		while(time1[T1]<200 && keepGoing && safe()){}
		storeMovements(fout,20,20,200);

		motorON();
		time1[T1]=0;
		while(time1[T1]<150 && keepGoing && safe()){}
		storeMovements(fout,60,30,150);

		motorOFF();
		time1[T1]=0;
		while(time1[T1]<300 && keepGoing && safe()){}

		//Checking for an empty cartridge and calling the refill dominoes
		//function if the cartridge was empty
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


void whatShape(TFileHandle & fout)
{
	//Gets user input for pattern to make
	displayString(0, "What pattern would you like");
	displayString(1, "to make?");
	displayString(3, "Press the left button for arc");
	displayString(4, "or the right button for snake");


	while (eEV3ButtonPressed()!=4 && eEV3ButtonPressed()!=5)
	{}


	bool arcShape=eEV3ButtonPressed()-4;


	while (eEV3ButtonPressed()==arcShape+4)
	{}


	eraseDisplay();


	//calls pattern function based on user input
	if(arcShape) arc(fout);
	else snake(fout);
}

void driveFunc(TFileHandle & fout, int & distDominoes)
{
	resetMotorEncoder(motorB);
	resetMotorEncoder(motorC);
	float valueLeft,valueRight;
	float setMotorLeft, setMotorRight;
	float errorLeft, errorRight;

	//while has travelled less than 5
	while ((getMotorEncoder(motorB)+getMotorEncoder(motorC))/2/360*2*PI*3<distDominoes && keepGoing && safe())
	{
		valueLeft=getColorReflected(S1);

		valueRight=getColorReflected(S3);

		errorLeft = valueLeft-LINEEDGE;
		errorRight=valueRight-LINEEDGE;

		if (valueLeft>=85)
		{	setMotorLeft= TURNPOWER+(errorLeft)*TURNCORRECTFACTOR;
			setMotorRight = TURNPOWER+(errorRight)*TURNCORRECTFACTOR;
		}
		else if (valueRight==100)
		{
			setMotorRight=TURNPOWER+(errorRight)*TURNCORRECTFACTOR;
			setMotorLeft=TURNPOWER+(errorLeft)*TURNCORRECTFACTOR;
		}
		else
		{
			setMotorLeft= MOTORPOWER+(errorLeft)*CORRECTFACTOR;
			setMotorRight= MOTORPOWER+(errorRight)*CORRECTFACTOR;
		}

		motor[motorB]=setMotorRight;
		motor[motorC]=setMotorLeft;
		if (valueRight==100||valueLeft==100)
			distDominoes =3;
		else
			distDominoes=3;
		//have a wait part to allow motors time to change
		wait1Msec(10);
		storeMovements(fout,setMotorLeft,setMotorRight,10);
	}
}

void followLine(TFileHandle & fout)
{
	//Loops until user doesn't want to load any more dominoes or unsafe
	do
	{
		//Determines number of dominoes in chute
		int dominoesLeft = refillDominoes();
		int distDominoes = 5;

		//Loops until no more dominoes or unsafe
		while(dominoesLeft>0 && keepGoing)
		{
			//Places domino and drives to next placing location
			placeDomino();
			dominoesLeft--;
			driveFunc(fout, distDominoes);
			motor[motorB]=0;
			motor[motorC]=0;
		}

		//Resets block to original position
		resetBlock();

	}while(keepGoing && loadMore());
}


void turn180()
{
	nMotorEncoder[motorB] = 0;


	motor[motorB] = 40;
	motor[motorC] = -40;


	while (keepGoing && safe() && nMotorEncoder[motorB] < 455 /* && keepGoing && safe()*/){
 	}


 	motor[motorB] = 0;
	motor[motorC] = 0;
}


void cleanUp (TFileHandle & fin) // ifstream of robotPath.txt should be initiated
{
	int leftMotorPower, rightMotorPower, timeMS, numMovements = 0;


	while (readIntPC(fin,leftMotorPower) && readIntPC(fin,rightMotorPower) && readIntPC(fin,timeMS))
	{
		numMovements ++;
	}


	for (int i = numMovements; i > 0; i --)
	{
		closeFilePC(fin);

		TFileHandle fin;
		openReadPC(fin,"robotPath.txt");

		for (int j = 0; j < i; j ++){
				readIntPC(fin,leftMotorPower);
				readIntPC(fin,rightMotorPower);
				readIntPC(fin,timeMS);
		}


		time1 [T1] = 0;


		motor[motorB] = leftMotorPower;
		motor[motorC] = rightMotorPower;


		while (keepGoing && safe() && time1 [T1] < timeMS)
		{}
	}

	motor[motorB]=motor[motorC]=0;
}


void prepareCleanUp ()
{
		nMotorEncoder[motorB] = 0;
		motor[motorB] = 50;
		motor[motorC] = 50;

		while (keepGoing && safe() && nMotorEncoder[motorB] < 200){}

		turn180();

		nMotorEncoder[motorB] = 0;
		motor[motorB] = 50;
		motor[motorC] = 50;

		while (keepGoing && safe() && nMotorEncoder[motorB] < 505){}

		motor[motorB] = 0;
		motor[motorC] = 0;



		TFileHandle fin;
		openReadPC(fin,"robotPath.txt");


		cleanUp(fin);

		closeFilePC(fin);
}


task main()
{
	TFileHandle fout;
	openWritePC(fout, "robotPath.txt");


	//Welcome message and wait for button press
	displayString(0, "Hello, I am the Dominator");
	displayString(2, "Press centre button to begin");


	while (eEV3ButtonPressed()!=2)
	{}


	while (eEV3ButtonPressed()==2)
	{}


	eraseDisplay();


	//Gets user input for how pattern is to be created
	displayString(0, "How would you like to create");
	displayString(1, "your pattern?");
	displayString(3, "Press the left button to");
	displayString(4, "follow a line");
	displayString(6, "Press the right button for");
	displayString(7, "a preset pattern");


	while (eEV3ButtonPressed()!=4 && eEV3ButtonPressed()!=5)
	{}


	bool line=eEV3ButtonPressed()-4;


	while (eEV3ButtonPressed()==line+4)
	{}


	eraseDisplay();


	//Calls appropriate placement function
	if(line)
	{
		followLine(fout);
	}
	else
	{
		whatShape(fout);
	}

	closeFilePC(fout);

	//If program hasn't stopped unexpectedly
	if (keepGoing)
	{
		//Determines if dominoes should be knocked over
		displayString(0, "Would you like me to knock");
		displayString(1, "over the dominoes?");
		displayString(3, "Press left button for yes");
		displayString(4, "or right button for no");


		while (eEV3ButtonPressed()!=4 && eEV3ButtonPressed()!=5)
		{}


		bool yes=eEV3ButtonPressed()-4;


		while (eEV3ButtonPressed()==yes+4)
		{}


		eraseDisplay();


		//If yes, calls function
		if(yes)
		{
			knockOver();
		}

		//Determines if dominoes should be cleaned up
		displayString(0, "Would you like me to clean up");
		displayString(1, "the dominoes?");
		displayString(3, "Press left button for yes");
		displayString(4, "or right button for no");


		while (eEV3ButtonPressed()!=4 && eEV3ButtonPressed()!=5)
		{}


		yes=eEV3ButtonPressed()-4;


		while (eEV3ButtonPressed()==yes+4)
		{}

		eraseDisplay();


		//If yes, calls function
		if(yes)
		{
			prepareCleanUp();
		}
	}
	else
	{
		//If ended unexpectedly, inform user and wait for button press
		displayString(0, "The Dominator has terminated");
		displayString(1, "due to an emergency stop or");
		displayString(2, "unsafe situation");
		displayString(4, "You may restart the program if");
		displayString(5, "you desire once it is safe to");
		displayString(6, "do so");
		displayString(8, "Press the centre button to");
		displayString(9, "continue...");


		while (eEV3ButtonPressed()!=2)
		{}


		while (eEV3ButtonPressed()==2)
		{}


		eraseDisplay();
	}


	//Closing messages and wait for button press
	displayString(0, "Thank you for using the");
	displayString(1, "Dominator!");
	displayString(3, "The Dominator would like to");
	displayString(4, "thank it's creators:");
	displayString(5, "Eren Rudy, Haaris Ahmed,");
	displayString(6, "Jack Jhu, and Owen Minnes");
	displayString(8, "Press centre button to exit...");


	while (eEV3ButtonPressed()!=2)
	{}
}
