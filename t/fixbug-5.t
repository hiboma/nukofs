use strict;
use warnings;
use t::Nukofs;
use Test::More tests => 1;
use IO::File;

t::Nukofs->reload;

my $file = "/mnt/nukofs/regular-file.txt";

IO::File->new($file, 'w')->close;
ok unlink $file, 'unlink(2) should success';

