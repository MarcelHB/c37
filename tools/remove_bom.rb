src_files = Dir["**/**.[ch]"]

src_files.each do |src|
  f = File.open(src, "rb+")
  contents = f.read
  
  unless contents[0..2] == "\xEF\xBB\xBF"
    f.close
    next
  end
  
  puts "removing BOM from #{src}"
  
  f.truncate(0)
  f.rewind
  f << contents[3..-1]  
  f.close
end