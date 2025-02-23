log "washer"

$white = drawColor 15 15 15
$blue = drawColor 0 0 10

#start

drawClear 

drawText 12 1 1 "washing" $blue

drawRoundRect 18 10 29 40 3 $white
drawLine 18 20 46 20 $white
drawRoundRect 20 14 5 2 1 $white
drawRoundRect 32 14 2 2 1 $white
drawRoundRect 36 14 2 2 1 $white
drawRoundRect 40 14 2 2 1 $white
drawCircle 32 33 10 $white

int $i = 0
{
  drawArc 32 33 10 $i 1 9 9 2 $blue
  draw
  drawArc 32 33 10 $i $i 9 9 2 0
  $i = $i + 1
  wait 100
} until $i > 36

goto  #start