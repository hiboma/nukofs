guard 'shell' do 
  watch(%r{^.+\.(h|c)$}) do 
    `vagrant ssh -- 'cd /vagrant && make'` 
  end 
end
