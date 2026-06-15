#include <iostream>
#include <ctime>
#include <random>
#include <algorithm>
#include <vector>
#include <cmath>

using namespace std;

// ===================== ASSUMPTIONS / CONSTANTS =====================
const double COST              = 0.01;    // annual management fee (ISC), 1%. On the website this is a user control.
const double INFLATION         = 0.02;    // assumed average inflation, to express results in today's euros
const double CONTRIB_GROWTH    = 0.02;    // contributions grow ~ inflation, so the REAL contribution stays constant
const double RETURN_TAX        = 0.20;    // annual tax on positive gains (20%; 12.5% on govt bonds - simplified)
const double DEDUCTION_CAP     = 5300.0;  // max yearly deductible contribution, 2026 Italian rule
const double RETIREMENT_RETURN = 0.02;    // assumed REAL return during retirement (residual stays invested)
const int    LIFE_EXPECTANCY   = 85;      // assumed life expectancy, for the monthly-pension estimate

// ===================== STRUCT =====================

struct Saver {
    int    age;
    int    retirementAge;
    int    annualContribution;   // employee, first year; grows at CONTRIB_GROWTH
    int    employerContribution; // employer, first year; grows too; 0 if none
    int    yearsToRetirement;
    double expectedReturn;       // mu    - from the risk profile
    double volatility;           // sigma - from the risk profile
    double marginalRate;         // IRPEF marginal rate - from the income bracket
    double deductedBase;         // sum of deducted contributions (employee + employer, capped); base for final tax
};

// ===================== PROTOTYPES =====================

void   readInputs(Saver &s);
void   chooseRiskProfile(Saver &s);
void   chooseIncomeBracket(Saver &s);
int    chooseSimulationCount();
double simulateOnce(Saver &s);
double monthlyPension(double pot, const Saver &s);
void   drawHistogram(const vector<double> &results);
void   printAssumptions();

// ===================== MAIN =====================

int main()
{
    Saver s;

    cout << "\n\n============================================================";
    cout << "\n        ORIZZONTE - MONTE CARLO SIMULATOR (v3)";
    cout << "\n============================================================\n";
    cout << "\nGoal: simulate retirement-savings accumulation in TODAY'S euros,";
    cout << "\nnet of fees and taxes.\n";
    cout << "\nPress ENTER to start...";
    cin.get();

    readInputs(s);
    chooseRiskProfile(s);
    chooseIncomeBracket(s);
    int nSims = chooseSimulationCount();

    s.yearsToRetirement = s.retirementAge - s.age;
    if (s.yearsToRetirement <= 0) {
        cout << "\nRetirement age must be greater than current age. Exiting.\n";
        return 1;
    }

    // Deterministic figures (contributions grow, so we sum year by year).
    double totalContributed = 0.0;   // employee's own money
    double totalEmployer    = 0.0;   // employer's money (free)
    double totalTaxSaving   = 0.0;   // saving on the EMPLOYEE's own contributions
    s.deductedBase = 0.0;
    for (int i = 0; i < s.yearsToRetirement; i++) {
        double growth    = pow(1.0 + CONTRIB_GROWTH, i);
        double empC      = s.annualContribution   * growth;
        double employerC = s.employerContribution * growth;
        totalContributed += empC;
        totalEmployer    += employerC;
        s.deductedBase   += min(empC + employerC, DEDUCTION_CAP);   // both within the regime, capped
        totalTaxSaving   += min(empC, DEDUCTION_CAP) * s.marginalRate;
    }

    cout << "\nRunning " << nSims << " simulations over " << s.yearsToRetirement << " years...\n";

    vector<double> results(nSims);
    for (int i = 0; i < nSims; i++)
        results[i] = simulateOnce(s);

    sort(results.begin(), results.end());

    double unlucky = results[nSims / 10];
    double median  = results[nSims / 2];
    double lucky   = results[nSims * 9 / 10];

    cout << "\n\n========== YOUR SCENARIOS (today's euros, net of fees & taxes) ==========\n";
    cout << "\nUnlucky scenario (worst 10%):   " << (int)unlucky << " EUR";
    cout << "\nTypical scenario (median case): " << (int)median  << " EUR";
    cout << "\nLucky scenario (best 10%):      " << (int)lucky   << " EUR";

    cout << "\n\nYou will pay in " << (int)totalContributed << " EUR (your money, nominal).";
    if (s.employerContribution > 0)
        cout << "\nYour employer will add " << (int)totalEmployer << " EUR on top (free money).";

    // ---- estimated monthly pension (annuity) ----
    cout << "\n\n---------------- ESTIMATED MONTHLY PENSION (today's euros) ----------------\n";
    cout << "Method: annuity - the residual stays invested at " << (int)(RETIREMENT_RETURN * 100)
         << "% real/yr, paid from age " << s.retirementAge << " to " << LIFE_EXPECTANCY << ".\n";
    cout << "Unlucky: ~" << (int)monthlyPension(unlucky, s) << " EUR/month";
    cout << "\nTypical: ~" << (int)monthlyPension(median, s)  << " EUR/month";
    cout << "\nLucky:   ~" << (int)monthlyPension(lucky, s)   << " EUR/month";

    // ---- tax deduction, shown separately ----
    cout << "\n\n---------------- TAX DEDUCTION (shown separately) ----------------\n";
    cout << "Over the period the deduction lowers YOUR taxes by about "
         << (int)totalTaxSaving << " EUR (nominal).";
    cout << "\nThis is NOT added to the pot above: it is tax you did not pay.\n";

    drawHistogram(results);
    printAssumptions();

    return 0;
}

