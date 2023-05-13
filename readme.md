# Slicor
Trancheuse de sable pour conçue pour le laboratoire de Sciences Naturelles de l'université de Lille

##Objectif

Trancher des blocs de sable de manière précise et sécuritaire pour faire un mapping 3D de ceux-ci afin de les étudier.

--------------------

## Construction

La trancheuse est constitué d'une structure rigide permettant d'interconnecter toute les parties mécanique utiles en un unique objet d'étude. Cette structure possède une plateau en métal sur lequel le bloc de sable doit se déplacer. Pour limiter les frottements il a était décidé de mettre une plaque en verre qui est lubrifier avant chaques utilisations. Pour absorber les chocs et limiter l'influence des variations de niveaux du plan en métal sur la plaque de verre (qui pourrait la fragiliser voir même la casser) une plaque en caoutchouc se trouve entre les deux plaques.

Une plaque mobile quand à elle permet de pousser le bloc de sable afin de le faire glisser sur la plaque en verre préalablement cité. Celui-ci est mis en mouvement par un vérin électrique. Un encodeur linéaire permet quand à lui de connaitre la position de la plaque à tous moment. Cette régle permet de mesurer des pats de déplacements dans un sens ou l'autre mais ne donne pas de référentiel c'est pourquoi un premier contacteur mécanique se trouve à l'extrémité zéro de la machine. Un second permet de savoir quand la plaque arrive en fin de course. Le vérin est contrôlé par un driver de moteur à courant continu lui même contrôler par une carte électronique (kicad de la carte dans le dossier électronique) permettant de récupérer les informations de tous les composants cités précédèment.

Un pupitre permet de faire l'interface homme-machine avec 3 voyants, respectivement : STOP (rouge), EN MOUVEMENT (JAUNE) et PRET (VERT) ainsi que deux boutons AVANT et ARRIERE. Un pavet numérique permet de sélectionner des valeurs avec un touche pour valider et 4 touches (A B C et D) permettant d'utiliser des valeurs preselectionnés. Un écran permet enfin d'afficher toute les informations nécessaires. Tous ces composants sont interconnectés via une autre carte électronique (kicad de la carte dans le dossier électronique). Le pupitre contient aussi l'alimentation, un bouton marche arret, et une bouton d'arrêt d'urgence.

Les deux cartes sont interconnectés via une liaison série (RX TX) UART afin permettre dans un sens d'envoyer les ordres et de recevoir dans l'autre sens un feedback de ce qui se passe.

--------------------

## Fonctionnement  

Dans un premier temps l'utilisateur fait le homming de la machine (la mise à zéro) afin que celle-ci se déplace pour se mettre en contact avec le premier contacteur.

Puis l'utilisateur entre une valeur et la valide sur le pavé.

Enfin il peut appuyer sur le bouton AVANT ou ARRIERE afin de déplacer le vérin et donc le bloc de sable de la distance souhaitée.

La valeur de déplacement peut être changer en dehors des moments de mouvements. Il faut alors à nouveau valider.

--------------------

## Préparation

Il faut mettre en position zéro la machine après chaques utilisations pour facilité la préparation de la fois suivante.

Premièrement il faut huiler la plaque de verre pour faciliter le déplacement.

Puis disposer le bloc de sable en buter sur la plaque mobile et le plan perpendiculaire à la plaque mobile.

Puis faire les étapes de fonctionnement précisé ci-dessus. 
