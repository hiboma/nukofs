guard 'shell' do 
  watch("nukofs.c") do
    puts "Building module ..."
    `vagrant ssh -- 'export LANG=C; cd /vagrant && make && make test'`
  end 
end
