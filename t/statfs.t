use strict;
use warnings;
use t::Nukofs;
use Test::More;

t::Nukofs->reload;

is system("df /mnt/nukofs"), 0;

done_testing;
