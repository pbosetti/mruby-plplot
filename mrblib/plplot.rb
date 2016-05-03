module PLPlot
  FORMATS = %w(pdf ps eps svg png)
  COLORS = {
    :white      => 0 ,
    :red        => 1 ,
    :yellow     => 2 ,
    :green      => 3 ,
    :aquamarine => 4 ,
    :pink       => 5 ,
    :wheat      => 6 ,
    :grey       => 7 ,
    :brown      => 8 ,
    :blue       => 9 ,
    :violet     => 10,
    :cyan       => 11,
    :turquoise  => 12,
    :magenta    => 13,
    :salmon     => 14,
    :black      => 15
  }
  SCALING_CODES = {free:0, equal:1, square:2}
  SCALING_CODES.default = 0
  BOX_CODES = {nothing:-2, box_only:-1, ticks:0, axes:1, major:2, minor:3}
  
  @grid = []
  @series = []
  @current_color = 1
  @scaling = 0
  @box = 0
  
  def self.colors; COLOR.keys; end
  
  def self.get_color(o)
    case o
    when Numeric
      raise ArgumentError, "Numbers from 0 to 15" unless (0..15).include? o
      return o.to_i
    when Symbol
      return COLORS[o] || 15
    when String
      return get_color(o.to_sym)
    else
      return ArgumentError, "Cannot convert #{o} to a color index"
    end
  end
  
  def self.scaling=(k)
    raise ArgumentError, "Only :free, :equal, or :square" unless SCALING_CODES.keys.include? k
    @scaling = SCALING_CODES[k]
  end
  
  def self.box=(k)
    case k
    when Fixnum
      if (0..3).include?(k.to_s[-1].to_i) && (-1..8).include?(k/10) then
        @box = k
      else
        raise ArgumentError, "Invalid box code (see plenv() C function)"
      end
    when Symbol 
      raise ArgumentError, "Unknown box key" unless BOX_CODES.keys.include? k
      @box = BOX_CODES[k]
    else
      raise ArgumentError, "Unsupported box key"
    end
  end
  
  def self.cycle_color
    @current_color = 0 if @current_color == 15
    return @current_color += 1
  end
  
  def self.plot(fname=nil, dev=nil, &block)
    if dev then 
      self.sdev(dev)
      self.sfnam("fname.#{dev}")
    elsif fname then
      ext = fname.split('.')[-1]
      if FORMATS.include? ext then
        self.sdev("#{ext}cairo")
      else
        raise ArgumentError, "Unsupported device #{ext}cairo"
      end
      self.sfnam(fname)
    end
    unless @grid == [] or @grid == [1,1] then
      self.star
    else
      self.init
    end
    yield self
    self.end
  end
  
  def self.set_grid(nx=1, ny=1)
    raise ArgumentError, "Need two Integers" unless nx.kind_of? Fixnum and ny.kind_of? Fixnum
    @grid[0] = nx
    @grid[1] = ny
    return @grid
  end
  
  def self.load(series)
    raise ArgumentError, "Need an Array of PLPlot:Series" unless series.kind_of? Array
    series.each do |s|
      s.instance_variable_set("@has_line", false)
      s.instance_variable_set("@has_points", false)
    end
    @series = series
    return self.env(*self.envelope)
  end
  
  def self.set_range(*limits)
    self.env(*limits, false)
  end
  
  def self.envelope(series=@series)
    raise ArgumentError, "Need an Array of PLPlot:Series" unless series.kind_of? Array
    pair = series[0].range
    series.inject(pair) {|m,v|
      r = v.range
      m[0] = [m[0], r[0]].min
      m[1] = [m[1], r[1]].max
      m[2] = [m[2], r[2]].min
      m[3] = [m[3], r[3]].max
      m
    }
  end
  
  def self.legend(series=@series)
    names = []
    options = []
    lines_c = []
    lines_w = []
    lines_s = []
    point_c = []
    point_g = []
    point_s = []
    
    series.each do |s|
      names << s.name
      options << s.what_in_legend
      # options[-1] += 1 if s.line_shown
      # options[-1] += 2 if s.points_shown
      lines_c << PLPlot.get_color(s.line_color)
      lines_w << s.width
      lines_s << s.style
      point_c << PLPlot.get_color(s.point_color)
      point_g << s.glyph
      point_s << s.scale
    end
    
    self._legend(
      :tl, names, options, 
      lines_c, lines_w, lines_s, 
      point_c, point_g, point_s
    )
  end
  
  
  class Series    
    attr_accessor :x, :y, :name
    attr_accessor :line_color, :width, :style
    attr_accessor :point_color, :glyph, :scale
    attr_reader :points_shown, :line_shown
    @@count = 0

    def self.count; @@count; end

    def initialize
      @@count += 1
      @name = "Series \##{@@count}"
      @line_color = :black
      @width = 1.0
      @style = 1
      @point_color = :black
      @glyph = 4
      @scale = 1
      @has_line = false
      @has_points = false
      self.clean
    end
    
    def length
      [@x.length, @y.length].min
    end
    
    def range
      return @x.min, @x.max, @y.min, @y.max
    end
    
    def <<(pair)
      raise ArgumentError, "Need a pair of values" unless pair.kind_of? Array
      raise ArgumentError, "Need a pair of values" unless pair.size == 2
      @x << pair[0].to_f
      @y << pair[1].to_f
      return self
    end
    
    def line(color=@line_color, width=@width, style=@style)
      @has_line = true
      @line_color = color; @width = width; @style = style
      PLPlot._line(@x, @y, PLPlot.get_color(color), width, style)
    end
    
    def points(color=@point_color, glyph=@glyph, scale=@scale)
      @has_points = true
      @point_color = color; @glyph = glyph; @scale = scale
      PLPlot._points(@x, @y, PLPlot.get_color(color), glyph, scale)
    end
    
    def what_in_legend
      return 0 + (@has_line ? 4 : 0) + (@has_points ? 8 : 0)
    end
    
    def clean
      @x = []
      @y = []
      return self
    end
  end
  
end