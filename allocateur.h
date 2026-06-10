#ifndef ALLOCATEUR_H

void initialiser_memoire_dynamique();

void *allocateur_malloc(size_t size);

void allocateur_free(void *ptr);


int ramasse_miettes_lire_marque(void *ptr);

int ramasse_miettes_poser_marque(void *ptr);

int bloc_libre(int i);

void ramasse_miettes_liberer(void);

#endif
