log "start solor"

mqttAttr pv/energy 1

$pc = 65120
$pt = 63488

$ps = 50

#loop
$pv = $pv/energy

if $pv < -1 {
  $pc = 65120
  $pi = 2016
}
if $pv >= -1 {
  $pc = 21162
  $pi = 63488
  $ps = 50
}
drawClear

# sun
drawArc 5 5 6 13 18 10 10 2 $pc
drawLine 6 15 6 17 $pc
drawLine 15 6 17 6 $pc 
drawLine 10 13 12 15 $pc
drawLine 13 10 15 12 $pc

# house
drawLine 1 61 63 61 $pt
drawRect  41 40 19 20 $pt
drawTriangle 40 40 60 40 50 30 $pt
drawRect 43 50 6 10 $pt
drawRect 51 50 6 6 $pt 
drawLine 38 40 50 28 $pt

#text
drawText 5 53 $pi 1 $pv

#shine
if $ps < 45 drawArc 5 5 6 15 15 $ps $ps 1 65120

draw

  wait 100
  $ps = $ps + 5
  if $ps <= 50 goto #loop
  $ps = 15

wait 1000

goto #loop



#end
mqttAttr pv/energy 0



