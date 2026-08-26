# KFS_3 — Gestion mémoire

Troisième kernel de la série KFS (42) : pagination x86, allocateur de frames physiques, allocateurs `kmalloc`/`vmalloc`, et `kernel_panic`. Construit par-dessus le KFS_2 (GDT/IDT/PIC/VGA/shell déjà en place).

## Ce qui a été fait

**Pagination** (`kernel/srcs/paging.c`)
- Identity map des 4 premiers Mo (`page_table_0`), activation via `CR3`/`CR0` (bit `PG`).
- **Recursive page directory mapping** : la dernière entrée du directory (`page_directory[1023]`) pointe sur lui-même, ce qui rend n'importe quelle table de pages accessible à une adresse virtuelle fixe (`0xFFC00000 + dir_index*0x1000`), quelle que soit sa position physique réelle. Corrige un bug où une nouvelle table créée au-delà des 4 premiers Mo était écrite via son adresse physique brute — donc potentiellement non mappée, avec risque de #PF ou de corruption silencieuse.
- `paging_get_table` / `paging_map_page` / `paging_get_phys` : création à la demande de nouvelles tables, écriture d'entrées arbitraires (mapping virtuel → physique quelconque).
- `page_fault_handler` : reporte l'adresse fautive (`CR2`) et le code d'erreur décodé, puis appelle `kernel_panic`.

