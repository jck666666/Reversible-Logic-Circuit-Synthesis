#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <climits>

/* 0 → 0-control, 1 → 1-control, 2 → copy bit, 3 → not */

// time cost: 35 min.
using namespace std;

#define rand_seed 114
#define population 100 // population 100
#define loop 5000      // generation 5000
#define test 50
#define delta 0.002
#define delta_change 0.001
#define mMAX 200
#define nMAX 10
#define FunctionNum 1

int m = 4, n = 6;

bool changeBest = false;
/* about Q matrix */
double Q[nMAX][mMAX][4] = {0};

int x[population][nMAX][mMAX] = {0};

/* about fitness */
double fit[population] = {0};
int best = 0, worst = 0; // population of best and worst
double b = 0.0, w = 100;
int gb[nMAX][mMAX] = {0}, gw[nMAX][mMAX] = {0};

// FIXME
int output[mMAX] = {0, 32, 1, 48, 2, 17, 33, 56, 4, 18, 34, 25, 3, 41, 49, 60, 8, 20, 36, 26, 5, 42, 50, 29, 6, 11, 19, 45, 35, 53, 57, 62, 16, 24, 40, 28, 9, 44, 52, 30, 10, 13, 21, 46, 37, 54, 58, 31, 12, 14, 22, 15, 38, 23, 27, 47, 7, 39, 43, 55, 51, 59, 61, 63}; // int output[power(2,n)]

// about parameter of KNQTS
int last_ham = INT_MAX;
double adaptive_delta = delta;

void init();
void ans();     // generate ans  5
void repair();  // repair ans
void fitness(); // A/B //FIXME //TODO
void update();

int cntCOP(int indexOfx);
int cntGate(int indexOfx);
bool correct(int indexOfx, int in, int out); // find the input is correct or not
int *toBinary(int num);                      // change number from 2 -> 10
int toDecimal(int *num);                     // change number from 10 -> 2
int gate();

int main()
{
    for (int test_m = 10; test_m <= 70; test_m++)
    {
        m = test_m;
        srand(rand_seed);
        int total = 0;
        int generation = 0;

        // getans for the number of correct ans
        // numGate for the avg num of gates
        // careGate for the avg num of gates (if it is correct)
        // getfit for the avg fit
        int getans = 0, numGate = 0, careGate = 0;
        double getfit = 0;
        int bestAns = 2000000; // Find the best ans in the 50Exp
        for (int time = 0; time < test; time++)
        {
            b = 0.0, w = 100, generation = 0;
            last_ham = INT_MAX, adaptive_delta = delta;
            init();
            for (int i = 0; i < loop; i++)
            {
                changeBest = false;
                ans();
                repair();
                fitness();
                update();
                if (changeBest)
                {
                    generation = i;
                }
            }

            int ngate = gate(); // 做完一次實驗得到的gate數
            cout << "====== experiment" << time + 1 << " ======\n";
            cout << "number of gate = " << ngate << endl;
            cout << "best fitness = " << b << endl;
            cout << "best generation = " << generation << endl
                 << endl;

            if (b >= 1) // care ans
            {
                ngate <= bestAns ? bestAns = ngate : bestAns = bestAns;
                getans++;
                careGate += ngate;
            }

            getfit += b;
            numGate += ngate;
        }

        cout << getans << "\t" << getfit / (double)test << "\t" << (double)numGate / (double)test
             << "\t" << (double)careGate / (double)getans << "\t" << m << "\t"
             << bestAns << endl;

        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (gb[j][i] == 2)
                    cout << 3 << " ";
                else if (gb[j][i] == 3)
                    cout << 2 << " ";
                else
                    cout << gb[j][i] << " ";
            }
        }
    }

    system("pause");
    return 0;
}

void init()
{
    /* initialize Q matrix */
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            for (int k = 0; k < 4; k++)
            {
                Q[i][j][k] = 0.25;
            }
        }
    }
}

