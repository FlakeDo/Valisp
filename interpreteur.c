
#include <stdio.h>
#include "interpreteur.h"       /* Nécéssaire car eval() et apply() sont mutuellement
                                 récursives. */
#include "erreurs.h"
#include "environnement.h"

/* E1 - Q2 */
/* (a) */

sexpr eval_list(sexpr liste, sexpr env) {
    /* Se référer à l'exemple diapo 18. La fonction eval_list() à pour
     but d'évaluer les paramètres de l'expression évaluée.  Par
     exemple, pour l'expression (* (+ 2 2) (- 30 20)), la liste des
     paramètres à évaluer est ((+ 2 2) (- 30 20)).  Comme on l'a
     définie dans le header, on peut utiliser eval() dans cette
     fonction.  C'est elle qui s'occuper d'évaluer les paramètres. */


    if (liste == NULL) return NULL;


    return cons( eval(car(liste), env) ,
                 eval_list(cdr(liste), env));
}


/* (b) */

sexpr bind(sexpr variables, sexpr liste, sexpr env) {
    /* On suit l'algorithme présenté diapo 24 du cours Valisp
       4. L'astuce d'utiliser une boucle while a été donnée en CM, on
       s'en sort aussi avec une recursion (méthode utilisée ici). Dans
       le cas d'une boucle while infini (while()), on sortira de la
       boucle avec un return. */

    sexpr new_env;
    sexpr liaison;

    /* Si varibales est un NULL, on renvoit l'env. */

    if (variables == NULL) return env;

    /* Si c'est un cons... */
    if (cons_p(variables)) {
        /* ...On vérifie que les paramètres sont du bon type... */
        if (!cons_p(liste)) {
            erreur(TYPAGE, "bind", "Mauvais type.", liste);
            return NULL;
        }
        if (!symbol_p(car(variables))) {
            erreur(TYPAGE, "bind", "Mauvais type.", variables);
            return NULL;
        }

        /* ...Si oui, on fait le nécéssaire. */
        liaison = cons(cons(car(variables), car(liste)), NULL);

        set_cdr(liaison, env);
        new_env = liaison;

        return bind(cdr(variables), cdr(liste), new_env);

        /* Si variables est un symbole... */
    } else if (symbol_p(variables)) {

        /* ...On créé une liaison. */
        liaison = cons(cons(variables, liste), NULL);

        set_cdr(liaison, env);
        new_env = liaison;

        return new_env;
    }
    else {
        erreur(TYPAGE, "bind()", "Mauvais type.", variables);
        return NULL;
    }
}


/* (c) */

sexpr eval(sexpr val, sexpr env) {
    /* On suit assez simplement l'algo énoncé dipao 16 du cours Valisp 4. */

    sexpr operateur;
    sexpr parametres;
    sexpr res = NULL;

    /* On regarde les différent cas : */
    if (val == NULL) return NULL;

    if (cons_p(val)) {
        /* Cas le plus complexe, on fait appel à la fonction apply().*/
        operateur = car(val);
        parametres = cdr(val);
        return apply(operateur, parametres, env);
    }

    if (symbol_p(val)) {
        /* On va récupérer dans l'environnement le symbole qui correspond. */

        /* Si le symbole n'existe pas dans l'environnement, on invoque une erreur. */
        if (trouver_variable(env, val, &res) < 0) {
            erreur(NOM, "eval()", "Symbole introuvable dans l'environnement.", val);
            return NULL;
        }
        else {
            /* Si on l'a trouvé, alors on renvoit la valeur associée au symbole. */
            return res;
        }
    }

    else {
        /*  Si c'est autre chose, on le renvoit simplement. */
        return val;
    }
}


/* (d) */

sexpr apply(sexpr fonction, sexpr liste, sexpr env) {
    /* On suit encore une fois la démarche du cours Valisp 4. On va d'abord s'occuper
       de savoir à quelle fonction on a affaire, puis on regarde en fonction de son type
       les cas : en effet, une forme spéciale ne doit pas évaluer les arguments au préalable.
    Dans le cas d'une primitive, on évalue. */

    sexpr eval_f = eval(fonction, env);
    sexpr arguments;
    sexpr parametres;
    sexpr new_env;
    sexpr code;
    sexpr res;


    /* On regarde tous les cas... */

    if (prim_p(eval_f)) {

        /* Diapo 18 du cours Valisp 4 : */

        parametres = eval_list(liste, env);
        res = run_prim(eval_f, parametres, env);
        return res; /* On a fait une fonction à l'époque qui fait
                                              exactement ce qu'on veut youpi ! */
    }

    if (spec_p(eval_f)) {
        /* Ici, on n'évalue pas les arguments : c'est le principe d'une spéciale. */

        return run_prim(eval_f, liste, env);
    }

    if (cons_p(eval_f)) {
        /* On gère ici le cas des lambdas et des macros : */

        if (symbol_match_p(car(eval_f), "lambda")) {
            /* Si c'est un lambda, eval_f est un cons de la forme :
               (lambda . (a b c ...) . (ligne1 ligne2 ...)) ... */

            /* On réccupère les arguments que prend la fonction sous forme de liste. */
            arguments = car(cdr(eval_f));
            /* On calcul les valeurs des arguments via la liste donnée en paramètre. */

            parametres = eval_list(liste, env);

            /* Avec les deux listes définies précédement, on créé un nouvel env qui
             comporte les liaisons éphémères entre les arguments et les valeurs. */
            new_env = bind(arguments, parametres, env);


            /* On évalue le code avec eval(). */

            /* On peut avoir plusieurs lignes de code : */
            for (code = cdr(cdr(eval_f)) ; code != NULL ; code = cdr(code)) {
                res = eval(car(code), new_env);
            }

            return res;

            /* /\* On évalue le code avec eval(). *\/ */
            /* code = car(cdr(cdr(eval_f))); */

            /* printf("DEBUG : Cons du code: "); */
            /* afficher(code); */
            /* printf("\n"); */

            /* return eval(code, new_env); */
        }

        else if (symbol_match_p(car(eval_f), "macro")) {
            /* Si c'est une macro, eval_f est un cons de la forme :
               (macro . (a b c ...) . (ligne1 ligne2 ...)) ... */


            /* On réccupère les arguments que prend la fonction sous forme de liste. */
            arguments = car(cdr(eval_f));
            /* On n'évalue pas les paramètres ici. On passe directement la liste liste. */


            /* Avec les deux listes définies précédement, on créé un nouvel env qui
             comporte les liaisons éphémères entre les arguments et les valeurs. */
            new_env = bind(arguments, liste, env);

            /* On évalue le code avec eval()... */

            /* ...D'abord dans l'env créé... */
            for (code = cdr(cdr(eval_f)) ; code != NULL ; code = cdr(code)) {
                res = eval(car(code), new_env);
            }

            /* ...Puis dans l'env original. */
            return eval(res, env);

        }
    }

    else {
        erreur(TYPAGE, "apply()", "La fonction est d'un type non-pris en charge.", fonction);
        return NULL;
    }
    return NULL;
}
