#!/bin/bash

export LANG=C
export PATH="/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin"

. /etc/profile.d/lang-perl.sh

cd /vagrant
make && make test
