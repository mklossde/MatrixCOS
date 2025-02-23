log "fire sign"


$white = drawColor 15 15 15
$red = drawColor 15 0 0
$green = drawColor 0 10 0
$man = $green

#start
drawClear 

#exit
fillRect 0 32 64 32 $green
drawRect 1 33 62 30 $white
fillRect 32 43 20 8 $white
fillTriangle 52 38 52 55 60 47 $white
fillRect 10 37 20 24 $white
$x = 22
fillCircle $x 41 2 $man
drawWLine $x - 1 45 $x - 3 52 3 1 $man
drawWLine $x - 1 45 $x - 5 45 1 2 $man
drawWLine $x - 4 45 $x - 7 49 2 1 $man
drawWLine $x + 1 45 $x + 4 49 2 1 $man
drawWLine $x + 4 49 $x + 7 49 1 2 $man
drawWLine $x - 3 52 $x - 4 54 2 1 $man
drawWLine $x - 4 55 $x - 8 55 1 2 $man
drawWLine $x - 2 50 $x + 4 59 2 1 $man   

#Fire
drawText 10 8 2 "FIRE" $red
$i = 10
{
  $y = random 5 8
  if $y <= 6 drawLine $i 6 $i $y $red
  $i = $i + 1
} until $i > 55

draw
wait 50
goto #start