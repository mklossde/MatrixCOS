log "bounce line"
$x1 = random 0 64
$y1 = random 0 64
$x2 = 50; $y2 = 5
$px1 = 1; $py1 = 1 ; $px2 = 1; $py2 = 1
$col = random 1 60000

#loop
  drawClear
  drawLine $x1 $y1 $x2 $y2 $col
  draw
  log  " a:" $x1 $y1 $x2 $y2 $col $px1 $py1 $px2 $py2

  $x1 = $x1 + $px1
  $y1 = $y1 + $py1
  $x2 = $x2 + $px2
  $y2 = $y2 + $py2

  log  " b:" $x1 $y1 $x2 $y2 $col $px1 $py1 $px2 $py2

  if $x1 > 64 {
    $px1 = $px1 * -1
    $col = random 1 60000
  }
  if $x1 <= 0 $px1 = $px1 * -1
  if $x2 > 64 $px2 = $px2 * -1
  if $x2 <= 0 $px2 = $px2 * -1
  if $y1 > 64 $py1 = $py1 * -1
  if $y1 <= 0 $py1 = $py1 * -1
  if $y2 > 64 $py2 = $py2 * -1
  if $y2 <= 0 $py2 = $py2 * -1

goto #loop