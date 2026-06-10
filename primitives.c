#include <stdio.h>

#include "memoire.h"
#include "erreurs.h"
#include "types.h"
#include "environnement.h"
#include "interpreteur.h"

/* Bien penser à ajouter toutes les fonction définies ici dans le header. */

/* E12 - Q1 */

void test_nb_parametres(sexpr liste, char* fonction, int taille) {
    /* On va parcourir la liste sexpr par sexpr jusqu'à ce que le cdr soit NULL. A la fin,
     si les tailles ne correspondent pas, on renvoit l'erreur d'arité. La fonction donnée
    en paramètre de l'erreur corrspond à celle donnée en paramètre de cette fonction.
    Les choses en commentaires sont des essais échus.*/

    sexpr cursor;
    int i;

    cursor = liste;

    for (i = 0 ; cdr(cursor) && i+1 <= taille ; i++) {
        cursor = cdr(cursor);
    }
    if (i+1 != taille) {
        erreur(ARITE, fonction, "Nombre de paramètres invalide.", cursor);
    }
}




/* E12 - Q2 */
/* (a) */

sexpr add_valisp(sexpr liste, sexpr env) {
    sexpr a;
    sexpr b;

    /* On utilise la fonction précédente pour vérifier l'arité. */
    test_nb_parametres(liste, "+", 2);

    a = car(liste);
    b = car(cdr(liste));        /* cdr est une liste, on veut seulement car de cdr. */

    if (!integer_p(a)) erreur(TYPAGE, "+", "Ce n'est pas un entier.", a);
    if (!integer_p(b)) erreur(TYPAGE, "+", "Ce n'est pas un entier.", b);

    /* a et b sont encore des objets valisp. Pour faire le calcul, on doit réccupérer
       la donnée. */
    return new_integer(get_integer(a) + get_integer(b));
}


/* (b) */

sexpr mul_valisp(sexpr liste, sexpr env) {
    sexpr a;
    sexpr b;

    test_nb_parametres(liste, "*", 2);

    a = car(liste);
    b = car(cdr(liste));

    if (!integer_p(a)) erreur(TYPAGE, "*", "Ce n'est pas un entier.", a);
    if (!integer_p(b)) erreur(TYPAGE, "*", "Ce n'est pas un entier.", b);

    /* a et b sont encore des objets valisp. Pour faire le calcul, on doit réccupérer
       la donnée. */
    return new_integer(get_integer(a) * get_integer(b));
}


/* (c) */

sexpr sub_valisp(sexpr liste, sexpr env) {
    sexpr a;
    sexpr b;

    /* Le test d'arité est ici un peu plus particulier : */
    if (!liste) {
        /* On regarde si la liste des paramètres est nulle. */
        erreur(ARITE, "-", "Nombre de paramètres invalide.", liste);

    }
    else if (!cdr(liste)) {
        /* Dans ce cas, la liste ne contient qu'un paramètre, c'est la négation. */

        a = car(liste);

        if (!integer_p(a)) erreur(TYPAGE, "-", "Ce n'est pas un entier.", a);

        return new_integer( - get_integer(a));

    }
    else {
        /* Dans ce cas, si la liste des paramètres est bien 2, on fait la
           soustraction. */
        test_nb_parametres(liste, "-", 2);

        a = car(liste);
        b = car(cdr(liste));

        if (!integer_p(a)) erreur(TYPAGE, "-", "Ce n'est pas un entier.", a);
        if (!integer_p(b)) erreur(TYPAGE, "-", "Ce n'est pas un entier.", b);

        return new_integer(get_integer(a) - get_integer(b));
    }

    a = car(liste);
    b = car(cdr(liste));

    if (!integer_p(a)) erreur(TYPAGE, "+", "Ce n'est pas un entier.", a);
    if (!integer_p(b)) erreur(TYPAGE, "+", "Ce n'est pas un entier.", b);

    /* a et b sont encore des objets valisp. Pour faire le calcul, on doit réccupérer
       la donnée. */
    return new_integer(get_integer(a) + get_integer(b));
}


/* (d) */

sexpr div_valisp(sexpr liste, sexpr env) {
    sexpr a;
    sexpr b;

    test_nb_parametres(liste, "/", 2);

    a = car(liste);
    b = car(cdr(liste));

    if (!integer_p(a)) erreur(TYPAGE, "/", "Ce n'est pas un entier.", a);
    if (!integer_p(b)) erreur(TYPAGE, "/", "Ce n'est pas un entier.", b);

    if (get_integer(b) == 0) erreur(DIVISION_PAR_ZERO, "/", "Ça fait l'infini !", liste);

    /* a et b sont encore des objets valisp. Pour faire le calcul, on doit réccupérer
       la donnée. */
    return new_integer(get_integer(a) / get_integer(b));
}


