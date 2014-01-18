package t::Nukofs;

use strict;
use warnings;
use Path::Class;

sub reload {
    system("make reload");
}

1;
