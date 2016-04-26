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
```

This results in the following image:

!(https://github.com/pbosetti/mruby-plplot/raw/master/block_plot.png)



## ToDo

* Legends
* Improve interface of `PLPlot::env`
* Allow to set chart size
* Add support to interctive drivers
* Add support to 3D charts
