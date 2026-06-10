#include "types.h"

#ifndef INTERPRETEUR_H
#define INTERPRETEUR_H

sexpr eval(sexpr val, sexpr env);

sexpr apply(sexpr fonction, sexpr liste, sexpr env);

#endif
