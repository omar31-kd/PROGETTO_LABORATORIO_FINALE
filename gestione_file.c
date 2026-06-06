/**
 * @file gestione_file.c
 * @brief Implementazione delle funzioni di I/O su file dichiarate in gestione_file.h.
 *
 * La lettura avviene esclusivamente tramite @c fgets e @c feof.
 * La scrittura avviene tramite @c fprintf.
 * Non viene utilizzato @c fscanf.
 */

#include "gestione_file.h"

#include <stdlib.h>
#include <stdio.h>

/* =========================================================================
 * Costanti interne
 * ========================================================================= */

/**
 * @brief Dimensione del buffer di lettura per @c fgets.
 *
 * Deve essere >= MAX_PROBLEMA + 2 (newline + terminatore).
 */
#define BUF_LEN 512

/**
 * @brief Stringa separatrice che delimita la fine di un blocco nel file.
 */
#define SEPARATORE "---"

/* =========================================================================
 * Funzioni di supporto interne (static)
 * ========================================================================= */

/**
 * @brief Rimuove il carattere di newline ('\\n') o carriage return ('\\r')
 *        in coda alla stringa prodotta da @c fgets.
 *
 * @param s Stringa da modificare in-place (non @c NULL).
 */
static void rimuovi_newline(char *s)
{
    int i;

    if (s == NULL) {
        return;
    }

    for (i = 0; s[i] != '\0'; i++) {
        if (s[i] == '\n' || s[i] == '\r') {
            s[i] = '\0';
            return;
        }
    }
}

/**
 * @brief Copia al massimo @p max_car - 1 caratteri da @p src in @p dst,
 *        garantendo la terminazione con '\\0'.
 *
 * @param dst     Buffer di destinazione (non @c NULL).
 * @param src     Stringa sorgente (non @c NULL).
 * @param max_car Dimensione totale del buffer, terminatore incluso.
 */
static void copia_str(char *dst, const char *src, int max_car)
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
 * @param a Prima stringa (non @c NULL).
 * @param b Seconda stringa (non @c NULL).
 * @return 0 se le stringhe sono uguali, valore diverso altrimenti.
 */
static int uguale(const char *a, const char *b)
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
 * @brief Converte una stringa in intero senza usare @c sscanf.
 *
 * Gestisce il segno iniziale e ignora caratteri non numerici successivi.
 *
 * @param s Stringa da convertire (non @c NULL).
 * @return Valore intero estratto, 0 in caso di stringa vuota o non numerica.
 */
static int str_a_int(const char *s)
{
    int risultato = 0;
    int segno     = 1;
    int i         = 0;

    if (s == NULL) {
        return 0;
    }

    if (s[0] == '-') {
        segno = -1;
        i     = 1;
    } else if (s[0] == '+') {
        i = 1;
    }

    for (; s[i] >= '0' && s[i] <= '9'; i++) {
        risultato = risultato * 10 + (s[i] - '0');
    }

    return risultato * segno;
}

/**
 * @brief Converte una stringa in long senza usare @c sscanf.
 *
 * @param s Stringa da convertire (non @c NULL).
 * @return Valore long estratto, 0 se la stringa non è numerica.
 */
static long str_a_long(const char *s)
{
    long risultato = 0;
    int  segno     = 1;
    int  i         = 0;

    if (s == NULL) {
        return 0L;
    }

    if (s[0] == '-') {
        segno = -1;
        i     = 1;
    } else if (s[0] == '+') {
        i = 1;
    }

    for (; s[i] >= '0' && s[i] <= '9'; i++) {
        risultato = risultato * 10 + (long)(s[i] - '0');
    }

    return risultato * (long)segno;
}

/**
 * @brief Converte una stringa decimale in double senza usare @c sscanf.
 *
 * Gestisce la parte intera e la parte frazionaria separata da '.'.
 *
 * @param s Stringa da convertire (non @c NULL).
 * @return Valore double estratto, 0.0 se la stringa non è numerica.
 */
static double str_a_double(const char *s)
{
    double intero    = 0.0;
    double frazione  = 0.0;
    double divisore  = 10.0;
    double segno_d   = 1.0;
    int    i         = 0;

    if (s == NULL) {
        return 0.0;
    }

    if (s[0] == '-') {
        segno_d = -1.0;
        i       = 1;
    } else if (s[0] == '+') {
        i = 1;
    }

    for (; s[i] >= '0' && s[i] <= '9'; i++) {
        intero = intero * 10.0 + (double)(s[i] - '0');
    }

    if (s[i] == '.') {
        i++;
        for (; s[i] >= '0' && s[i] <= '9'; i++) {
            frazione += (double)(s[i] - '0') / divisore;
            divisore *= 10.0;
        }
    }

    return segno_d * (intero + frazione);
}

