/**
 * @file main.c miao
 * @brief Punto di ingresso del sistema di gestione del centro di assistenza.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "colori.h"
#include "gestione_file.h"
#include "logica.h"
#include "richiesta.h"
#include "tecnico.h"

#define BUF 300

/* =========================================================================
 * Prototipi
 * ========================================================================= */

static void cls(void);
static void sep_doppio(void);
static void sep_singolo(void);
static void pausa(void);
static int leggi_scelta(const char *prompt);
static void leggi_stringa(char *dst, int max, const char *prompt);
static int parse_int(const char *s);
static double parse_double(const char *s);
static void dealloca_tutto(Richiesta **r, int nr, Tecnico **t, int nt);

static void schermo_iniziale(void);
static void titolo_menu_principale(void);
static void titolo_richieste(void);
static void titolo_tecnici(void);
static void titolo_ricerca(void);
static void titolo_report(void);
static void titolo_file(void);
static void titolo_statistiche(void);
static void titolo_web(void);

static void menu_richieste(Richiesta **r, int *nr, int max_r);
static void menu_tecnici(Richiesta **r, int nr, Tecnico **t, int *nt,
                         int max_t);
static void menu_ricerca(Richiesta **r, int nr, Tecnico **t, int nt);
static void menu_report(Richiesta **r, int nr, Tecnico **t, int nt);
static void menu_file(Richiesta **r, int *nr, int max_r, Tecnico **t, int *nt,
                      int max_t);
static void menu_statistiche(Richiesta **r, int nr, Tecnico **t, int nt);
static void menu_web(void);

/* =========================================================================
 * Punto di ingresso
 * ========================================================================= */

int main(void) {
  setvbuf(stdout, NULL, _IONBF, 0);
  Richiesta *richieste[MAX_RICHIESTE_SISTEMA];
  Tecnico *tecnici[MAX_TECNICI_SISTEMA];
  int n_richieste = 0, n_tecnici = 0, esci = 0, scelta, i;

  for (i = 0; i < MAX_RICHIESTE_SISTEMA; i++)
    richieste[i] = NULL;
  for (i = 0; i < MAX_TECNICI_SISTEMA; i++)
    tecnici[i] = NULL;

  schermo_iniziale();

  printf(BCYN "  Caricamento dati da file...\n\n" RST);
  if (carica_richieste(FILE_RICHIESTE, richieste, &n_richieste,
                       MAX_RICHIESTE_SISTEMA) == 0)
    printf(BGRN "  [OK] " RST "%d richieste caricate da " YEL "'%s'" RST ".\n",
           n_richieste, FILE_RICHIESTE);
  else
    printf(DIM "  [--] '%s' non trovato. Partenza con lista vuota.\n" RST,
           FILE_RICHIESTE);

  if (carica_tecnici(FILE_TECNICI, tecnici, &n_tecnici, MAX_TECNICI_SISTEMA,
                     richieste, n_richieste) == 0)
    printf(BGRN "  [OK] " RST "%d tecnici caricati da " YEL "'%s'" RST ".\n",
           n_tecnici, FILE_TECNICI);
  else
    printf(DIM "  [--] '%s' non trovato. Partenza con lista vuota.\n" RST,
           FILE_TECNICI);

  pausa();

  while (!esci) {
    cls();
    titolo_menu_principale();

    printf(BBLU "  Richieste: " BOLD BWHT "%d   " RST BBLU "Tecnici: " BOLD BWHT
                "%d\n\n" RST,
           n_richieste, n_tecnici);

    printf(BYEL "  1" RST " . Gestione Richieste\n");
    printf(BYEL "  2" RST " . Gestione Tecnici\n");
    printf(BYEL "  3" RST " . Ricerca\n");
    printf(BYEL "  4" RST " . Report e Dashboard HTML\n");
    printf(BYEL "  5" RST " . Salvataggio e Caricamento File\n");
    printf(BYEL "  6" RST " . Statistiche\n");
    printf(BYEL "  7" RST " . Consultazione Web\n");
    printf(DIM "  0  . Esci\n\n" RST);
    sep_singolo();

    scelta = leggi_scelta(BOLD "  Scelta: " RST);

    switch (scelta) {
    case 1:
      menu_richieste(richieste, &n_richieste, MAX_RICHIESTE_SISTEMA);
      break;
    case 2:
      menu_tecnici(richieste, n_richieste, tecnici, &n_tecnici,
                   MAX_TECNICI_SISTEMA);
      break;
    case 3:
      menu_ricerca(richieste, n_richieste, tecnici, n_tecnici);
      break;
    case 4:
      menu_report(richieste, n_richieste, tecnici, n_tecnici);
      break;
    case 5:
      menu_file(richieste, &n_richieste, MAX_RICHIESTE_SISTEMA, tecnici,
                &n_tecnici, MAX_TECNICI_SISTEMA);
      break;
    case 6:
      menu_statistiche(richieste, n_richieste, tecnici, n_tecnici);
      break;
    case 7:
      menu_web();
      break;
    case 0:
      printf("\n" BCYN "  Salvataggio dati in corso...\n" RST);
      if (salva_richieste(FILE_RICHIESTE, richieste, n_richieste) == 0)
        printf(BGRN "  [OK] " RST "%d richieste salvate su '%s'.\n",
               n_richieste, FILE_RICHIESTE);
      else
        printf(BRED "  [ATTENZIONE] " RST
                    "Salvataggio richieste non riuscito.\n");
      if (salva_tecnici(FILE_TECNICI, tecnici, n_tecnici) == 0)
        printf(BGRN "  [OK] " RST "%d tecnici salvati su '%s'.\n", n_tecnici,
               FILE_TECNICI);
      else
        printf(BRED "  [ATTENZIONE] " RST
                    "Salvataggio tecnici non riuscito.\n");
      esci = 1;
      break;
    default:
      printf("\n" BRED "  [ERRORE] " RST
             "Scelta non valida. Inserire un numero tra 0 e 7.\n");
      pausa();
      break;
    }
  }

  dealloca_tutto(richieste, n_richieste, tecnici, n_tecnici);
  printf("\n" BBLU "  Arrivederci!\n\n" RST);
  return 0;
}

/* =========================================================================
 * Helper di interfaccia
 * ========================================================================= */

static void cls(void) { printf("\033[2J\033[H"); }

static void sep_doppio(void) {
  printf(BBLU "  \xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
              "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
              "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
              "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
              "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
              "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
              "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
              "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
              "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
              "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
              "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
              "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
              "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
              "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
              "\xe2\x95\x90\n" RST);
}

