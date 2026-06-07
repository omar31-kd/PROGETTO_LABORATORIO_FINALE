/**
 * @file logica.h
 * @brief Dichiarazioni delle funzioni di logica applicativa del centro di
 *        assistenza: visualizzazione, inserimento, ricerca, modifica,
 *        cancellazione, ordinamento, reportistica e generazione dashboard.
 */

#ifndef LOGICA_H
#define LOGICA_H

#include "richiesta.h"
#include "tecnico.h"

/* =========================================================================
 * Costanti
 * ========================================================================= */

/** @brief Criterio di ordinamento per priorità (decrescente). */
#define ORDINA_PER_PRIORITA 0

/** @brief Criterio di ordinamento per data di apertura (crescente). */
#define ORDINA_PER_DATA     1

/** @brief Nome del file della dashboard HTML generata. */
#define FILE_DASHBOARD      "dashboard.html"

/** @brief Nome del file del report ordinato testuale. */
#define FILE_REPORT_ORDINATO "report_ordinato.txt"

/* =========================================================================
 * Visualizzazione
 * ========================================================================= */

/**
 * @brief Stampa a terminale i dettagli di una singola richiesta.
 * @param r Puntatore alla richiesta (non @c NULL).
 */
void visualizza_richiesta(const Richiesta *r);

/**
 * @brief Stampa a terminale l'elenco di tutte le richieste.
 * @param lista Array di puntatori a Richiesta.
 * @param n     Numero di elementi validi.
 */
void visualizza_richieste(Richiesta **lista, int n);

/**
 * @brief Stampa a terminale i dettagli di un singolo tecnico.
 * @param t Puntatore al tecnico (non @c NULL).
 */
void visualizza_tecnico(const Tecnico *t);

/**
 * @brief Stampa a terminale l'elenco di tutti i tecnici.
 * @param lista Array di puntatori a Tecnico.
 * @param n     Numero di elementi validi.
 */
void visualizza_tecnici(Tecnico **lista, int n);

/* =========================================================================
 * Inserimento
 * ========================================================================= */

/**
 * @brief Inserisce una nuova richiesta nell'array.
 *
 * @param lista       Array di puntatori a Richiesta.
 * @param n           Puntatore al contatore (aggiornato in caso di successo).
 * @param max         Capacità massima dell'array.
 * @param codice      Codice univoco della richiesta.
 * @param cliente     Nome del cliente.
 * @param dispositivo Descrizione del dispositivo.
 * @param problema    Descrizione del problema.
 * @param priorita    Livello di priorità.
 * @param costo_stimato Preventivo in euro.
 * @return @c 0 in caso di successo, @c -1 in caso di errore.
 */
int inserisci_richiesta(Richiesta **lista, int *n, int max,
                        const char *codice, const char *cliente,
                        const char *dispositivo, const char *problema,
                        Priorita priorita, double costo_stimato);

/**
 * @brief Inserisce un nuovo tecnico nell'array.
 *
 * @param lista           Array di puntatori a Tecnico.
 * @param n               Puntatore al contatore (aggiornato in caso di successo).
 * @param max             Capacità massima dell'array.
 * @param codice          Codice univoco del tecnico.
 * @param nome            Nome e cognome del tecnico.
 * @param specializzazione Area di competenza.
 * @param max_richieste   Carico massimo di richieste.
 * @return @c 0 in caso di successo, @c -1 in caso di errore.
 */
int inserisci_tecnico(Tecnico **lista, int *n, int max,
                      const char *codice, const char *nome,
                      const char *specializzazione, int max_richieste);

/* =========================================================================
 * Ricerca
 * ========================================================================= */

/**
 * @brief Cerca una richiesta per codice nell'array.
 * @param lista Array di puntatori a Richiesta.
 * @param n     Numero di elementi validi.
 * @param codice Codice da cercare.
 * @return Puntatore alla richiesta trovata, oppure @c NULL.
 */
Richiesta *cerca_richiesta(Richiesta **lista, int n, const char *codice);

/**
 * @brief Cerca un tecnico per codice nell'array.
 * @param lista Array di puntatori a Tecnico.
 * @param n     Numero di elementi validi.
 * @param codice Codice da cercare.
 * @return Puntatore al tecnico trovato, oppure @c NULL.
 */
