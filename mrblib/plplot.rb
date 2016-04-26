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
  @grid = []
  @current_color = 1
  
  def self.colors; COLOR.keys; end
  
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
  
  class Series
    attr_accessor :x, :y
    def initialize
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
    
    def clean
      @x = []
      @y = []
      return self
    end
  end
  
end