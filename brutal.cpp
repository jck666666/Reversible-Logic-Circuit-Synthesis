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

// tune--------
#define n 3
#define gate 5 // m
// tune--------

double W1 = 1, W2 = 0;
// const int func[32] = {1, 0, 3, 2, 5, 7, 4, 6};//6
// const int func[32] = {7,0,1,2,3,4,5,6};//6
// const int func[32] = {0,1,2,3,4,6,5,7};//6
 const int func[32] = {0, 1, 2, 4, 3, 5, 6, 7}; // 10
// const int func[32] = {1,2,3,4,5,6,7,0};//10
// const int func[32] = {3,6,2,5,7,1,0,4};//10
// const int func[32] = {1,2,7,5,6,3,0,4};//10
// const int func[32] = {4,3,0,2,7,5,6,1};//10
// const int func[32] = {7,5,4,2,0,1,6,3};//10
// const int func[32] = {7, 0, 1, 3, 4, 2, 6, 5};//10
// const int func[32] = {0, 2, 1, 4, 7, 5, 6, 3};//10+

// const int func[32] = {0, 1, 14, 15, 4, 5, 10, 11, 7, 9, 6, 8, 12, 13, 2, 3};//17
//const int func[32] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0}; // 6
// const int func[32] = {0, 7, 6, 9, 4, 11, 10, 13, 8, 15, 14, 1, 12, 3, 2, 5};//8
// const int func[32] = {6, 3, 14, 13, 2, 11, 7, 10, 0, 5, 8, 1, 12, 15, 9, 4};//13
// const int func[32] = {0, 9, 10, 5, 4, 15, 14, 8, 11, 2, 6, 3, 12, 7, 1, 13};//17
// const int func[32] = {6, 4, 11, 0, 9, 8, 12, 2, 15, 5, 3, 7, 10, 13, 14, 1}; // can't find//16
// const int func[32] = {13, 1, 14, 0, 9, 2, 15, 6, 12, 8, 11, 3, 4, 5, 7, 10}; // 16
int arr[gate][n];
double fit;
int nn;
int cnot_position[gate];
void fitness()
{
    fit = 0.5;
    int correct_cnt = 0;
    int num_of_wire = 0;
    for (int num = 0; num < nn; num++)
    {
        int num_bin[n];
        int num_dec = num;

        for (int j = 0; j < n; j++)
        {

            num_bin[n - j - 1] = num_dec % 2; //{2^n,2^(n-1),....2^1,2^0}
            num_dec /= 2;
        }
        num_of_wire = 0;
        for (int j = 0; j < gate; j++)
        {
            int ctrl_not_pos = -1;
            int ch = 1;
            for (int k = 0; k < n; k++)
            {
                if (arr[j][k] == 0)
                {
                    if (num_bin[k] != 0)
                        ch = 0;
                }
                else if (arr[j][k] == 1)
                {
                    if (num_bin[k] != 1)
                        ch = 0;
                }
                else if (arr[j][k] == ctrl_not)
                {
                    ctrl_not_pos = k;
                }
                else if (arr[j][k] == wire)
                {
                }

                else
                {
                    cout << "error in fitness" << endl;
                    exit(0);
                }
            }
            if (ctrl_not_pos == -1)
                num_of_wire++;
            if (ch == 1 && ctrl_not_pos != -1)
            {
                num_bin[ctrl_not_pos] = !num_bin[ctrl_not_pos];
            }
        }
        int exp = nn / 2;
        num_dec = 0;
        for (int j = 0; j < n; j++)
        {
            num_dec += num_bin[j] * exp;
            exp /= 2;
        }
        if (num_dec == func[num])
            correct_cnt++;
        // cout<<num_dec<<",";
    }
    // cout<<endl;
    double fit1 = ((double)correct_cnt / nn);
    fit = fit1;
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
    // cout<<pow(n,gate)<<endl;
    nn = pow(2, n);
    int num_of_cnot = pow(n, gate);
    int num_of_all = pow(3, ((n - 1) * gate));
    cout << num_of_all << endl;
    for (int i = 0; i < num_of_cnot; i++)
    {
        // cout << "sgfs" << endl;
        int tmp = i;
        for (int j = 0; j < gate; j++)
        {
            cnot_position[gate - j - 1] = tmp % n;
            arr[gate - j - 1][tmp % n] = ctrl_not;
            tmp /= n;
        }
        for (int num = 0; num < num_of_all; num++)
        {

            int tmp2 = num;
            for (int j = 0; j < gate; j++)
            {
                for (int k = 0; k < n; k++)
                {
                    if (k == cnot_position[j])
                        continue;
                    else
                    {
                        arr[j][k] = tmp2 % 3;
                        tmp2 /= 3;
                    }
                }
            }

            fitness();
        //    cout << fit << endl;
         //   printarr();
            // output2();
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

    // fitness();
}
