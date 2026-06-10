#include <stdint.h>

#ifndef TYPES_H
#define TYPES_H

/* E3-Q1 */

typedef int bool;


/* E3-Q2 */

struct valisp_object;

typedef struct valisp_object * sexpr;


/* E3-Q5 */

typedef sexpr (*primitive)(sexpr a, sexpr b); /* On procède de cette manière pour définir
                                               un type pointeur vers une fonction, en
                                              rajoutant typedef devant la definition de la
                                              fonction. */

/* E3-Q9 */

void afficher(sexpr exp);


/* ENTIERS */
sexpr new_integer(int32_t i);

bool integer_p(sexpr val);

int32_t get_integer(sexpr val);


/* CHAINES */

sexpr new_string(char *c);

bool string_p(sexpr val);

char *get_string(sexpr val);


/* SYMBOLES */

sexpr new_symbol(char *c);

bool symbol_p(sexpr val);

char *get_symbol(sexpr val);

bool symbol_match_p(sexpr val, const char *chaine);


/* LISTES */

sexpr cons(sexpr e1,sexpr e2);

bool cons_p(sexpr e);

sexpr car(sexpr e);
sexpr cdr(sexpr e);

void set_car(sexpr e, sexpr nouvelle);
void set_cdr(sexpr e, sexpr nouvelle);

void afficher_liste(sexpr e);


/* PRIMITIVES */

sexpr new_primitive(char *nom, primitive p);
sexpr new_speciale(char *nom, primitive p);

bool prim_p(sexpr val);
bool spec_p(sexpr val);

char* get_name(sexpr p);
primitive get_prim(sexpr e);

sexpr run_prim(sexpr p, sexpr liste, sexpr env);


bool sexpr_equal(sexpr e1, sexpr e2);

void test_nb_parametres(sexpr liste, char* fonction, int taille);

sexpr add_valisp(sexpr liste, sexpr env);

#endif
