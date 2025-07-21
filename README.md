# Bras_Robotique

**Avant d'exécuter ce programme, assurez-vous que vous avez installé l'éléments suivants :**

**1. Android Studio**

*Voici le liens qui permettra d'installer l'application Android Studio.*

(https://developer.android.com/studio)

**2. Fonctionnalités de l’application Android**

Cette application permet de contrôler un bras robotique à distance en envoyant des commandes via TCP/IP à un PC. Vous pouvez effectuer divers mouvements du bras, de la pince, etc., directement depuis l'application.

**3. Commandes**

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

**4. Configuration de l'application**

##### 1. Configurer votre environnement Android Studio

- Téléchargez et installez **[Android Studio](https://developer.android.com/studio)**.
- Ouvrez le projet dans Android Studio.

##### 2. Lancer l'application

- Exécutez l'application sur votre appareil Android ou un émulateur.
- L'application sera prête à envoyer des commandes une fois lancée.

##### 3. Connecter à un réseau local

- Assurez-vous que votre smartphone est connecté à un réseau **Wi-Fi local**.

##### 4. Authentification

- Avant de pouvoir envoyer des commandes, vous devez **vous authentifier** pour garantir une connexion sécurisée. L'application vous demandera d'entrer un mot de passe ou un code d'accès.
- Une fois l'authentification réussie, vous pourrez commencer à contrôler le robot.

##### 5. Utilisation de l'interface

- L'application propose des boutons pour envoyer des commandes.
- En appuyant sur les touches, les commandes sont envoyées au **PC via TCP/IP**.



**5. Objectif**

``L'objectif de cette application Android est de contrôler à distance un bras robotique. L'application envoie des commandes via TCP/IP au PC, qui les transmet ensuite au robot via une connexion USB. Cette application offre une interface conviviale permettant de piloter les mouvements du bras robotique, y compris les mouvements de la pince, de l'épaule, du coude, etc. Elle permet également de visualiser en direct les mouvements du robot grâce à la caméra intégrée.``
