En dessasemblant le binaire level2, on trouve deux fonctions:

`main()` qui fait un simple appel a `p()`.

`p()` déclare un pointeur, un tableau, ainsi qu'un int qui n'est pas utilisé.
La fonction va lire sur l'entrée standart, et stocker le resultat dans le
tableau `src`.

![image](https://user-images.githubusercontent.com/29956389/89816964-b12e8b80-db47-11ea-93b8-79094726c6be.png)

Ensuite une vérification est faite sur le pointeur, si la
condition est vrai, le programme affiche l'adresse du pointeur et quitte.

Si la condition est fausse, la chaine de caractère est affiché et copié, puis la
fonction retourne au main.

En utilisant le site `Buffer Overflow EIP Offset String Generator` (Disponible
dans le dossier `Ressources`):
https://projects.jason-rush.com/tools/buffer-overflow-eip-offset-string-generator/?eip-length=100&eip-output-string=Output+string+goes+here

On apprend que le tableau fait 80 bytes de taille.
Comme le tableau `src` est déclaré après `unaff_retaddr`, cette dernière adresse
sera écrasé par les données qui dépasse du buffer `src`.

La variable `unaff_retaddr` est déclaré mais pas initialisé, elle pointe donc
sur le pointeur d'instruction.
Comme la condition vérifie que le premier byte qui overflow le tableau commence
par `0xb (adresse de stack)`, on ne peut pas directement placer l'adresse de notre
nopsled dedans (Qui serait d'habitude sur la stack, soit dans l'environement
soit dans un buffer).
Par contre, on peut y déclarer toute autre valeur, comme par exemple l'adresse
de l'instruction `return` de cette fonction.

Pour ensuite rediriger l'EIP sur notre nopsled, on y rajoute l'adresse en plus.

Soit `[80 bytes pour remplir le buffer] + [Adresse instruction ret] + [&Nopsled]`.

En ajoutant cet addresse, nous allons réécrire la sauvegarde EIP, qui est stocké
avant l'appel a `call p()`. Le programme essayera donc de reprendre son
execution normal en revenant de la fonction `p()`, mais l'adresse de notre nopsled
y sera stocké, et l'EIP executera notre shellcode.

En code:

`export SLEDCODE=$(python -c 'print "\x90"*128')$(cat shellcode.bin)`

`python -c 'print "B"*80 + "\xff\xff\xff\x08" + "\x8c\xf8\xff\xbf"' > /tmp/buffer`

`cat /tmp/buffer - | ./level2`

Et on obtient un shell!