// ===================== NUMERIC INPUTS =====================

void readInputs(Saver &s)
{
    cout << "\n\n---------------- YOUR DATA ----------------\n";
    cout << "Current age: ";
    cin >> s.age;
    cout << "Age you want to retire at: ";
    cin >> s.retirementAge;
    cout << "Your annual contribution, first year (EUR): ";
    cin >> s.annualContribution;
    cout << "Employer's annual contribution, first year (EUR, 0 if none): ";
    cin >> s.employerContribution;
}

// ===================== RISK PROFILE =====================
// The user chooses HOW MUCH risk to take. The expected return is the
// CONSEQUENCE, not a free choice - and we reveal it.

void chooseRiskProfile(Saver &s)
{
    int choice;
    cout << "\n\n---------------- RISK PROFILE ----------------\n";
    cout << "Choose how much risk you are willing to take.\n";
    cout << "More risk = higher expected return, but bigger swings.\n\n";
    cout << "1) Conservative (bonds)\n";
    cout << "2) Balanced\n";
    cout << "3) Aggressive   (equities)\n";
    cout << "Choice: ";
    cin >> choice;

    switch (choice) {
        case 1: s.expectedReturn = 0.03; s.volatility = 0.05; break;
        case 2: s.expectedReturn = 0.05; s.volatility = 0.10; break;
        case 3: s.expectedReturn = 0.07; s.volatility = 0.15; break;
        default:
            cout << "Invalid choice, using the Balanced profile.\n";
            s.expectedReturn = 0.05; s.volatility = 0.10;
    }

    cout << "\n-> Expected average return: " << (int)(s.expectedReturn * 100) << "% per year,";
    cout << "\n   with swings of roughly +/- " << (int)(s.volatility * 100) << "%.\n";
}

// ===================== INCOME BRACKET =====================
// Only for the deduction benefit: sets the IRPEF marginal rate.

void chooseIncomeBracket(Saver &s)
{
    int choice;
    cout << "\n\n---------------- INCOME BRACKET (for the tax deduction) ----------------\n";
    cout << "Your taxable income decides how much the deduction saves you.\n\n";
    cout << "1) up to 28,000 EUR   (marginal rate 23%)\n";
    cout << "2) 28,000 - 50,000    (marginal rate 33%)\n";
    cout << "3) over 50,000        (marginal rate 43%)\n";
    cout << "Choice: ";
    cin >> choice;

    switch (choice) {
        case 1: s.marginalRate = 0.23; break;
        case 2: s.marginalRate = 0.33; break;
        case 3: s.marginalRate = 0.43; break;
        default:
            cout << "Invalid choice, using 23%.\n";
            s.marginalRate = 0.23;
    }
}

// ===================== NUMBER OF SIMULATIONS =====================

int chooseSimulationCount()
{
    int choice;
    cout << "\n\n---------------- NUMBER OF SIMULATIONS ----------------\n";
    cout << "More simulations = more stable estimate, but slower.\n\n";
    cout << "1) 1,000   (fast, shakier estimate)\n";
    cout << "2) 5,000\n";
    cout << "3) 10,000  (recommended)\n";
    cout << "4) 20,000  (slow, more stable estimate)\n";
    cout << "Choice: ";
    cin >> choice;

    switch (choice) {
        case 1: return 1000;
        case 2: return 5000;
        case 3: return 10000;
        case 4: return 20000;
        default:
            cout << "Invalid choice, using 10,000 by default.\n";
            return 10000;
    }
}

// ===================== CORE: ONE MONTE CARLO TRIAL =====================
// Final pot for ONE life, NET of fees and taxes, in TODAY'S euros.
// Returns are LOGNORMAL (never below -100%). Pipeline order is NOT commutative.

