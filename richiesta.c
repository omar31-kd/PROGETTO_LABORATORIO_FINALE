/**
 * @file richiesta.c
 * @brief Implementazione delle funzioni dichiarate in richiesta.h.
 */

#include "richiesta.h"

#include <stdlib.h>
#include <time.h>

/* =========================================================================
 * Funzioni di supporto interne (static)
 * ========================================================================= */

/**
 * @brief Copia al massimo @p max_car caratteri da @p src in @p dst,
 *        garantendo la terminazione con '\\0'.
 *
 * Equivale a un sottoinsieme sicuro di strncpy, implementato con un ciclo
 * elementare per rispettare il vincolo di non usare funzioni di libreria
 * per la manipolazione di stringhe.
 *
 * @param dst     Buffer di destinazione.
 * @param src     Stringa sorgente.
 * @param max_car Dimensione massima del buffer (incluso il terminatore).
 */
static void copia_stringa(char *dst, const char *src, int max_car) {
  int i;

  if (dst == NULL || src == NULL || max_car <= 0) {
    return;
  }

  for (i = 0; i < max_car - 1 && src[i] != '\0'; i++) {
    dst[i] = src[i];
  }
  dst[i] = '\0';
}

/**
 * @brief Confronta due stringhe carattere per carattere.
 *
 * @param a Prima stringa.
 * @param b Seconda stringa.
 * @return 0 se le stringhe sono uguali, valore diverso altrimenti.
 */
static int confronta_stringhe(const char *a, const char *b) {
  int i;

  if (a == NULL || b == NULL) {
    return -1;
  }

  for (i = 0; a[i] != '\0' && b[i] != '\0'; i++) {
    if (a[i] != b[i]) {
      return (int)(a[i]) - (int)(b[i]);
    }
  }
  return (int)(a[i]) - (int)(b[i]);
}

/* =========================================================================
 * Costruttore e distruttore
 * ========================================================================= */

Richiesta *richiesta_crea(const char *codice, const char *cliente,
                          const char *dispositivo, const char *problema,
                          Priorita priorita, double costo_stimato) {
  Richiesta *r;

  if (codice == NULL || cliente == NULL || dispositivo == NULL ||
      problema == NULL) {
    return NULL;
  }

  r = (Richiesta *)calloc(1, sizeof(Richiesta));
  if (r == NULL) {
    return NULL;
  }

  copia_stringa(r->codice, codice, MAX_CODICE);
  copia_stringa(r->cliente, cliente, MAX_CLIENTE);
  copia_stringa(r->dispositivo, dispositivo, MAX_DISPOSITIVO);
  copia_stringa(r->problema, problema, MAX_PROBLEMA);

  r->priorita = priorita;
  r->stato = STATO_APERTA;
  r->costo_stimato = (costo_stimato >= 0.0) ? costo_stimato : 0.0;
  r->costo_finale = 0.0;
  r->data_apertura = time(NULL);

  return r;
}

void richiesta_distruggi(Richiesta **r) {
  if (r == NULL || *r == NULL) {
    return;
  }
  free(*r);
  *r = NULL;
}

/* =========================================================================
 * Getter
 * ========================================================================= */

const char *richiesta_get_codice(const Richiesta *r) {
  if (r == NULL) {
    return NULL;
  }
  return r->codice;
}

const char *richiesta_get_cliente(const Richiesta *r) {
  if (r == NULL) {
    return NULL;
  }
  return r->cliente;
}

const char *richiesta_get_dispositivo(const Richiesta *r) {
  if (r == NULL) {
    return NULL;
  }
  return r->dispositivo;
}

const char *richiesta_get_problema(const Richiesta *r) {
  if (r == NULL) {
    return NULL;
  }
  return r->problema;
}

Priorita richiesta_get_priorita(const Richiesta *r) {
  if (r == NULL) {
    return PRIORITA_BASSA;
  }
  return r->priorita;
}

StatoRichiesta richiesta_get_stato(const Richiesta *r) {
  if (r == NULL) {
    return STATO_APERTA;
  }
  return r->stato;
}

double richiesta_get_costo_stimato(const Richiesta *r) {
  if (r == NULL) {
    return 0.0;
  }
  return r->costo_stimato;
}

double richiesta_get_costo_finale(const Richiesta *r) {
  if (r == NULL) {
    return 0.0;
  }
  return r->costo_finale;
}

time_t richiesta_get_data_apertura(const Richiesta *r) {
  if (r == NULL) {
    return (time_t)(-1);
  }
  return r->data_apertura;
}

/* =========================================================================
 * Setter
 * ========================================================================= */

void richiesta_set_codice(Richiesta *r, const char *codice) {
  if (r == NULL || codice == NULL) {
    return;
  }
  copia_stringa(r->codice, codice, MAX_CODICE);
}

void richiesta_set_cliente(Richiesta *r, const char *cliente) {
  if (r == NULL || cliente == NULL) {
    return;
  }
  copia_stringa(r->cliente, cliente, MAX_CLIENTE);
}

void richiesta_set_dispositivo(Richiesta *r, const char *dispositivo) {
  if (r == NULL || dispositivo == NULL) {
    return;
  }
  copia_stringa(r->dispositivo, dispositivo, MAX_DISPOSITIVO);
}

void richiesta_set_problema(Richiesta *r, const char *problema) {
  if (r == NULL || problema == NULL) {
    return;
  }
  copia_stringa(r->problema, problema, MAX_PROBLEMA);
}

void richiesta_set_priorita(Richiesta *r, Priorita priorita) {
  if (r == NULL) {
    return;
  }
  r->priorita = priorita;
}

void richiesta_set_stato(Richiesta *r, StatoRichiesta stato) {
  if (r == NULL) {
    return;
  }
  r->stato = stato;
}

void richiesta_set_costo_stimato(Richiesta *r, double costo_stimato) {
  if (r == NULL || costo_stimato < 0.0) {
    return;
  }
  r->costo_stimato = costo_stimato;
}

void richiesta_set_costo_finale(Richiesta *r, double costo_finale) {
  if (r == NULL || costo_finale < 0.0) {
    return;
  }
  r->costo_finale = costo_finale;
}

void richiesta_set_data_apertura(Richiesta *r, time_t data_apertura) {
  if (r == NULL) {
    return;
  }
  r->data_apertura = data_apertura;
}

/* Funzione interna esposta solo a tecnico.c tramite linkage — uso interno. */
int richiesta_confronta_codice(const Richiesta *r, const char *codice) {
  if (r == NULL || codice == NULL) {
    return -1;
  }
  return confronta_stringhe(r->codice, codice);
}
