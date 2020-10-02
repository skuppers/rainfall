## Level 3

Comme les niveau precedents, lors de notre arrive dans le compte level3, un
sommaire des informations concernant le binaire a exploiter nous est donne:

![Capture d’écran 2020-08-09 à 22 21 11](https://user-images.githubusercontent.com/25014717/89741132-f5118a00-da8e-11ea-9c4b-f4ed17875188.png)

Nous le desassemblons pour regarder comment il fonctionne:

![image](https://user-images.githubusercontent.com/29956389/94684852-ed50c200-0328-11eb-863e-9dedae67d881.png)

![image](https://user-images.githubusercontent.com/29956389/94684901-f93c8400-0328-11eb-8e31-ea24420b3d01.png)

Le code est assez simple: la fonction v est execute. Dans cette fonction, le
programme lit un maximum de 0x200(512) charactères sur l'entree standard. Cette chaine est ensuite directement passé a printf pour etre affiche. Suite a cela, si la globale "m" est egale a 0x40(64), l'acces a un shell nous est donné.
Le defi consiste donc a assigner cette valeur la variable "m":

![Capture d’écran 2020-08-09 à 22 32 34](https://user-images.githubusercontent.com/25014717/89741287-42dac200-da90-11ea-8739-9a833e920d13.png)

Nous allons bien noter l'adresse de "m", qui est importante. La faille que nous
exploiterons dans ce programme se situe au niveau de l'appel a printf. En effet,
la chaine lue sur l'entree standard est directement passe a la fonction, nous
permetant de passer des charactères de format de notre choix:

![Capture d’écran 2020-08-09 à 22 36 44](https://user-images.githubusercontent.com/25014717/89741349-dd3b0580-da90-11ea-8dbf-9aa0dccc49d3.png)

Du au fonctionnement de printf, comme notre chaine de format n'a pas de
variables correspondant, nous pouvons avoir acces au differentes variables presente sur la stack:

![Capture d’écran 2020-08-09 à 22 39 51](https://user-images.githubusercontent.com/25014717/89741411-65b9a600-da91-11ea-80fe-b92093df296c.png)

Nous pouvons egalement accede a une variable que nous avons passe par l'entree
standard, etant donne qu'apres sa lecture, elle se trouve sur la stack:

![Capture d’écran 2020-08-09 à 22 43 14](https://user-images.githubusercontent.com/25014717/89741450-be893e80-da91-11ea-8ef0-4b4e93120fc2.png)

Le formateur "%x" nous a permis d'identifie ou se situait notre variable dans la
stack. Grace au modificateur "%n" nous pouvons dereferencer une variable, et y
stocker le nombre de charactères imprimpes par printf. Par exemple, si nous
essayons d'acceder a la variable situe a 42424242, nous obtenons un segfault:

![Capture d’écran 2020-08-09 à 22 46 55](https://user-images.githubusercontent.com/25014717/89741518-4111fe00-da92-11ea-8ddb-b4ac551de599.png)

Mais comme nous avons l'adresse de la variable "m" que nous voulons modifier,
nous pouvons y acceder et modifier sa valeur sans probleme:

![Capture d’écran 2020-08-09 à 22 56 12](https://user-images.githubusercontent.com/25014717/89741686-8e429f80-da93-11ea-9fcb-cfdf770e884f.png)

Nous obtenons 4, car meme si ils n'apparaissent pas a l'ecran, les 4 characteres
constituant notre adresse ont bien ete imprimes. Il ne nous reste plus pour
obtenir 64, le resultat que nous voulons, a imprimer 60 charactères
supplémentaires, ce qui nous permet d'avoir acces au shell:

![Capture d’écran 2020-08-09 à 22 5944](https://user-images.githubusercontent.com/25014717/89741731-0d37d800-da94-11ea-94fd-737be836d6d0.png)

`$ python -c 'print "\x8c\x98\x04\x08" + "B" * 60 + "%4$n"' > /tmp/lvl3xpl`

`$ cat /tmp/lvl3xpl - |./level3`

