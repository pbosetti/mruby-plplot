MRuby::Gem::Specification.new('mruby-plplot') do |spec|
  # Note that it needs WiringPI libraries (http://wiringpi.com)
  spec.license = 'MIT'
  spec.author  = 'Paolo Bosetti, University of Trento'
  spec.summary = 'PLPlot mruby interface'
  spec.version = 0.1
  spec.description = spec.summary
  spec.homepage = "Not yet defined"

  if respond_to? :search_package
    search_package 'plplotd'
  elsif not build.kind_of? MRuby::CrossBuild then
    spec.cc.command = 'gcc' # clang does not work!
    spec.cc.flags << %w||
    spec.cc.include_paths << "/usr/local/include/plplot"
    spec.linker.library_paths << "/usr/local/lib"
    spec.linker.libraries << %w|plplot csirocsa ltdl m qsastime|
  else
    # complete for your case scenario
  end

end
