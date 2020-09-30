## Level 8

En lanceant le binaire, on observe un affichage de `(nil), (nil)`.
Peut importe ce qu'on écrit, le retour reste le meme.

![image](https://user-images.githubusercontent.com/29956389/89891530-8854d780-dbd5-11ea-9ce1-6d55f60aa886.png)

Le déssasemblage du binaire révèle un code plutot long et fastidieux a comprendre. Mais juste la fonction `main()` est définis.

En regardant les strings contenue dans le binaire, on observe `auth`, `service`, `reset` et `login`. Cela s'apparente a des commandes.

On observe également les variable globale `_auth` et `_service`,
respectivement au adresse `0x08049aac` et `0x08049ab0`.

![image](https://user-images.githubusercontent.com/29956389/89891759-eed9f580-dbd5-11ea-96f4-e7074fa22e38.png)

L'instruction qui nous affiche `(nil), (nil)` est la suivante:

![image](https://user-images.githubusercontent.com/29956389/94690219-1f195700-0330-11eb-8522-5fad0967a9ab.png)

Elle affiche donc l'adresse des variable `_auth` et `_service`.

En cherchans un peu plus, on observe un plusieurs appel a des fonction systèmes:

- `strcpy()` apres `auth`
- `strdup()` apres `service`
- `free()` apres `reset`
- `system(/bin/sh)` apres `login`

Cet dernier appel est celui qui nous intéresse, mais une condition préalable
doit etre vrai pour executer cet appel:

![image](https://user-images.githubusercontent.com/29956389/94690441-6b649700-0330-11eb-80d8-95497d931d2d.png)

Elle verifie que le 32eme bytes(8 x 4 bytes) de la variable `_auth` est différente de `0`.

Essayon donc, au lieu de mieux comprendre le code déssasemblé, de mieux
comprendre la gestion mémoire du binaire.

On lance donc le binaire dans GDB, on place quelque breakpoint autour de la
condition de `login`.

![image](https://user-images.githubusercontent.com/29956389/89892615-3a40d380-dbd7-11ea-86ca-787fc1a8a9f1.png)

On lance le programme et on rentre les commandes trouvé auparavent avec des argument facile a localiser dans la mémoire:

![image](https://user-images.githubusercontent.com/29956389/89892770-76743400-dbd7-11ea-81bf-e73346eb6ecd.png)

Les adresse pointé par les global sont affiché !

On step donc jusqu'au breakpoint, avec la commande `login`:

![image](https://user-images.githubusercontent.com/29956389/89893047-f4d0d600-dbd7-11ea-8fc0-c7dd891221be.png)

![image](https://user-images.githubusercontent.com/29956389/89893151-20ec5700-dbd8-11ea-99c9-bed7f143f121.png)

![image](https://user-images.githubusercontent.com/29956389/89893264-5133f580-dbd8-11ea-8907-664f8db643f6.png)

Et on observe que notre input passé a `service` est 16 bytes plus loin que l'input de `auth`. Ce qui veut dire que le 32 byte de l'input de `auth` (celui qui est vérifié pour lancer `system()`), est le 16eme byte de l'input de `service`.

Il nous suffit donc uniquement de fournir un `auth` quelconque et un `service` de 16 bytes:

![image](https://user-images.githubusercontent.com/29956389/89893791-4c237600-dbd9-11ea-8b38-b300f3a4d077.png)

Et on obtient un shell !
