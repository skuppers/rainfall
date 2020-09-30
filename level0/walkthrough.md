## Level 0

On observe le binaire level0, avec le bit SUID.

En le chargeant dans GDB, on observe qu'il lance `/bin/sh` si la condition
`atoi(av[1]) == 0x1a7` est vrai, et qu'il set son uid et gid. 0x1a7 vaut 423.

On essaye donc `./level0 423` et on obtiens un shell avec un `uid=2030(level1)`.
Il ne reste plus qu'a `cd ../level1/` et `cat .pass`.
