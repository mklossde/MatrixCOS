
# MatrixCOS - Timer

## Startup 

After 10sec startup the file "startup.cmd" is executed

	fsWrite startup.cmd "log started; drawOff ; drawColor 0 15 0; drawRect 10 10 20 20;draw"
	restart
	will write cmds into "startup.cmd"
	10 sec after startup the "cmds" are executed

## Timer 

Add a timer to execute programm on Timer
	e.g. 
	timer 1 0 22 -1 -1 -1 -1 "drawOff" => switch display off every day 22:00
	timer 1 0 8 -1 -1 -1 -1 "run morning.cmd" => run "morning.cmd" everay morning at 8:00	
	
![LOGO](../images/Hub75_logo_32x32.gif) a OpenOn.org project - develop by mk@almi.de 