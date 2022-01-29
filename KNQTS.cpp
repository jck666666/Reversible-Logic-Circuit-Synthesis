#include <iostream>
#include <string>
#include <stdlib.h>
#include <cmath>
#include <climits>

/* 0 → 0-control, 1 → 1-control, 2 → copy bit, 3 → not */

using namespace std;

#define rand_seed 114
#define population 100 // population 100
#define loop 5000      // generation 5000
#define test 50
//#define delta 0.002
#define m 10 // FIXME
#define n 3  // FIXME

bool changeBest = false;
/* about Q matrix */
double Q[n][m][4] = {0};

int x[population][n][m] = {0};

/* about fitness */
double fit[population] = {0};
int best = 0, worst = 0; // population of best and worst
double b = 0.0, w = 100;
int gb[n][m] = {0}, gw[n][m] = {0};

// FIXME
int output[16] = {7,5,4,2,0,1,6,3}; // int output[power(2,n)]

// about parameter of KNQTS
double delta = 0.002;
int diff = INT_MAX;

void init();
void ans();        // generate ans  5   
void repair();     // repair ans
void fitness();    // A/B //FIXME //TODO
void oldfitness(); // w1*fit1+w2*fit2 //FIXME //TODO
void update();

int cntCOP(int indexOfx);
int cntGate(int indexOfx);
bool correct(int indexOfx, int in, int out);
int *toBinary(int num);
int toDecimal(int *num);
int gate();

int main()
{
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
        init();
        for (int i = 0; i < loop; i++)
        {
            changeBest = false;
            ans();
            repair();
            oldfitness();
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

    cout << "get ans = " << getans << endl;
    cout << "avg_fit = " << getfit / (double)test << endl;
    cout << "avg_gate = " << (double)numGate / (double)test << endl;
    cout << "care_gate = " << (double)careGate / (double)getans << endl;
    cout << "best ans = " << bestAns << endl;

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
        int COP = cntCOP(i);
        int gate = cntGate(i);

        double fit1 = (double)COP / (double)pow(2, n);
        double fit2 = 0.0;

        /* count fit2 */
        if (gate == 0) // denominator of a fraction is 0 → fit2 is infinite
        {
            fit2 = (double)m;
        }
        else
        {
            fit2 = (double)gate / (double)m;
        }

        fit[i] = fit1 / fit2;
        // cout << "COP = " << COP << endl;
        // cout << "fit1 = " << fit1 << endl;
        // cout << "gate = " << gate << endl;
        // cout << "fit2 = " << fit2 << endl;
        // cout << "fit[i]" << fit[i] << endl << endl;
    }
}

void oldfitness()
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

    /* ↓ update delta ↓ */

    // hamming distance between sb and sw
    int nowdiff = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            if (x[sb][i][j] != x[sw][i][j])
            {
                nowdiff++;
            }
        }
    }

    //update delta
    diff == INT_MAX ? diff = nowdiff : diff = diff;

    //compare to last generation
    if (nowdiff > diff) // 差異變大
    {
        delta *= 1.01;
    }
    else if (nowdiff < diff)
    {
        delta *= 0.99;
    }
    /* ↑ update delta ↑ */

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
                Q[i][j][gb[i][j]] += delta;
                Q[i][j][gw[i][j]] -= delta;
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

            /* ↓ quantum NOT gate → the standard is 0.25 ↓ */
            if (gb[i][j] != gw[i][j])
            {
                if (Q[i][j][gb[i][j]] < 0.25) // NOT
                {
                    /* swap the Prob. of gb and gw */
                    double tmp = Q[i][j][gw[i][j]];
                    Q[i][j][gw[i][j]] = Q[i][j][gb[i][j]];
                    Q[i][j][gb[i][j]] = tmp;
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