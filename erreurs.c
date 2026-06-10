#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include "couleurs.h"
#include "erreurs.h"
#include "types.h"


/**********************/
/*                    */
/*  Erreurs Fatales   */
/*                    */
/**********************/

/* Pour s’arrêter lorsque le développeur tombe sur cas qui ne devrait
   jamais arriver. Normalement, si le programme est écrit sans bugs,
   les erreurs fatales ne devrait jamais être rencontrées.

   La variable TEST_ERREUR_FATALE permet l’écriture de test pour
   intercepter l’erreur lors des tests
*/

int TEST_ERREUR_FATALE = 0;


void erreur_fatale(char  *fichier, int ligne, char *causes) {
    if (TEST_ERREUR_FATALE--) return;

    fprintf(stderr,"%s", couleur_rouge);
    fprintf(stderr,"\n   /!\\ Erreur fatale /!\\");
    fprintf(stderr,"%s", couleur_defaut);
    fprintf(stderr,"   %s ligne %d\n\n", fichier, ligne);
    fprintf(stderr,"%s\n", causes);
    exit(1);
}



/**************************/
/*                        */
/*  Gestion du long jump  */
/*                        */
/**************************/

/* Merci de lire le cours pour comprendre ce que fait le type
   jmp_buf. Rapidement, c’est une structure qui sauvegarde l’état du
   programme que l’on pourra restaurer en cas d’erreur. On l’utilisera
   lors de l’exercice 11 */

jmp_buf buf;

jmp_buf *jump_buffer(void) {
    return &buf;
}





/************************/
/*                      */
/*  Erreur du parseur   */
/*                      */
/************************/

void erreur_parseur(char *explication) {

      erreur(SYNTAXE,"parseur vaλisp", explication, NULL);

}


/****************************/
/*                          */
/*  Erreurs (EXERCICE 11)   */
/*                          */
/****************************/


/* ATTENTION Il manque à s à erreurs.h dans les premières question, l'enum peut tout
 à fait être définie dans le fichier erreurs.h (c'est conseillé même ça évitera des
 soucis). */

/* E11 - Q2 */

sexpr SEXPR_ERREUR;
char *FONCTION_ERREUR;
char *MESSAGE_ERREUR;
enum erreurs TYPE_ERREUR;

/* Ajout indépendant du TP, pour pouvoir obtenir les types d'erreurs sous forme de
   chaine plus rapidement. */
char* taberreur[] = {"TYPAGE",
    "ARITE",
    "NOM",
    "MEMOIRE",
    "DIVISION_PAR_ZERO",
    "SYNTAXE",
    "MEMOIRE_PARSEUR",
    "RUNTIME"};


/* E11 - Q3 */
/* Ne pas oublier d'ajouter toutes fonction définies ici dans le header (erreurs.h). */

void afficher_erreur(void) {
    /* Ne pas hésiter à regarder la fin du cours Valisp 2 pour voir l'affichage. */

    printf("%s", couleur_rouge);

    printf("Erreur d'exécution [%s] : %s\n", taberreur[TYPE_ERREUR], MESSAGE_ERREUR);
    /* L'enum TYPE_ERREUR est un entier, donc on peut l'utiliser comme indice dans
     un tableau (très pratique, pas seulement ici donc bon à savoir). */
    printf("Fonction fautive : « %s »\nValeur fautive : « ", FONCTION_ERREUR);
    afficher(SEXPR_ERREUR);
    printf(" »");

    printf("%s", couleur_defaut);
}


/* E11 - Q4 */

void erreur(enum erreurs type, char *fonction, char *explication, sexpr s) {

    SEXPR_ERREUR = s;
    FONCTION_ERREUR = fonction;
    MESSAGE_ERREUR = explication;
    TYPE_ERREUR = type;

    longjmp(buf, 1);
}
