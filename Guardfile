guard 'shell' do 
  watch("nukofs.c") do
    `vagrant ssh -- 'cd /vagrant && make && sudo make insmod && sudo make rmmod'`
  end 
end
