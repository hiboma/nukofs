use strict;
use warnings;
use t::Nukofs;
use Path::Class;
use Test::More;

t::Nukofs->reload;

my $dir = "/mnt/nukofs/dir";

dir($dir)->mkpath(0, 0755);
ok rmdir $dir, 'rmdir(2) should success';

done_testing;
