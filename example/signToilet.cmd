log "WC sign"

$mode = 1

#start

$red = drawColor 15 0 0
$green = drawColor 0 15 0
$white = drawColor 15 15 15

$y = 10

drawClear
drawRect 10 3 41 34

# man
$x = 20
if $mode == 2 {
  fillRect $x - 7 $y - 4 15 30 $red 
}

fillCircle $x $y 3 $white
fillRect $x - 2 $y + 5 5 12
fillRect $x - 2 $y + 13 2 12
fillRect $x + 1 $y + 13 2 12
fillRect $x - 4 $y + 5 9 2
fillRect $x - 5 $y + 5 2 10
fillRect $x + 4 $y + 5 2 10

# woman
$x = 40
if $mode == 2 {
  fillRect $x - 7 $y - 4 15 30 $red 
}
fillCircle $x $y 3 $white
fillTriangle $x $y + 5 $x - 5 $y + 17 $x + 5 $y + 17 $white
fillRect $x - 4 $y + 5 9 2
fillRect $x - 2 $y + 13 2 12
fillRect $x + 2 $y + 13 2 12
#drawLine $x - 5 $y + 5 $x - 7 $y + 15
fillRect $x - 5 $y + 5 2 10
fillRect $x + 4 $y + 5 2 10

drawRect 30 7 2 28

# ponter
fillRect 10 38 45 5
if $mode != 2 {
  fillTriangle 55 35 60 40 55 45
}
draw

if $mode == 1 goto #freeLoop

wait 1000
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
wait 500
goto #start








