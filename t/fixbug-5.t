use strict;
use warnings;
use t::Nukofs;
use Path::Class;

use Test::More tests => 1;

t::Nukofs->reload;

my $file = "/mnt/nukofs/regular-file.txt";

file($file)->openw;
ok unlink $file, 'unlink(2) should success';

