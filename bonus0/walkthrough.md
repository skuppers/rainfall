## Bonus0

Au premier lancement, le binaire attend un premier argument sur l'entrée
standart, puis un deuxieme.

On observe un appel a `pp()`, qui fait deux appel a `p()`. C'est cette fonction
qui va nous intéresser. Elle affiche le caractère ` - `, lit sur l'entrée
standard, et stocke le resultat dans un buffer de `4096` bytes. Ensuite l'appel
a `strchr()` va checher le caractère `10 (\n)` dans le buffer, et le remplace
par `0 (\0)`, enfin, l'appel a `strncpy()` copie les 20 premiers bytes du buffer
dans la destination passé en argument a la fonction `p()`.

Cette fonction est vulnérable, en particulier l'appel a `strchr()`, car si aucun
`\n` n'est présent dans le buffer, la fonction renvoie `NULL`, et l'instruction d'après
essayera d'ecraser la valeur de cette adresse par `\0`.

#### Amusons nous a présent avec la mémoire

![image](https://user-images.githubusercontent.com/29956389/90001786-2a39fa00-dc92-11ea-94b4-af8989aa97d5.png)

On segfault sur `0x45444444` soit `DDDE`, l'offset est de `9` bytes. Le but est donc d'y insérer l'adresse
de notre shellcode.

On utilisera un fichier pour insérer notre input, cela sera plus facile que
d'insérer des caractères non-imprimables a la main dans GDB.

`$ python -c 'print 'i'*64 + '\x0a' + 'ABCDEFGHI' + '\xef\xbe\xad\xde' + '\x0a' > /tmp/xploit`

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
a executer se retrouve au mauvais endroit dans la stack! Il fau donc remplacer
`print 'i'*64 + \x0a` dans notre commande par `print 'i'*4095 + '\x0a'`.

On réesaye avec la commande corrigé:

`$ python -c 'print "i"*4095 + "\x0a" + "ABCDEFGHI" + "\xef\xbe\xad\xde" + "\x0a"' > /tmp/xploit`



python -c "print 'A' * 4095 + '\n' + 'i' + '\xf0\xf7\xff\xbf'*3 + 'B'*30" >
/tmp/file2