static void sep_singolo(void) {
  printf(DIM "  "
             "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
             "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
             "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
             "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
             "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
             "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
             "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
             "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
             "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
             "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
             "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
             "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
             "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
             "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
             "\xe2\x94\x80\n" RST);
}

static void pausa(void) {
  char buf[256];
  printf("\n" DIM "  Premi INVIO per continuare..." RST);
  fgets(buf, 256, stdin);
}

static int leggi_scelta(const char *prompt) {
  char buf[32];
  if (prompt)
    printf("%s", prompt);
  if (fgets(buf, 32, stdin) == NULL)
    return -1;
  if (buf[0] >= '0' && buf[0] <= '9')
    return (int)(buf[0] - '0');
  return -1;
}

static void leggi_stringa(char *dst, int max, const char *prompt) {
  int i;
  if (!dst || max <= 0)
    return;
  if (prompt)
    printf("%s", prompt);
  if (fgets(dst, max, stdin) == NULL) {
    dst[0] = '\0';
    return;
  }
  for (i = 0; dst[i]; i++)
    if (dst[i] == '\n' || dst[i] == '\r') {
      dst[i] = '\0';
      return;
    }
}

static int parse_int(const char *s) {
  int v = 0, i = 0;
  if (!s)
    return 0;
  for (; s[i] >= '0' && s[i] <= '9'; i++)
    v = v * 10 + (s[i] - '0');
  return v;
}

static double parse_double(const char *s) {
  double intero = 0.0, fraz = 0.0, div = 10.0;
  int i = 0;
  if (!s)
    return 0.0;
  for (; s[i] >= '0' && s[i] <= '9'; i++)
    intero = intero * 10.0 + (double)(s[i] - '0');
  if (s[i] == '.' || s[i] == ',') {
    i++;
    for (; s[i] >= '0' && s[i] <= '9'; i++) {
      fraz += (double)(s[i] - '0') / div;
      div *= 10.0;
    }
  }
  return intero + fraz;
}

/**
 * @brief Verifica che la stringa contenga almeno una lettera (a-z, A-Z).
 *
 * Utilizzata per validare campi testuali come nomi, descrizioni,
 * dispositivi e specializzazioni: non possono essere composti
 * esclusivamente da numeri o simboli.
 *
 * @param s Stringa da verificare (non NULL).
 * @return 1 se contiene almeno una lettera, 0 altrimenti.
 */
