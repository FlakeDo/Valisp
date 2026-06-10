#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "types.h"
#include "memoire.h"
#include "erreurs.h"

/* E3-Q3 */

enum valisp_type {entier, chaine, symbole, couple, prim, spec};


/* E3-Q4 */

struct cons{
    sexpr car;
    sexpr cdr;
};


/* E3-Q6 */

struct prim {
    char* nom;
    primitive f;
};


/* E3-Q7 */

union valisp_data {             /* Ls noms des different champs de l'union peuvent être
                                 trouvés diapo 12 du cours Valisp 2. Cela sert à définir
                                le type C des objets valisp qu'on aura.*/
    int32_t INTEGER;
    char* STRING;
    struct cons CONS;
    struct prim PRIMITIVE;
};


/* E3-Q8 */

struct valisp_object {
    enum valisp_type type;
    union valisp_data data;
};


/* ENTIERS */
/* E5 - Q1 */

sexpr new_integer(int32_t i) {
    sexpr intgr = valisp_malloc(sizeof(struct valisp_object)); /* On reserve la place d'un objet
                                                                valisp en mémoire. La fonction
                                                               valisp_malloc est dans memoire.h,
                                                               pas allocateur.h. */

    intgr->type = entier;
    intgr->data.INTEGER = i;  /* Il faut bien mettre l'attribut de l'union à la valeur
                                 i, pas seulement intgr->data. */

    return intgr;
}


/* E5 - Q2 */

bool integer_p(sexpr val) {     /* Les fonctions prédictat _p sont utilisées
                                 pour vérifier que la sexpr est bien du type voulu !*/

    /* On fait gaffe au cas val == NULL ici (pas très pertinent pour les entiers mais
       sera important plus tard). */

    return (val) ? ((val->type == entier) ? 1 : 0) : 0; /* Peut très bien se faire avec des
                                                         if, j'aime juste bien cette notation.*/
}


/* E5 - Q3 */

int32_t get_integer(sexpr val) {
    return val->data.INTEGER;
}


/* CHAINES */
/* E6 - Q1 */

int get_len(const char *c) {          /* Ne pas appeler cette fonction 'longueur', elle
                                 est déjà définie autre part, cela entrainera une erreur. */
    int len;
    for (len = 0 ; c[len] != '\0' ; len++);
    return len;
}

char *chaine_vers_memoire(const char *c) {
    int len = get_len(c);
    char *mem_c = valisp_malloc(len * sizeof(char) + 1); /* +1 pour le caractère \0 toujours. */

    for (len = 0 ; c[len] != '\0' ; len++) {
        mem_c[len] = c[len];
    }
    mem_c[len] = '\0';

    return mem_c;
}


/* E6 - Q2 */
/* (a) */

sexpr new_string(char *c) {
    sexpr strg = valisp_malloc(sizeof(struct valisp_object));

    strg->type = chaine;
    strg->data.STRING = chaine_vers_memoire(c);

    return strg;
}


/* (b) */

bool string_p(sexpr val) {
    return (val) ? ((val->type == chaine) ? 1 : 0) : 0;
}


/* (c) */

char *get_string(sexpr val) {
    return val->data.STRING;
}


/* SYMBOLES */
/* E7 - Q1 */
/* (a) */

sexpr new_symbol(char *c) {
    /* On procède de la même manière que pour les fonctions new des autres
     types. On a par contre un symbole spécial, nil, qui correspond à NULL.
    Pour gérer ce cas, on peut utiliser strcmp (pour savoir comment la fonction
    marche, tape ce qu'a écrit le prof dans le terminal, RECOMMANDÉ CAR NE FONCTIONNE
    PAS COMME ON PEUT LE PENSER AU PREMIER ABORD). */

    sexpr smbl = valisp_malloc(sizeof(struct valisp_object));

    if (!strcmp(c, "nil")){
        return NULL; /* .STRING s'attend à recevoir une chaine (autrement
                                         dit, un pointeur). On peut donc lui donner
                                      NULL (le pointeur vide). */

    } else {
        smbl->type = symbole;
        smbl->data.STRING = chaine_vers_memoire(c); /* Un symbole est une chaine qui a
                                                       un statut particulier, mais on
                                                       le stocke comme une chaine. */

        return smbl;
    }
}


/* (b) */

bool symbol_p(sexpr val) {      /* Erreur de segmentation ici, pour le cas général.
                                 SOLUTION : On ne peut pas faire le test (val == NULL)
                                 après le test symbole, sinon problème. */
    return (val) ? (val->type == symbole) : 1; /* On oublie pas le cas de nil. */
}


/* (c) */

char *get_symbol(sexpr val) {
    return (val) ? val->data.STRING : "nil"; /* On oublie toujours pas nil. */
}


/* E7 - Q2 */

bool symbol_match_p(sexpr val, const char *chaine) {
    return (!strcmp(get_symbol(val), chaine)) ? 1 : 0; /* Pas besoin de s'occupr de nil
                                                         on l'a fait avant ! \('o')/ */
}


/* LISTES */
/* E8 - Q1 */

sexpr cons(sexpr e1, sexpr e2) {
    /* On créé assze simplement l'objet, comme on l'a fait apparavant. Un cons est
       un couple entre deux objet valsip, il faut donc remplir les deux cases car et cdr. */

    sexpr new_cons = valisp_malloc(sizeof(struct valisp_object));

    new_cons->type = couple;
    new_cons->data.CONS.car = e1;
    new_cons->data.CONS.cdr = e2;

    return new_cons;
}


/* E8 - Q2 */