void ans()
{
    for (int i = 0; i < population; i++) // population
    {
        for (int j = 0; j < n; j++) // each component
        {
            for (int k = 0; k < m; k++)
            {
                double r = (double)rand() / RAND_MAX;

                /* 0 → 0-control, 1 → 1-control, 2 → copy bit, 3 → not */
                if (r <= Q[j][k][0])
                {
                    x[i][j][k] = 0;
                }
                else if (r > Q[j][k][0] && r <= (Q[j][k][0] + Q[j][k][1]))
                {
                    x[i][j][k] = 1;
                }
                else if (r > (Q[j][k][0] + Q[j][k][1]) && r <= (Q[j][k][0] + Q[j][k][1] + Q[j][k][2]))
                {
                    x[i][j][k] = 2;
                }
                else
                {
                    x[i][j][k] = 3;
                }
            }
        }
    }
}

void repair()
{
    for (int i = 0; i < population; i++)
    {
        for (int j = 0; j < m; j++) // gate
        {
            int maxIndex = -1;
            double maxProb = 0;
            for (int k = 0; k < n; k++)
            {

                if (x[i][k][j] == 3) // not
                {

                    /* find the max prob. of not */
                    if (Q[k][j][3] > maxProb)
                    {
                        /* repair last not */
                        if (maxIndex != -1) // not first
                        {
                            double max = 0.0;
                            int index = 0;
                            for (int a = 0; a < 3; a++)
                            {
                                if (Q[maxIndex][j][a] > max)
                                {
                                    index = a;
                                    max = Q[maxIndex][j][a];
                                }
                            }
                            x[i][maxIndex][j] = index;
                        }

                        maxIndex = k;
                        maxProb = Q[k][j][3];
                    }
                    else
                    {
                        double max = 0.0;
                        int index = 0;
                        for (int a = 0; a < 3; a++)
                        {
                            if (Q[k][j][a] > max)
                            {
                                index = a;
                                max = Q[k][j][a];
                            }
                        }
                        x[i][k][j] = index;
                    }
                }
            }
        }
    }
}

void fitness()
{
    for (int i = 0; i < population; i++)
    {
        double fit1 = 0.0, fit2 = 0.0, w1 = 1, w2 = 0.0;
        int COP = cntCOP(i);
        int gate = cntGate(i);
        int WG = m - gate;

        fit1 = (double)COP / (double)pow(2, n);
        if (fit1 == 1)
        {
            w2 = 0.4;
        }

        fit2 = 0.0;

        /* count fit2 */
        if (gate == 0) // denominator of a fraction is 0 → fit2 is infinite
        {
            fit2 = (double)(1 - WG);
        }
        else
        {
            fit2 = (double)WG / (double)m;
        }

        fit[i] = w1 * fit1 + w2 * fit2;
    }
}

