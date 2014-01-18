guard 'shell' do 
  watch("nukofs.c") do
    puts "Building module ..."
    `vagrant ssh -- /vagrant/script/make-and-make-test.sh`
  end 
end
