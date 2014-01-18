use strict;
use warnings;
use t::Nukofs;
use Path::Class;
use Test::More;

t::Nukofs->reload;

my $file = "/mnt/nukofs/regular-file.txt";

file($file)->openw;
ok unlink $file, 'unlink(2) should success';

done_testing;
