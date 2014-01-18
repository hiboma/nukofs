use strict;
use warnings;
use Test::More tests => 8;
use Path::Class;

BEGIN {
    system("make reload 2>/dev/null");
}

{
    # vagrant:vagrant = uid:500, gid:500
    my $dir = "/mnt/nukofs/dir";
    ok dir($dir)->mkpath(), 'mkdir(2) should success';
    ok ((chown 500, 500, $dir), 'chown(2) should success');

    my $st = dir($dir)->stat;
    is $st->uid, 500, 'uid should be 500';
    is $st->gid, 500, 'gid should be 500';
}

{
    my $file = "/mnt/nukofs/file";
    ok file($file)->openw;
    ok ((chown 500, 500, $file), 'chown(2) should success');

    my $st = file($file)->stat;
    is $st->uid, 500, 'uid should be 500';
    is $st->gid, 500, 'gid should be 500';
}

END {
    system("make reload 2>/dev/null");
}
