module PLPlot
  
  class DataFrame
    def initialize(*names)
      names.flatten!
      @names = Hash.new
      @data  = Array.new
      names.each_with_index do |n,i|
        @names[n.to_s] = i
        @data << Array.new
      end
    end
    
    def <<(ary)
      raise ArgumentError unless ary.kind_of?(Array) && (ary.length == @names.length)
      @data.each_with_index {|col,i| col << ary[i]}
    end
    
    def length
      @data[0].length
    end
    
    def [](col)
      case col
      when Fixnum
        return @data[col]
      when String
        raise ArgumentError, "Non-existing column #{col}" unless @names[col]
        return @data[@names[col]]
      else
        raise ArgumentError, "Either column number or name"
      end
    end
    
    
  end #DataFrame
  
end #module