log "alarm enabled"

$white = drawColor 15 15 15
$red = drawColor 15 0 0
$mode = 3

#start
drawClear

#light
fillRoundRect 25 10 20 20 3 $red
drawRoundRect 25 10 20 20 3 $white
drawRoundRect 20 29 30 10 3 $white

draw

if $mode == 1 goto #arming
if $mode == 2 goto #secured
if $mode == 3 goto #alarm
goto #start


#arming
$t = 0
$i = 0
$c = 10
{
  drawFull 10 53 50 9 1 $i 100 $white $red
  drawText 18 53 1 "arming" $white 
  fillRect 29 15 12 8 $red
  drawText 29 15 1 $c $white
  $i = $i + 1
  $t = $t + 1
  if $t == 10 {
    $c = $c - 1
    $t = 0
  }
  draw
  wait 100
} until $i > 100
goto #start

#secured
$i = 0
$c = 0
$r = 1
$x = 10
{
  drawText 15 42 1 "secured" $white 

  {
    drawCircle 10 + $x 54 5 $red
    fillCircle 10 + $x 54 3 $red
    draw
    wait 10
    fillCircle 10 + $x 54 5 0
    $x = $x + $r 
    if $x > 40 $r = -1
    if $x < 10 $r = 1 
    $i = $i + 1
  } until $i > 200
   

  goto #start

#alarm
  drawText 20 42 1 "Alarm" $white 
  drawText 21 42 1 "Alarm" $white

$i = 0
$c = 0

{
  $w = 64 - $i
  $w = $w - $i
  drawRect $i $i $w $w $red
  $h = 10 + $i + $i + $i
  drawArc 42 15 6 5 20 $h $h 1 $red
  drawArc 27 15 6 35 20 $h $h 1 $red
  draw
  wait 100 
  drawRect $i $i $w $w 0

  $i = $i + 1
  $c = $c + 1

} until $c > 3

  goto #start












