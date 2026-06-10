#include <stdio.h>
#include <stdint.h>
#include "allocateur.h"
#include "erreurs.h"


void * valisp_malloc(size_t size) {
    void *ptr = allocateur_malloc(size);

    if (ptr == NULL) {
        erreur(MEMOIRE, "valisp_malloc", "Mémoire pleine.", NULL);
    }
    return ptr;
}


/* =========== TP 3 =========== */

/* E7 - Q1 */

void ramasse_miettes_parcourir_et_marquer(sexpr s) {
    /* Il manque un s à "miettes" dans l'énoncé.

      En cours d'écriture de la fonction :
       Ne fonctionne pas : il faut essayer de comprendre la logique du parcours.
     On peut accéder au pointeur de l'environnment, mais celui-ci ne se trouve
    pas dans la mémoire Valisp. Il ne faut peut être pas utiliser les fonctions
    précédement définies, mais il faut trouver le moyen d'accéder au statut
    ramasse-miettes des objets Valisp. Regarder comment les objets Valisp sont
 stockés dans la mémoire Valisp.

    Après écriture de la fonction :
    Beaucoup plus simple en réalité : la sexpr passée en paramètre peut très bien être donnée
    aux fonctions précédement définies (qui demande des pointeurs). On parcourt donc
    LITTERALEMENT l'environnement en regardant si l'OBJET en question est marqué. On suit
    l'algo du cours, en verifiant bien pour chaque type que l'objet n'est pas déjà marqué.
    ATTENTION, si on ne teste pas si le coudeur vaut NULL pour le type cons, on peut se
    retrouver avec s = 0 : comme le pointeur nul correspond à un indice négatif (hors de
    la mémoire évidemment), cela entraine une erreur. */

    char *ch;

    if (s == NULL) return ;
    if (ramasse_miettes_lire_marque(s)) {
        /* On s'arrête */
        return;
    } else {
        /* On le marque */
        ramasse_miettes_poser_marque(s);
    }

    /* On regarde les cas en fonction du type ... */
    if (string_p(s)) {
        ch = get_string(s);
        if (!ramasse_miettes_lire_marque(ch)) ramasse_miettes_poser_marque(ch);

    } else if (symbol_p(s)) {
        ch = get_symbol(s);
        if (!ramasse_miettes_lire_marque(ch)) ramasse_miettes_poser_marque(ch);

    } else if (prim_p(s) || spec_p(s)) {
        ch = get_name(s);
        if (!ramasse_miettes_lire_marque(ch)) ramasse_miettes_poser_marque(ch);

    }
    else if (cons_p(s)) {
        ramasse_miettes_parcourir_et_marquer(car(s));
        ramasse_miettes_parcourir_et_marquer(cdr(s)); /* Si on rentre dans
                                                                           la fonction avec
                                                                          le pointeur NULL,
                                                                          on obtient une
                                                                          erreur, d'où ce
                                                                          test. */
    }
}


/* E6 - Q2 */

void valisp_ramasse_miettes(sexpr env) {
    /* Vraiment juste lire la question (faut vraiment pas faire d'effort pour pas avoir
       celle là ...) */

    ramasse_miettes_parcourir_et_marquer(env);
    ramasse_miettes_liberer();
}
