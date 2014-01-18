use strict;
use warnings;
use t::Nukofs;
use Test::More tests => 2;
use IO::File;

t::Nukofs->reload;

my $file = "/mnt/nukofs/regular-file.txt";

IO::File->new($file, 'w')->close;
ok stat $file, 'stat(2) should success';
ok system("umount /mnt/nukofs") == 0, 'umount(2) should success';
