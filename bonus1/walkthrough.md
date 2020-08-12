## Bonus 1 

Lors de notre arrive sur le compte bonus1, les informations habituelles nous
sont donnees:

![Capture d’écran 2020-08-12 à 14 17 02](https://user-images.githubusercontent.com/25014717/90013973-932b6d00-dca6-11ea-95a2-c392c4596d87.png)

Nous decompilons le binaire pour comprendre comment il fonctionne:

![Capture d’écran 2020-08-12 à 14 20 09](https://user-images.githubusercontent.com/25014717/90014204-f3221380-dca6-11ea-8d32-8ac8d0b02af9.png)

Ce programme prends deux arguments: le premier est converti en int via atoi, et
si le resultat de la conversion est inferieur a 10, un memcopy du deuxieme
argument sera effectue dans un buffer de 40 characteres. Il y a ensuite une
verification pour determiner si le resultat de notre atoi est egal a
0x574f4c46(1464814662). Si c'est le cas, un shell nous est donne.

Nous testons dans un premier temps le programme avec des arguments valides, pour
determiner l'etat de la memoire avant une copie, puis apres:

![Capture d’écran 2020-08-12 à 14 32 08](https://user-images.githubusercontent.com/25014717/90015292-ae977780-dca8-11ea-92a3-57ed1a36c546.png)

Pour prendre le controle de l'execution, nous avons besoin remplacer la valeur
de l'eip, stockee apres la fin de notre buffer:

![Capture d’écran 2020-08-12 à 14 36 05](https://user-images.githubusercontent.com/25014717/90015650-2d8cb000-dca9-11ea-8724-017d8fae1fa3.png)

Pour realiser notre exploit, nous allons utiliser les proprietes des fonctions
utilisees: atoi est capable de convertir des nombres negatifs et retourne un
int. memcopy prends en paramettre un unsigned 32. Nous allons donc passer une
valeur negative, pour faire overflow l'unsigned int passe a memcopy. Nous codons
un petit programme en C qui, partant de int min, nous permettra de calculer le
nombre a faire passer en parametre pour reecrire l'eip: 

![Capture d’écran 2020-08-12 à 14 39 58](https://user-images.githubusercontent.com/25014717/90016090-cb807a80-dca9-11ea-951b-318f87fde649.png)

Lorsque nous avons ecrit 9 characteres, l'eip se trouvait 6 adresses plus loin.
Nous avons donc besoin d'int_min + 15 pour arriver precisement sur l'adresse qui
nous interesse:

![Capture d’écran 2020-08-12 à 14 46 40](https://user-images.githubusercontent.com/25014717/90016735-bfe18380-dcaa-11ea-820f-49c5578d9bb6.png)

Apres test, nous ecrivons bien l'adresse de eip lorsque nous passons en premier
paramettre pour atoi -2147483633: 

![Capture d’écran 2020-08-12 à 14 51 34](https://user-images.githubusercontent.com/25014717/90017118-56ae4000-dcab-11ea-876b-8903f5cb2316.png)

Grace au differents characteres de notre chaine de copie, nous voyons que eip est
replace par "0x4f4f4f4f", soit la valeur hexadecimale de "O" dans la table ascii.
Il nous suffit simplement de remplacer les O dans notre chaine par l'adresse a
laquelle nous voulons envoyer le programme au retour de main. Nous l'envoyons
apres la verification de la valeur retournee par atoi, ici "0x08048482":

![Capture d’écran 2020-08-12 à 14 55 47](https://user-images.githubusercontent.com/25014717/90017585-0aafcb00-dcac-11ea-8f07-848f13bd8880.png)

Ce qui nous donne directement acces au shell aussi bien dans que hors de gdb :

![Capture d’écran 2020-08-12 à 14 59 53](https://user-images.githubusercontent.com/25014717/90017934-80b43200-dcac-11ea-89c3-9b090e559c78.png)

![Capture d’écran 2020-08-12 à 15 02 14](https://user-images.githubusercontent.com/25014717/90018185-d4268000-dcac-11ea-8e1d-9d14eda21725.png)
