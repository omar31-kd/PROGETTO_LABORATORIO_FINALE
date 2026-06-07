/**
 * @file logica.c
 * @brief Implementazione delle funzioni di logica applicativa dichiarate
 *        in logica.h: visualizzazione, inserimento, ricerca, modifica,
 *        cancellazione, ordinamento (Merge Sort), reportistica e
 *        generazione della dashboard HTML.
 */

#include "logica.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* =========================================================================
 * Funzioni di supporto interne (static)
 * ========================================================================= */

/**
 * @brief Confronta due stringhe carattere per carattere.
 * @return 0 se uguali, valore diverso altrimenti.
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
 * @brief Restituisce la stringa leggibile di una Priorità.
 */
static const char *priorita_a_stringa(Priorita p)
{
    switch (p) {
        case PRIORITA_BASSA:   return "Bassa";
        case PRIORITA_MEDIA:   return "Media";
        case PRIORITA_ALTA:    return "Alta";
        case PRIORITA_URGENTE: return "Urgente";
        default:               return "???";
    }
}

/**
 * @brief Restituisce la stringa leggibile di uno StatoRichiesta.
 */
static const char *stato_a_stringa(StatoRichiesta s)
{
    switch (s) {
        case STATO_APERTA:    return "Aperta";
        case STATO_ASSEGNATA: return "Assegnata";
        case STATO_IN_CORSO:  return "In Corso";
        case STATO_SOSPESA:   return "Sospesa";
        case STATO_CHIUSA:    return "Chiusa";
        case STATO_ANNULLATA: return "Annullata";
        default:              return "???";
    }
}

/* =========================================================================
 * Visualizzazione
 * ========================================================================= */

void visualizza_richiesta(const Richiesta *r)
{
    time_t data;
    struct tm *tm_info;

    if (r == NULL) {
        printf("  (richiesta NULL)\n");
        return;
    }

    data = richiesta_get_data_apertura(r);
    tm_info = localtime(&data);

    printf("  Codice      : %s\n", richiesta_get_codice(r));
    printf("  Cliente     : %s\n", richiesta_get_cliente(r));
    printf("  Dispositivo : %s\n", richiesta_get_dispositivo(r));
    printf("  Problema    : %s\n", richiesta_get_problema(r));
    printf("  Priorita'   : %s\n", priorita_a_stringa(richiesta_get_priorita(r)));
    printf("  Stato       : %s\n", stato_a_stringa(richiesta_get_stato(r)));
    printf("  Costo stim. : EUR %.2f\n", richiesta_get_costo_stimato(r));
    printf("  Costo finale: EUR %.2f\n", richiesta_get_costo_finale(r));
    if (tm_info != NULL) {
        printf("  Data apert. : %02d/%02d/%04d %02d:%02d\n",
               tm_info->tm_mday, tm_info->tm_mon + 1, tm_info->tm_year + 1900,
               tm_info->tm_hour, tm_info->tm_min);
    }
    printf("\n");
}

void visualizza_richieste(Richiesta **lista, int n)
{
    int i;

    if (lista == NULL || n <= 0) {
        printf("\n  Nessuna richiesta presente.\n");
        return;
    }

    printf("\n  === ELENCO RICHIESTE (%d) ===\n\n", n);
    for (i = 0; i < n; i++) {
        if (lista[i] != NULL) {
            printf("  --- Richiesta #%d ---\n", i + 1);
            visualizza_richiesta(lista[i]);
        }
    }
}

void visualizza_tecnico(const Tecnico *t)
{
    int i;
    int assegnate;
    Richiesta *r;

    if (t == NULL) {
        printf("  (tecnico NULL)\n");
        return;
    }

    assegnate = tecnico_get_assegnate(t);

    printf("  Codice         : %s\n", tecnico_get_codice(t));
    printf("  Nome           : %s\n", tecnico_get_nome(t));
    printf("  Specializzaz.  : %s\n", tecnico_get_specializzazione(t));
    printf("  Max richieste  : %d\n", tecnico_get_max_richieste(t));
    printf("  Assegnate      : %d\n", assegnate);

    if (assegnate > 0) {
        printf("  Richieste:\n");
        for (i = 0; i < assegnate; i++) {
            r = tecnico_get_richiesta(t, i);
            if (r != NULL) {
                printf("    [%d] %s - %s (%s)\n",
                       i + 1,
                       richiesta_get_codice(r),
                       richiesta_get_cliente(r),
                       stato_a_stringa(richiesta_get_stato(r)));
            }
        }
    }
    printf("\n");
}

void visualizza_tecnici(Tecnico **lista, int n)
{
    int i;

    if (lista == NULL || n <= 0) {
        printf("\n  Nessun tecnico presente.\n");
        return;
    }

    printf("\n  === ELENCO TECNICI (%d) ===\n\n", n);
    for (i = 0; i < n; i++) {
        if (lista[i] != NULL) {
            printf("  --- Tecnico #%d ---\n", i + 1);
            visualizza_tecnico(lista[i]);
        }
    }
}

/* =========================================================================
 * Inserimento
 * ========================================================================= */

int inserisci_richiesta(Richiesta **lista, int *n, int max,
                        const char *codice, const char *cliente,
                        const char *dispositivo, const char *problema,
                        Priorita priorita, double costo_stimato)
{
    Richiesta *nuova;
    int i;

    if (lista == NULL || n == NULL || codice == NULL ||
        cliente == NULL || dispositivo == NULL || problema == NULL) {
        return -1;
    }

    /* Rifiuta stringhe vuote */
    if (codice[0] == '\0' || cliente[0] == '\0' ||
        dispositivo[0] == '\0' || problema[0] == '\0') {
        return -1;
    }

    if (*n >= max) {
        return -1;
    }

    /* Verifica codice duplicato */
    for (i = 0; i < *n; i++) {
        if (lista[i] != NULL &&
            confronta_stringhe(richiesta_get_codice(lista[i]), codice) == 0) {
            return -1;
        }
    }

    nuova = richiesta_crea(codice, cliente, dispositivo, problema,
                           priorita, costo_stimato);
    if (nuova == NULL) {
        return -1;
    }

    lista[*n] = nuova;
    (*n)++;
    return 0;
}

int inserisci_tecnico(Tecnico **lista, int *n, int max,
                      const char *codice, const char *nome,
                      const char *specializzazione, int max_richieste)
{
    Tecnico *nuovo;
    int i;

    if (lista == NULL || n == NULL || codice == NULL ||
        nome == NULL || specializzazione == NULL) {
        return -1;
    }

    /* Rifiuta stringhe vuote */
    if (codice[0] == '\0' || nome[0] == '\0' ||
        specializzazione[0] == '\0') {
        return -1;
    }

    if (*n >= max) {
        return -1;
    }

    /* Verifica codice duplicato */
    for (i = 0; i < *n; i++) {
        if (lista[i] != NULL &&
            confronta_stringhe(tecnico_get_codice(lista[i]), codice) == 0) {
            return -1;
        }
    }

    nuovo = tecnico_crea(codice, nome, specializzazione, max_richieste);
    if (nuovo == NULL) {
        return -1;
    }

    lista[*n] = nuovo;
    (*n)++;
    return 0;
}

/* =========================================================================
 * Ricerca
 * ========================================================================= */

Richiesta *cerca_richiesta(Richiesta **lista, int n, const char *codice)
{
    int i;

    if (lista == NULL || codice == NULL || n <= 0) {
        return NULL;
    }

    for (i = 0; i < n; i++) {
        if (lista[i] != NULL &&
            confronta_stringhe(richiesta_get_codice(lista[i]), codice) == 0) {
            return lista[i];
        }
    }
    return NULL;
}

Tecnico *cerca_tecnico(Tecnico **lista, int n, const char *codice)
{
    int i;

    if (lista == NULL || codice == NULL || n <= 0) {
        return NULL;
    }

    for (i = 0; i < n; i++) {
        if (lista[i] != NULL &&
            confronta_stringhe(tecnico_get_codice(lista[i]), codice) == 0) {
            return lista[i];
        }
    }
    return NULL;
}

/* =========================================================================
 * Modifica
 * ========================================================================= */

