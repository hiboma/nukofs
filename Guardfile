guard 'shell' do 
  watch("nukofs.c") do
    puts "Building module ..."
    `vagrant ssh -- 'cd /vagrant && make && sudo make reload'`
  end 
end
