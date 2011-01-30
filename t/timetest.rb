require 'strongtyping'
include StrongTyping

def timeblock(msg = nil)
  start  = Time::now.to_f
  yield
  finish = Time::now.to_f

  print "[#{msg}] " if msg
  print "Total time: #{finish - start}\n"

  return finish - start
end

iterations = 100000

time_0 = timeblock("Base Time") {
  (1..iterations).each do |i| end
}

time_1 = timeblock("Expect") {
  (1..iterations).each do
    |i|
    expect(1, Integer, "abc", String)
  end
}

print "Difference for #{iterations} iterations: #{time_1 - time_0}\n"
