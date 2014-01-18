use strict;
use warnings;
use t::Nukofs;
use Test::More tests => 2;
use IO::File;

t::Nukofs->reload;

my $dir = "/mnt/nukofs/dir";

ok ((mkdir $dir, 0755), 'mkdir(2) should success');
ok rmdir $dir, 'rmdir(2) should success';
