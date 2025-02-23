log "room"

$white = drawColor 15 15 15
$col = drawColor 10 10 0
$nr = 1
$ticket = "#4711"
$name = "Mr Smith"

#start
drawClear 


drawText 2 5 2 "Room" $white
drawText 52 5 2 $nr $white
#drawLine 1 22 63 22 $white 
drawRect 0 0 64 22 $white

drawText 1 25 1 "next" $col
drawText 34 25 1 $ticket $col
drawText 5 40 1 $name $col


$y = 56
fillTriangle 55 $y - 3 60 $y + 2 55 $y + 7 $col




$i = 0
{
  fillRect 10 + $i $y 45 - $i 5 $col
  draw
  wait 10
  fillRect 10 + $i $y 45 - $i 5 0
  $i = $i + 1
} until $i > 45

goto #start