int modifica_stato_richiesta(Richiesta **lista, int n,
                             const char *codice, StatoRichiesta stato)
{
    Richiesta *r = cerca_richiesta(lista, n, codice);
    if (r == NULL) {
        return -1;
    }
    richiesta_set_stato(r, stato);
    return 0;
}

int modifica_problema_richiesta(Richiesta **lista, int n,
                                const char *codice, const char *problema)
{
    Richiesta *r;

    if (codice == NULL || codice[0] == '\0' ||
        problema == NULL || problema[0] == '\0') {
        return -1;
    }

    r = cerca_richiesta(lista, n, codice);
    if (r == NULL) {
        return -1;
    }
    richiesta_set_problema(r, problema);
    return 0;
}

int modifica_costo_stimato_richiesta(Richiesta **lista, int n,
                                     const char *codice, double costo_stimato)
{
    Richiesta *r = cerca_richiesta(lista, n, codice);
    if (r == NULL || costo_stimato < 0.0) {
        return -1;
    }
    richiesta_set_costo_stimato(r, costo_stimato);
    return 0;
}

int modifica_costo_finale_richiesta(Richiesta **lista, int n,
                                    const char *codice, double costo_finale)
{
    Richiesta *r = cerca_richiesta(lista, n, codice);
    if (r == NULL || costo_finale < 0.0) {
        return -1;
    }
    richiesta_set_costo_finale(r, costo_finale);
    return 0;
}

/* =========================================================================
 * Cancellazione
 * ========================================================================= */

int cancella_richiesta(Richiesta **lista, int *n, const char *codice)
{
    int i, trovato;

    if (lista == NULL || n == NULL || codice == NULL || *n <= 0) {
        return -1;
    }

    trovato = -1;
    for (i = 0; i < *n; i++) {
        if (lista[i] != NULL &&
            confronta_stringhe(richiesta_get_codice(lista[i]), codice) == 0) {
            trovato = i;
            break;
        }
    }

    if (trovato == -1) {
        return -1;
    }

    richiesta_distruggi(&lista[trovato]);

    /* Compatta l'array */
    for (i = trovato; i < *n - 1; i++) {
        lista[i] = lista[i + 1];
    }
    lista[*n - 1] = NULL;
    (*n)--;

    return 0;
}

/* =========================================================================
 * Ordinamento — Merge Sort
 * ========================================================================= */

/**
 * @brief Funzione di confronto per priorità (decrescente).
 * @return Valore > 0 se a deve precedere b.
 */
static int confronta_priorita(Richiesta *a, Richiesta *b)
{
    return (int)richiesta_get_priorita(b) - (int)richiesta_get_priorita(a);
}

/**
 * @brief Funzione di confronto per data di apertura (crescente).
 * @return Valore < 0 se a deve precedere b.
 */
static int confronta_data(Richiesta *a, Richiesta *b)
{
    time_t da = richiesta_get_data_apertura(a);
    time_t db = richiesta_get_data_apertura(b);
    if (da < db) return -1;
    if (da > db) return  1;
    return 0;
}

/**
 * @brief Fase di merge: fonde due sotto-array ordinati.
 */
