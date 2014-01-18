use strict;
use warnings;
use t::Nukofs;
use Path::Class;

use Test::More tests => 2;

t::Nukofs->reload;

my $file = "/mnt/nukofs/regular-file.txt";

file($file)->openw;
ok stat $file, 'stat(2) should success';
ok system("umount /mnt/nukofs") == 0, 'umount(2) should success';
