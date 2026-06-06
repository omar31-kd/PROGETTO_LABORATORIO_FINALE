# Progetto: Sistema Gestione Centro Assistenza Tecnica

Questo file funge da ancora di contesto permanente per lo sviluppo del progetto C. Ogni volta che si opera sul workspace, le linee guida, i vincoli e la struttura qui definiti devono essere rispettati rigorosamente.

## 1. Visione d'Insieme del Progetto
Il sistema gestisce le richieste di assistenza di un laboratorio tecnico e l'assegnazione delle stesse ai tecnici disponibili. Il flusso dati prevede il caricamento da file di testo, la gestione in memoria tramite strutture dinamiche avanzate e il salvataggio dei dati aggiornati, oltre alla generazione di un report HTML (dashboard).

---

## 2. Vincoli Tecnologici e Architetturali Rigidi

### 2.1 Limitazione Funzioni C (Invalicabile)
È consentito l'utilizzo **ESCLUSIVO** delle funzioni e dei costrutti standard documentati nel programma didattico di riferimento (`Puntatori-2-unito.pdf`).
- **Funzioni I/O e Stringhe ammesse**: `printf`, `puts`, `fgets`, `strlen`, `tolower`.
- **Gestione File ammessi**: `fopen`, `fclose`, `fprintf`, `fseek`, `rewind`, `feof`, `fread`, `fwrite`.
- **Allocazione Memoria**: `calloc`, `free`, `sizeof`.
- **DIVIETO ASSOLUTO**: È tassativamente vietato l'uso di `fscanf` per la lettura di file o input. Per l'acquisizione di stringhe e record da file si deve usare unicamente `fgets`. È vietato l'uso di funzioni non elementari di stringa (es. `strdup`, `strtok`, `atoi`) se non ricreate manualmente tramite logica algoritmica di base.

### 2.2 Requisiti di Progettazione (Stile OOP in C)
1. **Modularità**: Il codice deve essere suddiviso in moduli logici distinti, composti da file header (`.h`) e file di implementazione (`.c`).
   - `richiesta.h` / `richiesta.c`: Gestione dell'entità Richiesta.
   - `tecnico.h` / `tecnico.c`: Gestione dell'entità Tecnico.
   - `gestione_file.h` / `gestione_file.c`: Operazioni di I/O su file di testo (`richieste.txt`, `tecnici.txt`).
   - `logica.h` / `logica.c`: Core delle funzioni di business, associazioni e generazione della dashboard HTML.
   - `main.c`: Interfaccia utente a menu e orchestrazione.
2. **Incapsulamento**: I campi delle strutture dati (`struct`) non devono essere modificati direttamente al di fuori del proprio modulo. Tutta l'interazione avviene tramite funzioni Interfaccia. Le funzioni di utilità interna all'interno dei file `.c` devono essere dichiarate come `static`.
3. **Getters e Setters**: Ogni struct deve disporre di funzioni dedicate per la lettura (getter) e la scrittura (setter) dei suoi attributi.
4. **Costruttori e Distruttori**: La creazione delle istanze deve avvenire dinamicamente tramite funzioni di inizializzazione (costruttori che usano `calloc`), e la disallocazione completa deve essere gestita da appositi distruttori (`free`).
5. **Programmazione Difensiva**: Ogni funzione deve convalidare rigorosamente i parametri in ingresso. Prima di effettuare operazioni di dereferenziazione (`->` o `*`), è obbligatorio verificare che i puntatori non siano `NULL`. Controllare sempre i limiti massimi degli array e la corretta apertura dei descrittori di file.
6. **Sintesi del Codice**: Ottimizzare la logica per scrivere il minor numero di righe di codice possibile, prediligendo algoritmi chiari, lineari e privi di sovrastrutture teoriche complesse.

---

## 3. Specifiche delle Strutture Dati

### 3.1 Richiesta di Assistenza (`Richiesta`)
I campi minimi da gestire includono:
- Codice richiesta (identificativo univoco)
- Nome cliente
- Tipologia di dispositivo
- Descrizione del problema
- Priorità (es. enum o scala numerica)
- Stato della richiesta (Aperta, Assegnata, Chiusa, Annullata)
- Costo stimato e Costo finale
- Data di apertura

### 3.2 Tecnico (`Tecnico`)
I campi minimi da gestire includono:
- Codice tecnico
- Nome tecnico
- Area di specializzazione
- Numero massimo di richieste assegnabili
- Numero di richieste attualmente assegnate

---

## 4. Documentazione e Stile del Codice
- **Commenti Doxygen**: Ogni tipo di dato (`struct`, `enum`, `typedef`) e ogni singola funzione nei file header deve essere preceduta da blocchi di commento compatibili con Doxygen (`/** ... */`) che descrivano brevemente lo scopo, i parametri (`@param`) e il valore di ritorno (`@return`).
- **Anonimato dell'IA**: Il codice sorgente finale, i commenti interni e le stringhe stampate a video non devono contenere alcun riferimento a strumenti di intelligenza artificiale, prompt, o formati standard tipici dei generatori automatici. Il progetto deve risultare strutturato e scritto come opera nativa di un programmatore umano.

---

## 5. Output Web (Dashboard)
Il sistema deve prevedere la generazione di un file `dashboard.html` aggiornato ad ogni salvataggio o richiesta specifica dell'utente. Il file deve essere scritto interamente in C tramite flussi di `fprintf` su file, presentando in modo chiaro e organizzato i dati globali, i tecnici, le richieste urgenti e le statistiche prestazionali del centro assistenza.
