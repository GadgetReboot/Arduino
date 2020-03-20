Projects using the DHT22 Temperature/Humidity sensor with Arduino Uno

I'm adding my code to the project specifically for: 
Using the ASAIR AM2302 (DHT22) Temp/Humidity sensor with a Velleman VMA412 2.8" 240x320 resistive touch screen
Draws a thermometer bulb graphic with two buttons for you to choose to display the temperature in
degree Celsius or Fahrenheit and shows humidity on an analog meter needle display.

Turns out that I had to make a lot of changes to the sketch made by GadgetReboot to make it usable on my display
the only thing that I have in common is that analog pin 5 isn't used on my shield also, changing it to a digital pin number 19
makes it usable for the sensor... However that's about it, my calibration is completely different in pin usage, the most outer
touch values and usable pixels.
Therefore I had to rewrite pretty much the whole sketch because for me it's completely unusable as is.
It's not perfect yet but at least I managed to get the proportions better so now it starts to fit on the screen, 
I still need to figure out a couple things because I don't really know what I'm doing so I have to learn by changing one thing, 
then upload the sketch to see what changed which is a lot of work and takes a long time but my added sketch will be usable for 
anyone with a similar display that I'm using.
