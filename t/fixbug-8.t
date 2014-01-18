use strict;
use warnings;
use Test::More tests => 2;
use IO::File;

BEGIN {
    system("make reload 2>/dev/null");
}

my $dir = "/mnt/nukofs/dir";

ok ((mkdir $dir, 0755), 'mkdir(2) should success');
ok rmdir $dir, 'rmdir(2) should success';

END {
    system("make reload 2>/dev/null");
}
