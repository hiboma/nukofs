use strict;
use warnings;
use Test::More tests => 1;
use IO::File;

BEGIN {
    system("make reload 2>/dev/null");
}

my $file = "/mnt/nukofs/regular-file.txt";

IO::File->new($file, 'w')->close;
ok unlink $file, 'unlink(2) should success';

END {
    system("make reload 2>/dev/null");
}
