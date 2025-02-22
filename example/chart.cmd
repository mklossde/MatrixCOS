# chart exmaple

$col_red = drawColor 15 0 0 
$col_black = drawColor 0 0 0 
$col_white = drawColor 15 15 15

drawClear 

$i = 0 
#loop
  $v = random 0 21 
  $x = $i * 5 
  $x = $x + 1
  drawFull $x 40 5 21 1 $v 21 0 2016
  drawFull $x 40 5 20 1 10 50 888 2016
  $i = $i + 1
  log $i $x $v
  if $i < 12 goto #loop  
  
  drawLine 1 61 60 61 $col_red
	
#$i = 0 
#lines
#  $v random 0 21 
#  $x = $i * 5 
#  $x = $x + 3
#  drawLine $x 40+(a[i-1]) i*5+3,40+(a[i]) col_white
#  if $i < 12 #lines

draw  
log "end"