static void merge(Richiesta **lista, int sinistra, int centro, int destra,
                  int (*cmp)(Richiesta *, Richiesta *))
{
    int n1, n2, i, j, k;
    Richiesta **tmp_sx;
    Richiesta **tmp_dx;

    n1 = centro - sinistra + 1;
    n2 = destra - centro;

    tmp_sx = (Richiesta **)malloc((size_t)n1 * sizeof(Richiesta *));
    tmp_dx = (Richiesta **)malloc((size_t)n2 * sizeof(Richiesta *));
    if (tmp_sx == NULL || tmp_dx == NULL) {
        free(tmp_sx);
        free(tmp_dx);
        return;
    }

    for (i = 0; i < n1; i++)
        tmp_sx[i] = lista[sinistra + i];
    for (j = 0; j < n2; j++)
        tmp_dx[j] = lista[centro + 1 + j];

    i = 0;
    j = 0;
    k = sinistra;

    while (i < n1 && j < n2) {
        if (cmp(tmp_sx[i], tmp_dx[j]) <= 0) {
            lista[k] = tmp_sx[i];
            i++;
        } else {
            lista[k] = tmp_dx[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        lista[k] = tmp_sx[i];
        i++;
        k++;
    }
    while (j < n2) {
        lista[k] = tmp_dx[j];
        j++;
        k++;
    }

    free(tmp_sx);
    free(tmp_dx);
}

/**
 * @brief Procedura ricorsiva del Merge Sort.
 */
static void merge_sort_rec(Richiesta **lista, int sinistra, int destra,
                           int (*cmp)(Richiesta *, Richiesta *))
{
    int centro;

    if (sinistra >= destra) {
        return;
    }

    centro = sinistra + (destra - sinistra) / 2;
    merge_sort_rec(lista, sinistra, centro, cmp);
    merge_sort_rec(lista, centro + 1, destra, cmp);
    merge(lista, sinistra, centro, destra, cmp);
}

int ordina_richieste_merge_sort(Richiesta **lista, int n, int criterio)
{
    int (*cmp)(Richiesta *, Richiesta *);

    if (lista == NULL || n <= 0) {
        return -1;
    }

    if (n == 1) {
        return 0;
    }

    if (criterio == ORDINA_PER_PRIORITA) {
        cmp = confronta_priorita;
    } else if (criterio == ORDINA_PER_DATA) {
        cmp = confronta_data;
    } else {
        return -1;
    }

    merge_sort_rec(lista, 0, n - 1, cmp);
    return 0;
}

/* =========================================================================
 * Report e Dashboard
 * ========================================================================= */

int esporta_report_ordinato(Richiesta **lista, int n)
{
    FILE *fp;
    int i;
    Richiesta *r;
    time_t data;
    struct tm *tm_info;

    if (lista == NULL || n < 0) {
        return -1;
    }

    fp = fopen(FILE_REPORT_ORDINATO, "w");
    if (fp == NULL) {
        return -1;
    }

    fprintf(fp, "========================================\n");
    fprintf(fp, "  REPORT RICHIESTE  (%d elementi)\n", n);
    fprintf(fp, "========================================\n\n");

    for (i = 0; i < n; i++) {
        r = lista[i];
        if (r == NULL) {
            continue;
        }

        data = richiesta_get_data_apertura(r);
        tm_info = localtime(&data);

        fprintf(fp, "--- Richiesta #%d ---\n", i + 1);
        fprintf(fp, "  Codice      : %s\n", richiesta_get_codice(r));
        fprintf(fp, "  Cliente     : %s\n", richiesta_get_cliente(r));
        fprintf(fp, "  Dispositivo : %s\n", richiesta_get_dispositivo(r));
        fprintf(fp, "  Problema    : %s\n", richiesta_get_problema(r));
        fprintf(fp, "  Priorita'   : %s\n", priorita_a_stringa(richiesta_get_priorita(r)));
        fprintf(fp, "  Stato       : %s\n", stato_a_stringa(richiesta_get_stato(r)));
        fprintf(fp, "  Costo stim. : EUR %.2f\n", richiesta_get_costo_stimato(r));
        fprintf(fp, "  Costo finale: EUR %.2f\n", richiesta_get_costo_finale(r));
        if (tm_info != NULL) {
            fprintf(fp, "  Data apert. : %02d/%02d/%04d %02d:%02d\n",
                    tm_info->tm_mday, tm_info->tm_mon + 1,
                    tm_info->tm_year + 1900,
                    tm_info->tm_hour, tm_info->tm_min);
        }
        fprintf(fp, "\n");
    }

    fprintf(fp, "========================================\n");
    fprintf(fp, "  Fine report\n");
    fprintf(fp, "========================================\n");

    fclose(fp);
    return 0;
}

int genera_dashboard_html(Richiesta **richieste, int nr,
                          Tecnico **tecnici, int nt)
{
    FILE *fp;
    int i, j;
    int cnt_stato[6], cnt_prior[4];
    double tot_stimato, tot_finale;
    double circ, s, off;
    int assegnate, max_r, pct, attive, non_ass, occ;
    Richiesta *r;
    Tecnico *tec;
    time_t data_r, now;
    struct tm *tmi, *tm_now;
    int urg_cnt, ini_n;
    const char *np;
    char ini[3];

    static const char * const AV_COL[] = {
        "c-navy","c-teal","c-violet","c-amber","c-rose","c-slate"};
    static const char * const PCLS[] = {
        "p-bassa","p-media","p-alta","p-urgente"};
    static const char * const PLBL[] = {
        "Bassa","Media","Alta","Urgente"};
    static const char * const SCLS[] = {
        "s-aperta","s-assegnata","s-incorso","s-sospesa","s-chiusa","s-annullata"};
    static const char * const SLBL[] = {
        "Aperta","Assegnata","In Corso","Sospesa","Chiusa","Annullata"};

    if (richieste == NULL || tecnici == NULL) return -1;

    fp = fopen(FILE_DASHBOARD, "w");
    if (fp == NULL) return -1;

    /* Calcola statistiche */
    for (i = 0; i < 6; i++) cnt_stato[i] = 0;
    for (i = 0; i < 4; i++) cnt_prior[i] = 0;
    tot_stimato = tot_finale = 0.0;
    for (i = 0; i < nr; i++) {
        if (richieste[i] != NULL) {
            int si = (int)richiesta_get_stato(richieste[i]);
            int pi = (int)richiesta_get_priorita(richieste[i]);
            if (si >= 0 && si < 6)  cnt_stato[si]++;
            if (pi >= 0 && pi < 4)  cnt_prior[pi]++;
            tot_stimato += richiesta_get_costo_stimato(richieste[i]);
            tot_finale  += richiesta_get_costo_finale(richieste[i]);
        }
    }
    attive  = nr - cnt_stato[4] - cnt_stato[5];
    non_ass = cnt_stato[0];
    occ = 0;
    for (i = 0; i < nt; i++)
        if (tecnici[i] != NULL && tecnico_get_assegnate(tecnici[i]) > 0) occ++;

    now    = time(NULL);
    tm_now = localtime(&now);

    /* ================================================================
     * HTML output — Apple/macOS style dashboard
     * Usiamo fputs() per i blocchi statici (nessun escape di %)
     * e fprintf() solo per i valori dinamici.
     * ================================================================ */

    /* ---- DOCTYPE + HEAD + CSS ---- */
    fputs("<!DOCTYPE html>\n<html lang=\"it\">\n<head>\n"
          "<meta charset=\"UTF-8\">\n"
          "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0\">\n"
          "<title>Assistenza Tecnica \xe2\x80\x94 Dashboard</title>\n"
          "<style>\n"
          ":root{\n"
          "  --bg:#f5f5f7;--panel:#fff;--panel-2:#fbfbfd;--sidebar:#fff;\n"
          "  --ink:#1d1d1f;--ink-2:#6e6e73;--ink-3:#8e8e93;\n"
          "  --hair:rgba(0,0,0,.08);--hair-2:rgba(0,0,0,.06);\n"
          "  --accent:#1f3a5f;--accent-press:#172c47;\n"
          "  --accent-soft:#eef2f8;--accent-ink:#1f3a5f;--blue:#0a84ff;\n"
          "  --green:#34c759;--green-bg:#e9f8ee;--green-ink:#1a7f37;\n"
          "  --orange:#ff9f0a;--orange-bg:#fff4e2;--orange-ink:#b46a00;\n"
          "  --red:#ff3b30;--red-bg:#ffeceb;--red-ink:#c0392b;\n"
          "  --purple:#af52de;--purple-bg:#f3eeff;--purple-ink:#6e23cf;\n"
          "  --r-card:18px;--r-sm:12px;--r-ctrl:9px;--r-pill:980px;\n"
          "  --sh-1:0 1px 2px rgba(0,0,0,.04),0 1px 1px rgba(0,0,0,.03);\n"
          "  --sh-2:0 1px 3px rgba(0,0,0,.05),0 12px 28px -14px rgba(0,0,0,.16);\n"
          "  --gap:20px;--pad:22px;--side-w:236px;--head-h:60px;\n"
          "  --font:-apple-system,BlinkMacSystemFont,\"SF Pro Display\","
              "\"Helvetica Neue\",Arial,sans-serif;}\n"
          "*{box-sizing:border-box;margin:0;padding:0}\n"
          "html,body{height:100%}\n"
          "body{font-family:var(--font);background:var(--bg);color:var(--ink);\n"
          "  -webkit-font-smoothing:antialiased;font-size:15px;letter-spacing:-.01em}\n"
          "button,input{font-family:inherit} a{text-decoration:none;color:inherit}\n"
          ".ic{width:20px;height:20px;stroke:currentColor;fill:none;\n"
          "  stroke-width:1.7;stroke-linecap:round;stroke-linejoin:round;flex:none;display:block}\n"
          ".ic-16{width:16px;height:16px}.ic-18{width:18px;height:18px}\n",
          fp);

    fputs(
          ".topbar{position:sticky;top:0;z-index:30;height:var(--head-h);\n"
          "  display:flex;align-items:center;gap:18px;padding:0 22px;\n"
          "  background:rgba(255,255,255,.72);\n"
          "  backdrop-filter:saturate(180%) blur(20px);\n"
          "  -webkit-backdrop-filter:saturate(180%) blur(20px);\n"
          "  border-bottom:1px solid var(--hair)}\n"
          ".brand{display:flex;align-items:center;gap:11px;flex:none}\n"
          ".brand .mark{width:30px;height:30px;border-radius:8px;flex:none;\n"
          "  background:linear-gradient(160deg,var(--accent),var(--accent-press));\n"
          "  color:#fff;display:grid;place-items:center;box-shadow:var(--sh-1)}\n"
          ".brand .mark svg{width:17px;height:17px}\n"
          ".brand .name{font-size:15px;font-weight:600;letter-spacing:-.02em;white-space:nowrap}\n"
          ".brand .name span{color:var(--ink-3);font-weight:500}\n"
          ".head-right{margin-left:auto;display:flex;align-items:center;gap:12px}\n"
          ".search{display:flex;align-items:center;gap:8px;height:34px;padding:0 12px;\n"
          "  background:#f0f0f2;border-radius:9px;color:var(--ink-3);min-width:200px;\n"
          "  border:1px solid transparent;transition:all .15s}\n"
          ".search:focus-within{background:#fff;border-color:var(--hair)}\n"
          ".search input{border:0;background:transparent;outline:0;font-size:13.5px;\n"
          "  color:var(--ink);width:100%}\n"
          ".search input::placeholder{color:var(--ink-3)}\n"
          ".datechip{font-size:13px;color:var(--ink-2);white-space:nowrap}\n"
          ".iconbtn{width:34px;height:34px;border-radius:9px;border:1px solid transparent;\n"
          "  background:transparent;color:var(--ink-2);display:grid;place-items:center;\n"
          "  cursor:pointer;position:relative}\n"
          ".iconbtn:hover{background:#f0f0f2}\n"
          ".iconbtn .ping{position:absolute;top:7px;right:8px;width:7px;height:7px;\n"
          "  border-radius:50%;background:var(--red);border:1.5px solid #fff}\n"
          ".avatar-btn{width:34px;height:34px;border-radius:50%;border:1px solid var(--hair);\n"
          "  background:linear-gradient(160deg,#e8edf5,#d7e0ee);color:var(--accent);\n"
          "  display:grid;place-items:center;font-size:12px;font-weight:700;cursor:pointer}\n",
          fp);

    fputs(
          ".app-body{display:grid;grid-template-columns:var(--side-w) 1fr;align-items:start}\n"
          ".sidebar{position:sticky;top:var(--head-h);align-self:start;\n"
          "  height:calc(100vh - var(--head-h));\n"
          "  background:var(--sidebar);border-right:1px solid var(--hair);\n"
          "  padding:16px 12px;display:flex;flex-direction:column;gap:2px;overflow:auto}\n"
          ".nav-sec{font-size:11px;font-weight:600;letter-spacing:.04em;text-transform:uppercase;\n"
          "  color:var(--ink-3);margin:16px 10px 6px}.nav-sec:first-child{margin-top:2px}\n"
          ".nav-item{display:flex;align-items:center;gap:11px;padding:8px 10px;border-radius:8px;\n"
          "  color:var(--ink);font-size:14px;font-weight:450;cursor:pointer;transition:background .12s}\n"
          ".nav-item .ic{width:18px;height:18px;color:var(--ink-2)}\n"
          ".nav-item:hover{background:#f3f3f5}\n"
          ".nav-item.active{background:var(--accent-soft);color:var(--accent-ink);font-weight:600}\n"
          ".nav-item.active .ic{color:var(--accent)}\n"
          ".nav-item .badge{margin-left:auto;min-width:20px;height:20px;padding:0 6px;\n"
          "  border-radius:var(--r-pill);background:var(--red);color:#fff;\n"
          "  font-size:11.5px;font-weight:600;display:grid;place-items:center}\n"
          ".side-foot{margin-top:auto;padding:12px 10px 4px;border-top:1px solid var(--hair-2)}\n"
          ".side-foot .who{display:flex;align-items:center;gap:10px;padding-top:10px}\n"
          ".side-foot .who .av{width:32px;height:32px;border-radius:50%;\n"
          "  background:linear-gradient(160deg,#e8edf5,#d7e0ee);color:var(--accent);\n"
          "  display:grid;place-items:center;font-size:11px;font-weight:700;flex:none}\n"
          ".side-foot .who b{font-size:13.5px;font-weight:600;display:block}\n"
          ".side-foot .who small{font-size:12px;color:var(--ink-3)}\n"
          ".main{padding:24px 26px 60px;display:flex;flex-direction:column;gap:var(--gap);min-width:0}\n"
          ".page-head{display:flex;align-items:flex-end;gap:14px;flex-wrap:wrap}\n"
          ".page-head h1{font-size:26px;font-weight:600;letter-spacing:-.03em}\n"
          ".page-head .sub{color:var(--ink-2);font-size:14px;margin-bottom:3px}\n"
          ".page-head .spacer{flex:1}\n"
          ".btn{display:inline-flex;align-items:center;gap:7px;cursor:pointer;\n"
          "  font-size:13.5px;font-weight:600;padding:8px 14px;border-radius:var(--r-ctrl);\n"
          "  border:1px solid var(--hair);background:#fff;color:var(--ink);\n"
          "  box-shadow:var(--sh-1);transition:all .12s}\n"
          ".btn:hover{background:#fafafa}\n"
          ".btn.primary{background:var(--accent);border-color:var(--accent);color:#fff}\n",
          fp);

    fputs(
          ".card{background:var(--panel);border:1px solid var(--hair-2);\n"
          "  border-radius:var(--r-card);box-shadow:var(--sh-2);padding:var(--pad)}\n"
          ".card-head{display:flex;align-items:center;gap:10px;margin-bottom:16px}\n"
          ".card-head h2{font-size:16px;font-weight:600;letter-spacing:-.02em}\n"
          ".card-head .meta{color:var(--ink-3);font-size:13px;font-weight:450}\n"
          ".card-head .spacer{flex:1}\n"
          ".card-head .link{color:var(--blue);font-size:13px;font-weight:500}\n"
          ".kpis{display:grid;grid-template-columns:repeat(4,1fr);gap:var(--gap)}\n"
          ".kpi{background:var(--panel);border:1px solid var(--hair-2);\n"
          "  border-radius:var(--r-card);box-shadow:var(--sh-2);\n"
          "  padding:18px 20px;display:flex;flex-direction:column;gap:10px}\n"
          ".kpi .top{display:flex;align-items:center;gap:9px;color:var(--ink-2);font-size:13.5px;font-weight:500}\n"
          ".kpi .top .ico{width:30px;height:30px;border-radius:9px;\n"
          "  display:grid;place-items:center;flex:none;\n"
          "  background:var(--accent-soft);color:var(--accent)}\n"
          ".kpi.warn .top .ico{background:var(--orange-bg);color:var(--orange-ink)}\n"
          ".kpi.danger .top .ico{background:var(--red-bg);color:var(--red-ink)}\n"
          ".kpi.ok .top .ico{background:var(--green-bg);color:var(--green-ink)}\n"
          ".kpi .num{font-size:36px;font-weight:600;letter-spacing:-.04em;line-height:1;\n"
          "  font-variant-numeric:tabular-nums}\n"
          ".kpi .foot{display:flex;align-items:center;gap:5px;font-size:12.5px;color:var(--ink-3)}\n"
          ".kpi .foot .warnt{color:var(--orange-ink);font-weight:600}\n"
          ".kpi .foot .ok{color:var(--green-ink);font-weight:600}\n"
          ".kpi .foot .acc{color:var(--accent-ink);font-weight:600}\n"
          ".mid{display:grid;grid-template-columns:1.5fr 1fr;gap:var(--gap);align-items:start}\n"
          ".urg-list{display:flex;flex-direction:column;gap:10px}\n"
          ".urg{display:flex;align-items:center;gap:14px;padding:14px 16px;\n"
          "  border:1px solid var(--hair-2);border-radius:var(--r-sm);\n"
          "  background:var(--panel-2);transition:background .12s}\n"
          ".urg:hover{background:#fff;box-shadow:var(--sh-1)}\n"
          ".tag{display:inline-flex;align-items:center;gap:5px;font-size:11.5px;\n"
          "  font-weight:600;letter-spacing:.01em;padding:4px 9px;\n"
          "  border-radius:var(--r-pill);white-space:nowrap}\n"
          ".tag.urgent{background:var(--red-bg);color:var(--red-ink)}\n"
          ".tag.high{background:var(--orange-bg);color:var(--orange-ink)}\n"
          ".tag.dot::before{content:\"\";width:6px;height:6px;border-radius:50%;background:currentColor}\n"
          ".urg .desc{flex:1;min-width:0;font-size:14.5px;font-weight:500}\n"
          ".urg .desc small{display:block;color:var(--ink-3);font-size:12.5px;font-weight:400;margin-top:2px}\n"
          ".s-pill{display:inline-flex;align-items:center;gap:5px;\n"
          "  font-size:11.5px;font-weight:600;padding:3px 9px;border-radius:var(--r-pill)}\n"
          ".s-pill .dot{width:6px;height:6px;border-radius:50%;background:currentColor;flex:none}\n"
          ".s-aperta{background:var(--red-bg);color:var(--red-ink)}\n"
          ".s-assegnata{background:var(--purple-bg);color:var(--purple-ink)}\n"
          ".s-incorso{background:var(--orange-bg);color:var(--orange-ink)}\n"
          ".s-sospesa{background:#f3f3f5;color:var(--ink-3)}\n"
          ".s-chiusa{background:var(--green-bg);color:var(--green-ink)}\n"
          ".s-annullata{background:#f3f3f5;color:var(--ink-3)}\n"
          ".p-urgente{background:var(--red-bg);color:var(--red-ink)}\n"
          ".p-alta{background:var(--orange-bg);color:var(--orange-ink)}\n"
          ".p-media{background:#f0f5ff;color:#0050d0}\n"
          ".p-bassa{background:var(--green-bg);color:var(--green-ink)}\n",
          fp);

    fputs(
          ".donut-wrap{display:flex;align-items:center;gap:20px}\n"
          ".donut{width:128px;height:128px;flex:none;position:relative}\n"
          ".donut svg{transform:rotate(-90deg)}\n"
          ".donut .ctr{position:absolute;inset:0;display:grid;place-content:center;text-align:center}\n"
          ".donut .ctr b{font-size:24px;font-weight:600;letter-spacing:-.03em;font-variant-numeric:tabular-nums}\n"
          ".donut .ctr small{font-size:11.5px;color:var(--ink-3);display:block;margin-top:1px}\n"
          ".legend{display:flex;flex-direction:column;gap:11px;flex:1}\n"
          ".legend .row{display:flex;align-items:center;gap:10px;font-size:13.5px}\n"
          ".legend .sw{width:11px;height:11px;border-radius:4px;flex:none}\n"
          ".legend .row b{margin-left:auto;font-weight:600;font-variant-numeric:tabular-nums}\n"
          ".legend .row .cnt{color:var(--ink-3);font-size:12.5px;font-variant-numeric:tabular-nums}\n"
          ".divider{height:1px;background:var(--hair-2);margin:18px 0}\n"
          ".bars{display:flex;flex-direction:column;gap:13px}\n"
          ".bar{display:grid;grid-template-columns:90px 1fr 38px;align-items:center;gap:12px}\n"
          ".bar .lbl{font-size:13px;color:var(--ink-2)}\n"
          ".bar .track{height:8px;border-radius:var(--r-pill);background:#ececef;overflow:hidden}\n"
          ".bar .track>span{display:block;height:100%;border-radius:var(--r-pill);transition:width .6s}\n"
          ".bar .pct{font-size:13px;font-weight:600;text-align:right;color:var(--ink);\n"
          "  font-variant-numeric:tabular-nums}\n"
          ".fill-green{background:var(--green)}.fill-accent{background:var(--accent)}\n"
          ".fill-orange{background:var(--orange)}.fill-red{background:var(--red)}\n"
          ".tbl-card{padding:0;overflow:hidden}\n"
          ".tbl-card .card-head{padding:20px 22px 14px;margin:0}\n"
          ".tbl{width:100%;border-collapse:collapse}\n"
          ".tbl thead th{text-align:left;font-size:11.5px;font-weight:600;\n"
          "  letter-spacing:.03em;text-transform:uppercase;color:var(--ink-3);\n"
          "  padding:10px 18px;border-top:1px solid var(--hair-2);\n"
          "  border-bottom:1px solid var(--hair-2);background:var(--panel-2)}\n"
          ".tbl tbody td{padding:13px 18px;border-bottom:1px solid var(--hair-2);\n"
          "  font-size:14px;vertical-align:middle}\n"
          ".tbl tbody tr:last-child td{border-bottom:0}\n"
          ".tbl tbody tr:hover td{background:var(--panel-2)}\n"
          ".tbl .id{font-weight:700;color:var(--accent);\n"
          "  font-family:ui-monospace,\"SF Mono\",Menlo,monospace;font-size:13px}\n"
          ".tbl .nm{font-weight:500}.tbl .device{color:var(--ink-2);font-size:13px}\n"
          ".cellrow{display:flex;align-items:center;gap:10px}\n"
          ".av-sm{width:30px;height:30px;border-radius:50%;flex:none;\n"
          "  display:grid;place-items:center;font-size:11px;font-weight:700;color:#fff}\n"
          ".prog{display:flex;align-items:center;gap:9px}\n"
          ".prog .track{width:80px;height:6px;border-radius:var(--r-pill);\n"
          "  background:#ececef;overflow:hidden}\n"
          ".prog .track>span{display:block;height:100%;border-radius:var(--r-pill)}\n"
          ".prog .v{font-size:12.5px;color:var(--ink-2);font-variant-numeric:tabular-nums;min-width:30px}\n"
          ".tbl .cost{font-variant-numeric:tabular-nums;font-weight:600}\n"
          ".tbl .cost.ok{color:var(--green-ink)}.tbl .cost.dash{color:var(--ink-3)}\n"
          ".tbl .dt{color:var(--ink-3);font-variant-numeric:tabular-nums;font-size:13px}\n"
          ".c-navy{background:linear-gradient(160deg,#2c4f7c,#1f3a5f)}\n"
          ".c-teal{background:linear-gradient(160deg,#2aa6a0,#1c7d78)}\n"
          ".c-violet{background:linear-gradient(160deg,#7b6cf0,#5a48d6)}\n"
          ".c-amber{background:linear-gradient(160deg,#e8943a,#c9761f)}\n"
          ".c-rose{background:linear-gradient(160deg,#e7728a,#c94f6b)}\n"
          ".c-slate{background:linear-gradient(160deg,#6b7689,#4b5566)}\n"
          ".bottom-grid{display:grid;grid-template-columns:1fr 1fr;gap:var(--gap)}\n"
          "@media(max-width:1180px){.kpis{grid-template-columns:repeat(2,1fr)}\n"
          "  .mid{grid-template-columns:1fr}.bottom-grid{grid-template-columns:1fr}}\n"
          "@media(max-width:860px){.app-body{grid-template-columns:1fr}.sidebar{display:none}}\n"
          "</style>\n</head>\n<body>\n",
          fp);

    /* ---- SVG sprite ---- */
    fputs(
        "<svg width=\"0\" height=\"0\" style=\"position:absolute\" aria-hidden=\"true\">\n"
        "  <symbol id=\"i-grid\" viewBox=\"0 0 24 24\"><rect x=\"3\" y=\"3\" width=\"7\" height=\"9\" rx=\"1.5\"/>"
            "<rect x=\"14\" y=\"3\" width=\"7\" height=\"5\" rx=\"1.5\"/>"
            "<rect x=\"14\" y=\"12\" width=\"7\" height=\"9\" rx=\"1.5\"/>"
            "<rect x=\"3\" y=\"16\" width=\"7\" height=\"5\" rx=\"1.5\"/></symbol>\n"
        "  <symbol id=\"i-wrench\" viewBox=\"0 0 24 24\">"
            "<path d=\"M14.7 6.3a1 1 0 0 0 0 1.4l1.6 1.6a1 1 0 0 0 1.4 0l3.77-3.77"
            "a6 6 0 0 1-7.94 7.94l-6.91 6.91a2.12 2.12 0 0 1-3-3l6.91-6.91"
            "a6 6 0 0 1 7.94-7.94l-3.76 3.76z\"/></symbol>\n"
        "  <symbol id=\"i-users\" viewBox=\"0 0 24 24\">"
            "<path d=\"M16 21v-2a4 4 0 0 0-4-4H6a4 4 0 0 0-4 4v2\"/>"
            "<circle cx=\"9\" cy=\"7\" r=\"4\"/>"
            "<path d=\"M22 21v-2a4 4 0 0 0-3-3.87M16 3.13a4 4 0 0 1 0 7.75\"/></symbol>\n"
        "  <symbol id=\"i-bars\" viewBox=\"0 0 24 24\">"
            "<path d=\"M3 3v18h18\"/><path d=\"M18 17V9M13 17V5M8 17v-3\"/></symbol>\n"
        "  <symbol id=\"i-gauge\" viewBox=\"0 0 24 24\">"
            "<path d=\"M12 14 4 6\"/><path d=\"M3.34 19a10 10 0 1 1 17.32 0\"/></symbol>\n"
        "  <symbol id=\"i-download\" viewBox=\"0 0 24 24\">"
            "<path d=\"M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4\"/>"
            "<path d=\"M7 10l5 5 5-5M12 15V3\"/></symbol>\n"
        "  <symbol id=\"i-gear\" viewBox=\"0 0 24 24\">"
            "<circle cx=\"12\" cy=\"12\" r=\"3\"/>"
            "<path d=\"M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 1 1-2.83 2.83"
            "l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-4 0"
            "v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06"
            "a2 2 0 1 1-2.83-2.83l.06-.06A1.65 1.65 0 0 0 4.6 9"
            "a1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1 0-4h.09A1.65 1.65 0 0 0 4.6 9\"/></symbol>\n"
        "  <symbol id=\"i-search\" viewBox=\"0 0 24 24\">"
            "<circle cx=\"11\" cy=\"11\" r=\"8\"/><path d=\"m21 21-4.3-4.3\"/></symbol>\n"
        "  <symbol id=\"i-bell\" viewBox=\"0 0 24 24\">"
            "<path d=\"M6 8a6 6 0 0 1 12 0c0 7 3 9 3 9H3s3-2 3-9\"/>"
            "<path d=\"M10.3 21a1.94 1.94 0 0 0 3.4 0\"/></symbol>\n"
        "  <symbol id=\"i-alert\" viewBox=\"0 0 24 24\">"
            "<path d=\"M10.29 3.86 1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3"
            "L13.71 3.86a2 2 0 0 0-3.42 0z\"/>"
            "<path d=\"M12 9v4M12 17h.01\"/></symbol>\n"
        "  <symbol id=\"i-euro\" viewBox=\"0 0 24 24\">"
            "<path d=\"M4 10h12M4 14h12\"/>"
            "<path d=\"M19.5 9a6.5 6.5 0 1 0 0 6\"/></symbol>\n"
        "  <symbol id=\"i-dots\" viewBox=\"0 0 24 24\">"
            "<circle cx=\"5\" cy=\"12\" r=\"1.6\"/>"
            "<circle cx=\"12\" cy=\"12\" r=\"1.6\"/>"
            "<circle cx=\"19\" cy=\"12\" r=\"1.6\"/></symbol>\n"
        "</svg>\n",
        fp);

    /* ---- TOPBAR ---- */
    fputs(
        "<header class=\"topbar\">\n"
        "  <div class=\"brand\">\n"
        "    <span class=\"mark\"><svg viewBox=\"0 0 24 24\" class=\"ic\">"
            "<use href=\"#i-wrench\"/></svg></span>\n"
        "    <span class=\"name\">Assistenza Tecnica "
            "<span>&#xB7; Dashboard</span></span>\n"
        "  </div>\n"
        "  <div class=\"head-right\">\n"
        "    <label class=\"search\">\n"
        "      <svg class=\"ic ic-16\"><use href=\"#i-search\"/></svg>\n"
        "      <input type=\"text\" placeholder=\"Cerca interventi, tecnici\xe2\x80\xa6\""
            " id=\"searchInput\" oninput=\"filterTable()\">\n"
        "    </label>\n",
        fp);

    if (tm_now != NULL)
        fprintf(fp, "    <span class=\"datechip\">%02d/%02d/%04d &middot; %02d:%02d</span>\n",
            tm_now->tm_mday, tm_now->tm_mon+1, tm_now->tm_year+1900,
            tm_now->tm_hour, tm_now->tm_min);

    fputs(
        "    <button class=\"iconbtn\" aria-label=\"Notifiche\">"
            "<svg class=\"ic\"><use href=\"#i-bell\"/></svg>"
            "<span class=\"ping\"></span></button>\n"
        "    <button class=\"avatar-btn\">CA</button>\n"
        "  </div>\n"
        "</header>\n"
        "<div class=\"app-body\">\n",
        fp);

    /* ---- SIDEBAR ---- */
    fprintf(fp,
        "<aside class=\"sidebar\">\n"
        "  <div class=\"nav-sec\">Operazioni</div>\n"
        "  <a class=\"nav-item active\">"
            "<svg class=\"ic\"><use href=\"#i-grid\"/></svg>Dashboard</a>\n"
        "  <a class=\"nav-item\">"
            "<svg class=\"ic\"><use href=\"#i-wrench\"/></svg>Richieste"
            "<span class=\"badge\">%d</span></a>\n"
        "  <a class=\"nav-item\">"
            "<svg class=\"ic\"><use href=\"#i-users\"/></svg>Tecnici</a>\n"
        "  <div class=\"nav-sec\">Report</div>\n"
        "  <a class=\"nav-item\">"
            "<svg class=\"ic\"><use href=\"#i-bars\"/></svg>Statistiche</a>\n"
        "  <a class=\"nav-item\">"
            "<svg class=\"ic\"><use href=\"#i-gauge\"/></svg>Report SLA</a>\n"
        "  <a class=\"nav-item\">"
            "<svg class=\"ic\"><use href=\"#i-download\"/></svg>Esportazioni</a>\n"
        "  <div class=\"nav-sec\">Sistema</div>\n"
        "  <a class=\"nav-item\">"
            "<svg class=\"ic\"><use href=\"#i-gear\"/></svg>Impostazioni</a>\n"
        "  <div class=\"side-foot\">"
            "<div class=\"who\"><span class=\"av\">CA</span>"
            "<div><b>Centro Assistenza</b><small>Sistema v1.0</small></div>"
            "</div></div>\n"
        "</aside>\n",
        nr);

    /* ---- MAIN ---- */
    fputs("<main class=\"main\">\n", fp);

    if (tm_now != NULL)
        fprintf(fp,
            "<div class=\"page-head\">\n"
            "  <div><h1>Dashboard Operativa</h1>\n"
            "  <div class=\"sub\">Generata il %02d/%02d/%04d alle %02d:%02d</div></div>\n"
            "  <span class=\"spacer\"></span>\n"
            "  <button class=\"btn\" onclick=\"window.print()\">"
                "<svg class=\"ic ic-16\"><use href=\"#i-download\"/></svg>Stampa</button>\n"
            "</div>\n",
            tm_now->tm_mday, tm_now->tm_mon+1, tm_now->tm_year+1900,
            tm_now->tm_hour, tm_now->tm_min);

    /* ---- KPI ---- */
    fprintf(fp,
        "<section class=\"kpis\">\n"
        "  <div class=\"kpi\"><div class=\"top\"><span class=\"ico\">"
            "<svg class=\"ic ic-16\"><use href=\"#i-wrench\"/></svg></span>"
            "Richieste totali</div>\n"
        "  <div class=\"num\">%d</div>\n"
        "  <div class=\"foot\"><span class=\"acc\">%d attive</span> &middot; %d chiuse</div></div>\n"
        "  <div class=\"kpi\"><div class=\"top\"><span class=\"ico\">"
            "<svg class=\"ic ic-16\"><use href=\"#i-users\"/></svg></span>Tecnici</div>\n"
        "  <div class=\"num\">%d</div>\n"
        "  <div class=\"foot\"><span class=\"acc\">%d occupati</span> &middot; %d liberi</div></div>\n"
        "  <div class=\"kpi warn\"><div class=\"top\"><span class=\"ico\">"
            "<svg class=\"ic ic-16\"><use href=\"#i-alert\"/></svg></span>Non assegnate</div>\n"
        "  <div class=\"num\">%d</div>\n"
        "  <div class=\"foot\">%s</div></div>\n"
        "  <div class=\"kpi ok\"><div class=\"top\"><span class=\"ico\">"
            "<svg class=\"ic ic-16\"><use href=\"#i-euro\"/></svg></span>Costo finale</div>\n"
        "  <div class=\"num\">&euro; %.0f</div>\n"
        "  <div class=\"foot\"><span class=\"ok\">su &euro; %.0f stimati</span></div></div>\n"
        "</section>\n",
        nr, attive, cnt_stato[4],
        nt, occ, nt - occ,
        non_ass,
        non_ass > 0 ? "<span class=\"warnt\">da smistare subito</span>" : "nessuna urgenza",
        tot_finale, tot_stimato);

    /* ---- MID ROW ---- */
    fputs("<section class=\"mid\">\n", fp);

    /* Left card: urgenze + carico tecnici */
    fputs(
        "<div class=\"card\">\n"
        "  <div class=\"card-head\"><h2>Urgenze &amp; non assegnati</h2>"
            "<span class=\"meta\">priorit&agrave; alta / aperte</span>"
            "<span class=\"spacer\"></span></div>\n"
        "  <div class=\"urg-list\">\n",
        fp);

    urg_cnt = 0;
    /* Prima passata: URGENTE */
    for (i = 0; i < nr && urg_cnt < 3; i++) {
        r = richieste[i];
        if (r == NULL) continue;
        if (richiesta_get_priorita(r) != PRIORITA_URGENTE) continue;
        if (richiesta_get_stato(r) == STATO_CHIUSA ||
            richiesta_get_stato(r) == STATO_ANNULLATA) continue;
        data_r = richiesta_get_data_apertura(r);
        tmi    = localtime(&data_r);
        fprintf(fp,
            "    <div class=\"urg\"><span class=\"tag urgent dot\">URGENTE</span>\n"
            "      <div class=\"desc\">%s &mdash; %s"
                "<small>%s",
            richiesta_get_codice(r), richiesta_get_cliente(r),
            richiesta_get_dispositivo(r));
        if (tmi) fprintf(fp, " &middot; %02d/%02d/%04d",
            tmi->tm_mday, tmi->tm_mon+1, tmi->tm_year+1900);
        fprintf(fp, "</small></div>\n"
            "      <span class=\"s-pill %s\"><span class=\"dot\"></span>%s</span>"
            "</div>\n",
            SCLS[(int)richiesta_get_stato(r)], SLBL[(int)richiesta_get_stato(r)]);
        urg_cnt++;
    }
    /* Seconda passata: ALTA */
    for (i = 0; i < nr && urg_cnt < 3; i++) {
        r = richieste[i];
        if (r == NULL) continue;
        if (richiesta_get_priorita(r) != PRIORITA_ALTA) continue;
        if (richiesta_get_stato(r) == STATO_CHIUSA ||
            richiesta_get_stato(r) == STATO_ANNULLATA) continue;
        data_r = richiesta_get_data_apertura(r);
        tmi    = localtime(&data_r);
        fprintf(fp,
            "    <div class=\"urg\"><span class=\"tag high dot\">ALTA</span>\n"
            "      <div class=\"desc\">%s &mdash; %s"
                "<small>%s",
            richiesta_get_codice(r), richiesta_get_cliente(r),
            richiesta_get_dispositivo(r));
        if (tmi) fprintf(fp, " &middot; %02d/%02d/%04d",
            tmi->tm_mday, tmi->tm_mon+1, tmi->tm_year+1900);
        fprintf(fp, "</small></div>\n"
            "      <span class=\"s-pill %s\"><span class=\"dot\"></span>%s</span>"
            "</div>\n",
            SCLS[(int)richiesta_get_stato(r)], SLBL[(int)richiesta_get_stato(r)]);
        urg_cnt++;
    }
    if (urg_cnt == 0)
        fputs("    <div class=\"urg\"><div class=\"desc\">Nessuna urgenza attiva"
              "<small>Tutte le richieste sono sotto controllo</small></div></div>\n", fp);

    fputs("  </div>\n"
          "  <div class=\"divider\"></div>\n"
          "  <div class=\"card-head\" style=\"margin-bottom:14px\">"
              "<h2>Carico tecnici</h2>"
              "<span class=\"meta\">slot occupati</span></div>\n"
          "  <div class=\"bars\">\n",
          fp);

    for (i = 0; i < nt; i++) {
        tec = tecnici[i];
        if (tec == NULL) continue;
        assegnate = tecnico_get_assegnate(tec);
        max_r     = tecnico_get_max_richieste(tec);
        pct       = (max_r > 0) ? (assegnate * 100 / max_r) : 0;
        fprintf(fp,
            "    <div class=\"bar\">"
                "<span class=\"lbl\" style=\"font-size:12.5px;font-weight:500;color:var(--ink)\">"
                "%s</span>"
                "<span class=\"track\"><span class=\"%s\" style=\"width:%d%%\"></span></span>"
                "<span class=\"pct\">%d/%d</span></div>\n",
            tecnico_get_nome(tec),
            pct >= 100 ? "fill-red" : pct >= 60 ? "fill-orange" :
            pct > 0    ? "fill-accent" : "fill-green",
            pct, assegnate, max_r);
    }
    fputs("  </div>\n</div>\n", fp); /* end left card */

    /* Right column: donut + priority */
    fputs("<div style=\"display:flex;flex-direction:column;gap:var(--gap)\">\n", fp);

    /* Donut chart */
    fputs(
        "<div class=\"card\">\n"
        "  <div class=\"card-head\"><h2>Riepilogo richieste</h2>"
            "<span class=\"spacer\"></span>"
            "<span class=\"meta\">per stato</span></div>\n"
        "  <div class=\"donut-wrap\">\n"
        "    <div class=\"donut\">\n"
        "      <svg viewBox=\"0 0 120 120\" width=\"128\" height=\"128\">\n"
        "        <circle cx=\"60\" cy=\"60\" r=\"42\" fill=\"none\""
            " stroke=\"#ececef\" stroke-width=\"16\"/>\n",
        fp);

    circ = 2.0 * 3.14159265358979 * 42.0;
    off  = 0.0;
    /* segments: assegnata(1), in_corso(2), aperta(0), chiusa(4), sospesa+annullata(3+5) */
    s = (nr > 0) ? ((double)cnt_stato[1] / nr) * circ : 0.0;
    if (s > 0.01) {
        fprintf(fp, "        <circle cx=\"60\" cy=\"60\" r=\"42\" fill=\"none\""
            " stroke=\"#af52de\" stroke-width=\"16\""
            " stroke-dasharray=\"%.2f %.2f\" stroke-dashoffset=\"%.2f\""
            " stroke-linecap=\"round\"/>\n", s, circ - s, -off);
        off += s;
    }
    s = (nr > 0) ? ((double)cnt_stato[2] / nr) * circ : 0.0;
    if (s > 0.01) {
        fprintf(fp, "        <circle cx=\"60\" cy=\"60\" r=\"42\" fill=\"none\""
            " stroke=\"#ff9f0a\" stroke-width=\"16\""
            " stroke-dasharray=\"%.2f %.2f\" stroke-dashoffset=\"%.2f\""
            " stroke-linecap=\"round\"/>\n", s, circ - s, -off);
        off += s;
    }
    s = (nr > 0) ? ((double)cnt_stato[0] / nr) * circ : 0.0;
    if (s > 0.01) {
        fprintf(fp, "        <circle cx=\"60\" cy=\"60\" r=\"42\" fill=\"none\""
            " stroke=\"#ff3b30\" stroke-width=\"16\""
            " stroke-dasharray=\"%.2f %.2f\" stroke-dashoffset=\"%.2f\""
            " stroke-linecap=\"round\"/>\n", s, circ - s, -off);
        off += s;
    }
    s = (nr > 0) ? ((double)cnt_stato[4] / nr) * circ : 0.0;
    if (s > 0.01) {
        fprintf(fp, "        <circle cx=\"60\" cy=\"60\" r=\"42\" fill=\"none\""
            " stroke=\"#34c759\" stroke-width=\"16\""
            " stroke-dasharray=\"%.2f %.2f\" stroke-dashoffset=\"%.2f\""
            " stroke-linecap=\"round\"/>\n", s, circ - s, -off);
        off += s;
    }
    s = (nr > 0) ? ((double)(cnt_stato[3] + cnt_stato[5]) / nr) * circ : 0.0;
    if (s > 0.01)
        fprintf(fp, "        <circle cx=\"60\" cy=\"60\" r=\"42\" fill=\"none\""
            " stroke=\"#c7c7cc\" stroke-width=\"16\""
            " stroke-dasharray=\"%.2f %.2f\" stroke-dashoffset=\"%.2f\""
            " stroke-linecap=\"round\"/>\n", s, circ - s, -off);

    fprintf(fp,
        "      </svg>\n"
        "      <div class=\"ctr\"><b>%d</b><small>totali</small></div>\n"
        "    </div>\n"
        "    <div class=\"legend\">\n"
        "      <div class=\"row\"><span class=\"sw\" style=\"background:#af52de\"></span>"
            "Assegnate<span class=\"cnt\">%d%%</span><b>%d</b></div>\n"
        "      <div class=\"row\"><span class=\"sw\" style=\"background:#ff9f0a\"></span>"
            "In Corso<span class=\"cnt\">%d%%</span><b>%d</b></div>\n"
        "      <div class=\"row\"><span class=\"sw\" style=\"background:#ff3b30\"></span>"
            "Aperte<span class=\"cnt\">%d%%</span><b>%d</b></div>\n"
        "      <div class=\"row\"><span class=\"sw\" style=\"background:#34c759\"></span>"
            "Chiuse<span class=\"cnt\">%d%%</span><b>%d</b></div>\n"
        "      <div class=\"row\"><span class=\"sw\" style=\"background:#ececef\"></span>"
            "Sosp./Ann.<span class=\"cnt\">%d%%</span><b>%d</b></div>\n"
        "    </div>\n  </div>\n</div>\n",
        nr,
        nr > 0 ? cnt_stato[1]*100/nr : 0, cnt_stato[1],
        nr > 0 ? cnt_stato[2]*100/nr : 0, cnt_stato[2],
        nr > 0 ? cnt_stato[0]*100/nr : 0, cnt_stato[0],
        nr > 0 ? cnt_stato[4]*100/nr : 0, cnt_stato[4],
        nr > 0 ? (cnt_stato[3]+cnt_stato[5])*100/nr : 0, cnt_stato[3]+cnt_stato[5]);

    /* Priority bars */
    fprintf(fp,
        "<div class=\"card\">\n"
        "  <div class=\"card-head\"><h2>Distribuzione priorit&agrave;</h2></div>\n"
        "  <div class=\"bars\">\n"
        "    <div class=\"bar\"><span class=\"lbl\" style=\"color:var(--red-ink);font-weight:600\">"
            "Urgente</span><span class=\"track\"><span class=\"fill-red\" style=\"width:%d%%\">"
            "</span></span><span class=\"pct\">%d</span></div>\n"
        "    <div class=\"bar\"><span class=\"lbl\" style=\"color:var(--orange-ink);font-weight:600\">"
            "Alta</span><span class=\"track\"><span class=\"fill-orange\" style=\"width:%d%%\">"
            "</span></span><span class=\"pct\">%d</span></div>\n"
        "    <div class=\"bar\"><span class=\"lbl\" style=\"color:#0050d0;font-weight:600\">"
            "Media</span><span class=\"track\"><span class=\"fill-accent\" style=\"width:%d%%\">"
            "</span></span><span class=\"pct\">%d</span></div>\n"
        "    <div class=\"bar\"><span class=\"lbl\" style=\"color:var(--green-ink);font-weight:600\">"
            "Bassa</span><span class=\"track\"><span class=\"fill-green\" style=\"width:%d%%\">"
            "</span></span><span class=\"pct\">%d</span></div>\n"
        "  </div>\n</div>\n",
        nr > 0 ? cnt_prior[3]*100/nr : 0, cnt_prior[3],
        nr > 0 ? cnt_prior[2]*100/nr : 0, cnt_prior[2],
        nr > 0 ? cnt_prior[1]*100/nr : 0, cnt_prior[1],
        nr > 0 ? cnt_prior[0]*100/nr : 0, cnt_prior[0]);

    fputs("</div>\n</section>\n", fp); /* end right col + mid */

    /* ---- TABELLA RICHIESTE ---- */
    fprintf(fp,
        "<section class=\"card tbl-card\">\n"
        "  <div class=\"card-head\"><h2>Elenco Richieste</h2>"
            "<span class=\"meta\">%d interventi</span>"
            "<span class=\"spacer\"></span></div>\n"
        "  <table class=\"tbl\" id=\"richiesteTable\">\n"
        "  <thead><tr><th>#</th><th>Codice</th><th>Cliente</th><th>Dispositivo</th>"
            "<th>Priorit&agrave;</th><th>Stato</th>"
            "<th>Stim.</th><th>Fin.</th><th>Data</th></tr></thead>\n"
        "  <tbody>\n",
        nr);

    for (i = 0; i < nr; i++) {
        int pi_idx, si_idx;
        r = richieste[i];
        if (r == NULL) continue;
        data_r = richiesta_get_data_apertura(r);
        tmi    = localtime(&data_r);

        pi_idx = (int)richiesta_get_priorita(r);
        si_idx = (int)richiesta_get_stato(r);
        if (pi_idx < 0 || pi_idx > 3) pi_idx = 0;
        if (si_idx < 0 || si_idx > 5) si_idx = 0;

        fprintf(fp,
            "  <tr>\n"
            "    <td class=\"dt\">%d</td>\n"
            "    <td class=\"id\">%s</td>\n"
            "    <td class=\"nm\">%s</td>\n"
            "    <td class=\"device\">%s</td>\n"
            "    <td><span class=\"s-pill %s\"><span class=\"dot\"></span>%s</span></td>\n"
            "    <td><span class=\"s-pill %s\"><span class=\"dot\"></span>%s</span></td>\n"
            "    <td class=\"cost\">&euro; %.2f</td>\n",
            i + 1,
            richiesta_get_codice(r), richiesta_get_cliente(r),
            richiesta_get_dispositivo(r),
            PCLS[pi_idx], PLBL[pi_idx],
            SCLS[si_idx], SLBL[si_idx],
            richiesta_get_costo_stimato(r));

        if (richiesta_get_costo_finale(r) > 0.0)
            fprintf(fp, "    <td class=\"cost ok\">&euro; %.2f</td>\n",
                    richiesta_get_costo_finale(r));
        else
            fputs("    <td class=\"cost dash\">&mdash;</td>\n", fp);

        if (tmi != NULL)
            fprintf(fp, "    <td class=\"dt\">%02d/%02d/%04d</td>\n",
                    tmi->tm_mday, tmi->tm_mon+1, tmi->tm_year+1900);
        else
            fputs("    <td class=\"dt\">N/D</td>\n", fp);

        fputs("  </tr>\n", fp);
    }
    fputs("  </tbody>\n  </table>\n</section>\n", fp);

    /* ---- TABELLA TECNICI ---- */
    fprintf(fp,
        "<section class=\"card tbl-card\">\n"
        "  <div class=\"card-head\"><h2>Elenco Tecnici</h2>"
            "<span class=\"meta\">%d in organico</span>"
            "<span class=\"spacer\"></span></div>\n"
        "  <table class=\"tbl\">\n"
        "  <thead><tr><th>Codice</th><th>Tecnico</th><th>Specializzazione</th>"
            "<th>Carico</th><th>Avanzamento</th><th>Richieste</th></tr></thead>\n"
        "  <tbody>\n",
        nt);

    for (i = 0; i < nt; i++) {
        tec = tecnici[i];
        if (tec == NULL) continue;
        assegnate = tecnico_get_assegnate(tec);
        max_r     = tecnico_get_max_richieste(tec);
        pct       = (max_r > 0) ? (assegnate * 100 / max_r) : 0;

        /* Initials from name */
        ini[0] = ini[1] = ini[2] = '\0';
        ini_n = 0;
        for (np = tecnico_get_nome(tec); *np && ini_n < 2; np++) {
            if ((np == tecnico_get_nome(tec) || *(np-1) == ' ') && *np != ' ')
                ini[ini_n++] = (*np >= 'a' && *np <= 'z') ? (char)(*np-32) : *np;
        }

        fprintf(fp,
            "  <tr>\n"
            "    <td class=\"id\">%s</td>\n"
            "    <td><div class=\"cellrow\">"
                "<span class=\"av-sm %s\">%s</span>"
                "<span class=\"nm\">%s</span></div></td>\n"
            "    <td class=\"device\">%s</td>\n"
            "    <td class=\"dt\" style=\"font-weight:600;color:%s\">%d / %d slot</td>\n"
            "    <td><div class=\"prog\">"
                "<span class=\"track\">"
                "<span style=\"width:%d%%;background:%s\"></span>"
                "</span><span class=\"v\">%d%%</span></div></td>\n"
            "    <td>",
            tecnico_get_codice(tec),
            AV_COL[i % 6], ini, tecnico_get_nome(tec),
            tecnico_get_specializzazione(tec),
            pct >= 100 ? "var(--red-ink)" : pct >= 60 ? "var(--orange-ink)" : "var(--green-ink)",
            assegnate, max_r,
            pct, pct >= 100 ? "var(--red)" : pct >= 60 ? "var(--orange)" : "var(--green)",
            pct);

        if (assegnate == 0) {
            fputs("<span class=\"dt\">&mdash;</span>", fp);
        } else {
            fputs("<span class=\"id\" style=\"font-size:12.5px\">", fp);
            for (j = 0; j < assegnate; j++) {
                r = tecnico_get_richiesta(tec, j);
                if (r != NULL) {
                    if (j > 0) fputs(", ", fp);
                    fputs(richiesta_get_codice(r), fp);
                }
            }
            fputs("</span>", fp);
        }
        fputs("</td>\n  </tr>\n", fp);
    }
    fputs("  </tbody>\n  </table>\n</section>\n", fp);

    /* ---- JS + fine ---- */
    fputs(
        "</main>\n</div>\n"
        "<script>\n"
        "function filterTable(){\n"
        "  var q=document.getElementById('searchInput').value.toLowerCase();\n"
        "  document.querySelectorAll('#richiesteTable tbody tr').forEach(function(row){\n"
        "    row.style.display=row.textContent.toLowerCase().includes(q)?'':'none';\n"
        "  });\n"
        "}\n"
        "document.querySelectorAll('.nav-item').forEach(function(item){\n"
        "  item.addEventListener('click',function(){\n"
        "    document.querySelectorAll('.nav-item').forEach(function(n){"
            "n.classList.remove('active');});\n"
        "    item.classList.add('active');\n"
        "  });\n"
        "});\n"
        "</script>\n"
        "</body>\n</html>\n",
        fp);

    fclose(fp);
    return 0;
}
