use strict;
use warnings;
use t::Nukofs;
use Path::Class;
use Test::More;

t::Nukofs->reload;

my $file = "/mnt/nukofs/file.txt";

file($file)->openw or die $!;
ok ( link($file, "/mnt/nukofs/file-hardlink.txt"), "link(2) should success") or die $!;

done_testing;