/**
 * @brief Cerca nell'array @p lista la richiesta con il codice specificato.
 *
 * @param lista       Array di puntatori a Richiesta.
 * @param n           Numero di elementi validi in @p lista.
 * @param codice      Codice da ricercare (non @c NULL).
 * @return Puntatore alla Richiesta trovata, oppure @c NULL se assente.
 */
static Richiesta *cerca_per_codice(Richiesta **lista, int n, const char *codice)
{
    int i;

    if (lista == NULL || codice == NULL || n <= 0) {
        return NULL;
    }

    for (i = 0; i < n; i++) {
        if (lista[i] != NULL &&
            uguale(richiesta_get_codice(lista[i]), codice) == 0) {
            return lista[i];
        }
    }
    return NULL;
}

/* =========================================================================
 * Implementazione: Richieste
 * ========================================================================= */

int salva_richieste(const char *percorso, Richiesta **lista, int n)
{
    FILE      *fp;
    int        i;
    Richiesta *r;

    if (percorso == NULL || lista == NULL || n < 0) {
        return -1;
    }

    fp = fopen(percorso, "w");
    if (fp == NULL) {
        return -1;
    }

    for (i = 0; i < n; i++) {
        r = lista[i];
        if (r == NULL) {
            continue;
        }

        fprintf(fp, "%s\n",  richiesta_get_codice(r));
        fprintf(fp, "%s\n",  richiesta_get_cliente(r));
        fprintf(fp, "%s\n",  richiesta_get_dispositivo(r));
        fprintf(fp, "%s\n",  richiesta_get_problema(r));
        fprintf(fp, "%d\n",  (int)richiesta_get_priorita(r));
        fprintf(fp, "%d\n",  (int)richiesta_get_stato(r));
        fprintf(fp, "%.2f\n", richiesta_get_costo_stimato(r));
        fprintf(fp, "%.2f\n", richiesta_get_costo_finale(r));
        fprintf(fp, "%ld\n", (long)richiesta_get_data_apertura(r));
        fprintf(fp, "%s\n",  SEPARATORE);
    }

    fclose(fp);
    return 0;
}

int carica_richieste(const char *percorso, Richiesta **lista, int *n, int max)
{
    FILE      *fp;
    char       buf[BUF_LEN];
    char       codice[MAX_CODICE];
    char       cliente[MAX_CLIENTE];
    char       dispositivo[MAX_DISPOSITIVO];
    char       problema[MAX_PROBLEMA];
    int        priorita_i    = 0;
    int        stato_i       = 0;
    double     costo_stimato = 0.0;
    double     costo_finale  = 0.0;
    long       data_long     = 0L;
    int        campo         = 0;
    Richiesta *r;

    if (percorso == NULL || lista == NULL || n == NULL || max <= 0) {
        return -1;
    }

    fp = fopen(percorso, "r");
    if (fp == NULL) {
        return -1;
    }

    *n = 0;

    while (!feof(fp)) {
        if (fgets(buf, BUF_LEN, fp) == NULL) {
            break;
        }
        rimuovi_newline(buf);

        if (uguale(buf, SEPARATORE) == 0) {
            /*
             * Fine blocco: crea la Richiesta con i valori accumulati
             * e sovrascrive data e stato che il costruttore imposta
             * con valori di default.
             */
            if (*n < max) {
                r = richiesta_crea(codice, cliente, dispositivo, problema,
                                   (Priorita)priorita_i, costo_stimato);
                if (r != NULL) {
                    richiesta_set_stato(r, (StatoRichiesta)stato_i);
                    richiesta_set_costo_finale(r, costo_finale);
                    richiesta_set_data_apertura(r, (time_t)data_long);
                    lista[*n] = r;
                    (*n)++;
                }
            }
            campo        = 0;
            priorita_i   = 0;
            stato_i      = 0;
            costo_stimato = 0.0;
            costo_finale  = 0.0;
            data_long     = 0L;
        } else {
            switch (campo) {
                case 0: copia_str(codice,      buf, MAX_CODICE);      break;
                case 1: copia_str(cliente,     buf, MAX_CLIENTE);     break;
                case 2: copia_str(dispositivo, buf, MAX_DISPOSITIVO); break;
                case 3: copia_str(problema,    buf, MAX_PROBLEMA);    break;
                case 4: priorita_i    = str_a_int(buf);    break;
                case 5: stato_i       = str_a_int(buf);    break;
                case 6: costo_stimato = str_a_double(buf); break;
                case 7: costo_finale  = str_a_double(buf); break;
                case 8: data_long     = str_a_long(buf);   break;
                default: break;
            }
            campo++;
        }
    }

    fclose(fp);
    return 0;
}

