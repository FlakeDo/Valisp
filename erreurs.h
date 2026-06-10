#include <setjmp.h>
#include "types.h"
#ifndef VALISP_ERREUR_H
#define VALISP_ERREUR_H

/* E2-Q1 */
#define ERREUR_FATALE(CAUSE) erreur_fatale(__FILE__, __LINE__, #CAUSE)

void erreur_fatale(char  *fichier, int ligne, char* causes);

void erreur_parseur(char *explication);

jmp_buf *jump_buffer(void);

void afficher_erreur(void);

enum erreurs {
    /* Trouvable dans le cours Valisp 2 diapo 28. */

    TYPAGE,
    ARITE,
    NOM,
    MEMOIRE,
    DIVISION_PAR_ZERO,
    SYNTAXE,
    MEMOIRE_PARSEUR,
    RUNTIME
};


void erreur(enum erreurs type, char *fonction, char *explication, sexpr s);

#endif
