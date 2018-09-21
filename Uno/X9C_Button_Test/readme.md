Testing a library for the X9C Intersil digital potentiometer from https://github.com/philbowles/Arduino-X9C

X9C Datasheet:  https://www.intersil.com/content/dam/Intersil/documents/x9c1/x9c102-103-104-503.pdf

This test sketch uses push buttons to test the functions, allowing control of the potentiometer to 
move the wiper up/down by one position, ten positions, or set directly to min/mid/max levels.

A save function is added to the ten position up button to test that the pot can store the wiper position
when requested, allowing that wiper setting to be restored upon power on.

Note that in order to get the sketch operating as expected, two functions need to be modified from the original library mentioned above.  These changes are noted in the comments at the top of the sketch file X9C_Button_Test.ino

X9C_Button_Test.ino is the main test sketch

X9C_555_Osc_Control.ino is a modification to turn the buttons into sound effect generators when the X9C is used to replace a frequency control pot of a 555 oscillator.