**Multiboot + frame allocator** (`kernel/srcs/memory/frame_allocator.c`, `kernel/includes/memory/multiboot.h`)
- Parsing de la structure `multiboot_info` transmise par GRUB (magic `EAX`, pointeur `EBX`, tous deux passés depuis `boot.asm` jusqu'à `kernel_main`).
- Lecture de la memory map (`mmap_addr`/`mmap_length`) pour calculer la RAM réelle installée (`g_total_ram`), en ne comptant que les régions `type == 1` (disponibles).
- Bitmap statique (1 bit/frame de 4 Ko, plafonnée à 4 Go — `MAX_FRAMES_SUPPORTED`), qui vit dans le `.bss` du kernel : le kernel et la bitmap elle-même sont donc automatiquement couverts par le marquage `[0, kernel_end)` = used.
- Marquage des zones réservées de la mmap (ACPI, MMIO...) au-delà de `kernel_end`.
- `frame_alloc()` / `frame_free()` : premier frame libre trouvé par balayage linéaire.

**`kmalloc` / `kfree` / `ksize` / `kbrk`** (`kernel/srcs/memory/kmalloc.c`) — tas à chunks (header + liste chaînée), avec split des blocs libres trop grands et fusion (forward) au `kfree`. `kbrk` mappe de nouvelles pages via `frame_alloc` + `paging_map_page`, borné à `[0x00400000, 0x01000000)`.

**`vmalloc` / `vfree` / `vsize` / `vbrk`** (`kernel/srcs/memory/vmalloc.c`) — même mécanique que `kmalloc`, tas démarrant à `0x01000000`, pages mappées avec `PAGE_USER` (contrairement à `kmalloc`, kernel-only) — prépare la séparation kernel/user space évoquée par le sujet.

**`kernel_panic`** (`kernel/srcs/kernel_panic.c`) — message formaté + stack trace + halt définitif (interruptions coupées). Réservé aux erreurs non récupérables ; une allocation qui échoue, par exemple, retourne juste `NULL`/`KBRK_FAILED`.

**Commandes shell de démonstration** (`kernel/srcs/shell.c`) — voir plus bas.

## Sources

Théorie et implémentation basées principalement sur le wiki OSDev :

- [Paging](https://wiki.osdev.org/Paging) — mécanique x86 (CR0/CR3, format des entrées, traduction directory/table/offset)
- [Page Tables](https://wiki.osdev.org/Page_Tables) — recursive page directory mapping (technique utilisée pour corriger le bug ci-dessus)
- [Memory Management](https://wiki.osdev.org/Memory_Management) — vue d'ensemble physique vs virtuelle
- [Memory Management Unit](https://wiki.osdev.org/Memory_Management_Unit) — rôle du MMU dans la traduction d'adresses
- [Page Frame Allocation](https://wiki.osdev.org/Page_Frame_Allocation) — bitmap de frames physiques
- [Memory Allocation](https://wiki.osdev.org/Memory_Allocation) — allocateurs de tas (chunks, split/coalesce)

Sujet KFS_3 (PDF fourni par 42) pour les exigences (Chapitre III : objectifs, Chapitre V : partie obligatoire).

## Tutoriel d'exécution

```sh
make          # compile kernel.bin
make run      # construit l'ISO et lance QEMU
make debug    # comme run, mais sans reboot automatique sur crash
make re       # rebuild complet
```

Au menu GRUB, valider l'entrée **KFS** (Entrée). Le kernel démarre directement dans son propre shell (pas de login).

## Ce que vous allez voir à l'écran

**Au boot**, une série de lignes de diagnostic s'affiche automatiquement, par exemple :

```
Paging enabled, directory at 0x107000 (PG=1)
frame_allocator_init: total RAM 134086656 bytes, 32736 frames
42
GDT initialised at 0x800
Multiboot info structure at 0x10000
=== Kernel Stack Trace ===
  ...
```

Les adresses (`0x107000`, `0x10000`...) ne sont pas censées vous dire quelque chose de particulier — ce sont des positions mémoire déterminées par GRUB/le linker à chaque boot, elles peuvent légèrement varier. Ce qui compte, c'est la cohérence : `PG=1` confirme que la pagination est active, et le nombre de frames doit correspondre à la RAM allouée à la VM (visible aussi via `meminfo`).

**Ensuite**, un curseur clignotant attend une commande. Tapez `help` pour lister toutes les commandes disponibles (liste générée automatiquement depuis la table interne, jamais désynchronisée). Les plus utiles pour observer le travail de ce KFS :

- **`meminfo`** — RAM totale détectée, nombre de frames total/utilisés/libres. Le nombre de "used" doit être petit (quelques centaines) : ce sont les frames occupés par le kernel lui-même et la bitmap, pas par vos allocations.
- **`kheap`** / **`vheap`** — état du tas kernel/utilisateur : `break` (limite actuelle du tas) et `mapped_end` (jusqu'où les pages sont physiquement mappées), suivis de la liste des blocs alloués (adresse, taille, `free`/`used`). Avant toute allocation, les deux valeurs sont à `0x0` — c'est normal, le tas s'initialise seulement au premier appel.
- **`kalloc-test`** / **`valloc-test`** — alloue deux blocs, affiche leurs pointeurs (des adresses comme `0x40000c` : ce sont des adresses **virtuelles** dans la plage réservée au tas kernel `[0x400000, 0x1000000)` ou utilisateur `[0x1000000, ...)` — rien à voir avec une position physique réelle en RAM), libère le premier, puis republie l'état du tas. Rejouez la commande une deuxième fois : le premier bloc revient exactement à la même adresse (réutilisation du bloc libéré), le second obtient une nouvelle adresse (jamais libéré, donc le tas doit grandir).
- **`page-fault`** — déclenche volontairement une faute de page (accès à `0x500000`, hors de la zone identity-mappée) pour vérifier que `kernel_panic` s'affiche correctement (fond rouge, message d'erreur détaillé, stack trace) plutôt que de planter la VM sans explication. **La VM reste ensuite figée en `hlt` — c'est voulu, pas un bug supplémentaire.** `kernel_panic` coupe les interruptions (`cli`) puis boucle sur `hlt` indéfiniment :
  - `cli` avant `hlt` évite qu'un timer ou une frappe clavier ne réveille le CPU en pleine panique et ne reprenne l'exécution dans un état déjà corrompu.
  - La boucle (`while(1) hlt`) protège contre une NMI, qui pourrait en théorie contourner `cli` et réveiller le CPU une fois — il retombe alors immédiatement sur un nouveau `hlt`.
  - `hlt` plutôt qu'une boucle vide `while(1){}` : le CPU s'arrête réellement (basse consommation) au lieu de tourner à 100% pour ne rien faire.
  - Sur le fond, un panic est réservé aux erreurs **non récupérables** : l'objectif n'est pas de réparer, mais de figer la machine dans un état lisible, le temps de lire le message et la stack trace, plutôt que de perdre cette info dans un redémarrage automatique. Il faut relancer la VM manuellement (`make run`/`make debug`) pour repartir.
- **`dmesg`** / **`print-stack`** — journal des messages `kprintk` déjà émis / trace de la pile courante, hérités du KFS_2.
