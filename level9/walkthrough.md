## Level 9

En déssasemblant le binaire, on observe un code en c++.

Il nous demande une entrée.

On observe un appel a `memcpy()`, qui copie notre entré dans un espace mémoire.

On observe également un pointeur sur fonction qui est appellé a la fin du code.

On devra probablement réécrire ce pointeur sur fonction, pour executer un
shellcode, sachant qu'il n'y a aucune fonction caché, ou appel a `system()`.

On essaye de le faire segfault, et on y arrive avec une entrée de plus de 108 bytes.

![image](https://user-images.githubusercontent.com/29956389/89921241-b8b06c00-dbfd-11ea-96d3-85b5285a8c16.png)

Observons ou exactement le segfault se produit:

![image](https://user-images.githubusercontent.com/29956389/89921241-b8b06c00-dbfd-11ea-96d3-85b5285a8c16.png)

Logiquement, en revenant de la fonction qui fait l'appel a `memcpy()`, a
l'instruction:

`=> 0x08048682 <+142>:   mov    edx,DWORD PTR [eax]`

##### Analysons a présent ce qui se passe en mémoire:

On va placer un premier breakpoint avant l'appel a `memcpy()`.

![image](https://user-images.githubusercontent.com/29956389/89921779-6a4f9d00-dbfe-11ea-9fd2-3bbcc8b67065.png)

Puis un second, avant l'instruction `0x08048682 <+142>`.

![image](https://user-images.githubusercontent.com/29956389/89922513-6e2fef00-dbff-11ea-87ec-6a32def979d2.png)








