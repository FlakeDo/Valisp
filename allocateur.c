#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "erreurs.h"

#define TAILLE_MEMOIRE_DYNAMIQUE (1<<15) /* 2^15 */



/* === EXERCICE 1 === */

/* Q3 */

typedef uint32_t bloc;          /* On renomme le type unsigned int32 en bloc. */


/* Q4 */
/* Voir macros au début du fichier. */


/* Q5 */

bloc MEMOIRE_DYNAMIQUE[TAILLE_MEMOIRE_DYNAMIQUE];



/* === EXERCICE 2 === */

/* Q1 */

void initialiser_memoire_dynamique() {
    MEMOIRE_DYNAMIQUE[0] = (1 << 15) - 1; /* Mettre des 1 dans tout l'espace dédié au bloc suivant, càd 15 1. */

    /* 0 000000000000000 0 111111111111111 */

    MEMOIRE_DYNAMIQUE[TAILLE_MEMOIRE_DYNAMIQUE - 1] = (1 << 16) - 1; /* Mettre des 1 dans l'espace d
                                                                        édié au bloc suivant, et à la disponibilité, càd 16 1. */
    /* 0 000000000000000 1 111111111111111 */
}


/* === EXERCICE 3 === */

/* Q1 */

bloc cons_bloc(int rm, int precedant, int libre, int suivant) {
    bloc b = (rm << 31);        /* On créé le bloc et on place chaque valeur à sa place. */
    b = b | (precedant << 16);
    b = b | (libre << 15);
    b = b | (suivant);

    return b;
}


/* Q2 */

int bloc_suivant(int indice) {
    return (MEMOIRE_DYNAMIQUE[indice] & ((1 << 15) - 1));
}


int bloc_precedant(int indice) {
    return ((MEMOIRE_DYNAMIQUE[indice] >> 16) & ((1 << 15) - 1));
}


int usage_bloc(int indice) {
    return ((MEMOIRE_DYNAMIQUE[indice] >> 15) & 1);
}


int rm_bloc(int indice) {
    return ((MEMOIRE_DYNAMIQUE[indice] >> 31) & 1);
}


/* Q3 */

int taille_bloc(int indice) {
    /* Hypothèse : on veut indice_suivant - indice. Dans ce cas, on aura le nombre de blocs libres entre les deux blocs responsables ? C'est bien ça. Pour ne pas prendre en compte le bloc suivant, on décrémente de 1.
       La deuxième ligne permet de ne pas se retrouver avec un taille négative.*/

    int taille;

    taille = bloc_suivant(indice) - indice -1;
    taille = (taille < 0) ? 0 : taille;

    return taille;
}


/* Q5 */

/* Il y a 4 régions libres d'indices 17, 36, 112 et 128. Ils sont de taille respective 16, 60, 7 et 32638. */



/* === EXERCICE 4 === */

/* Q1 */

int rechercher_bloc_libre(int nombre_blocs) {
    /* On se déplace de bloc en bloc, et on compare la taille dispo dans ce bloc avec la taille voulue. */

    int indice;

    for (indice = 0 ; indice < TAILLE_MEMOIRE_DYNAMIQUE - 1 ; indice = bloc_suivant(indice)) {
        if (!usage_bloc(indice) && taille_bloc(indice) >= nombre_blocs) return indice;
    }

    return -1;
}


/* Q2 */

int allocateur_balloc(int nombre_blocs) {
    /* On va chercher un endroit disponible avec la fonction précédente, puis mettre à jour les metadonnées (mettre à jour le bloc responsable de la zone anciennement libre et mettre à jour le bloc suivant la zone allouée, si la zone allouée est plus petite que l'espace dispo). */

    int indice_libre;
    int taille_bloc_libre;
    int indice_nouveau_bloc;
    int indice_libre_suivant;

    indice_libre = rechercher_bloc_libre(nombre_blocs);
    if (indice_libre < 0) return -1;

    taille_bloc_libre = taille_bloc(indice_libre);
    indice_nouveau_bloc = indice_libre + nombre_blocs + 1;
    indice_libre_suivant = bloc_suivant(indice_libre);

    /* On met d'abord à jour le potentiel nouveau bloc responsable. */
    if (taille_bloc_libre > nombre_blocs) {
        MEMOIRE_DYNAMIQUE[indice_nouveau_bloc] = cons_bloc(0, indice_libre, 0, bloc_suivant(indice_libre));
        /* On met à jour l'ancien bloc suivant de indice_libre pour que son nouveau précédent soit le nouveau bloc créé. */
        MEMOIRE_DYNAMIQUE[bloc_suivant(indice_libre)] = cons_bloc(rm_bloc(indice_libre_suivant), indice_nouveau_bloc, usage_bloc(indice_libre_suivant), bloc_suivant(indice_libre_suivant));
    }

    /* On met à jour le bloc responsable de notre zone allouée. */
    MEMOIRE_DYNAMIQUE[indice_libre] = cons_bloc(rm_bloc(indice_libre), bloc_precedant(indice_libre), 1, indice_libre + nombre_blocs + 1);

    return indice_libre;

}


