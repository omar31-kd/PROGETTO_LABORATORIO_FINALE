/**
 * @file gestione_file.h
 * @brief Interfaccia per il salvataggio e il caricamento su file di testo
 *        delle liste di Richieste e Tecnici del centro di assistenza.
 *
 * Formato del file richieste.txt (un blocco per richiesta):
 * @code
 *   codice
 *   cliente
 *   dispositivo
 *   problema
 *   priorita (intero)
 *   stato    (intero)
 *   costo_stimato
 *   costo_finale
 *   data_apertura (long UNIX timestamp)
 *   ---
 * @endcode
 *
 * Formato del file tecnici.txt (un blocco per tecnico):
 * @code
 *   codice
 *   nome
 *   specializzazione
 *   max_richieste (intero)
 *   n_assegnate   (intero)
 *   codice_richiesta_0
 *   ...
 *   codice_richiesta_n-1
 *   ---
 * @endcode
 */

#ifndef GESTIONE_FILE_H
#define GESTIONE_FILE_H

#include "richiesta.h"
#include "tecnico.h"

/* =========================================================================
 * Costanti di configurazione
 * ========================================================================= */

/** @brief Percorso predefinito del file di testo per le richieste. */
#define FILE_RICHIESTE "richieste.txt"

/** @brief Percorso predefinito del file di testo per i tecnici. */
#define FILE_TECNICI   "tecnici.txt"

/** @brief Capacità massima dell'array di Richieste gestite dal sistema. */
#define MAX_RICHIESTE_SISTEMA 256

/** @brief Capacità massima dell'array di Tecnici gestiti dal sistema. */
#define MAX_TECNICI_SISTEMA    64

/* =========================================================================
 * Funzioni per le Richieste
 * ========================================================================= */

/**
 * @brief Salva l'intera lista di richieste nel file specificato.
 *
 * Apre il file in modalità scrittura (sovrascrivendo il contenuto
 * precedente) e serializza ogni Richiesta non NULL in un blocco di
 * righe terminato dal separatore "---".
 *
 * @param percorso Percorso del file di destinazione (non @c NULL).
 * @param lista    Array di puntatori alle richieste (non @c NULL).
 * @param n        Numero di elementi validi in @p lista (>= 0).
 * @return @c 0 in caso di successo, @c -1 se i parametri sono invalidi
 *         o il file non può essere aperto.
 */
int salva_richieste(const char *percorso, Richiesta **lista, int n);

/**
 * @brief Carica le richieste dal file specificato.
 *
 * Legge il file riga per riga con @c fgets, ricostruisce le struct
 * Richiesta tramite @c richiesta_crea e le inserisce nell'array @p lista.
 * La lettura si interrompe quando si raggiunge la fine del file (@c feof)
 * o quando @p lista è piena.
 *
 * @param percorso Percorso del file sorgente (non @c NULL).
 * @param lista    Array di puntatori dove memorizzare le richieste lette
 *                 (non @c NULL, capacità >= @p max).
 * @param n        Puntatore al contatore degli elementi caricati (non @c NULL).
 *                 Viene azzerato prima della lettura.
 * @param max      Numero massimo di richieste da caricare.
 * @return @c 0 in caso di successo, @c -1 se i parametri sono invalidi
 *         o il file non può essere aperto.
 */
int carica_richieste(const char *percorso, Richiesta **lista, int *n, int max);

/* =========================================================================
 * Funzioni per i Tecnici
 * ========================================================================= */

/**
 * @brief Salva l'intera lista di tecnici nel file specificato.
 *
 * Per ciascun tecnico vengono salvati i campi anagrafici e, di seguito,
 * i codici delle richieste attualmente assegnate. Il blocco è terminato
 * dal separatore "---".
 *
 * @param percorso Percorso del file di destinazione (non @c NULL).
 * @param lista    Array di puntatori ai tecnici (non @c NULL).
 * @param n        Numero di elementi validi in @p lista (>= 0).
 * @return @c 0 in caso di successo, @c -1 se i parametri sono invalidi
 *         o il file non può essere aperto.
 */
int salva_tecnici(const char *percorso, Tecnico **lista, int n);

/**
 * @brief Carica i tecnici dal file specificato e ricollegali alle richieste.
 *
 * Legge il file riga per riga con @c fgets. Per ogni blocco ricostruisce
 * un Tecnico tramite @c tecnico_crea e, per ciascun codice di richiesta
 * elencato nel blocco, cerca il corrispondente puntatore nell'array
 * @p richieste (già caricato in precedenza) e lo assegna con
 * @c tecnico_assegna_richiesta.
 *
 * @param percorso     Percorso del file sorgente (non @c NULL).
 * @param lista        Array di puntatori dove memorizzare i tecnici letti
 *                     (non @c NULL, capacità >= @p max).
 * @param n            Puntatore al contatore degli elementi caricati (non @c NULL).
 *                     Viene azzerato prima della lettura.
 * @param max          Numero massimo di tecnici da caricare.
 * @param richieste    Array di puntatori alle richieste già in memoria,
 *                     usato per ricollegare i puntatori interni al tecnico.
 * @param n_richieste  Numero di elementi validi in @p richieste.
 * @return @c 0 in caso di successo, @c -1 se i parametri sono invalidi
 *         o il file non può essere aperto.
 */
int carica_tecnici(const char *percorso,
                   Tecnico   **lista,
                   int        *n,
                   int         max,
                   Richiesta **richieste,
                   int         n_richieste);

#endif /* GESTIONE_FILE_H */
