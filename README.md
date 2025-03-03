#  TFTP - Abdelkarim 

## 1. Utilisation des arguments passés à la ligne de commande
On lit deux arguments en ligne de commande : l'adresse du serveur TFTP (`host`) et le nom du fichier (`file`). Ces arguments sont nécessaires pour identifier le serveur et le fichier à transférer.

---

## 2. Appel à `getaddrinfo` pour obtenir l'adresse du serveur
On utilise la fonction `getaddrinfo` pour résoudre l'adresse IP ou le nom d'hôte fourni par l'utilisateur. Une fois l'adresse résolue, elle est affichée dans le terminal.

---

## 3. Réservation d’un socket de connexion vers le serveur
On crée un socket UDP pour établir la connexion avec le serveur TFTP. Si le socket est créé avec succès, son descripteur est affiché.

---

## 4. Gestion des requêtes GET (RRQ - Read Request)

### a) Construction et envoi d’une requête de lecture (RRQ)
On construit une requête de lecture (RRQ) avec le nom du fichier et le mode de transfert `octet`, puis on l'envoie au serveur.

### b) Réception d’un fichier constitué d’un seul paquet de données (DAT)
On reçoit un fichier constitué d’un seul paquet de données (`DAT`) et on envoie un accusé de réception (`ACK`) après réception.

### c) Réception d’un fichier constitué de plusieurs paquets de données (DAT)
On gère les fichiers plus volumineux en recevant plusieurs paquets (`DAT`) et en envoyant un `ACK` pour chaque paquet.

---

## 5. Gestion des requêtes PUT (WRQ - Write Request)

### a) Construction et envoi d’une requête d’écriture (WRQ)
On construit une requête d’écriture (WRQ) avec le nom du fichier et le mode de transfert `octet`, puis on l'envoie au serveur.

### b) Envoi d’un fichier constitué d’un seul paquet de données (DAT)
On envoie un fichier constitué d’un seul paquet de données (`DAT`) et on attend un `ACK` du serveur pour confirmer la réception.

### c) Envoi d’un fichier constitué de plusieurs paquets de données (DAT)
On gère les fichiers plus volumineux en les découpant en plusieurs paquets (`DAT`) et en envoyant chaque bloc avec attente d’un `ACK` correspondant.