/* (e) */

sexpr mod_valisp(sexpr liste, sexpr env) {
    sexpr a;
    sexpr b;

    test_nb_parametres(liste, "%", 2);

    a = car(liste);
    b = car(cdr(liste));

    if (!integer_p(a)) erreur(TYPAGE, "%", "Ce n'est pas un entier.", a);
    if (!integer_p(b)) erreur(TYPAGE, "%", "Ce n'est pas un entier.", b);

    if (get_integer(b) == 0) erreur(DIVISION_PAR_ZERO, "%", "Modulo par zéro.", liste);

    /* a et b sont encore des objets valisp. Pour faire le calcul, on doit réccupérer
       la donnée. */
    return new_integer(get_integer(a) % get_integer(b));
}


/* (f) */

sexpr less_than_valisp(sexpr liste, sexpr env) {
    sexpr a;
    sexpr b;
    sexpr bool_res = NULL;

    test_nb_parametres(liste, "<", 2);

    a = car(liste);
    b = car(cdr(liste));

    if (!integer_p(a)) erreur(TYPAGE, "<", "Ce n'est pas un entier.", a);
    if (!integer_p(b)) erreur(TYPAGE, "<", "Ce n'est pas un entier.", b);

    /* a et b sont encore des objets valisp. Pour faire le calcul, on doit réccupérer
       la donnée. */
    if (get_integer(a) < get_integer(b)) {

        /* On veut retourner t en cas de réussite du test. On utilise pour ça
           trouver_variable(). */
        trouver_variable(env, new_symbol("t"), &bool_res);
        return bool_res;
    }
    else {
        return bool_res;
    }
}


/* E12 - Q3 */

/* (a) */

sexpr car_valisp(sexpr liste, sexpr env) {


    sexpr head;

    if (car(liste) == NULL) return NULL;

    test_nb_parametres(liste, "car", 1);

    liste = car(liste);         /* L'élément de la liste ici est lui même
                                   une liste, d'où cette ligne pour bien avoir
                                liste en tant que la liste étudiée. */

    head = car(liste);

    if (prim_p(head) || spec_p(head)) erreur(TYPAGE, "car", "Le premier élément est une fonction.", head);

    return head;
}


/* (b) */

sexpr cdr_valisp(sexpr liste, sexpr env) {


    sexpr tail;
    sexpr head;

    if (car(liste) == NULL) return NULL;

    test_nb_parametres(liste, "cdr", 1);

    liste = car(liste);         /* L'élément de la liste ici est lui même
                                   une liste, d'où cette ligne pour bien avoir
                                liste en tant que la liste étudiée. */
    tail = cdr(liste);
    head = car(liste);

    if (prim_p(head) || spec_p(head)) erreur(TYPAGE, "cdr", "Le premier élément est une fonction.", head);

    return tail;
}


/* (c) */

sexpr cons_valisp(sexpr liste, sexpr env) {


    sexpr head;
    sexpr tail;

    test_nb_parametres(liste, "cons", 2);

    /* Ici, on se fiche de savoir si les éléments de liste sont des cons ou pas,
     on n'utilise pas la ligne des fonctions précédentes. */
    head = /* (cons_p(car(liste))) ? car(car(liste)) : */ car(liste);
    tail = (cons_p(cdr(liste))) ? car(cdr(liste)) : cdr(liste);

    return cons(head, tail);
}


/* E12 - Q4 */

/* (a) */

sexpr equal_valisp(sexpr liste, sexpr env) {
    sexpr a;
    sexpr b;
    sexpr bool_res = NULL;

    test_nb_parametres(liste, "=", 2);

    a = car(liste);
    b = car(cdr(liste));

    if (sexpr_equal(a, b)) {

        /* On veut retourner t en cas de réussite du test. On utilise pour ça
           trouver_variable(). */
        trouver_variable(env, new_symbol("t"), &bool_res);
        return bool_res;
    }
    else {
        return bool_res;
    }
}


/* (b) */

sexpr print_valisp(sexpr liste, sexpr env) {

    sexpr elem;

    for (; liste != NULL ; liste = cdr(liste)) {
        elem = car(liste);
        afficher(elem);
    }
    return elem;
}


