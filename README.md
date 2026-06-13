# Orizzonte

Simulatore Monte Carlo del divario pensionistico — *a Monte Carlo simulator of the pension gap.*
Gratuito e open source, per i lavoratori italiani.

---

## 🇮🇹 Italiano

### Perché esiste

In Italia l'alfabetizzazione finanziaria è tra le più basse d'Europa, e per le nuove generazioni la pensione pubblica coprirà una quota dello stipendio sempre minore. Orizzonte nasce per rendere visibile, in modo concreto e gratuito, una domanda che riguarda ogni lavoratore: dato quanto riesco a risparmiare, quali scenari mi aspettano al momento della pensione?

### Cosa fa

Simula 10.000 possibili "vite finanziarie" e, invece di una falsa previsione unica, restituisce un ventaglio di esiti:
- scenario sfortunato (10% peggiore)
- scenario tipico (mediana)
- scenario fortunato (10% migliore)

### Come si usa

Compila:  `g++ orizzonte.cpp -o orizzonte`
Lancia:   `./orizzonte`

Il programma chiede pochi dati (età, anni di contributi già versati, versamento annuo) e mostra gli scenari a schermo.

### Il modello

- 10.000 simulazioni Monte Carlo
- rendimento medio annuo: 7%
- volatilità: 15%
- rendimenti estratti da una distribuzione normale
- capitalizzazione (interesse composto) su base annua
- anni rimanenti stimati da soglie contributive italiane semplificate

### Limiti attuali (onestà prima di tutto)

Questa è una versione prototipo. Al momento il modello **non** considera:
- l'inflazione
- la fase di prelievo dopo la pensione
- le code grasse dei mercati (i crolli estremi sono sottostimati dalla distribuzione normale)
- le regole pensionistiche italiane reali, qui semplificate

I numeri vanno letti come ordini di grandezza, non come certezze.

### Sviluppi futuri

- versione web accessibile a chiunque, senza compilare codice
- rendimenti a code grasse e gestione dell'inflazione
- eventuale raccolta di statistiche aggregate e anonime (nessun dato personale, solo parametri non identificativi), previa informativa privacy e consenso, conforme al GDPR, *non ancora implementata*

### Disclaimer

Orizzonte è uno strumento educativo. **Non è consulenza finanziaria** e non sostituisce un consulente abilitato.

---

## 🇬🇧 English

### Why it exists

In Italy, financial literacy is among the lowest in Europe, and for younger generations the public pension will replace a shrinking share of their salary. Orizzonte was created to make one question concrete and accessible to every worker, for free: given how much I can save, what scenarios await me at retirement?

### What it does

It runs 10,000 possible "financial lives" and, instead of a single false prediction, returns a range of outcomes:
- an unlucky scenario (worst 10%)
- a typical scenario (median)
- a lucky scenario (best 10%)

### How to use it

Compile:  `g++ orizzonte.cpp -o orizzonte`
Run:      `./orizzonte`

The program asks for a few inputs (age, years of contributions already paid, annual savings) and prints the scenarios on screen.

> Note: the program's interface is in Italian, since its intended users are Italian workers.

### The model

- 10,000 Monte Carlo simulations
- average annual return: 7%
- volatility: 15%
- returns drawn from a normal distribution
- annual compounding
- remaining years estimated from simplified Italian contribution thresholds

### Current limitations (honesty first)

This is a prototype. The model currently does **not** account for:
- inflation
- the withdrawal phase after retirement
- fat tails (extreme crashes are underestimated by a normal distribution)
- the real Italian pension rules, which are simplified here

The figures should be read as orders of magnitude, not certainties.

### Roadmap

- a web version accessible to anyone, with no code to compile
- fat-tailed returns and inflation handling
- optional collection of aggregate, anonymous statistics (no personal data, only non-identifying parameters), with a privacy policy and consent, GDPR-compliant, *not yet implemented*

### Disclaimer

Orizzonte is an educational tool. It is **not financial advice** and does not replace a licensed advisor.

---

## License / Licenza

MIT
