/**********************************************************************
Filename: EV3ButtonPressed.c
Date: October 31, 2016
File Version: 1.0

Mimic the functionality of nNxtButtonPressed by polling all of the
buttons using getButtonPress and returning the result.  As these functions
only return one number, priortiy is given to the lowest button number.

int eEV3ButtonPressed() returns an integer using EV3 numbering, and -1 for not pushed
int nEV3ButtonPressed() returns an integer using NXT numbering, or returns 0 for unmapped buttons

In general, eEV3ButtonPressed should be used so that all possible button
possibilities are returned. The function nEV3ButtonPressed is provided
for backwards compatibility with the NXT button mapping.

History
Ver  Date       Comment
1.1  Nov 18/16  removed test task main
1.0  Oct 31/16  original release

**********************************************************************/
#pragma SystemFile

#define _MAIN_EV3BUTTONPRESSED 0  // 1 for test main program, 0 for use in other programs

#ifndef EV3BUTTONPRESSED_C
#define EV3BUTTONPRESSED_C

#if _MAIN_EV3BUTTONPRESSED
const int _NUM_NXT_BUTTON = 4;
#endif
const int _NXT_NO_BUTTON = -1;
const int _NXT_STOP_BUTTON = 0;
const int _NXT_RIGHT_BUTTON = 1;
const int _NXT_LEFT_BUTTON = 2;
const int _NXT_CENTRE_BUTTON = 3;

const int _NUM_EV3_BUTTON = 8;
#if _MAIN_EV3BUTTONPRESSED
const int _EV3_NO_BUTTON = 0;
const int _EV3_UP_BUTTON = 1;
const int _EV3_CENTRE_BUTTON = 2;
const int _EV3_DOWN_BUTTON = 3;
const int _EV3_RIGHT_BUTTON = 4;
const int _EV3_LEFT_BUTTON = 5;
const int _EV3_BACK_BUTTON = 6;
#endif
const int _EV3_ANY_BUTTON = 7;

const int _EV3_START_BUTTON_CHECK = 1;
const int _EV3_END_BUTTON_CHECK = 6;

const int _NXT_BUTTON_MAP[_NUM_EV3_BUTTON] = {0, 0, _NXT_CENTRE_BUTTON, 0,_NXT_RIGHT_BUTTON,
	                            _NXT_LEFT_BUTTON, _NXT_STOP_BUTTON, 0};

#if _MAIN_EV3BUTTONPRESSED
const string _EV3_NAMES_BUTTON[_NUM_EV3_BUTTON] = {"none", "up", "centre", "down", "right",
                                            "left", "back", "any"};
const string _NXT_NAMES_BUTTON[_NUM_NXT_BUTTON] = {"other", "right", "left", "centre"};
#endif

int eEV3ButtonPressed()
{
    int buttonState = _NXT_NO_BUTTON;
    if ( (bool)getButtonPress(_EV3_ANY_BUTTON) )
    {
        for (int button = _EV3_START_BUTTON_CHECK;
                buttonState == _NXT_NO_BUTTON && button <= _EV3_END_BUTTON_CHECK; button++)
            if ((bool)getButtonPress(button))
                buttonState = button;
    }
    return buttonState;
}

int nEV3ButtonPressed()
{
    int buttonState = eEV3ButtonPressed();
    if (buttonState >= 0)
        buttonState = _NXT_BUTTON_MAP[buttonState];
    return buttonState;
}
#endif

#if _MAIN_EV3BUTTONPRESSED
task main()
{
	  setBlockBackButton(true);

	  displayString(0,"EV3 button test");
	  displayString(1,"Hit any button to continue");
    while (eEV3ButtonPressed() == -1)
    {}
    while (eEV3ButtonPressed() != -1)
    {}

    for (int i=_EV3_START_BUTTON_CHECK; i <= _EV3_END_BUTTON_CHECK; i++)
    {
        eraseDisplay();
        displayString(0,"Hit %s button",_EV3_NAMES_BUTTON[i]);
        while (eEV3ButtonPressed() != i)
        {}
        displayString(1,"Button %s hit",_EV3_NAMES_BUTTON[i]);
        while (eEV3ButtonPressed() != -1)
        {}
        displayString(2,"Button %s released",_EV3_NAMES_BUTTON[i]);
        displayString(3,"Waiting 5 seconds....");
        wait1Msec(5000);
    }

    eraseDisplay();
	  displayString(0,"Button test - NXT map");
	  displayString(1,"Hit any button to continue");
    while (nEV3ButtonPressed() == -1)
    {}
    while (nEV3ButtonPressed() != -1)
    {}
	  setBlockBackButton(true);
    for (int i=_EV3_START_BUTTON_CHECK; i <= _EV3_END_BUTTON_CHECK; i++)
    {
        eraseDisplay();
        displayString(0,"Hit %s button",_EV3_NAMES_BUTTON[i]);
        int buttonCheck = 0;
        if (i == _EV3_RIGHT_BUTTON)
            buttonCheck = _NXT_RIGHT_BUTTON;
        else if (i == _EV3_LEFT_BUTTON)
        	  buttonCheck = _NXT_LEFT_BUTTON;
        else if (i == _EV3_CENTRE_BUTTON)
        	  buttonCheck = _NXT_CENTRE_BUTTON;
        while (nEV3ButtonPressed() != buttonCheck)
        {}
        displayString(1,"Button %s hit",_NXT_NAMES_BUTTON[buttonCheck]);
        while (nEV3ButtonPressed() != -1)
        {}
        displayString(2,"Button %s released",_NXT_NAMES_BUTTON[buttonCheck]);
        displayString(3,"Waiting 5 seconds....");
        wait1Msec(5000);
    }

    setBlockBackButton(false);
}
#endif
/*
task main()
{
	displayString(0,"%d",eEV3ButtonPressed());
	displayString(1,"%d",nEV3ButtonPressed());
}
*/
/**********************************************************************
Copyright(c) 2016 C.C.W. Hulls, P.Eng.
Students, staff, and faculty members at the University of Waterloo
are granted a non-exclusive right to copy, modify, or use this
software for non-commercial teaching, learning, and research purposes
provided the author(s) are acknowledged except for as noted below.
**********************************************************************/
