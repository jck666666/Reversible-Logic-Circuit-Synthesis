#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <climits>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
/* 0 -> 0-control, 1 -> 1-control, 2 -> copy bit, 3 -> not */

// time cost: 35 min.
using namespace std;

#define rand_seed 114
#define population 100 // population 100
#define loop 5000      // generation 5000
#define test 5
#define delta 0.002
#define delta_change 0.001
#define mMAX 70
#define nMAX 10
#define FunctionNum 1

int m = 30, n = 7;

bool changeBest = false;
/* about Q matrix */
double Q[nMAX][mMAX][4] = {0};

int x[population][nMAX][mMAX] = {0};

/* about fitness */
double fit[population] = {0};
int best = 0, worst = 0; // population of best and worst
double b = 0.0, w = 100;
int gb[nMAX][mMAX] = {0}, gw[nMAX][mMAX] = {0};
int bestAns = 2000000; // Find the best ans in the 50Exp

// about input
vector<int> output;
int allgb[nMAX][mMAX] = {0};
int mingb[nMAX][mMAX] = {0};

// about parameter of KNQTS
int last_ham = INT_MAX;
double adaptive_delta = delta;
void input_target();
void init();
void ans();     // generate ans  5
void repair();  // repair ans
void fitness(); // A/B //FIXME //TODO
void update();
void draw_circuit();
void TruthTable();
string toBinaryString(int n,int NumberOfBits);
int cntCOP(int indexOfx);
int cntGate(int indexOfx);
bool correct(int indexOfx, int in, int out); // find the input is correct or not
int *toBinary(int num);                      // change number from 2 -> 10
int toDecimal(int *num);                     // change number from 10 -> 2
int gate();

// draw line
bool line[nMAX][mMAX];
void getTheLines();

int main()
{
    input_target();
    srand(rand_seed);

    // set m
    if (n <= 3)
        m = 16;
    else if (n > 4)
        m = 40;

    // getans for the number of correct ans
    // numGate for the avg num of gates
    // careGate for the avg num of gates (if it is correct)
    // getfit for the avg fit
    int getans = 0, numGate = 0, careGate = 0;
    double getfit = 0;
    for (int time = 0; time < test; time++)
    {
        b = 0.0, w = 100;
        last_ham = INT_MAX, adaptive_delta = delta;
        init();
        for (int i = 0; i < loop; i++)
        {
            ans();
            repair();
            fitness();
            update();
        }

        int ngate = gate(); // gate count in Exp
        cout << "====== experiment" << time + 1 << " ======\n";
        cout << "number of gate = " << ngate << endl;
        cout << "best fitness = " << b << endl
             << endl;

        if (b >= 1) // care ans
        {
            if (ngate <= bestAns)
            {
                bestAns = ngate;
                for (int i = 0; i < n; i++)
                {
                    for (int j = 0; j < bestAns; j++)
                    {
                        mingb[i][j] = allgb[i][j];
                    }
                }
            }
            getans++;
            careGate += ngate;
        }

        getfit += b;
        numGate += ngate;
    }

    draw_circuit();
    TruthTable();
    return 0;
}

