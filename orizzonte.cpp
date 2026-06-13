#include <iostream>
#include <ctime>
#include <string>
#include <random>
#include <algorithm>

using namespace std;

const int maxSim = 10000;

// ===================== STRUCT =====================

struct info {
    string nome;
    string cognome;
    string sesso;
    int eta;
    string data;
};

struct dati {
    int eta;
    int contributiDATI;
    int stipendio;
    int versamentoXanno;
    int annirimasti;
};

// ===================== PROTOTIPI =====================

void biometrici(info &persona);
void specifici(dati &pers);
double montecarlo(dati &pers);
void grafico(int f1, int f2, int f3, int f4, int f5);

// ===================== MAIN =====================

int main()
{
    info persona;
    dati pers;

    double finale[maxSim];

    int f1 = 0, f2 = 0, f3 = 0, f4 = 0, f5 = 0;

    cout << "\n\n============================================================";
    cout << "\n        SIMULATORE MONTECARLO - VERSIONE PROTOTIPO";
    cout << "\n============================================================\n";

    cout << "\nObiettivo: simulare scenari pensionistici e finanziari\n";

    cout << "\nPremi INVIO per iniziare...";
    cin.get();

    biometrici(persona);

    cout << "\n\u2714 Dati biometrici acquisiti correttamente.";
    cout << "\nBenvenuto, " << persona.nome << ".\n";

    pers.eta = persona.eta;

    specifici(pers);

    cout << "\n\u2714 Acquisizione completata. Avvio simulazione...\n";

    // ===================== ANNI RIMASTI =====================

    if (persona.sesso == "M" || persona.sesso == "m" || persona.sesso == "maschio")
        pers.annirimasti = 42 - pers.contributiDATI;
    else if (persona.sesso == "F" || persona.sesso == "f" || persona.sesso == "femmina")
        pers.annirimasti = 41 - pers.contributiDATI;
    else
        pers.annirimasti = 40 - pers.contributiDATI;

    // ===================== MONTECARLO =====================

    for (int i = 0; i < maxSim; i++)
    {
        finale[i] = montecarlo(pers);
    }

    // ===================== RISULTATI =====================

    sort(finale, finale + maxSim);      // mette i 10.000 risultati in fila, dal piu' piccolo al piu' grande

    double sfortunato = finale[1000];   // sotto questo sta il 10% peggiore
    double tipico     = finale[5000];   // il valore in mezzo alla fila
    double fortunato  = finale[9000];   // sopra questo sta il 10% migliore

    cout << "\n\n================ I TUOI SCENARI ================\n";
    cout << "\nScenario sfortunato (10% peggiore): " << (int)sfortunato << " EUR";
    cout << "\nScenario tipico (meta' dei casi):   " << (int)tipico << " EUR";
    cout << "\nScenario fortunato (10% migliore):  " << (int)fortunato << " EUR";

    // ===================== ANALISI =====================

    for (int i = 0; i < maxSim; i++)
    {
        if (finale[i] < 30000)
            f1++;
        else if (finale[i] < 60000)
            f2++;
        else if (finale[i] < 90000)
            f3++;
        else if (finale[i] < 120000)
            f4++;
        else
            f5++;
    }

    cout << "\n\n================ RISULTATI PROBABILISTICI ================\n";

    cout << "\nBasso rendimento:  " << (f1 * 100) / maxSim << "%";
    cout << "\nMedio rendimento:  " << (f2 * 100) / maxSim << "%";
    cout << "\nBuon rendimento:   " << (f3 * 100) / maxSim << "%";
    cout << "\nAlto rendimento:   " << (f4 * 100) / maxSim << "%";
    cout << "\nEccellente:        " << (f5 * 100) / maxSim << "%";

    // ===================== ISTOGRAMMA =====================

    grafico(f1, f2, f3, f4, f5);

    return 0;
}

// ===================== FUNZIONI =====================

void biometrici(info &persona)
{
    cout << "\n\n---------------- DATI PERSONALI ----------------\n";

    cout << "Nome: ";
    cin >> persona.nome;

    cout << "Cognome: ";
    cin >> persona.cognome;

    cout << "Genere (M/F): ";
    cin >> persona.sesso;

    cout << "Data nascita: ";
    cin >> persona.data;

    cout << "Eta': ";
    cin >> persona.eta;
}

void specifici(dati &pers)
{
    cout << "\n\n---------------- DATI CONTRIBUTIVI ----------------\n";

    cout << "Anni contributi versati: ";
    cin >> pers.contributiDATI;

    cout << "Stipendio mensile (\u20ac): ";
    cin >> pers.stipendio;

    cout << "Versamento annuo (\u20ac): ";
    cin >> pers.versamentoXanno;
}

// ===================== CUORE: MONTECARLO =====================

double montecarlo(dati &pers)
{
    // create una volta sola, condivise da tutte le 10.000 simulazioni
    static mt19937 gen(time(NULL));
    static normal_distribution<double> campana(0.07, 0.15); // media 7%, volatilita' 15%

    double tot = 0.0;

    for (int i = 0; i < pers.annirimasti; i++)
    {
        double r = campana(gen);                       // rendimento casuale dell'anno, dalla campana
        tot = tot * (1.0 + r) + pers.versamentoXanno;  // TUTTO il capitale cresce, poi aggiungo il versamento
    }

    return tot;
}

// ===================== ISTOGRAMMA =====================

void grafico(int f1, int f2, int f3, int f4, int f5)
{
    cout << "\n\n================ DISTRIBUZIONE RISULTATI ================\n";

    cout << "\n0 - 30k      ";
    for (int i = 0; i < f1 / 100; i++) cout << "\u2588";

    cout << "\n30k - 60k    ";
    for (int i = 0; i < f2 / 100; i++) cout << "\u2588";

    cout << "\n60k - 90k    ";
    for (int i = 0; i < f3 / 100; i++) cout << "\u2588";

    cout << "\n90k - 120k   ";
    for (int i = 0; i < f4 / 100; i++) cout << "\u2588";

    cout << "\n120k+        ";
    for (int i = 0; i < f5 / 100; i++) cout << "\u2588";

    cout << "\n==========================================================\n";
}
