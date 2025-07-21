# Bras_Robotique

**Avant d'exécuter ce programme, assurez-vous que vous avez installé l'éléments suivants :**

**1. Arduino**

*Voici le liens qui permettra d'installer l'application QT.*

(https://www.qt.io/download-dev)

**2. Commandes :**

| **Action**                         | **Touche** |
|-----------------------------------|------------|
| Épaule ↓                          | e          |
| Épaule ↑                          | E          |
| Base ←                            | b          |
| Base →                            | B          |
| Coude ↑                           | c          |
| Coude ↓                           | C          |
| Pince ← (Rotation gauche)         | r          |
| Pince → (Rotation droite)         | R          |
| Robot réinitialisé                | I          |
| Arrêt de la machine               | S          |
| Pince fermée                      | o          |
| Pince ouverte                     | O          |
| Poignet Haut                      | M          |
| Poignet Bas                       | m          |

**3. OBJECTIF**

  ``Ce programme en C++ permet d'avoir une interface utilisable sur QT afin de piloter le robot par USB et d'avoir une caméra fonctionelle afin de voir les mouvement du robot.``


**4. Configuration de l'application**
Configurer l’environnement Qt

1.Installer Qt et Qt Creator

Ouvrir le projet fourni dans Qt Creator

2.Lancer l’application PC

Exécuter sur un ordinateur connecté au robot

L’interface est prête à recevoir et envoyer des commandes

3.Charger les fichiers nécessaires

Télécharger et exécuter les fichiers source fournis (.cpp, .ui, etc.)

S'assurer que la communication série fonctionne (Arduino connecté en USB)

4.Utilisation de l’interface Qt

Commandes manuelles par boutons ou raccourcis clavier

Les actions sont envoyées directement à la carte Arduino