Tecnico *cerca_tecnico(Tecnico **lista, int n, const char *codice);

/* =========================================================================
 * Modifica
 * ========================================================================= */

/**
 * @brief Modifica lo stato di una richiesta individuata per codice.
 * @param lista Array di puntatori a Richiesta.
 * @param n     Numero di elementi validi.
 * @param codice Codice della richiesta da modificare.
 * @param stato  Nuovo stato.
 * @return @c 0 in caso di successo, @c -1 se non trovata.
 */
int modifica_stato_richiesta(Richiesta **lista, int n,
                             const char *codice, StatoRichiesta stato);

/**
 * @brief Modifica la descrizione del problema di una richiesta.
 * @param lista    Array di puntatori a Richiesta.
 * @param n        Numero di elementi validi.
 * @param codice   Codice della richiesta da modificare.
 * @param problema Nuova descrizione del problema.
 * @return @c 0 in caso di successo, @c -1 se non trovata.
 */
int modifica_problema_richiesta(Richiesta **lista, int n,
                                const char *codice, const char *problema);

/**
 * @brief Modifica il costo stimato di una richiesta.
 * @param lista        Array di puntatori a Richiesta.
 * @param n            Numero di elementi validi.
 * @param codice       Codice della richiesta da modificare.
 * @param costo_stimato Nuovo valore del preventivo.
 * @return @c 0 in caso di successo, @c -1 se non trovata o valore negativo.
 */
int modifica_costo_stimato_richiesta(Richiesta **lista, int n,
                                     const char *codice, double costo_stimato);

/**
 * @brief Modifica il costo finale di una richiesta.
 * @param lista       Array di puntatori a Richiesta.
 * @param n           Numero di elementi validi.
 * @param codice      Codice della richiesta da modificare.
 * @param costo_finale Nuovo valore del consuntivo.
 * @return @c 0 in caso di successo, @c -1 se non trovata o valore negativo.
 */
int modifica_costo_finale_richiesta(Richiesta **lista, int n,
                                    const char *codice, double costo_finale);

/* =========================================================================
 * Cancellazione
 * ========================================================================= */

/**
 * @brief Cancella una richiesta dall'array, liberando la memoria.
 *
 * Cerca la richiesta per codice, la dealloca, compatta l'array e
 * decrementa il contatore.
 *
 * @param lista  Array di puntatori a Richiesta.
 * @param n      Puntatore al contatore (aggiornato in caso di successo).
 * @param codice Codice della richiesta da cancellare.
 * @return @c 0 in caso di successo, @c -1 se non trovata.
 */
int cancella_richiesta(Richiesta **lista, int *n, const char *codice);

/* =========================================================================
 * Ordinamento
 * ========================================================================= */

/**
 * @brief Ordina l'array di richieste con Merge Sort.
 *
 * @param lista    Array di puntatori a Richiesta.
 * @param n        Numero di elementi validi.
 * @param criterio Criterio di ordinamento:
 *                 @c ORDINA_PER_PRIORITA o @c ORDINA_PER_DATA.
 * @return @c 0 in caso di successo, @c -1 in caso di errore.
 */
int ordina_richieste_merge_sort(Richiesta **lista, int n, int criterio);

/* =========================================================================
 * Report e Dashboard
 * ========================================================================= */

/**
 * @brief Esporta un report testuale ordinato su file.
 * @param lista Array di puntatori a Richiesta.
 * @param n     Numero di elementi validi.
 * @return @c 0 in caso di successo, @c -1 in caso di errore.
 */
int esporta_report_ordinato(Richiesta **lista, int n);

/**
 * @brief Genera una dashboard HTML con riepilogo di richieste e tecnici.
 * @param richieste Array di puntatori a Richiesta.
 * @param nr        Numero di richieste.
 * @param tecnici   Array di puntatori a Tecnico.
 * @param nt        Numero di tecnici.
 * @return @c 0 in caso di successo, @c -1 in caso di errore.
 */
int genera_dashboard_html(Richiesta **richieste, int nr,
                          Tecnico **tecnici, int nt);

#endif /* LOGICA_H */
