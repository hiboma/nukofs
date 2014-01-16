guard 'shell' do 
  watch(%r{^.+\.(h|c)$}) do
    `vagrant ssh -- 'cd /vagrant && make && sudo make insmod && sudo make rmmod'`
  end 
end
