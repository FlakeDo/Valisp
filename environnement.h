#ifndef ENVIRONNEMENT_H
#define ENVIRONNEMENT_H



sexpr environnement_global(void);

void initialiser_memoire(void);

int longueur_env(sexpr env);

void valisp_stat_memoire(void);

void afficher_env(sexpr env);

int trouver_variable(sexpr env, sexpr variable, sexpr *resultat);

int modifier_variable(sexpr env, sexpr variable, sexpr valeur);

void definir_variable_globale(sexpr variable, sexpr valeur);

void charger_primitives();

#endif
