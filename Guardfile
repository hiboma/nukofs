guard 'shell' do 
  watch("nukofs.c") do
    puts "Building module ..."
    `vagrant ssh -- sudo /vagrant/script/make-and-make-test.sh`
  end 
end
