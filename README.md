nukofs
======

for learning Linux Kernel Development

## How to hack

You need

 * Ruby
 * Vagrant
 * VirtualBox

```
$ git clone git@github.com:hiboma/nukofs.git
$ vagarnt up
$ bundle install --path vendor/bundle
$ bundle exec guard
```

Open another ssh session

```
$ vagrant ssh

[vagrant@vagrant-centos65 ~]$ mount | grep nukofs
nukofs on /mnt/nukofs type nukofs (rw)
```
