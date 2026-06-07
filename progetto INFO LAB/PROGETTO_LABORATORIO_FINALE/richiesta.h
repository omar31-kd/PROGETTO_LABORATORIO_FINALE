/**
 * @file richiesta.h
 * @brief Definizione della struttura Richiesta e delle relative funzioni
 *        per la gestione delle richieste di assistenza tecnica.
 */

#ifndef RICHIESTA_H
#define RICHIESTA_H

#include <time.h>

/* =========================================================================
 * Costanti simboliche
 * ========================================================================= */

/** @brief Lunghezza massima del codice identificativo. */
#define MAX_CODICE       16

/** @brief Lunghezza massima del nome cliente. */
#define MAX_CLIENTE      64

/** @brief Lunghezza massima della descrizione del dispositivo. */
#define MAX_DISPOSITIVO  64

/** @brief Lunghezza massima della descrizione del problema. */
#define MAX_PROBLEMA    256

/* =========================================================================
 * Enumerazioni
 * ========================================================================= */

/**
 * @brief Livello di priorità assegnato alla richiesta.
 */
typedef enum {
    PRIORITA_BASSA   = 0, /**< Bassa priorità. */
    PRIORITA_MEDIA   = 1, /**< Media priorità. */
    PRIORITA_ALTA    = 2, /**< Alta priorità. */
    PRIORITA_URGENTE = 3  /**< Priorità urgente. */
} Priorita;

/**
 * @brief Stato corrente del ciclo di vita della richiesta.
 */
typedef enum {
    STATO_APERTA      = 0, /**< Richiesta aperta, in attesa di presa in carico. */
    STATO_ASSEGNATA   = 1, /**< Richiesta assegnata a un tecnico. */
    STATO_IN_CORSO    = 2, /**< Lavorazione in corso. */
    STATO_SOSPESA     = 3, /**< Richiesta temporaneamente sospesa. */
    STATO_CHIUSA      = 4, /**< Richiesta chiusa con successo. */
    STATO_ANNULLATA   = 5  /**< Richiesta annullata. */
} StatoRichiesta;

/* =========================================================================
 * Struttura principale
 * ========================================================================= */

/**
 * @brief Struttura che rappresenta una richiesta di assistenza tecnica.
 *
 * Raccoglie tutte le informazioni relative a un intervento registrato
 * nel sistema del centro di assistenza.
 */
typedef struct {
    char          codice[MAX_CODICE];       /**< Codice univoco della richiesta. */
    char          cliente[MAX_CLIENTE];     /**< Nome o ragione sociale del cliente. */
    char          dispositivo[MAX_DISPOSITIVO]; /**< Tipo e modello del dispositivo. */
    char          problema[MAX_PROBLEMA];   /**< Descrizione dettagliata del problema. */
    Priorita      priorita;                 /**< Livello di priorità della richiesta. */
    StatoRichiesta stato;                   /**< Stato corrente della richiesta. */
    double        costo_stimato;            /**< Preventivo iniziale in euro. */
    double        costo_finale;             /**< Importo definitivo a consuntivo. */
    time_t        data_apertura;            /**< Timestamp di apertura della richiesta. */
} Richiesta;

/* =========================================================================
 * Costruttore e distruttore
 * ========================================================================= */

/**
 * @brief Crea e inizializza dinamicamente una nuova richiesta.
 *
 * Alloca memoria per una struttura Richiesta, copia i valori forniti
 * nei campi corrispondenti e imposta la data di apertura all'istante
 * corrente.
 *
 * @param codice      Codice univoco della richiesta (stringa terminata da '\\0').
 * @param cliente     Nome del cliente.
 * @param dispositivo Descrizione del dispositivo.
 * @param problema    Descrizione del problema riscontrato.
 * @param priorita    Livello di priorità iniziale.
 * @param costo_stimato Preventivo iniziale in euro (>= 0.0).
 * @return Puntatore alla struttura allocata, oppure @c NULL in caso di
 *         errore di allocazione.
 */
Richiesta *richiesta_crea(const char *codice,
                           const char *cliente,
                           const char *dispositivo,
                           const char *problema,
                           Priorita    priorita,
                           double      costo_stimato);

/**
 * @brief Dealloca la memoria occupata da una richiesta.
 *
 * Libera la memoria puntata da @p r e imposta il puntatore a @c NULL
 * per prevenire l'uso dopo la liberazione.
 *
 * @param r Puntatore al puntatore della richiesta da distruggere.
 *          Se @p *r è @c NULL, la funzione non ha effetto.
 */
void richiesta_distruggi(Richiesta **r);

/* =========================================================================
 * Getter
 * ========================================================================= */

/**
 * @brief Restituisce il codice univoco della richiesta.
 * @param r Puntatore alla richiesta (non @c NULL).
 * @return Puntatore alla stringa contenente il codice.
 */
