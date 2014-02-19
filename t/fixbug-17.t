use strict;
use warnings;
use t::Nukofs;
use Path::Class;
use Test::More;

t::Nukofs->reload;

my $file = "/mnt/nukofs/file.txt";
file($file)->openw->close;

ok(rename $file, "/mnt/nukofs/renamed-file.txt") or die $!;

done_testing;