bool cons_p(sexpr e) {
    /* Assze direct ici aussi. On oublie pas de tester si l'expression existe bien. */

    return (e) ? (e->type == couple) : 0;
}


/* E8 - Q3 */

sexpr car(sexpr e) {
    /* On fera attention au cas d'erreur si e est NULL. */

    if (!e) ERREUR_FATALE("L'expression fournie est NULL.");

    return e->data.CONS.car;
}

sexpr cdr(sexpr e) {
    /* Même chose ici. */

    if (!e) ERREUR_FATALE("L'expression fournie est NULL.");

    return e->data.CONS.cdr;
}


/* E8 - Q4 */

void set_car(sexpr e, sexpr nouvelle) {
    e->data.CONS.car = nouvelle;
}

void set_cdr(sexpr e, sexpr nouvelle) {
    e->data.CONS.cdr = nouvelle;
}


/* E8 - Q5 */

void affiche_liste(sexpr e) {
    /* Fonction auxilliaire utilisée dans afficher_liste(). Elle suit le
     protocole décrit diapo 17 du cours Valisp 2. */

    afficher(car(e));

    if (cdr(e)) {
        if (cons_p(cdr(e))) {
            printf(" ");
            affiche_liste(cdr(e));
        } else {
            printf(" . ");
            afficher(cdr(e));
        }
    }
}

void afficher_liste(sexpr e) {
    printf("(");
    affiche_liste(e);
    printf(")");
}


/* E9 - Q1 */

sexpr new_primitive(char *nom, primitive p) {
    /* Similaire aux autres types. */
    /* PROBLEME : nom de la primitive non ajoutée à la mémoire.
     SOLUTION : comme d'hab, on peut pas juste faire = avec un chaine de caractères.
    On utilise alors chaine_vers_memoire(). */

    sexpr new_prim = valisp_malloc(sizeof(struct valisp_object));

    new_prim->type = prim;
    new_prim->data.PRIMITIVE.nom = chaine_vers_memoire(nom);
    new_prim->data.PRIMITIVE.f = p;

    return new_prim;
}

sexpr new_speciale(char *nom, primitive p) {
    /* La seule différence avec une primitive est le type de l'objet. */

    sexpr new_spec = valisp_malloc(sizeof(struct valisp_object));

    new_spec->type = spec;
    new_spec->data.PRIMITIVE.nom = chaine_vers_memoire(nom);
    new_spec->data.PRIMITIVE.f = p;

    return new_spec;
}


/* E9 - Q2 */

bool prim_p(sexpr val) {
    /* Pour cette fonction et la suivante, on fait toujours gaffe à l'existence de
     l'expression reçue. */

    return (val) ? (val->type == prim) : 0;
}

bool spec_p(sexpr val) {
    return (val) ? (val->type == spec) : 0;
}


/* E9 - Q3 */

char* get_name(sexpr p) {
    return p->data.PRIMITIVE.nom;
}

primitive get_prim(sexpr p) {
    return p->data.PRIMITIVE.f;
}


/* E9 - Q4 */

sexpr run_prim(sexpr p, sexpr liste, sexpr env) {
    /* Une primitive renvoit un objet valisp, on doit donc avant l'execution lui allouer
     de la place. Ensuite on réccupère simplement la primitive et on lui donne les
     arguments. */

    sexpr resultat = valisp_malloc(sizeof(struct valisp_object));

    resultat = get_prim(p)(liste, env);

    return resultat;
}


/* E10 */

bool sexpr_equal(sexpr e1, sexpr e2) {


    if (integer_p(e1) && integer_p(e2)) {
        if (get_integer(e1) == get_integer(e2)) return 1;

    }
    else if (string_p(e1) && string_p(e2)) {
        if (!strcmp(get_string(e1), get_string(e2))) return 1;

    }
    else if (symbol_p(e1) && symbol_p(e2)) {
        if (symbol_match_p(e1, get_symbol(e2))) return 1;

    }
    else if (cons_p(e1) && cons_p(e2)) {
        /* On vide les listes jusqu'à ce qu'une soit vide. On vérifie que les
           deux sont bien vide à l'issue de la boucle. */
        for (; e1 != NULL && e2 != NULL ; e1 = cdr(e1), e2 = cdr(e2));
        if (e1 == NULL && e2 == NULL) return 1;

    }
    else if (prim_p(e1) && prim_p(e2)) {
        if (!strcmp(get_name(e1), get_name(e2))) return 1;

    }
    else if (spec_p(e1) && spec_p(e2)) {
        if (!strcmp(get_name(e1), get_name(e2))) return 1;

    }
    return 0;
}






/* AFFICHER */

void afficher(sexpr exp) {

    if (!exp) {                 /* Mis à jour après la définition des symboles : on test */
        printf("%s", get_symbol(exp));
    } else {

        switch (exp->type) {        /* On utilise un switch pour discriminer les cas et
                                       afficher le bon type en fonction de la sexpr donnée.*/
        case entier:
            printf("%d", get_integer(exp));
            break;
        case chaine:
            printf("\"%s\"", get_string(exp));
            break;
        case symbole:
            printf("%s", get_symbol(exp));
            break;
        case couple:
            afficher_liste(exp);
            break;
        case prim:
            printf("#p<%s>", get_name(exp));
            break;
        case spec:
            printf("#s<%s>", get_name(exp));
            break;
        default:
            printf("<\?\?\?>");    /* Apparement "??>" corrspond au caractère "}", on
                                      met donc des \ pour "casser" le motif (c'est utilisé
                                      en général
                                      pour taper comme une chaine de caractères un truc qui
                                      est prédéfini par C, comme \n par exemple)*/
            break;
        }
    }
}