static int contiene_lettera(const char *s) {
  int i;
  if (!s)
    return 0;
  for (i = 0; s[i] != '\0'; i++) {
    if ((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z'))
      return 1;
  }
  return 0;
}

/**
 * @brief Verifica che la stringa contenga solo caratteri alfanumerici,
 *        trattini (-) e underscore (_).
 *
 * Utilizzata per validare i codici identificativi (richieste e tecnici),
 * che non devono contenere spazi ne' caratteri speciali.
 *
 * @param s Stringa da verificare (non NULL).
 * @return 1 se la stringa e' valida come codice, 0 altrimenti.
 */
static int solo_alfanumerico_o_trattino(const char *s) {
  int i;
  if (!s || s[0] == '\0')
    return 0;
  for (i = 0; s[i] != '\0'; i++) {
    if (!((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z') ||
          (s[i] >= '0' && s[i] <= '9') || s[i] == '-' || s[i] == '_'))
      return 0;
  }
  return 1;
}

/**
 * @brief Verifica che la stringa rappresenti un numero valido (>= 0).
 *
 * Accetta cifre con un eventuale separatore decimale ('.' o ',').
 * Rifiuta stringhe vuote, contenenti solo il separatore, o con
 * caratteri non numerici.
 *
 * @param s Stringa da verificare (non NULL).
 * @return 1 se la stringa e' un numero valido, 0 altrimenti.
 */
static int e_numero_valido(const char *s) {
  int i = 0, cifre = 0, punti = 0;
  if (!s || s[0] == '\0')
    return 0;
  for (i = 0; s[i] != '\0'; i++) {
    if (s[i] >= '0' && s[i] <= '9') {
      cifre++;
    } else if (s[i] == '.' || s[i] == ',') {
      punti++;
      if (punti > 1)
        return 0;
    } else {
      return 0;
    }
  }
  return cifre > 0;
}

static void dealloca_tutto(Richiesta **r, int nr, Tecnico **t, int nt) {
  int i;
  for (i = 0; i < nt; i++)
    tecnico_distruggi(&t[i]);
  for (i = 0; i < nr; i++)
    richiesta_distruggi(&r[i]);
}

/* =========================================================================
 * Schermata iniziale
 * ========================================================================= */

static void schermo_iniziale(void) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  cls();
  printf("\n");
  sep_doppio();
  printf("\n");
  /* ASCII-art: CENTRO  (figlet font "small") */
  printf(BOLD BBLU "   ___  ___  _  _  ____  ____  ___ \n"
                   "  / __|| __|| \\| ||_   _||  _ \\/ _ \\\n"
                   " | (__ | _| |  ' |  | |  |   /| (_) |\n"
                   "  \\___||___||_|\\_|  |_|  |_|_\\ \\___/\n" RST);
  printf("\n");
  printf(BOLD BMAG "       D I   A S S I S T E N Z A\n" RST);
  printf(DIM "        Sistema di Gestione  v1.0\n" RST);
  printf("\n");

  if (t != NULL) {
    printf(BYEL "   Data: %02d/%02d/%04d    Ora: %02d:%02d\n" RST, t->tm_mday,
           t->tm_mon + 1, t->tm_year + 1900, t->tm_hour, t->tm_min);
  }
  printf("\n");
  sep_doppio();
  printf("\n");
}

/* =========================================================================
 * Titoli sezioni  — ASCII art (figlet "small")
 * ========================================================================= */

static void titolo_menu_principale(void) {
  sep_doppio();
  printf(BOLD BYEL
         "   __  __ ___ _  _ _   _   ___ ___ ___ _  _  ___ ___ ___  _   _    ___ \n"
         "  |  \\/  | __| \\| | | | | | _ \\ _ \\_ _| \\| |/ __|_ _| _ \\/_\\ | |  | __|\n"
         "  | |\\/| | _|| .` | |_| | |  _/   /| || .` | (__ | ||  _/ _ \\| |__| _| \n"
         "  |_|  |_|___|_|\\_|\\___/  |_| |_|_\\___|_|\\_|\\___|___|_|/_/ \\_\\____|___|\n"
         RST);
  sep_doppio();
  printf("\n");
}

static void titolo_richieste(void) {
  sep_doppio();
  printf(BOLD BMAG
         "   ___ ___ ___ _  _ ___ ___ ___ _____ ___ \n"
         "  | _ \\_ _/ __| || |_ _| __/ __|_   _| __|\n"
         "  |   /| | (__| __ || || _|\\__ \\ | | | _| \n"
         "  |_|_\\___\\___|_||_|___|___|___/ |_| |___|\n"
         RST);
  printf(MAG "  Gestione completa delle richieste di assistenza\n" RST);
  sep_doppio();
  printf("\n");
}

static void titolo_tecnici(void) {
  sep_doppio();
  printf(BOLD BGRN
         "   _____ ___ ___ _  _ ___ ___ ___ \n"
         "  |_   _| __/ __| \\| |_ _/ __|_ _|\n"
         "    | | | _| (__| .` || | (__ | | \n"
         "    |_| |___\\___|_|\\_|___\\___|___|\n"
         RST);
  printf(GRN "  Gestione del personale tecnico\n" RST);
  sep_doppio();
  printf("\n");
}

static void titolo_ricerca(void) {
  sep_doppio();
  printf(BOLD BCYN
         "   ___ ___ ___ ___ ___  ___   _   \n"
         "  | _ \\_ _/ __| __| _ \\/ __| /_\\  \n"
         "  |   /| | (__| _||   / (__ / _ \\ \n"
         "  |_|_\\___\\___|___|_|_\\\\___/_/ \\_\\\n"
         RST);
  printf(CYN "  Cerca richieste e tecnici per codice\n" RST);
  sep_doppio();
  printf("\n");
}

static void titolo_report(void) {
  sep_doppio();
  printf(BOLD BMAG
         "   ___ ___ ___  ___  ___ _____ \n"
         "  | _ \\ __| _ \\/ _ \\| _ \\_   _|\n"
         "  |   / _||  _/ (_) |   / | |  \n"
         "  |_|_\\___|_|  \\___/|_|_\\ |_|  \n"
         RST);
  printf(MAG "  Genera la Dashboard HTML e stampa report\n" RST);
  sep_doppio();
  printf("\n");
}

static void titolo_file(void) {
  sep_doppio();
  printf(BOLD BYEL
         "   ___   _   _ __   ___ _____ _   ___  ___ ___ ___  \n"
         "  / __| /_\\ | |\\ \\ / /_\\_   _/_\\ / __|/ __|_ _/ _ \\ \n"
         "  \\__ \\/ _ \\| |_\\ V / _ \\| |/ _ \\ (_ | (_ || | (_) |\n"
         "  |___/_/ \\_\\____\\_/_/ \\_\\_/_/ \\_\\___|\\___|___\\___/ \n"
         RST);
  printf(YEL "  Salva e carica i dati su file\n" RST);
  sep_doppio();
  printf("\n");
}

static void titolo_statistiche(void) {
  sep_doppio();
  printf(BOLD BCYN
         "   ___ _____ _ _____ ___ ___ _____ ___ ___ _  _ ___ \n"
         "  / __|_   _/_\\_   _|_ _/ __|_   _|_ _/ __| || | __|\n"
         "  \\__ \\ | |/ _ \\| |  | |\\__ \\ | |  | | (__| __ | _| \n"
         "  |___/ |_/_/ \\_\\_| |___|___/ |_| |___\\___|_||_|___|\n"
         RST);
  printf(CYN "  Analisi e distribuzioni dei dati del sistema\n" RST);
  sep_doppio();
  printf("\n");
}

static void titolo_web(void) {
  sep_doppio();
  printf(BOLD BBLU
         "    ___ ___  _  _ ___ _   _ _  _____ _    _______ ___  _  _ ___  __      _____ ___ \n"
         "   / __/ _ \\| \\| / __| | | | ||_   _/_\\  |_  /_ _/ _ \\| \\| | __| \\ \\    / / __| _ )\n"
         "  | (_| (_) | .` \\__ \\ |_| | |__| |/ _ \\  / / | | (_) | .` | _|   \\ \\/\\/ /| _|| _ \\\n"
         "   \\___\\___/|_|\\_|___/\\___/|____|_/_/ \\_\\/___|___\\___/|_|\\_|___|   \\_/\\_/ |___|___/\n"
         RST);
  printf(BBLU "  Risorse tecniche online\n" RST);
  sep_doppio();
  printf("\n");
}

/* =========================================================================
 * Sottomenu: Gestione Richieste
 * ========================================================================= */

static void menu_richieste(Richiesta **r, int *nr, int max_r) {
  int scelta, esci, ris, priorita_i, stato_i;
  char codice[MAX_CODICE];
  char cliente[MAX_CLIENTE];
  char dispositivo[MAX_DISPOSITIVO];
  char problema[MAX_PROBLEMA];
  char buf[BUF];
  double costo;

  esci = 0;
  while (!esci) {
    cls();
    titolo_richieste();
    printf(BMAG "  Richieste in sistema: " BOLD BWHT "%d" RST BMAG
                " / %d\n\n" RST,
           *nr, max_r);

    printf(BYEL "  1" RST " . Visualizza tutte le richieste\n");
    printf(BYEL "  2" RST " . Inserisci nuova richiesta\n");
    printf(BYEL "  3" RST " . Modifica stato\n");
    printf(BYEL "  4" RST " . Modifica descrizione problema\n");
    printf(BYEL "  5" RST " . Modifica costo stimato\n");
    printf(BYEL "  6" RST " . Modifica costo finale\n");
    printf(BRED "  7" RST " . " BRED "Cancella richiesta\n" RST);
    printf(DIM "  0  . Torna al menu principale\n\n" RST);
    sep_singolo();

    scelta = leggi_scelta(BOLD "  Scelta: " RST);

    switch (scelta) {

    case 1:
      visualizza_richieste(r, *nr);
      pausa();
      break;

    case 2:
      if (*nr >= max_r) {
        printf("\n" BRED "  [ERRORE] " RST "Lista richieste piena.\n");
        pausa();
        break;
      }
      printf("\n");
      leggi_stringa(codice, MAX_CODICE, BCYN "  Codice richiesta    : " RST);
      if (codice[0] == '\0') {
        printf("\n" BRED "  [ERRORE] " RST "Il codice non puo' essere vuoto.\n");
        pausa();
        break;
      }
      if (!solo_alfanumerico_o_trattino(codice)) {
        printf("\n" BRED "  [ERRORE] " RST
               "Il codice deve contenere solo lettere, cifre, '-' o '_'.\n");
        pausa();
        break;
      }
      leggi_stringa(cliente, MAX_CLIENTE, BCYN "  Nome cliente        : " RST);
      if (cliente[0] == '\0') {
        printf("\n" BRED "  [ERRORE] " RST "Il nome cliente non puo' essere vuoto.\n");
        pausa();
        break;
      }
      if (!contiene_lettera(cliente)) {
        printf("\n" BRED "  [ERRORE] " RST
               "Il nome cliente deve contenere almeno una lettera.\n");
        pausa();
        break;
      }
      leggi_stringa(dispositivo, MAX_DISPOSITIVO,
                    BCYN "  Dispositivo         : " RST);
      if (dispositivo[0] == '\0') {
        printf("\n" BRED "  [ERRORE] " RST "Il dispositivo non puo' essere vuoto.\n");
        pausa();
        break;
      }
      if (!contiene_lettera(dispositivo)) {
        printf("\n" BRED "  [ERRORE] " RST
               "Il dispositivo deve contenere almeno una lettera.\n");
        pausa();
        break;
      }
      leggi_stringa(problema, MAX_PROBLEMA,
                    BCYN "  Descrizione problema: " RST);
      if (problema[0] == '\0') {
        printf("\n" BRED "  [ERRORE] " RST "La descrizione del problema non puo' essere vuota.\n");
        pausa();
        break;
      }
      if (!contiene_lettera(problema)) {
        printf("\n" BRED "  [ERRORE] " RST
               "La descrizione deve contenere almeno una lettera.\n");
        pausa();
        break;
      }
      printf(BCYN "  Priorita'"
                  " (" BYEL "0" BCYN "=Bassa  " BYEL "1" BCYN "=Media  " BYEL
                  "2" BCYN "=Alta  " BYEL "3" BCYN "=Urgente" BCYN ")\n" RST);
      leggi_stringa(buf, BUF, BCYN "  Scelta            : " RST);
      priorita_i = parse_int(buf);
      if (priorita_i < 0 || priorita_i > 3)
        priorita_i = 0;
      leggi_stringa(buf, BUF, BCYN "  Costo stimato EUR : " RST);
      if (!e_numero_valido(buf)) {
        printf("\n" BRED "  [ERRORE] " RST "Costo non valido. Inserire un valore numerico >= 0.\n");
        pausa();
        break;
      }
      costo = parse_double(buf);

      ris = inserisci_richiesta(r, nr, max_r, codice, cliente, dispositivo,
                                problema, (Priorita)priorita_i, costo);
      if (ris == 0)
        printf("\n" BGRN "  [OK] " RST "Richiesta '%s' inserita.\n", codice);
      else
        printf("\n" BRED "  [ERRORE] " RST
               "Inserimento fallito: codice duplicato o dati non validi.\n");
      pausa();
      break;

    case 3:
      printf("\n");
      leggi_stringa(codice, MAX_CODICE, BCYN "  Codice richiesta: " RST);
      if (codice[0] == '\0') {
        printf("\n" BRED "  [ERRORE] " RST "Il codice non puo' essere vuoto.\n");
        pausa();
        break;
      }
      printf(BCYN "  Nuovo stato:\n"
                  "    " BYEL "0" BCYN "=Aperta   " BYEL "1" BCYN
                  "=Assegnata  " BYEL "2" BCYN "=In Corso\n"
                  "    " BYEL "3" BCYN "=Sospesa  " BYEL "4" BCYN
                  "=Chiusa     " BYEL "5" BCYN "=Annullata\n" RST);
      leggi_stringa(buf, BUF, BCYN "  Scelta: " RST);
      if (buf[0] == '\0') {
        printf("\n" BRED "  [ERRORE] " RST "Selezione vuota, inserire un valore (0-5).\n");
        pausa();
        break;
      }
      stato_i = parse_int(buf);
      if (stato_i < 0 || stato_i > 5) {
        printf("\n" BRED "  [ERRORE] " RST "Valore non valido (0-5).\n");
      } else {
        ris = modifica_stato_richiesta(r, *nr, codice, (StatoRichiesta)stato_i);
        if (ris == 0)
          printf("\n" BGRN "  [OK] " RST "Stato aggiornato.\n");
        else
          printf("\n" BRED "  [ERRORE] " RST
                 "Nessuna richiesta con quel codice.\n");
      }
      pausa();
      break;

    case 4:
      printf("\n");
      leggi_stringa(codice, MAX_CODICE, BCYN "  Codice richiesta : " RST);
      if (codice[0] == '\0') {
        printf("\n" BRED "  [ERRORE] " RST "Il codice non puo' essere vuoto.\n");
        pausa();
        break;
      }
      leggi_stringa(problema, MAX_PROBLEMA, BCYN "  Nuova descrizione: " RST);
      if (problema[0] == '\0') {
        printf("\n" BRED "  [ERRORE] " RST "La descrizione non puo' essere vuota.\n");
        pausa();
        break;
      }
      if (!contiene_lettera(problema)) {
        printf("\n" BRED "  [ERRORE] " RST
               "La descrizione deve contenere almeno una lettera.\n");
        pausa();
        break;
      }
      ris = modifica_problema_richiesta(r, *nr, codice, problema);
      if (ris == 0)
        printf("\n" BGRN "  [OK] " RST "Descrizione aggiornata.\n");
      else
        printf("\n" BRED "  [ERRORE] " RST
               "Nessuna richiesta con quel codice.\n");
      pausa();
      break;

    case 5:
      printf("\n");
      leggi_stringa(codice, MAX_CODICE, BCYN "  Codice richiesta     : " RST);
      if (codice[0] == '\0') {
        printf("\n" BRED "  [ERRORE] " RST "Il codice non puo' essere vuoto.\n");
        pausa();
        break;
      }
      leggi_stringa(buf, BUF, BCYN "  Nuovo costo stimato  : " RST);
      if (!e_numero_valido(buf)) {
        printf("\n" BRED "  [ERRORE] " RST "Costo non valido. Inserire un valore numerico >= 0.\n");
        pausa();
        break;
      }
      costo = parse_double(buf);
      ris = modifica_costo_stimato_richiesta(r, *nr, codice, costo);
      if (ris == 0)
        printf("\n" BGRN "  [OK] " RST "Costo stimato aggiornato.\n");
      else
        printf("\n" BRED "  [ERRORE] " RST
               "Richiesta non trovata o valore non valido.\n");
      pausa();
      break;

    case 6:
      printf("\n");
      leggi_stringa(codice, MAX_CODICE, BCYN "  Codice richiesta    : " RST);
      if (codice[0] == '\0') {
        printf("\n" BRED "  [ERRORE] " RST "Il codice non puo' essere vuoto.\n");
        pausa();
        break;
      }
      leggi_stringa(buf, BUF, BCYN "  Nuovo costo finale  : " RST);
      if (!e_numero_valido(buf)) {
        printf("\n" BRED "  [ERRORE] " RST "Costo non valido. Inserire un valore numerico >= 0.\n");
        pausa();
        break;
      }
      costo = parse_double(buf);
      ris = modifica_costo_finale_richiesta(r, *nr, codice, costo);
      if (ris == 0)
        printf("\n" BGRN "  [OK] " RST "Costo finale aggiornato.\n");
      else
        printf("\n" BRED "  [ERRORE] " RST
               "Richiesta non trovata o valore non valido.\n");
      pausa();
      break;

    case 7:
      printf("\n");
      leggi_stringa(codice, MAX_CODICE,
                    BRED "  Codice richiesta da cancellare: " RST);
      if (codice[0] == '\0') {
        printf("\n" BRED "  [ERRORE] " RST "Codice non valido.\n");
      } else {
        printf(BYEL "  Conferma cancellazione di '%s'? "
                    "(1=Si, 0=No): " RST,
               codice);
        leggi_stringa(buf, BUF, "");
        if (buf[0] == '1') {
          ris = cancella_richiesta(r, nr, codice);
          if (ris == 0)
            printf("\n" BGRN "  [OK] " RST
                   "Richiesta '%s' cancellata e memoria liberata.\n",
                   codice);
          else
            printf("\n" BRED "  [ERRORE] " RST
                   "Nessuna richiesta con codice '%s'.\n",
                   codice);
        } else {
          printf("\n" DIM "  Cancellazione annullata.\n" RST);
        }
      }
      pausa();
      break;

    case 0:
      esci = 1;
      break;

    default:
      printf("\n" BRED "  [ERRORE] " RST "Scelta non valida.\n");
      pausa();
      break;
    }
  }
}

/* =========================================================================
 * Sottomenu: Gestione Tecnici
 * ========================================================================= */

static void menu_tecnici(Richiesta **r, int nr, Tecnico **t, int *nt,
                         int max_t) {
  int scelta, esci, ris, max_ric;
  char codice_t[MAX_CODICE_TECNICO];
  char codice_r[MAX_CODICE];
  char nome[MAX_NOME_TECNICO];
  char spec[MAX_SPECIALIZZAZIONE];
  char buf[BUF];
  Tecnico *tec;
  Richiesta *req;

  esci = 0;
  while (!esci) {
    cls();
    titolo_tecnici();
    printf(BGRN "  Tecnici in sistema: " BOLD BWHT "%d" RST BGRN
                " / %d\n\n" RST,
           *nt, max_t);

    printf(BYEL "  1" RST " . Visualizza tutti i tecnici\n");
    printf(BYEL "  2" RST " . Inserisci nuovo tecnico\n");
    printf(BYEL "  3" RST " . Assegna richiesta a tecnico\n");
    printf(BYEL "  4" RST " . Rimuovi richiesta da tecnico\n");
    printf(DIM "  0  . Torna al menu principale\n\n" RST);
    sep_singolo();

    scelta = leggi_scelta(BOLD "  Scelta: " RST);

    switch (scelta) {

    case 1:
      visualizza_tecnici(t, *nt);
      pausa();
      break;

    case 2:
      if (*nt >= max_t) {
        printf("\n" BRED "  [ERRORE] " RST "Lista tecnici piena.\n");
        pausa();
        break;
      }
      printf("\n");
      leggi_stringa(codice_t, MAX_CODICE_TECNICO,
                    BGRN "  Codice tecnico       : " RST);
      if (codice_t[0] == '\0') {
        printf("\n" BRED "  [ERRORE] " RST "Il codice tecnico non puo' essere vuoto.\n");
        pausa();
        break;
      }
      if (!solo_alfanumerico_o_trattino(codice_t)) {
        printf("\n" BRED "  [ERRORE] " RST
               "Il codice deve contenere solo lettere, cifre, '-' o '_'.\n");
        pausa();
        break;
      }
      leggi_stringa(nome, MAX_NOME_TECNICO,
                    BGRN "  Nome e cognome       : " RST);
      if (nome[0] == '\0') {
        printf("\n" BRED "  [ERRORE] " RST "Il nome non puo' essere vuoto.\n");
        pausa();
        break;
      }
      if (!contiene_lettera(nome)) {
        printf("\n" BRED "  [ERRORE] " RST
               "Il nome deve contenere almeno una lettera.\n");
        pausa();
        break;
      }
      leggi_stringa(spec, MAX_SPECIALIZZAZIONE,
                    BGRN "  Specializzazione     : " RST);
      if (spec[0] == '\0') {
        printf("\n" BRED "  [ERRORE] " RST "La specializzazione non puo' essere vuota.\n");
        pausa();
        break;
      }
      if (!contiene_lettera(spec)) {
        printf("\n" BRED "  [ERRORE] " RST
               "La specializzazione deve contenere almeno una lettera.\n");
        pausa();
        break;
      }
      leggi_stringa(buf, BUF, BGRN "  Max richieste (1-128): " RST);
      if (!e_numero_valido(buf)) {
        printf("\n" BRED "  [ERRORE] " RST "Valore non valido. Inserire un numero da 1 a 128.\n");
        pausa();
        break;
      }
      max_ric = parse_int(buf);
      if (max_ric < 1 || max_ric > MAX_RICHIESTE_ASSOLUTO) {
        printf("\n" BRED "  [ERRORE] " RST "Il numero di richieste deve essere tra 1 e %d.\n",
               MAX_RICHIESTE_ASSOLUTO);
        pausa();
        break;
      }

      ris = inserisci_tecnico(t, nt, max_t, codice_t, nome, spec, max_ric);
      if (ris == 0)
        printf("\n" BGRN "  [OK] " RST "Tecnico '%s' inserito.\n", codice_t);
      else
        printf("\n" BRED "  [ERRORE] " RST
               "Inserimento fallito: codice duplicato o dati non validi.\n");
      pausa();
      break;

    case 3:
      printf("\n");
      leggi_stringa(codice_t, MAX_CODICE_TECNICO,
                    BGRN "  Codice tecnico  : " RST);
      if (codice_t[0] == '\0') {
        printf("\n" BRED "  [ERRORE] " RST "Il codice tecnico non puo' essere vuoto.\n");
        pausa();
        break;
      }
      leggi_stringa(codice_r, MAX_CODICE, BGRN "  Codice richiesta: " RST);
      if (codice_r[0] == '\0') {
        printf("\n" BRED "  [ERRORE] " RST "Il codice richiesta non puo' essere vuoto.\n");
        pausa();
        break;
      }
      tec = cerca_tecnico(t, *nt, codice_t);
      if (tec == NULL) {
        printf("\n" BRED "  [ERRORE] " RST "Tecnico non trovato.\n");
      } else {
        req = cerca_richiesta(r, nr, codice_r);
        if (req == NULL) {
          printf("\n" BRED "  [ERRORE] " RST "Richiesta non trovata.\n");
        } else {
          ris = tecnico_assegna_richiesta(tec, req);
          if (ris == 0) {
            printf("\n" BGRN "  [OK] " RST "Richiesta assegnata al tecnico.\n");
            modifica_stato_richiesta(r, nr, codice_r, STATO_ASSEGNATA);
          } else {
            printf("\n" BRED "  [ERRORE] " RST
                   "Tecnico al massimo del carico o richiesta gia' assegnata.\n");
          }
        }
      }
      pausa();
      break;

    case 4:
      printf("\n");
      leggi_stringa(codice_t, MAX_CODICE_TECNICO,
                    BGRN "  Codice tecnico  : " RST);
      if (codice_t[0] == '\0') {
        printf("\n" BRED "  [ERRORE] " RST "Il codice tecnico non puo' essere vuoto.\n");
        pausa();
        break;
      }
      leggi_stringa(codice_r, MAX_CODICE, BGRN "  Codice richiesta: " RST);
      if (codice_r[0] == '\0') {
        printf("\n" BRED "  [ERRORE] " RST "Il codice richiesta non puo' essere vuoto.\n");
        pausa();
        break;
      }
      tec = cerca_tecnico(t, *nt, codice_t);
      if (tec == NULL) {
        printf("\n" BRED "  [ERRORE] " RST "Tecnico non trovato.\n");
      } else {
        ris = tecnico_rimuovi_richiesta(tec, codice_r);
        if (ris == 0)
          printf("\n" BGRN "  [OK] " RST "Richiesta rimossa dal tecnico.\n");
        else
          printf("\n" BRED "  [ERRORE] " RST
                 "Richiesta non presente nel profilo del tecnico.\n");
      }
      pausa();
      break;

    case 0:
      esci = 1;
      break;

    default:
      printf("\n" BRED "  [ERRORE] " RST "Scelta non valida.\n");
      pausa();
      break;
    }
  }
}

/* =========================================================================
 * Sottomenu: Ricerca
 * ========================================================================= */

static void menu_ricerca(Richiesta **r, int nr, Tecnico **t, int nt) {
  int scelta, esci;
  char codice[MAX_CODICE_TECNICO];
  Richiesta *req;
  Tecnico *tec;

  esci = 0;
  while (!esci) {
    cls();
    titolo_ricerca();

    printf(BYEL "  1" RST " . Cerca richiesta per codice\n");
    printf(BYEL "  2" RST " . Cerca tecnico per codice\n");
    printf(DIM "  0  . Torna al menu principale\n\n" RST);
    sep_singolo();

    scelta = leggi_scelta(BOLD "  Scelta: " RST);

    switch (scelta) {

    case 1:
      printf("\n");
      leggi_stringa(codice, MAX_CODICE, BCYN "  Codice richiesta: " RST);
      if (codice[0] == '\0') {
        printf("\n" BRED "  [ERRORE] " RST "Il codice non puo' essere vuoto.\n");
        pausa();
        break;
      }
      req = cerca_richiesta(r, nr, codice);
      if (req == NULL)
        printf("\n" BYEL "  [--] " RST "Nessuna richiesta con codice '%s'.\n",
               codice);
      else {
        printf("\n" BGRN "  [OK] " RST "Richiesta trovata:\n");
        sep_singolo();
        visualizza_richiesta(req);
      }
      pausa();
      break;

    case 2:
      printf("\n");
      leggi_stringa(codice, MAX_CODICE_TECNICO, BCYN "  Codice tecnico: " RST);
      if (codice[0] == '\0') {
        printf("\n" BRED "  [ERRORE] " RST "Il codice non puo' essere vuoto.\n");
        pausa();
        break;
      }
      tec = cerca_tecnico(t, nt, codice);
      if (tec == NULL)
        printf("\n" BYEL "  [--] " RST "Nessun tecnico con codice '%s'.\n",
               codice);
      else {
        printf("\n" BGRN "  [OK] " RST "Tecnico trovato:\n");
        sep_singolo();
        visualizza_tecnico(tec);
      }
      pausa();
      break;

    case 0:
      esci = 1;
      break;

    default:
      printf("\n" BRED "  [ERRORE] " RST "Scelta non valida.\n");
      pausa();
      break;
    }
  }
}

/* =========================================================================
 * Sottomenu: Report e Dashboard HTML
 * ========================================================================= */

static void menu_report(Richiesta **r, int nr, Tecnico **t, int nt) {
  int scelta, esci, ris;

  esci = 0;
  while (!esci) {
    cls();
    titolo_report();

    printf(BYEL "  1" RST " . Genera dashboard HTML  ->  " BWHT FILE_DASHBOARD
                "\n" RST);
    printf(BYEL "  2" RST " . Stampa report richieste (terminale)\n");
    printf(BYEL "  3" RST " . Stampa report tecnici   (terminale)\n");
    sep_singolo();
    printf(BOLD BCYN "  Ordinamento e Report su File\n" RST);
    printf(BYEL "  4" RST " . Ordina richieste per " BOLD "priorita'" RST
                "  (Merge Sort)\n");
    printf(BYEL "  5" RST " . Ordina richieste per " BOLD "data" RST
                "      (Merge Sort)\n");
    printf(BYEL "  6" RST " . Esporta " BWHT FILE_REPORT_ORDINATO RST
                "  (report testuale)\n");
    printf(DIM "  0  . Torna al menu principale\n\n" RST);
    sep_singolo();

    scelta = leggi_scelta(BOLD "  Scelta: " RST);

    switch (scelta) {

    case 1:
      printf("\n" BMAG "  Generazione dashboard in corso...\n" RST);
      ris = genera_dashboard_html(r, nr, t, nt);
      if (ris == 0) {
        printf(BGRN "  [OK] " RST "File '" BWHT FILE_DASHBOARD RST
                    "' generato con successo.\n");
        printf(DIM "       Aprilo in un browser per visualizzare"
                   " il riepilogo.\n" RST);
      } else {
        printf(BRED "  [ERRORE] " RST "Impossibile creare '" FILE_DASHBOARD
                    "'.\n");
        printf(DIM "           Verificare i permessi di scrittura"
                   " nella directory.\n" RST);
      }
      pausa();
      break;

    case 2:
      visualizza_richieste(r, nr);
      pausa();
      break;

    case 3:
      visualizza_tecnici(t, nt);
      pausa();
      break;

    case 4:
      printf("\n" BMAG "  Ordinamento per priorita' in corso...\n" RST);
      ris = ordina_richieste_merge_sort(r, nr, ORDINA_PER_PRIORITA);
      if (ris == 0) {
        printf(BGRN "  [OK] " RST
                    "%d richieste ordinate per priorita' (decrescente).\n",
               nr);
        visualizza_richieste(r, nr);
      } else {
        printf(BRED "  [ERRORE] " RST "Ordinamento fallito.\n");
      }
      pausa();
      break;

    case 5:
      printf("\n" BMAG "  Ordinamento per data in corso...\n" RST);
      ris = ordina_richieste_merge_sort(r, nr, ORDINA_PER_DATA);
      if (ris == 0) {
        printf(BGRN "  [OK] " RST
                    "%d richieste ordinate per data di apertura (crescente).\n",
               nr);
        visualizza_richieste(r, nr);
      } else {
        printf(BRED "  [ERRORE] " RST "Ordinamento fallito.\n");
      }
      pausa();
      break;

    case 6:
      printf("\n" BMAG "  Esportazione report ordinato...\n" RST);
      ris = esporta_report_ordinato(r, nr);
      if (ris == 0) {
        printf(BGRN "  [OK] " RST "File '" BWHT FILE_REPORT_ORDINATO RST
                    "' generato con successo.\n");
        printf(DIM "       Contiene %d richieste nello stato"
                   " corrente (ordinato o meno).\n" RST,
               nr);
      } else {
        printf(BRED "  [ERRORE] " RST
                    "Impossibile creare '" FILE_REPORT_ORDINATO "'.\n");
      }
      pausa();
      break;

    case 0:
      esci = 1;
      break;

    default:
      printf("\n" BRED "  [ERRORE] " RST "Scelta non valida.\n");
      pausa();
      break;
    }
  }
}

/* =========================================================================
 * Sottomenu: Salvataggio e Caricamento File
 * ========================================================================= */

static void menu_file(Richiesta **r, int *nr, int max_r, Tecnico **t, int *nt,
                      int max_t) {
  int scelta, esci, ris, i;

  esci = 0;
  while (!esci) {
    cls();
    titolo_file();
    printf(BYEL "  File richieste : " BWHT "%s\n" RST, FILE_RICHIESTE);
    printf(BYEL "  File tecnici   : " BWHT "%s\n\n" RST, FILE_TECNICI);

    printf(BYEL "  1" RST " . Salva richieste su file\n");
    printf(BYEL "  2" RST " . Salva tecnici su file\n");
    printf(BYEL "  3" RST " . Carica richieste da file  " DIM
                "[rimuove quelle in memoria]\n" RST);
    printf(BYEL "  4" RST " . Carica tecnici da file    " DIM
                "[rimuove quelli in memoria]\n" RST);
    printf(DIM "  0  . Torna al menu principale\n\n" RST);
    sep_singolo();

    scelta = leggi_scelta(BOLD "  Scelta: " RST);

    switch (scelta) {

    case 1:
      ris = salva_richieste(FILE_RICHIESTE, r, *nr);
      if (ris == 0)
        printf("\n" BGRN "  [OK] " RST "%d richieste salvate su '%s'.\n", *nr,
               FILE_RICHIESTE);
      else
        printf("\n" BRED "  [ERRORE] " RST
               "Salvataggio richieste non riuscito.\n");
      pausa();
      break;

    case 2:
      ris = salva_tecnici(FILE_TECNICI, t, *nt);
      if (ris == 0)
        printf("\n" BGRN "  [OK] " RST "%d tecnici salvati su '%s'.\n", *nt,
               FILE_TECNICI);
      else
        printf("\n" BRED "  [ERRORE] " RST
               "Salvataggio tecnici non riuscito.\n");
      pausa();
      break;

    case 3:
      for (i = 0; i < *nt; i++)
        tecnico_distruggi(&t[i]);
      *nt = 0;
      for (i = 0; i < *nr; i++)
        richiesta_distruggi(&r[i]);
      *nr = 0;
      ris = carica_richieste(FILE_RICHIESTE, r, nr, max_r);
      if (ris == 0)
        printf("\n" BGRN "  [OK] " RST "%d richieste caricate.\n", *nr);
      else
        printf("\n" BRED "  [ERRORE] " RST
               "Caricamento fallito. File non trovato?\n");
      printf(BYEL "  [NOTA] " RST
                  "Caricare nuovamente i tecnici (opzione 4).\n");
      pausa();
      break;

    case 4:
      for (i = 0; i < *nt; i++)
        tecnico_distruggi(&t[i]);
      *nt = 0;
      ris = carica_tecnici(FILE_TECNICI, t, nt, max_t, r, *nr);
      if (ris == 0)
        printf("\n" BGRN "  [OK] " RST "%d tecnici caricati.\n", *nt);
      else
        printf("\n" BRED "  [ERRORE] " RST
               "Caricamento fallito. File non trovato?\n");
      pausa();
      break;

    case 0:
      esci = 1;
      break;

    default:
      printf("\n" BRED "  [ERRORE] " RST "Scelta non valida.\n");
      pausa();
      break;
    }
  }
}

/* =========================================================================
 * Sottomenu: Statistiche
 * ========================================================================= */

static void stampa_barra(int val, int max, const char *color) {
  int filled, i;
  filled = (max > 0) ? (val * 20 / max) : 0;
  if (filled > 20)
    filled = 20;
  printf("%s  [", color);
  for (i = 0; i < filled; i++)
    printf("\xe2\x96\x88"); /* █ */
  for (i = filled; i < 20; i++)
    printf("\xe2\x96\x91"); /* ░ */
  printf("] %d/%d" RST "\n", val, max);
}

static void menu_statistiche(Richiesta **r, int nr, Tecnico **t, int nt) {
  int scelta, esci, i;
  int cnt_stato[6];
  int cnt_prior[4];
  int asg, mx, pct, asg_tot, cap_tot;
  double costo_st, costo_fi;

  esci = 0;
  while (!esci) {
    cls();
    titolo_statistiche();

    printf(BYEL "  1" RST " . Distribuzione richieste per stato\n");
    printf(BYEL "  2" RST " . Distribuzione richieste per priorita'\n");
    printf(BYEL "  3" RST " . Analisi costi\n");
    printf(BYEL "  4" RST " . Carico di lavoro tecnici\n");
    printf(DIM "  0  . Torna al menu principale\n\n" RST);
    sep_singolo();

    scelta = leggi_scelta(BOLD "  Scelta: " RST);

    switch (scelta) {

    case 1:
      for (i = 0; i < 6; i++)
        cnt_stato[i] = 0;
      for (i = 0; i < nr; i++)
        if (r[i] != NULL) {
          int si = (int)richiesta_get_stato(r[i]);
          if (si >= 0 && si < 6) cnt_stato[si]++;
        }
      printf("\n");
      sep_singolo();
      printf(BOLD BCYN "  Distribuzione per Stato\n" RST);
      sep_singolo();
      printf(BBLU "  Aperte    : " BOLD BWHT "%d\n" RST, cnt_stato[0]);
      printf(BMAG "  Assegnate : " BOLD BWHT "%d\n" RST, cnt_stato[1]);
      printf(BYEL "  In Corso  : " BOLD BWHT "%d\n" RST, cnt_stato[2]);
      printf(DIM "  Sospese   : " BOLD BWHT "%d\n" RST, cnt_stato[3]);
      printf(BGRN "  Chiuse    : " BOLD BWHT "%d\n" RST, cnt_stato[4]);
      printf(BRED "  Annullate : " BOLD BWHT "%d\n" RST, cnt_stato[5]);
      sep_singolo();
      printf(BOLD "  TOTALE    : " BWHT "%d\n" RST, nr);
      pausa();
      break;

    case 2:
      for (i = 0; i < 4; i++)
        cnt_prior[i] = 0;
      for (i = 0; i < nr; i++)
        if (r[i] != NULL) {
          int pi = (int)richiesta_get_priorita(r[i]);
          if (pi >= 0 && pi < 4) cnt_prior[pi]++;
        }
      printf("\n");
      sep_singolo();
      printf(BOLD BCYN "  Distribuzione per Priorita'\n" RST);
      sep_singolo();
      printf(DIM "  Bassa   : " BOLD BWHT "%d\n" RST, cnt_prior[0]);
      printf(BBLU "  Media   : " BOLD BWHT "%d\n" RST, cnt_prior[1]);
      printf(BYEL "  Alta    : " BOLD BWHT "%d\n" RST, cnt_prior[2]);
      printf(BRED "  Urgente : " BOLD BWHT "%d\n" RST, cnt_prior[3]);
      sep_singolo();
      printf(BOLD "  TOTALE  : " BWHT "%d\n" RST, nr);
      pausa();
      break;

    case 3:
      costo_st = 0.0;
      costo_fi = 0.0;
      for (i = 0; i < nr; i++) {
        if (r[i] != NULL) {
          costo_st += richiesta_get_costo_stimato(r[i]);
          costo_fi += richiesta_get_costo_finale(r[i]);
        }
      }
      printf("\n");
      sep_singolo();
      printf(BOLD BCYN "  Analisi Costi\n" RST);
      sep_singolo();
      printf(BBLU "  Totale stimato : " BOLD BWHT "EUR %.2f\n" RST, costo_st);
      printf(BGRN "  Totale finale  : " BOLD BWHT "EUR %.2f\n" RST, costo_fi);
      if (nr > 0)
        printf(BCYN "  Media stimata  : " BOLD BWHT "EUR %.2f\n" RST,
               costo_st / (double)nr);
      {
        double delta = costo_fi - costo_st;
        if (delta >= 0.0)
          printf(BRED "  Delta          : " BOLD BWHT "+EUR %.2f\n" RST, delta);
        else
          printf(BGRN "  Delta          : " BOLD BWHT "EUR %.2f\n" RST, delta);
      }
      pausa();
      break;

    case 4:
      printf("\n");
      sep_singolo();
      printf(BOLD BCYN "  Carico di Lavoro Tecnici\n" RST);
      sep_singolo();
      if (nt == 0) {
        printf(DIM "  Nessun tecnico nel sistema.\n" RST);
      } else {
        asg_tot = 0;
        cap_tot = 0;
        for (i = 0; i < nt; i++) {
          if (t[i] == NULL)
            continue;
          asg = tecnico_get_assegnate(t[i]);
          mx = tecnico_get_max_richieste(t[i]);
          pct = (mx > 0) ? (asg * 100 / mx) : 0;
          printf(BOLD BWHT "  %-22s " RST, tecnico_get_nome(t[i]));
          if (pct >= 100)
            stampa_barra(asg, mx, BRED);
          else if (pct >= 60)
            stampa_barra(asg, mx, BYEL);
          else
            stampa_barra(asg, mx, BGRN);
          asg_tot += asg;
          cap_tot += mx;
        }
        sep_singolo();
        if (cap_tot > 0) {
          printf(BOLD "  Sistema: " BWHT "%d / %d slot occupati  (%d%%)\n" RST,
                 asg_tot, cap_tot, asg_tot * 100 / cap_tot);
        }
      }
      pausa();
      break;

    case 0:
      esci = 1;
      break;

    default:
      printf("\n" BRED "  [ERRORE] " RST "Scelta non valida.\n");
      pausa();
      break;
    }
  }
}

/* =========================================================================
 * Sottomenu: Consultazione Web
 * ========================================================================= */

static void menu_web(void) {
  int scelta, esci;

  esci = 0;
  while (!esci) {
    cls();
    titolo_web();
    printf(BBLU "  Risorse tecniche disponibili:\n\n" RST);

    printf(BYEL "  1" RST " . Apple Support\n");
    printf(DIM "     https://support.apple.com\n" RST);
    printf(BYEL "  2" RST " . Tom's Hardware  " DIM
                "(database componenti e benchmark)\n" RST);
    printf(DIM "     https://www.tomshardware.com\n" RST);
    printf(BYEL "  3" RST " . DriverPack  " DIM
                "(driver e firmware aggiornati)\n" RST);
    printf(DIM "     https://drp.su/it\n" RST);
    printf(BYEL "  4" RST " . TeamViewer  " DIM
                "(supporto e accesso remoto)\n" RST);
    printf(DIM "     https://www.teamviewer.com/it\n" RST);
    printf("\n");
    printf(DIM "  0  . Torna al menu principale\n\n" RST);
    sep_singolo();

    scelta = leggi_scelta(BOLD "  Scelta: " RST);

    switch (scelta) {
    case 1:
      printf("\n" BBLU "  Apertura: https://support.apple.com\n" RST);
      system("open https://support.apple.com");
      pausa();
      break;
    case 2:
      printf("\n" BBLU "  Apertura: https://www.tomshardware.com\n" RST);
      system("open https://www.tomshardware.com");
      pausa();
      break;
    case 3:
      printf("\n" BBLU "  Apertura: https://drp.su/it\n" RST);
      system("open https://drp.su/it");
      pausa();
      break;
    case 4:
      printf("\n" BBLU "  Apertura: https://www.teamviewer.com/it\n" RST);
      system("open https://www.teamviewer.com/it");
      pausa();
      break;
    case 0:
      esci = 1;
      break;
    default:
      printf("\n" BRED "  [ERRORE] " RST
             "Scelta non valida (1-4 o 0 per uscire).\n");
      pausa();
      break;
    }
  }
}
