use strict;
use warnings;
use t::Nukofs;
use Path::Class;

use Test::More tests => 2;

t::Nukofs->reload;

my $dir = "/mnt/nukofs/dir";

ok dir($dir)->mkpath(0, 0755), 'mkdir(2) should success';
ok rmdir $dir, 'rmdir(2) should success';
