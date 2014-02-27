use strict;
use warnings;
use t::Nukofs;
use Path::Class;
use Test::More;

t::Nukofs->reload;

my $file = "/mnt/nukofs/file";

my $io_w = file($file)->openw;
$io_w->write("12345");
$io_w->close;

my $io_r = file($file)->openr;
my $line =  <$io_r>;
is $line, "12345", "read(2) must success";

done_testing;

