# counter 9
$c = 9
$x = 0; $y = 64


#loop
  drawClear
  drawColor 15 15 15
  drawRect $x $x $y $y
  drawColor 0 0 15
  drawText 20 15 -1 5 $c
  draw
  log counter $c
  wait 1000
  
  $x = $x + 3
  $y = $y - 6
  $c = $c - 1 
  if $c >= 0 #loop


$c = 3
#light
  drawClear
  drawColor 15 15 15
  FillRect 0 0 64 64 
  draw
  wait 10

  drawClear
  draw
  wait 100

  $c = $c - 1
  if $c >= 0 #light


#happy
  drawClear
  drawColor 15 0 0
  drawText 20 10 -1 1 "Happy"
  drawText 10 20 -1 1 "Birthday"
  draw

log "end"
