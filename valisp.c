#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <getopt.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "allocateur.h"
#include "memoire.h"
#include "types.h"
#include "primitives.h"
#include "interpreteur.h"
#include "environnement.h"
#include "erreurs.h"
#include "couleurs.h"
#include "lib_memoire.h"
#include "lib_repl.h"
#include "parseur.h"





/**********/
/*        */
/*  REPL  */
/*        */
/**********/






#define TAILLE_BUFFER  10000




int repl(void) {
  char* invite_defaut = "\001\033[1;33m\002vaλisp>\001\033[1;0m\002 ";
  char* invite = invite_defaut;
  char* ligne;
  char BUFFER_READ[10000];
  int POSITION = 0;
  sexpr val;
  int res;
  jmp_buf *buf  = jump_buffer();
  sexpr envg = environnement_global();

  using_history();
                                                                       /********/
  while (1) { /* REPL= Read-Eval-Print LOOP */                         /* LOOP */
                                                                       /********/

      ligne = readline(invite);
      if (ligne == NULL) break;
      if (ligne[0] == '\0')  continue ;


      if (!setjmp(*buf)) { /* En cas d’erreur, le code reprend à partir du else */

          /* On gère les directives */
            if (ligne[0] == '@') {
                int numero_tp = 2;
                add_history(ligne);
                lire_directive(ligne, numero_tp);
                free(ligne);
                continue;
            }

            /* La ligne est ajoutée à un buffer pour permettre des
             * expressions sur plusieurs lignes. Un expression
             * incopmlète renvoie le code -2 et la prochaine ligne lue
             * sera alors ajoutée à la suite du buffer à l’indice
             * POSITION qui n’aura pas été réinitialisé */

          POSITION = ajout_buffer(BUFFER_READ, POSITION, ligne);
          free(ligne);
                                                                       /********/
          res = valisp_read(BUFFER_READ, &val);                        /* READ */
                                                                       /********/

	  /* L’expression est vide, on ignore et on continue */
          if (res == -1)  {
              continue;
          }

          /* L’expression n’est pas finie, on refait un tour pour la finir */
          if (res == -2)  {
              invite = "";
              continue;
          }

          /* Il n'y pas d’autre code d’erreur négatif possible */
          if (res < 0) {
              char msg[100];
              sprintf(msg, "erreur du parseur invalide [%d]\n", res);
              ERREUR_FATALE(msg);
          }

          /* On sauvegarde l’entrée dans l’historique et on
           * réinitialise le tout */
          supprime_retour_ligne_finale_buffer(BUFFER_READ);
          add_history(BUFFER_READ);
          invite = invite_defaut;
          POSITION=0;

          if (res > 0) {
	      erreur(SYNTAXE, "parseur vaλisp", "Pas plus d’une sexpr par ligne", NULL);
          }


          /* On a réussi à lire l’expression, on peut l’évaluer */

	  /* Tout ce que valisp affichera sera en bleu*/
          printf("%s", couleur_bleu);                                   /********/
          val = eval(val,environnement_global());                       /* EVAL */
          printf("%s", couleur_defaut);                                 /********/

	  /* Et la valeur de retour sera en vert */
          printf("%s", couleur_vert);                                   /*********/
          afficher(val);                                                /* PRINT */
          printf("%s", couleur_defaut);                                 /*********/


      } else {
          /* Si on rencontre une « exception » */
          POSITION=0;
          printf("%s", couleur_rouge);
          afficher_erreur();
          printf("%s", couleur_defaut);
      }

      printf("\n\n");
      valisp_ramasse_miettes(envg);
  }

  return 0;
}




struct option long_options[] = {
  {"debug", no_argument, 0, 'd'},
  {"help", no_argument, 0, 'h'},
  {"load", required_argument, 0, 'l'},
  {"script", required_argument, 0, 's'},
  { NULL, 0, NULL, 0 }
};



char aide_valisp[] = "Usage %s [OPTIONS]\n\
\t-l, --load <fichier> (plus tard)\n\
\t-s, --script <fichier> (plus tard)\n\
\t-n, --no-stdlib (plus tard)\n\
\t-h, --help\n\
\t-d, --debug (plus tard)\n";



void afficher_banniere(void) {
    printf("VAΛISP V.0.0.2\n\n");
/****************************************/
/*                                      */
/*  Ok je me suis un peu fait plaisir   */
/*                                      */
/****************************************/

  printf("%s", couleur_vert);
  printf("           %s__%s                    \n", couleur_jaune, couleur_vert);
  printf("           %s\\ \\%s   _             \n", couleur_jaune, couleur_vert);
  printf("__   ____ _ %s\\ \\%s (_)___ _ __    \n", couleur_jaune, couleur_vert);
  printf("\\ \\ / / _` |%s/  \\%s| / __| '_ \\ \n", couleur_jaune, couleur_vert);
  printf(" \\ V / (_| %s/ /\\%s | \\__ \\ |_) |\n", couleur_jaune, couleur_vert);
  printf("  \\_/ \\__,%s/_/  \\%s|_|___/ .__/  \n", couleur_jaune, couleur_vert);
  printf("                      |_|       \n");
  printf("                                 \n");
  printf("%s", couleur_defaut);
}

int valisp_main(int argc, char *argv[]) {

    int c;
    int option_index = 0;

    while ((c = getopt_long(argc, argv, "l:h", long_options, &option_index)) != -1) {
        switch (c) {
        case 'h':
            printf(aide_valisp, argv[0]);
            exit(EXIT_SUCCESS);
        default: /* '?' */
            fprintf(stderr, aide_valisp, argv[0]);
            exit(EXIT_FAILURE);
        }
    }


    afficher_banniere();

    printf("Initialisation mémoire");
    initialiser_memoire();
    printf(" [OK]\n");

    printf("Chargement des primitives");
    charger_primitives();
    printf(" [OK]\n");


    printf("\n");

    printf("Pour obtenir de l’aide sur les directives : %s@aide%s\n\n",
	   couleur_bleu,
	   couleur_defaut);
    c = repl();
    printf("À bientôt\n");

    return c;
}

