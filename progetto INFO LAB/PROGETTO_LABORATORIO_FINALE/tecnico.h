/**
 * @file tecnico.h
 * @brief Definizione della struttura Tecnico e delle relative funzioni
 *        per la gestione del personale tecnico del centro di assistenza.
 */

#ifndef TECNICO_H
#define TECNICO_H

#include "richiesta.h"

/* =========================================================================
 * Costanti simboliche
 * ========================================================================= */

/** @brief Lunghezza massima del codice identificativo del tecnico. */
#define MAX_CODICE_TECNICO       16

/** @brief Lunghezza massima del nome e cognome del tecnico. */
#define MAX_NOME_TECNICO         64

/** @brief Lunghezza massima della descrizione della specializzazione. */
#define MAX_SPECIALIZZAZIONE     64

/** @brief Numero massimo assoluto di richieste assegnabili a un tecnico. */
#define MAX_RICHIESTE_ASSOLUTO  128

/* =========================================================================
 * Struttura principale
 * ========================================================================= */

/**
 * @brief Struttura che rappresenta un tecnico del centro di assistenza.
 *
 * Contiene le informazioni anagrafiche e operative del tecnico,
 * nonché l'elenco delle richieste a lui correntemente assegnate.
 */
typedef struct {
    char       codice[MAX_CODICE_TECNICO];         /**< Codice univoco del tecnico. */
    char       nome[MAX_NOME_TECNICO];             /**< Nome e cognome del tecnico. */
    char       specializzazione[MAX_SPECIALIZZAZIONE]; /**< Area di competenza principale. */
    int        max_richieste;                      /**< Numero massimo di richieste gestibili simultaneamente. */
    int        assegnate;                          /**< Numero corrente di richieste assegnate. */
    Richiesta *richieste[MAX_RICHIESTE_ASSOLUTO];  /**< Array di puntatori alle richieste assegnate. */
} Tecnico;

/* =========================================================================
 * Costruttore e distruttore
 * ========================================================================= */

/**
 * @brief Crea e inizializza dinamicamente un nuovo tecnico.
 *
 * Alloca memoria per una struttura Tecnico, copia i valori forniti
 * nei campi corrispondenti e azzera il contatore delle richieste assegnate.
 *
 * @param codice           Codice univoco del tecnico (stringa terminata da '\\0').
 * @param nome             Nome e cognome del tecnico.
 * @param specializzazione Area di competenza del tecnico.
 * @param max_richieste    Carico massimo di richieste assegnabili (>= 1 e
 *                         <= @c MAX_RICHIESTE_ASSOLUTO).
 * @return Puntatore alla struttura allocata, oppure @c NULL in caso di
 *         errore di allocazione o parametri non validi.
 */
Tecnico *tecnico_crea(const char *codice,
                      const char *nome,
                      const char *specializzazione,
                      int         max_richieste);

/**
 * @brief Dealloca la memoria occupata da un tecnico.
 *
 * Libera la memoria puntata da @p t e imposta il puntatore a @c NULL
 * per prevenire l'uso dopo la liberazione. Le richieste puntate
 * dall'array interno NON vengono deallocate (ownership esterna).
 *
 * @param t Puntatore al puntatore del tecnico da distruggere.
 *          Se @p *t è @c NULL, la funzione non ha effetto.
 */
void tecnico_distruggi(Tecnico **t);

/* =========================================================================
 * Getter
 * ========================================================================= */

/**
 * @brief Restituisce il codice univoco del tecnico.
 * @param t Puntatore al tecnico (non @c NULL).
 * @return Puntatore alla stringa contenente il codice.
 */
const char *tecnico_get_codice(const Tecnico *t);

/**
 * @brief Restituisce il nome e cognome del tecnico.
 * @param t Puntatore al tecnico (non @c NULL).
 * @return Puntatore alla stringa contenente il nome.
 */
const char *tecnico_get_nome(const Tecnico *t);

