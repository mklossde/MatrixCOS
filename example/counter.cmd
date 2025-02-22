# counter 9
$c = 9
$x = 0
$y = 64
$white = drawColor 15 15 15
$red = drawColor 15 0 0

{
  drawClear  
  drawRect $x $x $y $y $white
  drawText 20 15 $red 5 $c
  draw
  log counter $c
  wait 1000
  
  $x = $x + 3
  $y = $y - 6
  $c = $c - 1 
} until $c < 0 


$c = 3
{
  drawClear
  fillRect 0 0 64 64 $white
  draw
  wait 10

  drawClear
  draw
  wait 100

  $c = $c - 1
} until $c < 0


#happy
  drawClear
  drawText 20 10 $red 1 "Happy"
  drawText 10 20 $red 1 "Birthday"
  draw

log "end"
