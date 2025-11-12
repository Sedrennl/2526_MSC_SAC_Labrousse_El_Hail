# CR TP Système d'acquisition 

## Sédrenn Labrousse & Mohamed Amin El Haid

### Présentation
Dans le cadre du module de Systèmes Asservis Commandés (SAC), ce travail pratique a pour objectif de mettre en œuvre la commande d’un hacheur complet à l’aide d’une carte Nucleo-STM32G474RE.
L’objectif principal est de réaliser la commande complémentaire décalée des quatre transistors du hacheur, d’assurer l’acquisition des capteurs, puis de développer un asservissement en temps réel.
Le projet est mené à travers un environnement STM32CubeIDE, entièrement versionné sur un dépôt GitHub partagé avec l’enseignant dès la première séance.

--- 
# Préparation prise en main 
### Test du Shell

Après avoir branché la carte et lancé un terminal (dans mon cas, minicom) nous pouvons tester le Shell proposé dans le code :
![Test du Shell](./photo/test_Shell_fonctions.png)

On voit bien sur la paillasse le on et off de la led et la fonction test renvoie les valeurs demandées.