/**
 * @brief Restituisce la specializzazione del tecnico.
 * @param t Puntatore al tecnico (non @c NULL).
 * @return Puntatore alla stringa contenente la specializzazione.
 */
const char *tecnico_get_specializzazione(const Tecnico *t);

/**
 * @brief Restituisce il numero massimo di richieste gestibili.
 * @param t Puntatore al tecnico (non @c NULL).
 * @return Valore intero del carico massimo consentito.
 */
int tecnico_get_max_richieste(const Tecnico *t);

/**
 * @brief Restituisce il numero corrente di richieste assegnate.
 * @param t Puntatore al tecnico (non @c NULL).
 * @return Numero di richieste attualmente in carico al tecnico.
 */
int tecnico_get_assegnate(const Tecnico *t);

/**
 * @brief Restituisce la richiesta in posizione @p indice.
 *
 * @param t      Puntatore al tecnico (non @c NULL).
 * @param indice Indice nell'array delle richieste assegnate
 *               (0 <= @p indice < @c assegnate).
 * @return Puntatore alla richiesta, oppure @c NULL se l'indice è fuori range.
 */
Richiesta *tecnico_get_richiesta(const Tecnico *t, int indice);

/* =========================================================================
 * Setter
 * ========================================================================= */

/**
 * @brief Imposta il codice univoco del tecnico.
 * @param t      Puntatore al tecnico (non @c NULL).
 * @param codice Nuovo codice (non @c NULL, lunghezza < @c MAX_CODICE_TECNICO).
 */
void tecnico_set_codice(Tecnico *t, const char *codice);

/**
 * @brief Imposta il nome e cognome del tecnico.
 * @param t    Puntatore al tecnico (non @c NULL).
 * @param nome Nuovo nome (non @c NULL, lunghezza < @c MAX_NOME_TECNICO).
 */
void tecnico_set_nome(Tecnico *t, const char *nome);

/**
 * @brief Imposta la specializzazione del tecnico.
 * @param t                Puntatore al tecnico (non @c NULL).
 * @param specializzazione Nuova specializzazione (non @c NULL).
 */
void tecnico_set_specializzazione(Tecnico *t, const char *specializzazione);

/**
 * @brief Imposta il numero massimo di richieste gestibili.
 *
 * Il valore deve essere compreso tra 1 e @c MAX_RICHIESTE_ASSOLUTO.
 * Se il nuovo limite è inferiore al numero corrente di richieste
 * assegnate, la funzione non modifica il campo e restituisce @c -1.
 *
 * @param t             Puntatore al tecnico (non @c NULL).
 * @param max_richieste Nuovo limite massimo.
 * @return @c 0 in caso di successo, @c -1 se il valore non è valido.
 */
int tecnico_set_max_richieste(Tecnico *t, int max_richieste);

/**
 * @brief Assegna una nuova richiesta al tecnico.
 *
 * Aggiunge @p r all'array interno e incrementa il contatore
 * @c assegnate, a condizione che il carico massimo non sia già raggiunto.
 *
 * @param t Puntatore al tecnico (non @c NULL).
 * @param r Puntatore alla richiesta da assegnare (non @c NULL).
 * @return @c 0 in caso di successo, @c -1 se il tecnico è al massimo
 *         del carico o il puntatore è @c NULL.
 */
int tecnico_assegna_richiesta(Tecnico *t, Richiesta *r);

/**
 * @brief Rimuove una richiesta dall'elenco del tecnico tramite codice.
 *
 * Cerca nell'array la richiesta il cui codice corrisponde a @p codice,
 * la rimuove compattando l'array e decrementa @c assegnate.
 *
 * @param t      Puntatore al tecnico (non @c NULL).
 * @param codice Codice della richiesta da rimuovere (non @c NULL).
 * @return @c 0 in caso di successo, @c -1 se la richiesta non è stata trovata.
 */
int tecnico_rimuovi_richiesta(Tecnico *t, const char *codice);

#endif /* TECNICO_H */
