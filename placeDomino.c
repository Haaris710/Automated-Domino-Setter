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