/* Q3 */

int octets_vers_blocs(size_t size) {
    /* On reprend la convresion vue en CM : 4 octets = 4*8 = 32 = 1 bloc.
     On doit bien penser à ajouter + 1 si size n'est pas multiple pour contenir le reste. */

    return (size % sizeof(bloc) == 0) ? size / sizeof(bloc) : (size / sizeof(bloc) + 1);
}


/* Q4 */

void *allocateur_malloc(size_t size) {
    /* On veut simplement convertir size en nombre de blocs (octets_vers_blocs()) et allouer ce nombre de
     blocs (allocateur_balloc()). Il faut ensuite renvoyer le bon pointeur (pas celui vers le bloc
     responsable, mais vers la première case de l'espace alloué). */

    int indice_resp;

    indice_resp = allocateur_balloc(octets_vers_blocs(size));

    if (indice_resp < 0) return NULL;

    return &MEMOIRE_DYNAMIQUE[indice_resp + 1];
}


/* === EXERCICE 5 === */

/* Q1 */

void allocateur_bree(int i) {
    /* On trait les 4 cas possibles ici, en fonction de si les blocs [i-1] et [i+1] sont libres ou non.
     A NOTER : les blocs qui disparaissent pendant une fusion doivent toujours contenir les indices
     des blocs responsables précédent et suivant (en prenant en compte les màj effectuées
     bien évidemment). */

    int bloc_s;
    int bloc_p;
    int bloc_2s;

    bloc_s = bloc_suivant(i);
    bloc_p = bloc_precedant(i);
    bloc_2s = bloc_suivant(bloc_s);

    if (!usage_bloc(bloc_p) && !usage_bloc(bloc_s)) {
        /* Dans ce cas, on a un fusion, il faut MàJ les blocs [i+2] et [i-1]. Les blocs [i] et [i+1] disparaissent. */

                        /* MàJ bloc[i-1] */
        MEMOIRE_DYNAMIQUE[bloc_p] = cons_bloc(rm_bloc(bloc_p), bloc_precedant(bloc_p), usage_bloc(bloc_p), bloc_2s);

        /* MàJ bloc[i + 2] */
        MEMOIRE_DYNAMIQUE[bloc_2s] = cons_bloc(rm_bloc(bloc_2s), bloc_p, usage_bloc(bloc_2s), bloc_suivant(bloc_2s));

        /* Les blocs [i] et [i+1] sont réinitialisés */
        MEMOIRE_DYNAMIQUE[bloc_s] = cons_bloc(rm_bloc(bloc_s), bloc_p, 0, bloc_2s);

        MEMOIRE_DYNAMIQUE[i] = cons_bloc(rm_bloc(i), bloc_p, 0, bloc_2s);
    }

    else if (!usage_bloc(bloc_p) && usage_bloc(bloc_s)) {
        /* Dans ce cas, on a une fusion, il faut MàJ les blocs [i+1] et [i-1]. Le bloc [i] disparait. */

        /* MàJ bloc[i + 1] */
        MEMOIRE_DYNAMIQUE[bloc_s] = cons_bloc(rm_bloc(bloc_s), bloc_p, usage_bloc(bloc_s), bloc_2s);

        /* MàJ bloc[i - 1] */
        MEMOIRE_DYNAMIQUE[bloc_p] = cons_bloc(rm_bloc(bloc_p), bloc_precedant(bloc_p), usage_bloc(bloc_p), bloc_s);

        /* Le bloc [i] est réinitialisé */
        MEMOIRE_DYNAMIQUE[i] = cons_bloc(rm_bloc(i), bloc_p, 0, bloc_s);;
    }

    else if (usage_bloc(bloc_p) && !usage_bloc(bloc_s)) {
        /* Dans ce cas, on a une fusion. On MàJ les blocs [i] et [i+2]. Le bloc [i+1] disparait. */

                /* MàJ bloc[i] */
        MEMOIRE_DYNAMIQUE[i] = cons_bloc(rm_bloc(i), bloc_p, 0, bloc_2s);

        /* MàJ bloc[i + 2] */
        MEMOIRE_DYNAMIQUE[bloc_2s] = cons_bloc(rm_bloc(bloc_2s), i, usage_bloc(bloc_2s), bloc_suivant(bloc_2s));

        /* Le bloc [i+1] est réinitialisé */
        MEMOIRE_DYNAMIQUE[bloc_s] = cons_bloc(rm_bloc(bloc_s), i, 0, bloc_2s);;
    }

    else {
        /* On doit juste libérer [i] */

        MEMOIRE_DYNAMIQUE[i] = cons_bloc(rm_bloc(i), bloc_p, 0, bloc_s);
    }
}


