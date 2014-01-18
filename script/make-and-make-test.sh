#!/bin/bash

export LANG=C
. /etc/profile.d/lang-perl.sh

cd /vagrant
make && make test
