mqttAttr shellies/shellyem3/emeter/0/power 1
mqttAttr shellies/shellyem3/emeter/1/power 1 
mqttAttr shellies/shellyem3/emeter/2/power 1

#loop
  drawClear
  drawColor 15 15 15
  drawText 5 1 -1 1 "Shelly EM"
  drawColor 15 0 0 

  valueFull 1 12 "Power0" $shellies/shellyem3/emeter/0/power 1000 888 444
  valueFull 1 30 "Power1" $shellies/shellyem3/emeter/1/power 1000 888 444
  valueFull 1 47 "Power2" $shellies/shellyem3/emeter/2/power 1000 888 444

  draw
  wait 2000
  goto #loop

#end 
  mqttAttr shellies/shellyem3/emeter/0/power 0
  mqttAttr shellies/shellyem3/emeter/1/power 0 
  mqttAttr shellies/shellyem3/emeter/2/power 0