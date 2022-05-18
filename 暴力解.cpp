#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <cfloat>
#include <windows.h>
using namespace std;
#define ctrl_not 3
#define wire 2

#define n 3
#define gate 5

int cntCOP();
bool correct(int in, int out);
int cntGate();
int *toBinary(int num);                      // change number from 2 -> 10
int toDecimal(int *num);  

const int func[32] = {0, 1, 2, 4, 3, 5, 6, 7};
//const int func[32] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};
//const int func[32] = {};

int arr[gate][n];
double fit;
int COP = pow(2, n);
int cnot_position[gate];

int cntCOP()
{
    int cnt = 0;
    for (int i = 0; i < pow(2, n); i++)
    {
        if (correct(i, func[i])) // is correct
        {
            cnt++;
        }
    }

    return cnt;
}

bool correct(int in, int out)
{
    int *qin = toBinary(in);

    for (int i = 0; i < gate; i++) // through m gates
    {
        int hasNOT = -1;            // whether it has not or not and it's index
        bool chg = true;            // change the not bit or not
        for (int j = 0; j < n; j++) // through n bits of a gate
        {
            if (arr[j][i] == 0 || arr[j][i] == 1)
            {
                if (qin[j] != arr[j][i]) // not map the gate → not change
                {
                    chg = false;
                    break;
                }
            }
            else if (arr[j][i] == 3) // has not gate
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

int cntGate()
{
    int cnt = 0;
    for (int i = 0; i < gate; i++) // check all of the gate
    {
        for (int j = 0; j < n; j++) // check each bits of the gate
        {
            if (arr[j][i] == 3) // have not gate → not wire gate
            {
                cnt++;
                break;
            }
        }
    }

    return cnt;
}

void fitness()
{
        double fit1 = 0.0, fit2 = 0.0, w = 0;
        int COP = cntCOP();
        int num_of_gate = cntGate();
        int WG = gate - num_of_gate;

        fit1 = (double)COP / (double)pow(2, n);
        if (fit1 == 1)
        {
            w = 1;
        }

        fit2 = 0.0;

        /* count fit2 */
        if (gate == 0) // denominator of a fraction is 0 → fit2 is infinite
        {
            fit2 = (double)(1 - WG);
        }
        else
        {
            fit2 = (double)WG / (double)gate;
        }

        fit = fit1 + w*fit2;
}

void printarr()
{
    for (int k = 0; k < n; k++)
    {
        for (int j = 0; j < gate; j++)
        {

            cout << arr[j][k];
        }
        cout << endl;
    }
    cout << endl;
}

void output2()
{
    cout << "output(ctrl_not:2,wire:3):" << endl;
    int cnt = 0;
    for (int j = 0; j < gate; j++)
    {
        int flag = 0;
        for (int k = 0; k < n; k++)
        {
            if (arr[j][k] == ctrl_not)
                flag = 1;
            if (arr[j][k] == ctrl_not)
                cout << 2;
            else if (arr[j][k] == wire)
                cout << 3;
            else
                cout << arr[j][k];
        }

        if (flag)
        {
            cout << endl;
            cnt++;
        }

        else
            cout << "\r";
    }
    cout << "total gate: " << cnt << endl;
}

int main()
{
    int cnot_num = pow(n, gate); // CNOT 所有可能的位置
    int all_possible = pow(3, (n - 1) * gate);

    for (int i = 0; i < cnot_num; i++) // 所有NOT位置可能性
    {
        int tmp = i; // 現在cnot的位置 (用n進制編碼)
        for (int j = 0; j < gate; j++)
        {
            cnot_position[j] = tmp % n;
            arr[j][tmp%n] = ctrl_not;
            tmp /= n;
        }

        for (int num = 0; num < all_possible; num++)
        {
            int temp = num;
            for (int j = 0; j < gate; j++)
            {
                for (int k = 0; k < n; k++) // each bit
                {
                    if (k != cnot_position[j])
                    {
                        arr[j][k] = temp % 3;
                        temp /= 3;
                    }

                }
            }

            fitness();
            if (fit >= 1)
            {
                cout << "find" << endl;
                printarr();
                cout << endl;
                output2();
                Beep(287, 500);
                system("pause");
            }

        }
    }
    
    cout << "end\n";

}
