/**
 * @file tecnico.c
 * @brief Implementazione delle funzioni dichiarate in tecnico.h.
 */

#include "tecnico.h"

#include <stdlib.h>

/* =========================================================================
 * Funzioni di supporto interne (static)
 * ========================================================================= */

/**
 * @brief Copia al massimo @p max_car caratteri da @p src in @p dst,
 *        garantendo la terminazione con '\\0'.
 *
 * @param dst     Buffer di destinazione.
 * @param src     Stringa sorgente.
 * @param max_car Dimensione massima del buffer (incluso il terminatore).
 */
static void copia_stringa(char *dst, const char *src, int max_car)
{
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
 * @return 0 se uguali, valore diverso da 0 altrimenti.
 */
static int confronta_stringhe(const char *a, const char *b)
{
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

/**
 * @brief Cerca nell'array del tecnico la posizione della richiesta
 *        con il codice specificato.
 *
 * @param t      Puntatore al tecnico.
 * @param codice Codice da ricercare.
 * @return Indice nell'array, oppure -1 se non trovato.
 */
static int cerca_indice_richiesta(const Tecnico *t, const char *codice)
{
    int i;

    if (t == NULL || codice == NULL) {
        return -1;
    }

    for (i = 0; i < t->assegnate; i++) {
        if (t->richieste[i] != NULL &&
            confronta_stringhe(richiesta_get_codice(t->richieste[i]), codice) == 0) {
            return i;
        }
    }
    return -1;
}

/* =========================================================================
 * Costruttore e distruttore
 * ========================================================================= */

Tecnico *tecnico_crea(const char *codice,
                      const char *nome,
                      const char *specializzazione,
                      int         max_richieste)
{
    Tecnico *t;

    if (codice == NULL || nome == NULL || specializzazione == NULL) {
        return NULL;
    }

    /* Rifiuta stringhe vuote */
    if (codice[0] == '\0' || nome[0] == '\0' || specializzazione[0] == '\0') {
        return NULL;
    }

    if (max_richieste < 1 || max_richieste > MAX_RICHIESTE_ASSOLUTO) {
        return NULL;
    }

    t = (Tecnico *)calloc(1, sizeof(Tecnico));
    if (t == NULL) {
        return NULL;
    }

    copia_stringa(t->codice,           codice,           MAX_CODICE_TECNICO);
    copia_stringa(t->nome,             nome,             MAX_NOME_TECNICO);
    copia_stringa(t->specializzazione, specializzazione, MAX_SPECIALIZZAZIONE);

    t->max_richieste = max_richieste;
    t->assegnate     = 0;

    return t;
}

void tecnico_distruggi(Tecnico **t)
{
    if (t == NULL || *t == NULL) {
        return;
    }
    free(*t);
    *t = NULL;
}

/* =========================================================================
 * Getter
 * ========================================================================= */

const char *tecnico_get_codice(const Tecnico *t)
{
    if (t == NULL) {
        return NULL;
    }
    return t->codice;
}

const char *tecnico_get_nome(const Tecnico *t)
{
    if (t == NULL) {
        return NULL;
    }
    return t->nome;
}

const char *tecnico_get_specializzazione(const Tecnico *t)
{
    if (t == NULL) {
        return NULL;
    }
    return t->specializzazione;
}

int tecnico_get_max_richieste(const Tecnico *t)
{
    if (t == NULL) {
        return 0;
    }
    return t->max_richieste;
}

int tecnico_get_assegnate(const Tecnico *t)
{
    if (t == NULL) {
        return 0;
    }
    return t->assegnate;
}

Richiesta *tecnico_get_richiesta(const Tecnico *t, int indice)
{
    if (t == NULL || indice < 0 || indice >= t->assegnate) {
        return NULL;
    }
    return t->richieste[indice];
}

/* =========================================================================
 * Setter
 * ========================================================================= */

void tecnico_set_codice(Tecnico *t, const char *codice)
{
    if (t == NULL || codice == NULL) {
        return;
    }
    copia_stringa(t->codice, codice, MAX_CODICE_TECNICO);
}

void tecnico_set_nome(Tecnico *t, const char *nome)
{
    if (t == NULL || nome == NULL) {
        return;
    }
    copia_stringa(t->nome, nome, MAX_NOME_TECNICO);
}

void tecnico_set_specializzazione(Tecnico *t, const char *specializzazione)
{
    if (t == NULL || specializzazione == NULL) {
        return;
    }
    copia_stringa(t->specializzazione, specializzazione, MAX_SPECIALIZZAZIONE);
}

int tecnico_set_max_richieste(Tecnico *t, int max_richieste)
{
    if (t == NULL) {
        return -1;
    }
    if (max_richieste < 1 || max_richieste > MAX_RICHIESTE_ASSOLUTO) {
        return -1;
    }
    if (max_richieste < t->assegnate) {
        return -1;
    }
    t->max_richieste = max_richieste;
    return 0;
}

int tecnico_assegna_richiesta(Tecnico *t, Richiesta *r)
{
    int i;

    if (t == NULL || r == NULL) {
        return -1;
    }
    if (t->assegnate >= t->max_richieste) {
        return -1;
    }

    /* Verifica duplicato: la stessa richiesta non puo' essere assegnata due volte */
    for (i = 0; i < t->assegnate; i++) {
        if (t->richieste[i] == r) {
            return -1;
        }
    }

    t->richieste[t->assegnate] = r;
    t->assegnate++;
    return 0;
}

int tecnico_rimuovi_richiesta(Tecnico *t, const char *codice)
{
    int indice;
    int i;

    if (t == NULL || codice == NULL) {
        return -1;
    }

    indice = cerca_indice_richiesta(t, codice);
    if (indice == -1) {
        return -1;
    }

    for (i = indice; i < t->assegnate - 1; i++) {
        t->richieste[i] = t->richieste[i + 1];
    }
    t->richieste[t->assegnate - 1] = NULL;
    t->assegnate--;

    return 0;
}
