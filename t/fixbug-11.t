use strict;
use warnings;
use t::Nukofs;
use Path::Class;
use Test::More;

t::Nukofs->reload;

my $file = "/mnt/nukofs/file.txt";

my $io = file($file)->openw;
is $io->syswrite("1234567890", 10), 10, 'write(2) should success';
$io->close;

done_testing;
