
# MatrixCOS - Mqtt 

## Config Mqtt Server 

use cmd "mqtt" to config  mqtt Server
	e.g. mqtt://192.168.1.1:1833
or with user+ password + ssl
	e.g. mqtts://admin:pas@192.168.1.1:1833

## Receive a topic 

use mqttAttr to recevie all messages from a topic into attr
	e.g. mqttAttr dev/myesp/energy 1
will subscribe topic "dev/myesp/energy" and set attr "$dev/myesp/energy" on every value change

	mqttAttr dev/myesp/energy 1
	wait 1000
	log "myesp energy" $dev/myesp/energy
	drawText 10 10 888 1 $dev/myesp/energy => draw value on display

e.g. mqttAttr dev/esp/energy => all posts will put into attr $dev/esp/energy 

## Send to a topic

mqttSend topic maessage
	e.g. mqtttSend dev/myesp/info "prgramm xyz started"
	
	
	
![LOGO](../images/Hub75_logo_32x32.gif) a OpenOn.org project - develop by mk@almi.de 