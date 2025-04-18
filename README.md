
# MatrixCOS - Esp32 HUB75 Matrix

develop by OpenOn.org

![CmdOS LOGO](images/Hub75_logo.gif)


This Application based on 
- <a href="http://github.com/mklossde/CmdOs">Use CmdOS V0.1.0</a>
- <a href="https://github.com/mklossde/GFXDisplayCOS">GFXDisplayCOS</A>

This is a ESP32 based Applciation to display information via HUB75 Matrix display.

It a christmas projet inspired by 
	<a href='https://github.com/JPlenert/eely-hodi'>eely-HoDi Make Articel</a> 

![Display-Title](images/pageTitle.gif)		

##  Example animation gif frame
![Display-Example](images/frame1.gif)
![Display-Example](images/frame2.gif)
![Display-Example](images/images.gif)		
Display images as a pixel frame 

## Example alarm sign
![example](images/alarm1.gif) ![example](images/alarm2.gif) ![example](images/alarm3.gif)
draw alarm sign  <a href='example/alarm.cmd'>alarm.cmd</a>

## Example toilet sign
![example](images/toilet1.gif) ![example](images/toilet2.gif) ![example](images/toilet3.gif)
![example](images/toilet11.gif) ![example](images/toilet22.gif)
draw toilet sign  <a href='example/signToilet.cmd'>signToilet.cmd</a>

## Fire and Exit
![example](images/FireAndExit.gif)
draw fire and exit sign  <a href='example/fireAndExit.cmd'>fireAndExit.cmd</a>

## Digital wash information
![example](images/wash2.gif) ![example](images/wash.gif)
washer example  <a href='example/wash.cmd'>wash.cmd</a>
(e.g. use by read $m from tasmota or shelly plug and show wash if acutal power > 10 and done if actual power < 5)

## Room
![example](images/room.gif)
Next please in room X  <a href='example/room.cmd'>room.cmd</a>

##  Example Home Assistant / Shelly / Mqtt
![Display-Example](images/pageEsp.gif)	
![Display-Example](images/shellyEM.gif)	
![Display-Example](images/example_pv2.gif)	
Display power values of a shelly EM (via MQTT) 

	mqttAttr shellies/shellyem3/emeter/0/power 1
	mqttAttr shellies/shellyem3/emeter/1/power 1 
	mqttAttr shellies/shellyem3/emeter/2/power 1
	#loop
	  drawClear
	  drawColor 15 15 15
	  drawText 5 1 1 "Shelly EM"
	  drawColor 15 0 0 
	  valueFull 1 12 "Power0" $shellies/shellyem3/emeter/0/power 1000 888 444
	  valueFull 1 30 "Power1" $shellies/shellyem3/emeter/1/power 1000 888 444
	  valueFull 1 47 "Power2" $shellies/shellyem3/emeter/2/power 1000 888 444
	  draw
	  wait 2000
	  goto #loop

## Effects

![Display-Example](images/pageTime.gif)	
<a href='doc/Effect.md'>Effects</a>

## Command Examples 
<a href='example/examples.md'>command Examples</a>

## Feature 
 - <a href='doc/Config.md'>Hardware install and config</a>
 - <a href='doc/Commands.md'>matrix commands</a>
 - <a href='doc/Web.md'>web control</a> 
 - <a href='doc/Mqtt.md'>mqtt control</a>
 - <a href='doc/Rest.md'>rest control</a>
 - <a href='doc/Timer.md'>autostart and timer</a> 
 - <a href='doc/Multi.md'>multi display</a> 
 - <a href='https://github.com/mklossde/CmdOs/blob/main/doc/CmdOsCmds.md'>CmdOS Commands</a>
 


## Setup
	matrix 64 64 1 90 0 0,15,4,16,27,17,5,18,19,21,12,33,25,22
	matrix2 0 1 1 0


[<a href="libs.md">Require Libs</a>]

<hr> :warning: Attention: The use of information and programmes can be dangerous !  This is a private hobby project and makes no claim to completeness, runnability or compliance with regulations. Any use of this or referenced content is at your own risk and excludes any liability by authors. 
<hr>

![LOGO](images/OpenOnOrg.gif) a OpenOn.org project - develop by michael@openon.org

If you use/like this project please [https://buymeacoffee.com/openon](https://buymeacoffee.com/openon) (a good choise is 1Euro per Device ;-) 

