Testing a library for the X9C Intersil digital potentiometer from https://github.com/philbowles/Arduino-X9C

X9C Datasheet:  https://www.intersil.com/content/dam/Intersil/documents/x9c1/x9c102-103-104-503.pdf

This test sketch uses push buttons to test the functions, allowing control of the potentiometer to 
move the wiper up/down by one position, ten positions, or set directly to min/mid/max levels.

A save function is added to the ten position up button to test that the pot can store the wiper position
when requested, allowing that wiper setting to be restored upon power on.
