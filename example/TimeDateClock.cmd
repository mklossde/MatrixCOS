#time and date clock
#loop
  drawClear
  drawColor 15 0 0 
  drawTime 10 5
  drawDate 2 20
  drawLine 5 15 60 15
  draw
  wait 500

  drawColor 15 15 15
  drawLine 5 15 60 15
  draw

  wait 500 
  goto #loop