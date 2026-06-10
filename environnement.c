#include <stddef.h>
#include <stdio.h>

#include "types.h"
#include "allocateur.h"
#include "lib_memoire.h"
#include "couleurs.h"
#include "erreurs.h"
#include "primitives.h"

#define CHARGER_UNE_PRIMITIVE(NOM, FONCTION) charger_une_primitive(NOM, #FONCTION, FONCTION)
#define CHARGER_UNE_SPECIALE(NOM, FONCTION) charger_une_speciale(NOM, #FONCTION, FONCTION)


/* E2 - Q1 */

sexpr ENV = NULL;


/* E2 - Q2 */

sexpr environnement_global(void) {
    return ENV;
}

void initialiser_memoire(void) {
    /* L'initialisation de
       l'environnement ne se fait pas en ajoutant un cons (t . t), il faut initialiser une
    liste chainée (donc un cons de cons qui se termine par nil (NULL)), et mettre comme
    premier élément le cons (t . t). */

    sexpr t;

    initialiser_memoire_dynamique();

    t  = new_symbol("t");

    ENV = cons(cons(t, t), NULL);
}


/* E3 - Q1 */

int longueur_env(sexpr env) {
    /* On parcours l'environnement comme présenté diapo 4 du cours Valisp 3. */

    sexpr liste;
    int len = 0;

    for (liste = env ; liste != NULL ; liste = cdr(liste)) len++; /* L'incrément est ici
                                                                     l'environnement
                                                                     lui-même: tant qu'il n'est
                                                                     pas vide,
                                                                     on incrémente la longueur. */

    return len;
}


/* E3 - Q2 */

void valisp_stat_memoire(void) {
    /* La fonction afficher_stat_memoire() se trouve dans le fichier lib_memoire.c, ne prend
     aucun paramètre et ne renvoi rien. */

    sexpr env = environnement_global();

    afficher_stat_memoire();
    printf(" → %d variables", longueur_env(env));
}


/* E3 - Q3 */

void afficher_env(sexpr env) {
    /* On va parcourir l'environnement comme décrit dans le cours Valisp 3. */

    sexpr liste;                /* On fait une copie de l'environnement pour le parcourir. */
    sexpr liaison;              /* Une liaison est un cons, donc un objet Valisp. */
    sexpr nom;
    sexpr val;                  /* C'est pratique, maintenant qu'on a défini les types,
                                 On créé juste des objets Valisp à chaque fois. */

    for (liste = env ; liste != NULL ; liste = cdr(liste)) {
        liaison = car(liste);
        nom = car(liaison);
        val = cdr(liaison);

        printf("%s", couleur_bleu); /* On utilise cette syntaxe pour changer la couleur. Cela
                                     nécéssite l'import de couleurs.h. ATTENTION : ne pas
                                    utiliser printf(couleur_bleu) comme dans le cours, ne
                                    pas utiliser de "" peut entrainer un fuite mémoire. */
        afficher(nom);
        printf("%s", couleur_defaut);
        printf(" ");
        afficher(val);
        printf("\n");
    }
}


/* E4 - Q1 */

int trouver_variable(sexpr env, sexpr variable, sexpr *resultat) {
    /* On parcours encore l'environnement. Ne pas hésiter à utiliser les fonctions
     du TP précédent pour accéder au type C des objets Valisp. Ici par exemple, on a
    besoin du nom de l'objet sous forme de chaine, donc la réccupère avec get_string(). */

    sexpr liste;
    sexpr liaison;
    sexpr nom;

    for (liste = env ; liste != NULL ; liste = cdr(liste)) {
        liaison = car(liste);
        nom = car(liaison);

        if (symbol_match_p(variable, get_string(nom))) {  /* On utilise la fonction
                                                            symbol_match_p(). Attention,
                                                         le deuxième argument est une chaine
                                                         de carectères. */

            *resultat = cdr(liaison); /* resultat est un pointeur, on utilise donc * pour
                                       accéder à la valeur pointée. */
            return 0;
        }
    }

    return -1;                  /* Si la valeur n'est pas trouvée. */
}


/* E4 - Q2 */

int modifier_variable(sexpr env, sexpr variable, sexpr valeur) {
    /* Encore une fois, il faut bien penser à utiliser les fonctions qu'on a défini dans
     types.c : elles nous permettent les modifications nécéssaires à ce que l'on veut
     faire. */


    sexpr liste;
    sexpr liaison;
    sexpr nom;
    char *var_nom = get_symbol(variable); /* Estetique et pratique (éviter l'appel à
                                             la fonction à chaque tour de boucle.) */

    for (liste = env ; liste != NULL ; liste = cdr(liste)) {
        liaison = car(liste);
        nom = car(liaison);

        if (symbol_match_p(nom, var_nom)) {
            set_cdr(liaison, valeur); /* Permet de modifier le coudeur, définie
                                         dans types.c. */
            return 0;
        }
    }
    return -1;
}