/* (c) */

sexpr type_of_valisp(sexpr liste, sexpr env) {

    sexpr obj = car(liste);

    test_nb_parametres(liste, "typeof", 1);

    if (integer_p(obj)) return new_symbol("int");

    else if (string_p(obj)) return new_symbol("string");

    else if (symbol_p(obj)) return new_symbol("symbol");

    else if (cons_p(obj)) return new_symbol("cons");

    else if (prim_p(obj)) return new_symbol("primitive");

    else if (spec_p(obj)) return new_symbol("speciale");

    else {
        ERREUR_FATALE("La chose renseignée n'est pas un objet Valisp.");
        return NULL;
    }
}




/* E5 - Q1 */

sexpr defvar_valisp(sexpr liste, sexpr env) {
    sexpr nom, exp, res;

    test_nb_parametres(liste, "defvar", 2);
    nom = car(liste);           /* (defvar nom exp) */
    exp = car(cdr(liste));

    if (!symbol_p(nom)) {
        erreur(TYPAGE, "defvar", "le 1er paramètre doit être un sybmbole.", nom);
    }
    res = eval(exp, env);                   /* Sera modifié après avoir défini la fonction eval.
                                  Il faut évaluer à la main le second paramètre exp. */

    definir_variable_globale(nom, res);
    return res;
}


/* E5 - Q2 */

sexpr setq_valisp(sexpr liste, sexpr env) {
    /* Exactement la même que la précédente, mais avec modifier_variable(). */

    sexpr nom, exp, res;

    test_nb_parametres(liste, "setq", 2);
    nom = car(liste);
    exp = car(cdr(liste));

    if (!symbol_p(nom)) {
        erreur(TYPAGE, "setq", "Le premier paramètre doit être un symbole.", nom);
    }

    res = eval(exp, env);                  /* A modifier quand eval() sera définie. */
    modifier_variable(env, nom, res);
    return res;
}


/* === TP n°4 === */

/* E2 - Q1 */

/* (a) */

sexpr quote_valisp(sexpr liste, sexpr env) {
    /* En suivant littéralement la dernière ligne de la diapo 31 du cours Valisp 4,
     "renvoie toujours son premier et unique paramètre.". On va cependant quand
    même faire un test d'arité pour vérifier qu'on a bien qu'un seul paramètre. */

    test_nb_parametres(liste, "quote_valisp", 1); /* Le test d'arité. */

    return car(liste);          /* Et on retourne le CAR de la liste (on a bien
                                   vérifié que liste n'avait qu'un élément,
                                   on retourne cet élément). */
}


/* (b) */

sexpr lambda_valisp(sexpr liste, sexpr env) {
    /* Lors d'un appel de cette fonction, on utilise 'lambda a b c ...'. Le paramètre liste
     ici correspond à (a . b . c . ...).
    Pour le moment, on créé un nouveau symbole systématiquement, on trouvera un moyen de faire autrement plus tard. */

    return cons(new_symbol("lambda"), liste);
}


/* (c) */

sexpr macro_valisp(sexpr liste, sexpr env) {
    /* Même chose que pour les lambdas, mais avec le mot clé 'macro' */

    return cons(new_symbol("macro"), liste);
}


/* (d) */

sexpr if_valisp(sexpr liste, sexpr env) {
    /* Le test et les codes en cas d'echec et de succès du test sont dans la liste (j'imagine, sinon on
       aurait eu 3 paramètres).
    On peut se permettre de juste faire eval(test), car NULL st interprété comme faux en C. */

    sexpr test = car(liste);
    sexpr test_s = car(cdr(liste));
    sexpr test_f = car(cdr(cdr(liste)));

    if (eval(test, env)) {
        return eval(test_s, env);
    }
    else {
        return eval(test_f, env);
    }
}


/* E2 - Q2 */

/* (a) */

sexpr eval_valisp(sexpr liste, sexpr env) {
    test_nb_parametres(liste, "eval", 1);

    return eval(car(liste), env);
}

/* (b) */

sexpr apply_valisp(sexpr liste, sexpr env) {
    /* On a ici 2 paramètres : la fonction à appliquer et ce sur quoi
       l'appliquer. */

    sexpr fonction;
    sexpr cible;

    test_nb_parametres(liste, "apply", 2);

    fonction = car(liste);
    cible = car(cdr(liste));

    return apply(fonction, cible, env);
}
