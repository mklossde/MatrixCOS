log "random color"
$px = 0; $py = 0; $pc = 0
$pa random 0 16
$pb random 8 265

drawClear

#loop
  drawPixel $px $py $pc
log pixel $px $py $pc
  $pc = $pc + $pa
  $px = $px + 1
  if $px < 64 #loop
  $px = 0
  $py = $py + 1
  $pc = $pc + $pb
  if $py < 64 #loop

draw
log "end"