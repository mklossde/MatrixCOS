# draw wifi
drawClear
$red = drawColor 15 0 0 
drawArc  30 50 3 90 60 30 30 3 $red
drawArc  30 50 3 90 60 20 20 3 $red
drawArc  30 50 3 90 60 10 10 3 $red
fillTriangle 30 50 0 0 0 64 0
fillTriangle 30 50 64 0 64 64 0
fillCircle 30 55 3 -1
draw