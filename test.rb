l1 = PLPlot::Series.new  # first data series
l2 = PLPlot::Series.new  # second data series

100.times do |i|         # fill data sets with points [x, y]
  l1 << [i, i**2]
  l2 << [i, i**2.2]
end

PLPlot.set_grid(1,3)     # I want a grid of 1 column, 3 rows of charts

PLPlot.plot("block_plot.png") do |p|  # driver is inferred from file extension

  p.env(*l1.range, 0, 1)
  p.lab("x", "y", "Test Plot from mruby")
  l1.line(:blue, 1, 3)
  l2.line(:red, 2.5)
    
  p.env(*l1.range, 0, 1)
  p.lab("x", "y", "Test Plot from mruby")
  l1.line(:violet)
  l2.line(:brown)  

  p.env(*l1.range, 0, 1)
  p.lab("x", "y", "Test Plot from mruby")
  l1.line(PLPlot.cycle_color)
  l2.line(PLPlot.cycle_color)  
  l2.points(:black)

end


