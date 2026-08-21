# kfs-3
kfs-3

Étape 1 — Activer la pagination
Construire un Page Directory et au moins une Page Table en mémoire (souvent une identity-map au début : adresse virtuelle = adresse physique, pour ne pas te faire sauter le kernel au moment de l'activation). Charger l'adresse du directory dans CR3, puis mettre le bit PG (bit 31) de CR0 à 1.

Étape 2 — Structure mémoire théorique
Coder les structures C qui représentent les entrées de page directory/table (avec les flags P, R/W, U/S...), et une façon de savoir quelles pages/frames physiques sont libres ou occupées (souvent un bitmap des frames physiques de 4 Ko).
Étape 1 — Activer la pagination
Construire un Page Directory et au moins une Page Table en mémoire (souvent une identity-map au début : adresse virtuelle = adresse physique, pour ne pas te faire sauter le kernel au moment de l'activation). Charger l'adresse du directory dans CR3, puis mettre le bit PG (bit 31) de CR0 à 1.

Étape 2 — Structure mémoire théorique
Coder les structures C qui représentent les entrées de page directory/table (avec les flags P, R/W, U/S...), et une façon de savoir quelles pages/frames physiques sont libres ou occupées (souvent un bitmap des frames physiques de 4 Ko).

Étape 3 — Séparer Kernel space / User space
Définir une convention d'adresses : par exemple le kernel vit dans les adresses hautes (ex: à partir de 0xC0000000, classique), l'espace utilisateur dans les basses. Ça prépare le terrain pour l'isolation entre processus plus tard (même si aujourd'hui il n'y a encore qu'un seul "processus", le kernel).

Étape 4 — Fonctions de gestion de pages
Une fonction pour créer/obtenir une page (mapper une adresse virtuelle vers une frame physique dans les tables), et son inverse pour démapper.

Étape 5 — Allocateur physique : kmalloc, kfree, ksize, kbrk
Un allocateur qui travaille sur la mémoire physique (frames de 4 Ko), avec un compteur de taille par bloc pour ksize, et kbrk pour déplacer la frontière du tas (comme le brk UNIX).

Étape 6 — Allocateur virtuel : vmalloc, vfree, vsize, vbrk
Le pendant côté adresses virtuelles : ça doit gérer le mapping page virtuelle → frame physique via la structure de l'étape 4, en plus de la logique d'allocation.

Étape 7 — Kernel panic
Une fonction qui affiche un message d'erreur et arrête le kernel proprement (interruptions désactivées, boucle infinie ou halt). Point important du sujet : tous les panics ne sont pas fatals — il faut distinguer une erreur récupérable (ex: allocation qui échoue, tu retournes NULL) d'une erreur fatale (ex: corruption détectée dans tes structures internes).

Étape 8 — Contrainte
Tout ça ne doit pas dépasser 10 Mo de code/données.

Contraintes générales à ne pas oublier

- Architecture i386 obligatoire
- Flags de compilation sans dépendances (-fno-builtin -nostdlib -nodefaultlibs, etc. — déjà probablement en place dans ton Makefile depuis KFS-2)
- Tu peux réutiliser ton code KFS-2 (GDT/IDT déjà en place) — c'est même recommandé, la pagination vient s'ajouter par-dessus.

Une remarque du sujet à garder en tête

▎ "all of this is theoric at the moment" — tu n'as pas encore les outils pour savoir qui accède à la mémoire (pas de mode utilisateur / processus réel), donc les droits R/W et U/S que tu poses sont pour l'instant posés "en théorie", en préparation des sujets suivants.

---

Tu veux qu'on attaque l'étape 1 (identity mapping + activation du bit PG dans CR0) en regardant ton code actuel (gdt.c, boot.asm) pour voir où l'accrocher ? Ou tu préfères d'abord qu'on regarde en détail le format exact des structures C à définir pour le Page Directory / Page Table ?