const char *richiesta_get_codice(const Richiesta *r);

/**
 * @brief Restituisce il nome del cliente associato alla richiesta.
 * @param r Puntatore alla richiesta (non @c NULL).
 * @return Puntatore alla stringa contenente il nome del cliente.
 */
const char *richiesta_get_cliente(const Richiesta *r);

/**
 * @brief Restituisce la descrizione del dispositivo.
 * @param r Puntatore alla richiesta (non @c NULL).
 * @return Puntatore alla stringa contenente il dispositivo.
 */
const char *richiesta_get_dispositivo(const Richiesta *r);

/**
 * @brief Restituisce la descrizione del problema.
 * @param r Puntatore alla richiesta (non @c NULL).
 * @return Puntatore alla stringa contenente il problema.
 */
const char *richiesta_get_problema(const Richiesta *r);

/**
 * @brief Restituisce il livello di priorità della richiesta.
 * @param r Puntatore alla richiesta (non @c NULL).
 * @return Valore dell'enumerazione @c Priorita.
 */
Priorita richiesta_get_priorita(const Richiesta *r);

/**
 * @brief Restituisce lo stato corrente della richiesta.
 * @param r Puntatore alla richiesta (non @c NULL).
 * @return Valore dell'enumerazione @c StatoRichiesta.
 */
StatoRichiesta richiesta_get_stato(const Richiesta *r);

/**
 * @brief Restituisce il costo stimato (preventivo) della richiesta.
 * @param r Puntatore alla richiesta (non @c NULL).
 * @return Costo stimato in euro.
 */
double richiesta_get_costo_stimato(const Richiesta *r);

/**
 * @brief Restituisce il costo finale (consuntivo) della richiesta.
 * @param r Puntatore alla richiesta (non @c NULL).
 * @return Costo finale in euro.
 */
double richiesta_get_costo_finale(const Richiesta *r);

/**
 * @brief Restituisce la data e l'ora di apertura della richiesta.
 * @param r Puntatore alla richiesta (non @c NULL).
 * @return Timestamp UNIX della data di apertura.
 */
time_t richiesta_get_data_apertura(const Richiesta *r);

/* =========================================================================
 * Setter
 * ========================================================================= */

/**
 * @brief Imposta il codice univoco della richiesta.
 * @param r      Puntatore alla richiesta (non @c NULL).
 * @param codice Nuova stringa codice (non @c NULL, lunghezza < @c MAX_CODICE).
 */
void richiesta_set_codice(Richiesta *r, const char *codice);

/**
 * @brief Imposta il nome del cliente.
 * @param r       Puntatore alla richiesta (non @c NULL).
 * @param cliente Nuovo nome del cliente (non @c NULL).
 */
void richiesta_set_cliente(Richiesta *r, const char *cliente);

/**
 * @brief Imposta la descrizione del dispositivo.
 * @param r           Puntatore alla richiesta (non @c NULL).
 * @param dispositivo Nuova descrizione del dispositivo (non @c NULL).
 */
void richiesta_set_dispositivo(Richiesta *r, const char *dispositivo);

/**
 * @brief Imposta la descrizione del problema.
 * @param r        Puntatore alla richiesta (non @c NULL).
 * @param problema Nuova descrizione del problema (non @c NULL).
 */
void richiesta_set_problema(Richiesta *r, const char *problema);

/**
 * @brief Imposta il livello di priorità della richiesta.
 * @param r        Puntatore alla richiesta (non @c NULL).
 * @param priorita Nuovo valore di priorità.
 */
void richiesta_set_priorita(Richiesta *r, Priorita priorita);

/**
 * @brief Imposta lo stato corrente della richiesta.
 * @param r     Puntatore alla richiesta (non @c NULL).
 * @param stato Nuovo stato della richiesta.
 */
void richiesta_set_stato(Richiesta *r, StatoRichiesta stato);

/**
 * @brief Imposta il costo stimato (preventivo) della richiesta.
 * @param r             Puntatore alla richiesta (non @c NULL).
 * @param costo_stimato Nuovo valore del preventivo in euro (>= 0.0).
 */
void richiesta_set_costo_stimato(Richiesta *r, double costo_stimato);

/**
 * @brief Imposta il costo finale (consuntivo) della richiesta.
 * @param r            Puntatore alla richiesta (non @c NULL).
 * @param costo_finale Nuovo valore del consuntivo in euro (>= 0.0).
 */
void richiesta_set_costo_finale(Richiesta *r, double costo_finale);

/**
 * @brief Imposta la data di apertura della richiesta.
 * @param r             Puntatore alla richiesta (non @c NULL).
 * @param data_apertura Timestamp UNIX della data di apertura.
 */
void richiesta_set_data_apertura(Richiesta *r, time_t data_apertura);

#endif /* RICHIESTA_H */
