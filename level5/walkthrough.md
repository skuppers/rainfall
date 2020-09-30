## Level 5

Lors de notre arrive dans le compte level5, un
sommaire des informations concernant le binaire a exploiter nous est donne:

![Capture d’écran 2020-08-10 à 13 31 40](https://user-images.githubusercontent.com/25014717/89778520-f6ce6280-db0d-11ea-8e79-5a5658097ed5.png)

Nous desassemblons et decompilons le binaire pour comprendre son fonctionnement:

![image](https://user-images.githubusercontent.com/29956389/94686151-b67bab80-032a-11eb-9799-c449981c32d7.png)

![image](https://user-images.githubusercontent.com/29956389/94686169-bda2b980-032a-11eb-9461-7d41e9e3552e.png)

![image](https://user-images.githubusercontent.com/29956389/94686185-c4313100-032a-11eb-8fd5-41e96a664780.png)


Nous avons donc le main, qui appelle la fonction n. Cette fonction n lit sur
l'entree standard une string, directement passe comme argument a printf. Nous
avons egalement une fonction o, qui nous donne un shell, mais qui n'est pas
appele. Le but est donc de prendre le controle de l'execution et de faire en
sorte que cette fonction o se lance. Pour se faire, nous allons dans un premier
temps determiner ou nous pouvons placer l'adresse de la variable a modifier dans
notre chaine format: 

![Capture d’écran 2020-08-10 à 13 39 44](https://user-images.githubusercontent.com/25014717/89779111-fbdfe180-db0e-11ea-940b-36637197008c.png)

Maintenant que nous savons ou placer l'adresse a modifier, il nous faut
determiner quelle est elle. Pour prendre le controle de l'execution, nous
voulons reecrire l'adresse a laquelle l'eip devra retourner a la fin du
printf. En effet, en modifiant cette valeur, lorsque printf aura fini son
execution et voudra retourner a sa fonction appelante, le code sautera au lieu
de notre choix. Nous determinons l'adresse de ce pointeur a l'aide de gdb:

![Capture d’écran 2020-08-10 à 13 47 33](https://user-images.githubusercontent.com/25014717/89779608-11a1d680-db10-11ea-9e7b-d74bf436fa70.png)

Grace a gdb, nous savons que l'adresse qui nous interesse est stocke a l'adresse "0xbffff4fc". Pour
valider que cette adresse est bien celle que nous recherchons, nous regardons
son contenu, et nous voyons bien que l'adresse stockee est celle de la prochaine
instruction apres l'appel a printf de la fonction n.

Nous devons maintenant determiner ce que nous voulons placer dans la variable,
c'est a dire, ou nous voulons faire sauter notre code. Nous recuperons pour cela
l'adresse du debut de la fonction "o":

![Capture d’écran 2020-08-10 à 13 50 51](https://user-images.githubusercontent.com/25014717/89779857-8a089780-db10-11ea-8c77-494da7d667d0.png)

Toutes les informations dont nous avons besoin etant en notre possesion,
l'exploit est desormais possible. Nous devons placer a l'adresse de retour de l'eip, ici "0xbffff4fc", la
valeur "0x080484a4", soit 134513828. Comme nous imprimons avant les 4
charactères de l'adresse, nous devons soustraire 4 dans notre chaine de format,
pour compenser. L'adresse de l'eip sauvegardée est suceptible de changer, donc toujours la récupérer directement, mais le reste est fixe.

Et nous avons ainsi acces au shell: 

![Capture d’écran 2020-08-10 à 14 02 56](https://user-images.githubusercontent.com/25014717/89780782-5e86ac80-db12-11ea-8f90-c98408973bc6.png)

![Capture d’écran 2020-08-10 à 14 03 54](https://user-images.githubusercontent.com/25014717/89780777-5dee1600-db12-11ea-8c8b-d6b99fd5af27.png)