/* =========================================================================
 * Implementazione: Tecnici
 * ========================================================================= */

int salva_tecnici(const char *percorso, Tecnico **lista, int n)
{
    FILE      *fp;
    int        i;
    int        j;
    int        assegnate;
    Tecnico   *t;
    Richiesta *r;

    if (percorso == NULL || lista == NULL || n < 0) {
        return -1;
    }

    fp = fopen(percorso, "w");
    if (fp == NULL) {
        return -1;
    }

    for (i = 0; i < n; i++) {
        t = lista[i];
        if (t == NULL) {
            continue;
        }

        assegnate = tecnico_get_assegnate(t);

        fprintf(fp, "%s\n", tecnico_get_codice(t));
        fprintf(fp, "%s\n", tecnico_get_nome(t));
        fprintf(fp, "%s\n", tecnico_get_specializzazione(t));
        fprintf(fp, "%d\n", tecnico_get_max_richieste(t));
        fprintf(fp, "%d\n", assegnate);

        for (j = 0; j < assegnate; j++) {
            r = tecnico_get_richiesta(t, j);
            if (r != NULL) {
                fprintf(fp, "%s\n", richiesta_get_codice(r));
            }
        }

        fprintf(fp, "%s\n", SEPARATORE);
    }

    fclose(fp);
    return 0;
}

int carica_tecnici(const char *percorso,
                   Tecnico   **lista,
                   int        *n,
                   int         max,
                   Richiesta **richieste,
                   int         n_richieste)
{
    FILE    *fp;
    char     buf[BUF_LEN];
    char     codice[MAX_CODICE_TECNICO];
    char     nome[MAX_NOME_TECNICO];
    char     spec[MAX_SPECIALIZZAZIONE];
    int      max_ric          = 1;
    int      n_ric_nel_file   = 0;
    int      n_ric_lette      = 0;
    char     codici_ric[MAX_RICHIESTE_ASSOLUTO][MAX_CODICE];
    int      campo            = 0;
    int      j;
    Tecnico  *t;
    Richiesta *r;

    if (percorso == NULL || lista == NULL || n == NULL || max <= 0) {
        return -1;
    }

    fp = fopen(percorso, "r");
    if (fp == NULL) {
        return -1;
    }

    *n = 0;

    while (!feof(fp)) {
        if (fgets(buf, BUF_LEN, fp) == NULL) {
            break;
        }
        rimuovi_newline(buf);

        if (uguale(buf, SEPARATORE) == 0) {
            /*
             * Fine blocco: crea il Tecnico e ricollega i puntatori
             * alle Richieste già presenti in memoria.
             */
            if (*n < max) {
                t = tecnico_crea(codice, nome, spec, max_ric);
                if (t != NULL) {
                    for (j = 0; j < n_ric_lette; j++) {
                        r = cerca_per_codice(richieste, n_richieste,
                                             codici_ric[j]);
                        if (r != NULL) {
                            tecnico_assegna_richiesta(t, r);
                        }
                    }
                    lista[*n] = t;
                    (*n)++;
                }
            }
            /* Ripristina lo stato per il prossimo blocco */
            campo          = 0;
            n_ric_nel_file = 0;
            n_ric_lette    = 0;
        } else if (campo < 5) {
            /*
             * Lettura dei cinque campi fissi del Tecnico.
             */
            switch (campo) {
                case 0: copia_str(codice, buf, MAX_CODICE_TECNICO);   break;
                case 1: copia_str(nome,   buf, MAX_NOME_TECNICO);     break;
                case 2: copia_str(spec,   buf, MAX_SPECIALIZZAZIONE); break;
                case 3: max_ric        = str_a_int(buf);              break;
                case 4: n_ric_nel_file = str_a_int(buf);              break;
                default: break;
            }
            campo++;
        } else {
            /*
             * Lettura dei codici delle richieste assegnate.
             * Il limite è il minimo tra il numero dichiarato nel file
             * e la capacità massima dell'array interno.
             */
            if (n_ric_lette < n_ric_nel_file &&
                n_ric_lette < MAX_RICHIESTE_ASSOLUTO) {
                copia_str(codici_ric[n_ric_lette], buf, MAX_CODICE);
                n_ric_lette++;
            }
        }
    }

    fclose(fp);
    return 0;
}