double simulateOnce(Saver &s)
{
    static mt19937 gen(time(NULL));
    normal_distribution<double> bell(s.expectedReturn, s.volatility);

    double total = 0.0;

    for (int i = 0; i < s.yearsToRetirement; i++)
    {
        double draw = bell(gen);
        double r = exp(draw - 0.5 * s.volatility * s.volatility) - 1.0;  // lognormal return

        double gain = total * (r - COST);              // (1) fees eat into the return
        if (gain > 0) gain *= (1.0 - RETURN_TAX);      // (2) positive gains taxed at 20% annually

        double growth = pow(1.0 + CONTRIB_GROWTH, i);
        double yearlyContribution = (s.annualContribution + s.employerContribution) * growth;
        total = total + gain + yearlyContribution;     // employee + employer money goes in
    }

    // (3) final benefit tax: 15%, -0.30%/yr beyond the 15th, floor 9%, on the deducted base.
    double payoutRate = 0.15;
    if (s.yearsToRetirement > 15)
        payoutRate = 0.15 - 0.003 * (s.yearsToRetirement - 15);
    if (payoutRate < 0.09) payoutRate = 0.09;
    total -= s.deductedBase * payoutRate;

    // (4) inflation: ALWAYS LAST.
    total /= pow(1.0 + INFLATION, s.yearsToRetirement);

    return total;
}

// ===================== MONTHLY PENSION (annuity) =====================

double monthlyPension(double pot, const Saver &s)
{
    int retirementYears = LIFE_EXPECTANCY - s.retirementAge;
    if (retirementYears < 1) retirementYears = 1;

    double n  = retirementYears * 12.0;
    double rm = pow(1.0 + RETIREMENT_RETURN, 1.0 / 12.0) - 1.0;
    if (rm <= 0) return pot / n;

    return pot * rm / (1.0 - pow(1.0 + rm, -n));
}

// ===================== HISTOGRAM (adaptive bins) =====================

void drawHistogram(const vector<double> &results)
{
    int n = (int)results.size();
    double lo = results[0];
    double hi = results[n - 1];

    cout << "\n\n================ RESULT DISTRIBUTION (today's euros) ================\n";

    double width = (hi - lo) / 10.0;
    if (width <= 0) {
        cout << "\nAll outcomes are about " << (int)lo << " EUR.\n";
        cout << "====================================================================\n";
        return;
    }

    int count[10] = {0};
    for (int i = 0; i < n; i++) {
        int b = (int)((results[i] - lo) / width);
        if (b > 9) b = 9;
        count[b]++;
    }

    int maxCount = 0;
    for (int b = 0; b < 10; b++)
        if (count[b] > maxCount) maxCount = count[b];

    for (int b = 0; b < 10; b++) {
        int binLo = (int)(lo + b * width);
        int binHi = (int)(lo + (b + 1) * width);
        cout << "\n" << binLo << " - " << binHi << "   ";
        int bars = (count[b] * 40) / maxCount;
        for (int k = 0; k < bars; k++) cout << "\u2588";
        cout << "  " << (count[b] * 100) / n << "%";
    }
    cout << "\n====================================================================\n";
}

// ===================== ASSUMPTIONS FOOTER =====================

void printAssumptions()
{
    cout << "\n\n================ ASSUMPTIONS (please read) ================\n";
    cout << "- Results are in TODAY'S euros (deflated at " << (int)(INFLATION * 100) << "%/yr).\n";
    cout << "- Yearly returns drawn from a LOGNORMAL distribution (never below -100%).\n";
    cout << "- Contributions grow at " << (int)(CONTRIB_GROWTH * 100) << "%/yr (keeps the REAL contribution ~constant).\n";
    cout << "- Management fee (ISC) assumed at " << (int)(COST * 100) << "%/yr.\n";
    cout << "- Returns taxed " << (int)(RETURN_TAX * 100) << "%/yr on positive gains (12.5% on govt bonds, simplified).\n";
    cout << "- Final benefit taxed 15% -> 9% by years of participation, on the deducted-contributions base.\n";
    cout << "- Deduction capped at " << (int)DEDUCTION_CAP << " EUR/yr; saving at your marginal rate (bracket straddle ignored).\n";
    cout << "- Employer contributions (if any) go into the pot and are taxed 15%->9% at the end like yours;\n";
    cout << "  they also count toward the deductible cap (an extra benefit not added to the saving figure shown).\n";
    cout << "- Monthly pension via annuity: residual invested at " << (int)(RETIREMENT_RETURN * 100)
         << "% real/yr, paid to age " << LIFE_EXPECTANCY << " (ignores longevity risk past that).\n";
    cout << "- Risk and return are coupled (no risk-free high return).\n";
    cout << "- Educational model, NOT financial advice. Verify against COVIP / Agenzia delle Entrate.\n";
    cout << "===========================================================\n";
}