void input_target()
{
    cout << "Input the target circuit: ";
    string str;
    getline(cin, str);
    istringstream in(str);
    string t;
    while (getline(in, t, ' '))
    {
        output.push_back(stoi(t));
    }
    cout << "Your Input:";
    for (int i = 0; i < output.size(); i++)
        cout << output[i] << " ";
    cout << endl;
    int tmp = output.size();
    n = 0;
    while (tmp != 1)
    {
        tmp /= 2;
        n++;
    }
    //cout << n;
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

                /* 0 -> 0-control, 1 -> 1-control, 2 -> copy bit, 3 -> not */
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
        double fit1, fit2, w1 = 1, w2 = 0.0;
        int COP = cntCOP(i);
        int gate = cntGate(i);
        int WG = m - gate;

        fit1 = (double)COP / (double)pow(2, n);
        if (fit1 == 1)
        {
            w2 = 0.4;
        }

        /* count fit2 */
        if (gate == 0) 
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
    /* ¡õ find local best(sb) and local worst(sw) ¡õ */
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
    /* ¡ô find local best(sb) and local worst(sw) ¡ô */

    /* ¡õ KNQTS ¡õ */

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
    if (ham > last_ham) // difference bigger
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
    /*  ¡ô update delta  ¡ô */

    last_ham = ham;

    /*  ¡ô KNQTS  ¡ô */

    /* ¡õ update global value b and w ¡õ */
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
    /* ¡ô update global value b and w ¡ô */

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

            /* ¡õ repair ans ¡õ */
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
            /* ¡ô repair ans ¡ô */

            /* ¡õ quantum NOT gate ??? the standard is gb < lw ¡õ */
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
            /* ¡ô quantum NOT gate ??? the standard is 0.25 ¡ô */
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
                if (qin[j] != x[indexOfx][j][i]) // not map the gate ??? not change
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
            if (x[indexOfx][j][i] == 3) // have not gate ??? not wire gate
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
            if (gb[j][i] == 3) // have not gate ??? not wire gate
            {

                for (int k = 0; k < n; k++)
                {
                    allgb[k][cnt] = gb[k][i];
                }
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
void draw_circuit()
{
    cout << "\n==== Circuit Diagram ====\n(¡³: low-level control bit)\n(¡´: high-level control bit)\n(¡ò: not-gate)\n";
    getTheLines();
    for (int i = 0; i < n; i++)
    {
        cout << "q" << i;
        for (int j = 0; j < bestAns; j++)
        {
            switch (mingb[i][j])
            {
            case 0:
                cout << "--¡³-";
                break;
            case 1:
                cout << "--¡´-";
                break;
            case 3:
                cout << "--¡ò-";
                break;
            case 2:
                cout << "-----";
                break;
            }
            if (j == bestAns - 1 && i != n - 1)
            {
                cout << "\n";
                for (int k = 0; k < bestAns; k++)
                {
                    if (k == 0)
                    {
                    	if (line[i][k])
                    		cout << "    ¡U ";
                    	else
                    		cout << "       ";
					}
                    else if (line[i][k])
                    {
                    	cout << "  ¡U ";
					}
					else
					{
						cout << "     ";
					}
                }
                cout << "\n";
            }
        }
    }
    cout << endl;
}

void getTheLines()
{
	// draw or not
	for (int i = 0; i < bestAns; i++)	// through m gates
	{
		bool lineStart = false;
		int start = 0, end = 0;
		for (int j = 0; j < n; j++) // through n bits
		{
			if (!lineStart)
			{
				if (mingb[j][i] == 2)
				{
					start++;
				}
				else
				{
					end = start;
					lineStart = true;
				}
			}
			else // find end
			{
				if (mingb[j][i] != 2)
				{
					end = j-1;
				}
			}
		}
		for (int j = start; j <= end; j++)
		{
			line[j][i] = true;
		}
	}
}

void TruthTable(){
	int OutputLength = output.size();
	int tmp = OutputLength;
	int BitsNumber = 0;
	int tmpinput[nMAX][mMAX] = {0};
	while(tmp != 1){
		BitsNumber += 1;
		tmp /= 2;
	}
	for (int i = 0; i < n; ++i)
      for (int j = 0; j < bestAns; ++j) {
         tmpinput[j][i] = mingb[i][j];
      }
	cout << endl << endl << "====== Truth Table ======\n" ;

	for(int i = 0;i < pow(2,BitsNumber);i++){ //pow(2,qubit)
		string request = toBinaryString(i,BitsNumber);
		string output;
		int flag = 1;
		int notbit = 0;
		for(int j = 0;j < BitsNumber;j++){
			cout << request[BitsNumber - j - 1] << " ";
		}
		cout << "	->	";
		for(int j = 0;j < bestAns;j++){// 0 for 0 control,1 for 1 control, 2 for wire, 3 for not
			flag = 1;
			for(int k = 0;k < BitsNumber;k++){ //0 for not, 1 for normal wire, 2 for positive control, 3 for negative control
				if(tmpinput[j][k] == 1 && request[BitsNumber - k - 1] != '1') // condition fall
					flag = 0;
				else if(tmpinput[j][k] == 0 && request[BitsNumber - k - 1] != '0') // condition fall
					flag = 0;
				else if(tmpinput[j][k] == 3)
					notbit = k;
			}
			if(flag == 1){// control not
				if(request[BitsNumber - notbit - 1] == '0')
					request[BitsNumber - notbit - 1] = '1';
				else
					request[BitsNumber - notbit - 1] = '0';
			}
		}
		for(int j = 0;j < BitsNumber;j++){
			cout << request[BitsNumber - j - 1] << " ";
		}
		cout << "\n";
	}
}

string toBinaryString(int n,int NumberOfBits)
{
    string r;
    int len = 0;
    while (n != 0){
        r += ( n % 2 == 0 ? "0" : "1" );
        len += 1;
        n /= 2;
    }
    while(len != NumberOfBits){
    	r += "0";
    	len++;
	}
    return r;
}



