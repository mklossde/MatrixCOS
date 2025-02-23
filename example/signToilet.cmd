log "WC sign"

$mode = 1
$h = 0
$w = 0

#start

$red = drawColor 15 0 0
$green = drawColor 0 15 0
$white = drawColor 15 15 15

$y = 10


drawClear
drawRect 10 3 41 34
if $mode != 2 drawText 15 50 1 "Toilet" $white 
if $mode == 2 drawText 10 50 1 "occupied" $red

# man
$x = 20
if $mode == 2 {
  fillRect $x - 7 $y - 4 15 30 $red 
}

fillCircle $x $y 3 $white
fillRect $x - 2 $y + 5 5 12
#fillRect $x - 2 $y + 13 2 12
drawLine  $x - 2 $y + 13 $x - 2 - $h $y + 20
drawLine  $x - 2 - $h $y + 20 $x - 2 + $h $y + 24
drawLine  $x - 1 $y + 13 $x - 1 - $h $y + 20
drawLine  $x - 1 - $h $y + 20 $x - 1 + $h $y + 24
#fillRect $x + 1 $y + 13 2 12
drawLine  $x + 1 $y + 13 $x + 1 $y + 18
drawLine  $x + 1 $y + 19 $x + 1 $y + 24
drawLine  $x + 2 $y + 13 $x + 2 $y + 18
drawLine  $x + 2 $y + 19 $x + 2 $y + 24

fillRect $x - 4 $y + 5 9 2
drawLine $x - 5 $y + 5 $x - 5 - $h $y + 14 
drawLine $x - 4 $y + 5 $x - 4 - $h $y + 14 
drawLine $x + 4 $y + 5 $x + 4 - $h $y + 14 
drawLine $x + 5 $y + 5 $x + 5 - $h $y + 14 

# woman
$x = 40
if $mode == 2 {
  fillRect $x - 7 $y - 4 15 30 $red 
}
fillCircle $x $y 3 $white
fillTriangle $x $y + 5 $x - 5 $y + 17 $x + 5 $y + 17 $white
fillRect $x - 4 $y + 5 9 2
#fillRect $x - 2 $y + 13 2 12
drawLine  $x - 2 $y + 13 $x - 2 - $w $y + 20
drawLine  $x - 2 - $w $y + 20 $x - 2 + $w $y + 24
drawLine  $x - 1 $y + 13 $x - 1 - $w $y + 20
drawLine  $x - 1 - $w $y + 20 $x - 1 + $w $y + 24
#fillRect $x + 2 $y + 13 2 12
drawLine  $x + 1 $y + 13 $x + 1 $y + 18
drawLine  $x + 1 $y + 19 $x + 1 $y + 24
drawLine  $x + 2 $y + 13 $x + 2 $y + 18
drawLine  $x + 2 $y + 19 $x + 2 $y + 24
#drawLine $x - 5 $y + 5 $x - 7 $y + 15
drawLine $x - 5 $y + 5 $x - 5 - $w $y + 14 
drawLine $x - 4 $y + 5 $x - 4 - $w $y + 14 
drawLine $x + 4 $y + 5 $x + 4 - $w $y + 14 
drawLine $x + 5 $y + 5 $x + 5 - $w $y + 14 

drawRect 30 7 2 28

# ponter
fillRect 10 38 45 5
fillTriangle 55 35 60 40 55 45
draw

if $mode == 1 goto #freeLoop
else if $mode == 3 {
  $h = $h + 1
  if $h > 3 $h = 0
  wait 500
  goto #start
}else if $mode == 4 {
  $w = $w + 1
  if $w > 3 $w = 0
  wait 500
  goto #start
}else {
  wait 1000
}
goto #start

#freeLoop
$px = 10
#ploop
{
  drawLine $px 38 $px 42 $green
  draw
  wait 50
  $px = $px + 1
} until $px > 54
fillTriangle 55 35 60 40 55 45 $green
draw
wait 500
goto #start








