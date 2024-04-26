# TP_noyau

1. Le ficher main. est dans le dossier SRC
2. À quoi servent les commentaires indiquant BEGIN et END (appelons ça des balises)? Les commentaires sont là pour identifier les sections des codes
3. Quels sont les paramètres à passer à HAL_Delay et HAL_GPIO_WritePin ? Maintenez Ctrl et cliquez sur le nom d’une fonction pour voir son code. Delay en ms et dans write pin on va mettre le port de gpio et le numero de la pin
4. Dans quel fichier les ports d’entrée/sorties sont-ils définis ? Dans le ficher GPIO.h
5. Écrivez un programme simple permettant de faire clignoter la LED.
![image](https://github.com/Ngoduu/TP_noyau/assets/145014223/15c4703f-c370-4295-bec7-3699d35b3fad)

1 FreeRTOS, tâches et sémaphores

1. En quoi le paramètre TOTAL_HEAP_SIZE a-t-il de l’importance? Le heapsizeest important pour allouer une place pour les variables
2. Quel est le rôle de la macro portTICK_PERIOD_MS? C'est pour convertir le temps en ms au nombre de ticks de la carte

1.4 Queues

8. Modifiez le code pour obtenir le même fonctionnement en utilisant des task notifications à la place des sémaphores.

   ![image](https://github.com/Ngoduu/TP_noyau/assets/145014223/45beac52-1a66-4b6c-9af4-ae0bfeaca829)

1.5 Réentrance et exclusion mutuelle

11. Les deux tâches s'endors pour meme nombre de ticks, mais tâche 1 doit avoir 1 tick et pas 2.
12. Voici la solution
![image](https://github.com/Ngoduu/TP_noyau/assets/145014223/f76e6a2d-b4e4-42fb-a716-dedb7c49a309)

2 On joue avec le Shell
3. Expliquer les mécanismes qui mènent à l’exécution de la fonction. On va initialiser shell et par rapport à la lettre choisie on execute une fonction.
4. Il y a un probleme de synchronisation avec l'UART
5. On va mettre en place un sémaphore.

2. Que se passe-t-il si l’on ne respecte pas les priorités décrites précédemment ? FreeRTOS peut échouer
4.
  ![image](https://github.com/Ngoduu/TP_noyau/assets/145014223/84b659cf-0b63-4235-8b42-9f45a19f90d6)

3 Debug, gestion d’erreur et statistiques
1. Quel est le nom de la zone réservée à l’allocation dynamique ? Tas (Heap). C'est la zone dans laquelle sont écrites les zones mémoires.
2. Est-ce géré par FreeRTOS ou la HAL ? FREERTOS
3. 
RAM utilisé : 5,90%
Flash 3,14%


![image](https://github.com/Ngoduu/TP_noyau/assets/145014223/2a2e08c5-409c-4732-a293-ad4d51f26068)

![image](https://github.com/Ngoduu/TP_noyau/assets/145014223/5e5ebc2b-fb58-4f6c-94c1-c70aa43208f3)

![image](https://github.com/Ngoduu/TP_noyau/assets/145014223/ca938020-3194-4fea-80b5-ebf1224a9478)

![image](https://github.com/Ngoduu/TP_noyau/assets/145014223/c647e1cf-4982-4b79-9bc2-90c1ebcd55ac)





