log "draw random images"

#loop
  drawClear
  $max = fsDirSize .gif
  $r = random 0 $max
  $file = fsFile .gif $r 0
#  log "draw" $file
  drawFile $file 0 0

  wait 1000
  drawStop 
  $rx = random -1 3
  $ry = random -1 3
  attr rx $rx * 5
  $ry = $ry * 5
  effect 1 64 20 $rx $ry
  wait 1300
  goto #loop