# PLPlot — Plotting gem for mruby

This library is based on [PLplot](http://plplot.sourceforge.net) for quickly plotting charts with mruby.

While PLplot has a huge arsenal of features, this gem is focused on provide a simple, sensible OO interface to it, sometimes deliberately altering the original PLplot interface style in favor to what I (arguably) consider as a faster and more convenient way for getting the work done.

## Requirements

Of course a working install of PLplot is needed. It is cross-platform and known to work on Linux, OS X, and Windows. At the moment, **only Cairo drivers are here directly supported**

## Mini How-to

As simple as that:

```ruby
l1 = PLPlot::Series.new  # first data series
l2 = PLPlot::Series.new  # second data series

100.times do |i|         # fill data sets with points [x, y]
  l1 << [i, i**2]
  l2 << [i, i**2.2]
end

range = PLPlot.envelope [l1, l2] # calculate the XY envelope of l1 and l2

puts "Printing using PLPlot v#{PLPlot.version}"
puts "Available file formats: #{PLPlot::FORMATS}"

PLPlot.set_grid(1,3)      # I want a grid of 1 column, 3 rows of charts
PLPlot.set_page(720, 540) # make it 720px x 540px (default)

PLPlot.plot("block_plot.png") do |p|  # driver is inferred from file extension

  p.env(*range)        # use full range for both l1 and l2
  p.lab("x", "y", "Test Plot from mruby")
  l1.line(:blue, 1, 3) # thickness 1, line type 3
  l2.line(:red, 2.5)   # thickness 2.5, line type 0 (default)
  
  p.box = :major       # see PLPlot::BOX_CODES and PLPlot::SCALING_CODES
  p.env(*l1.range)     # use only range of l1
  p.lab("x", "y", "Second Test Plot from mruby")
  l1.line(:violet)
  l2.line(:brown)  

  p.box = :ticks              # sticky attribute (for next subplots)
  p.scaling = :square         # sticky attribute
  p.env(*l1.range)
  p.chr_scale = 1.3            # increase font size by 30%
  p.lab("x", "y", "Square plot")
  l1.line(PLPlot.cycle_color) # cycles line color from 1 to 15 at every call
  l2.line(PLPlot.cycle_color)  
  l2.points(:black, 0)        # black square points
  p.chr_scale = 1              # back to default font size

end
```

This results in the following image:

![example plot](https://github.com/pbosetti/mruby-plplot/raw/master/block_plot.png)



## ToDo

* Legends
* ~~Improve interface of `PLPlot::env`~~
* ~~Allow to set chart size~~
* Add support to interctive drivers
* Add support to 3D charts