/* E4 - Q3 */

void definir_variable_globale(sexpr variable, sexpr valeur) {
    /* On n'utilise pas la fonction précédente ici car elle ne nous permettrait pas
     d'accéder à la fin de l'environnement (on pourrait mais il faudrait parcourir
     2 fois l'environnement.) Sinon c'est globalement une copie conforme de la fonction,
    en sauvegardant le dernier coudeur à chaque fois.
    On rajoute un dernier élément de la même manière qu'on a rajouté le cons (t . t)
    dans l'initialisation e l'environnement. */

    sexpr liste;
    sexpr liaison;
    sexpr nom;
    sexpr sauv;                 /* On souhaite savegarder l'état précédent de la liste
                                   dans la boucle pour éviter de se retrouver avec une
                                liste vide après la fin de la boucle qui n'est pas
                                modifiable. */
    char *var_nom = get_symbol(variable); /* Estetique et pratique (éviter l'appel à
                                             la fonction à chaque tour de boucle.) */

    for (liste = ENV ; liste != NULL ; liste = cdr(liste)) {
        liaison = car(liste);
        nom = car(liaison);

        if (symbol_match_p(nom, var_nom)) {
            set_cdr(liaison, valeur);
            return;
        }
        sauv = liste;
    }

    /* Lorsque l'on a atteint la fin de la liste, liste vaut NULL, mais on a gardé
     sa valeur précédente. */
    set_cdr(sauv, cons(cons(variable, valeur), NULL));
}


/* E4 - Q4 */

/* VOIR EN BAS DU FICHIER. */


/* E4 - Q5 */

/* (a) */

/* Pour ces deux fonctions, il faut créer des objets Valisp adaptés à passer en
   paramètre de la fonction definir_variable_globale(). On utilise les fonctions
de types.c. */

void charger_une_primitive(char *nom_valisp, char *nom_c, primitive p) {
    definir_variable_globale(new_symbol(nom_valisp), new_primitive(nom_c, p));
}

/* (b) */

void charger_une_speciale(char *nom_valisp, char *nom_c, primitive p) {
    definir_variable_globale(new_symbol(nom_valisp), new_speciale(nom_c, p));
}


/* E4 - Q5 */

void charger_primitives() {
    /* Voir début du fichier pour la macro CHARGER_UNE_PRIMITIVE(NOM, FONCTION). */

    /* L'addition : */
    CHARGER_UNE_PRIMITIVE("+", add_valisp);

    /* La multiplication : */
    CHARGER_UNE_PRIMITIVE("*", mul_valisp);

    /* La soustraction : */
    CHARGER_UNE_PRIMITIVE("-", sub_valisp);

    /* La division : */
    CHARGER_UNE_PRIMITIVE("/", div_valisp);

    /* Le modulo : */
    CHARGER_UNE_PRIMITIVE("%", mod_valisp);

    /* L'infériorité : */
    CHARGER_UNE_PRIMITIVE("<", less_than_valisp);


    /* Le car d'une liste : */
    CHARGER_UNE_PRIMITIVE("car", car_valisp);

    /* Le coudeur d'une liste : */
    CHARGER_UNE_PRIMITIVE("cdr", cdr_valisp);

    /* Le cons d'une liste : */
    CHARGER_UNE_PRIMITIVE("cons", cons_valisp);


    /* L'égalité : */
    CHARGER_UNE_PRIMITIVE("=", equal_valisp);

    /* L'affichage : */
    CHARGER_UNE_PRIMITIVE("print", print_valisp);

    /* Le type d'un objet : */
    CHARGER_UNE_PRIMITIVE("typeof", type_of_valisp);


    /* Defvar : */
    CHARGER_UNE_SPECIALE("defvar", defvar_valisp);

    /* Setq : */
    CHARGER_UNE_SPECIALE("setq", setq_valisp);


    /* Quote : */
    CHARGER_UNE_SPECIALE("quote", quote_valisp);

    /* Lambdas : */
    CHARGER_UNE_SPECIALE("lambda", lambda_valisp);

    /* Macros : */
    CHARGER_UNE_SPECIALE("macro", macro_valisp);

    /* Le if : */
    CHARGER_UNE_SPECIALE("if", if_valisp);


    /* L'évaluation : */
    CHARGER_UNE_PRIMITIVE("eval", eval_valisp);

    /* L'application : */
    CHARGER_UNE_PRIMITIVE("apply", apply_valisp);
}
