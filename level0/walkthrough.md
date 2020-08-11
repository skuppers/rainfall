## Level 1

On observe le binaire level0.
En le chargeant dans Cutter, on observe qu'il lance `/bin/sh` si la condition
`iVar1 == 0x1a7` est vrai. 0x1a7 vaut 423.

On essaye donc `./level0 423` et on obtiens un shell avec un `uid=2030(level1)`.
Il ne reste plus qu'a `cd ../level1/` et `cat .pass`.
