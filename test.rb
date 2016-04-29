l1 = PLPlot::Series.new  # first data series
l2 = PLPlot::Series.new  # second data series
l1.point_color = :blue   # Set default point color (also for glyph type)
l2.line_color = :brown   # Set default line color (also for width and type)

100.times do |i|         # fill data sets with points [x, y]
  l1 << [i, i**2]
  l2 << [i, i**2.2]
end

range = PLPlot.envelope [l1, l2] # calculate the XY envelope of l1 and l2

puts "Printing using PLPlot v#{PLPlot.version}"
puts "Available file formats: #{PLPlot::FORMATS}"

PLPlot.set_grid(1,3)           # I want a grid of 1 column, 3 rows of charts
PLPlot.set_page(720, 540)      # make it 720px x 540px (default)

PLPlot.plot("block_plot.png") do |p|  # driver is inferred from file extension

  p.env(*range)                # use full range for both l1 and l2
  p.lab("x", "y", "Test Plot from mruby")
  l1.line(:blue, 1, 3)         # thickness 1, line type 3
  l2.line(:red, 2.5)           # thickness 2.5, line type 0 (default)
  
  p.box = :major               # see PLPlot::BOX_CODES and PLPlot::SCALING_CODES
  p.env(*l1.range)             # use only range of l1
  p.lab("x", "y", "Second Test Plot from mruby")
  l1.line(:violet)
  l2.line  

  p.box = :ticks               # sticky attribute (for next subplots)
  p.scaling = :square          # sticky attribute
  p.env(*l1.range)
  p.chr_scale = 1.3            # increase font size by 30%
  p.lab("x", "y", "Square plot")
  l1.line(PLPlot.cycle_color)  # cycles line color from 1 to 15 at every call
  l2.line(PLPlot.cycle_color)  
  l2.points                    # brown square points
  p.chr_scale = 1              # back to default font size
  
end