void update()
{
    /* ↓ find local best(sb) and local worst(sw) ↓ */
    double max = 0, min = 100;
    int sb = 0, sw = 0;

    for (int i = 0; i < population; i++)
    {
        /* find best */
        if (fit[i] >= max)
        {
            max = fit[i];
            sb = i;
        }

        /* find worst */
        if (fit[i] <= min)
        {
            min = fit[i];
            sw = i;
        }
    }
    /* ↑ find local best(sb) and local worst(sw) ↑ */

    /* ↓ KNQTS ↓ */

    // hamming distance between sb and sw

    int ham = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            if (x[sb][i][j] != x[sw][i][j])
            {
                ham++;
            }
        }
    }

    // update delta
    last_ham == INT_MAX ? last_ham = ham : last_ham = last_ham;

    // compare to last generation
    if (ham > last_ham) // 差異變大
    {
        adaptive_delta *= 1.001;
    }
    else if (ham < last_ham)
    {
        adaptive_delta *= 0.999;
    }
    else
    {
        adaptive_delta = adaptive_delta;
    }
    /* ↑ update delta ↑ */

    last_ham = ham;

    /* ↑ KNQTS ↑ */

    /* ↓ update global value b and w ↓ */
    if (max >= b)
    {
        changeBest = true;
        b = max;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < m; j++)
            {
                gb[i][j] = x[sb][i][j];
            }
        }
    }

    if (min <= w)
    {
        w = min;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < m; j++)
            {
                gw[i][j] = x[sw][i][j];
            }
        }
    }
    /* ↑ update global value b and w ↑ */

    /* update Q matrix */
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            if (gb[i][j] != gw[i][j]) // have to update
            {
                Q[i][j][gb[i][j]] += adaptive_delta;
                Q[i][j][gw[i][j]] -= adaptive_delta;
            }

            /* ↓ repair ans ↓ */
            if (Q[i][j][gw[i][j]] <= 0)
            {
                Q[i][j][gw[i][j]] = 0;

                /* Q[i][j][gb[i][j]] = 1 - remain */
                Q[i][j][gb[i][j]] = 1;
                for (int k = 0; k < n; k++)
                {
                    if (k != gb[i][j])
                    {
                        Q[i][j][gb[i][j]] -= Q[i][j][k];
                    }
                }
            }
            /* ↑ repair ans ↑ */

            /* ↓ quantum NOT gate → the standard is gb < lw ↓ */
            if (gb[i][j] != gw[i][j])
            {
                if (Q[i][j][gb[i][j]] < 0.25) // NOT
                {
                    /* find max */
                    int maxIndex = 0, minIndex = 0;
                    double max = Q[i][j][0], min = Q[i][j][0];
                    for (int k = 1; k < 4; k++)
                    {
                        if (Q[i][j][k] > max)
                        {
                            max = Q[i][j][k];
                            maxIndex = k;
                        }
                        else if (Q[i][j][k] < min)
                        {
                            min = Q[i][j][k];
                            minIndex = k;
                        }
                    }

                    /* swap the Prob. of gb and max */
                    double tmp = Q[i][j][maxIndex];
                    Q[i][j][maxIndex] = Q[i][j][gb[i][j]];
                    Q[i][j][gb[i][j]] = tmp;

                    /* swap the Prob. of lw and min */
                    tmp = Q[i][j][minIndex];
                    Q[i][j][minIndex] = Q[i][j][gw[i][j]];
                    Q[i][j][gw[i][j]] = tmp;
                }
            }
            /* ↑ quantum NOT gate → the standard is 0.25 ↑ */
        }
    }
}

int cntCOP(int indexOfx)
{
    int cnt = 0;
    for (int i = 0; i < pow(2, n); i++)
    {
        if (correct(indexOfx, i, output[i])) // is correct
        {
            cnt++;
        }
    }

    return cnt;
}

bool correct(int indexOfx, int in, int out)
{
    int *qin = toBinary(in);

    for (int i = 0; i < m; i++) // through m gates
    {
        int hasNOT = -1;            // whether it has not or not and it's index
        bool chg = true;            // change the not bit or not
        for (int j = 0; j < n; j++) // through n bits of a gate
        {
            if (x[indexOfx][j][i] == 0 || x[indexOfx][j][i] == 1)
            {
                if (qin[j] != x[indexOfx][j][i]) // not map the gate → not change
                {
                    chg = false;
                    break;
                }
            }
            else if (x[indexOfx][j][i] == 3) // has not gate
            {
                hasNOT = j;
            }
        }

        if ((hasNOT >= 0) && chg)
        {
            qin[hasNOT] = !qin[hasNOT];
        }
    }

    if (toDecimal(qin) == out)
    {
        delete qin;
        return true;
    }

    delete qin;
    return false;
}

int cntGate(int indexOfx)
{
    int cnt = 0;
    for (int i = 0; i < m; i++) // check all of the gate
    {
        for (int j = 0; j < n; j++) // check each bits of the gate
        {
            if (x[indexOfx][j][i] == 3) // have not gate → not wire gate
            {
                cnt++;
                break;
            }
        }
    }

    return cnt;
}

int gate()
{
    int cnt = 0;
    for (int i = 0; i < m; i++) // check all of the gate
    {
        for (int j = 0; j < n; j++) // check each bits of the gate
        {
            if (gb[j][i] == 3) // have not gate → not wire gate
            {
                cnt++;
                break;
            }
        }
    }

    return cnt;
}

int *toBinary(int num)
{
    int *binary = new int[n];

    for (int i = n - 1; i >= 0; i--)
    {
        binary[i] = num % 2;
        num /= 2;
    }

    return binary;
}

int toDecimal(int *num)
{
    int decimal = 0, exp = 1;

    for (int i = n - 1; i >= 0; i--)
    {
        decimal += num[i] * exp;
        exp *= 2;
    }

    return decimal;
}