/* Q2 */

void allocateur_free(void *ptr) {
    /* On utilise le raisonnement vu en CM pour obtenir l'indice du responsable à partir du pointeur.
     A parti de là, on free avec allocateur_bree().*/

    int indice;

    indice = (bloc*) ptr - MEMOIRE_DYNAMIQUE - 1;

    allocateur_bree(indice);
}


/* =========== TP 3 =========== */

/* E6 - Q1 */

int pointeur_vers_indice(void *ptr) {
    /* On réutilise le raisonnement vu en CM pour réccupérer l'indice du
       bloc responsable lié au pointeur. */

    int indice = (bloc*) ptr - MEMOIRE_DYNAMIQUE - 1;

    if (indice < 0 || indice >= TAILLE_MEMOIRE_DYNAMIQUE-1) { /* On regarde la taille de la
                                                               mémoire -1 pour éviter
                                                              le cas du dernier bloc.
                                                              C'ÉTAIT LONG TOUT ÇA POUR
                                                              ÇA.*/
        ERREUR_FATALE("Le pointeur renseigné pointe hors de la mémoire.");
    }

    return indice;
}


/* E6 - Q2 */

int ramasse_miettes_lire_marque(void *ptr) {
    /* On a déjà une fonction qui retourne le bit correspondant au ramasse-miettes,
     il faut juste trouver l'indice associé au pointeur grâce à la fonction précédente. */

    return rm_bloc(pointeur_vers_indice(ptr));
}


/* E6 - Q3 */

int ramasse_miettes_poser_marque(void *ptr) {
    /* Je ne sais pas ce que dois renvoyer la fonction, mais elle passe les tests en
       renvoyant 1 donc j'imagine que c'est bon.
       On modifie juste le bit du ramasse-miettes pour qu'il vaille 1. */

    int indice = pointeur_vers_indice(ptr);

    if (ramasse_miettes_lire_marque(ptr)) ERREUR_FATALE("Bloc déjà marqué.");

    MEMOIRE_DYNAMIQUE[indice] = MEMOIRE_DYNAMIQUE[indice] | (1 << 31);

    return 1;
}


/* E6 - Q4 */

int bloc_libre(int i) {
    /* On regarde simplment le bit du rm avec la fonction rm_bloc (puisqu'ici on a l'indice
       et pas un pointeur). Si il est marqué (=1), alors on supprime pas. */

    if (i == TAILLE_MEMOIRE_DYNAMIQUE - 1) return 0; /* On supprime jamais le dernier
                                                        bloc. */

    if (!rm_bloc(i)) return 1;  /* Si il n'est pas marqué (=0), le bloc est supprimé. */
    return 0;
}


/* E6 - Q5 */

void ramasse_miettes_liberer(void) {
    /* A priori, on parcourt la mémoire et on free (avec bree) les blocs qui doivent être
     supprimés (content d'avoir fait bree à l'époque.)
    ATTENTION : Si on obsrve la mémoire qu'on obtient avec ça, on voit que comparé à ce
    que l'on dvrait obtenir, il y a des * un peu partout. Cela signifie que nos blocs
    sont toujours marqués, ce qu'on ne veut pas (c'est pas marqué dans la question.) On
    se débrouille donc pour démarquer les blocs. */

    int i,j;

    for (i = 0 ; i != bloc_suivant(i) ; i = bloc_suivant(i)) {
        if (!bloc_libre(i)) {
            MEMOIRE_DYNAMIQUE[i] = cons_bloc(0, bloc_precedant(i), 1, bloc_suivant(i));
        } else {
            for (j=bloc_suivant(i); bloc_libre(j) ; j = bloc_suivant(j));

            MEMOIRE_DYNAMIQUE[i] = cons_bloc(0, bloc_precedant(i), 0, j);
            MEMOIRE_DYNAMIQUE[j] = cons_bloc(0, i, 1, bloc_suivant(j));
            i=j; /* J est déja traité*/
        }


    }
}



/* Merci à Sarah pour sa contribution. */
