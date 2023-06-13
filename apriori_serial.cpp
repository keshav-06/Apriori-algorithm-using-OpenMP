/* Serial Code for apriori algorithm */
#include <bits/stdc++.h>
#include <omp.h> // here omp is used for calculating net execution time
using namespace std;
#define structure map<vector<int>, int>
#define FOR_MAP(i, T) for (structure::iterator(i) = (T).begin(); (i) != (T).end(); (i)++)
#define FOR_next_MAP(j, i, T) for (structure::iterator(j) = (i); (j) != (T).end(); (j)++)
#define VI vector<int>
int MIN_SUP;
structure C;
structure L;
void C1(string);                  /* Function to generate the first list of candidate items from the transaction file */
void L1();                        /* Function to calculate and prune the items with frequency above support count. */
void generate_C();                /* Generating all the cadidates of size k from frequency list of size k-1 */
void generate_L();                /* Finding the frequent itemsets of size k by removing all candidate item sets having their count less than minimum support count */
void output(structure);           /* Prints all the candidates or frequency list along with their count */
void scan_D(string);              /* Scanning the database and calling set_count for each transaction */
void prune();                     /* Removing all the candidates of size k whose subsets of size k-1 are not in the frequency list of size k-1 */
bool check_compatibility(VI, VI); /* Checking if any two frequency item sets are same or not */
void set_count(VI);               /* Incrementing the count of the candidate if it exists in the transaction, it is done for all candidates */
int main(int argc, char const *argv[])
{
    int l;
    string file_name;
    cout << "Enter the minimum support count: " << endl;
    cin >> MIN_SUP;
    cout << "Enter the input_file name" << endl;
    cin >> file_name;
    double start, end, time;
    start = omp_get_wtime();
    C.clear();
    L.clear();
    bool mv = true;
    int index = 2;
    while (true)
    {
        if (mv)
        {
            C1(file_name);
            cout << "Initial Candidates\n";
            output(C);
            L1();
            cout << "Candidates with sufficient Frequency\n";
            output(L);
            mv = !mv;
        }
        else
        {

            generate_C();
            if (C.size() == 0)
            {
                // cout << "C is null\n";
                break;
            }
            prune();
            if (C.size() == 0)
            {
                // cout << "C is null\n";
                break;
            }
            scan_D(file_name);
            generate_L();
            if (L.size() == 0)
            {
                // cout << "L is null\n";
                break;
            }
            cout << "\nFrequency List " << index << "\n";
            output(L);

            index++;
        }
    }
    end = omp_get_wtime();
    time = end - start;
    cout << "the time required is : " << time << "\n";
    cout << "no of iterations : " << index << "\n";
    return 0;
}
void C1(string file_name)
{
    ifstream fin;
    fin.open(file_name.c_str());
    if (!fin)
    {
        cout << "Input file opening error\n";
        exit(0);
    }
    int n;
    VI v;
    while (fin >> n)
    {
        v.clear();
        if (n == -1) // -1 indicates end of line (transaction)
        {
            continue;
        }
        v.push_back(n);
        if (C.count(v) > 0)
            C[v]++;
        else
            C[v] = 1;
    }
    fin.close();
}
void output(structure T)
{
    cout << "\n";
    VI v;
    FOR_MAP(ii, T)
    {
        v.clear();
        v = ii->first;
        for (int i = 0; i < v.size(); ++i)
        {
            cout << v[i] << " ";
        }
        cout << " ---(frequency)----->> " << ii->second;
        cout << "\n";
    }
}
void L1()
{
    FOR_MAP(i, C)
    {
        if (i->second >= MIN_SUP)
        {
            L[i->first] = i->second;
        }
    }
}
void generate_C()
{
    C.clear();
    FOR_MAP(ii, L)
    {
        FOR_next_MAP(jj, ii, L)
        {
            if (jj == ii)
                continue;
            VI a, b;
            a.clear();
            b.clear();
            a = ii->first;
            b = jj->first;
            if (check_compatibility(a, b))
            {
                a.push_back(b.back());
                sort(a.begin(), a.end());
                C[a] = 0;
            }
        }
    }
}
bool check_compatibility(VI a, VI b)
{
    bool compatible = true;
    for (int i = 0; i < a.size() - 1; ++i)
    {
        if (a[i] != b[i])
        {
            compatible = false;
            break;
        }
    }
    return compatible;
}
void prune()
{
    VI a, b;

    FOR_MAP(ii, C)
    {
        a.clear();
        b.clear();
        a = ii->first;
        for (int i = 0; i < a.size(); i++)
        {
            b.clear();
            for (int j = 0; j < a.size(); ++j)
            {
                if (j == i)
                    continue;
                b.push_back(a[j]); // all the subsets are generated one by one and stored in b
            }
            if (L.find(b) == L.end()) // check if subset exists in frequency item - set
            {
                ii->second = -1;
                break;
            }
        }
    }
    structure temp;
    temp.clear();
    FOR_MAP(ii, C)
    {
        if (ii->second != -1)
        {
            temp[ii->first] = ii->second;
        }
    }

    C.clear();
    C = temp;
    temp.clear();
}
void scan_D(string file_name)
{
    ifstream fin;
    fin.open(file_name.c_str());
    if (!fin)
    {
        cout << "Input file opening error\n";
        exit(0);
    }
    int n;
    VI a;
    while (fin >> n)
    {
        if (n == -1 && a.size() > 0)
        {
            set_count(a);
            a.clear();
        }
        else if (n != -1)
        {
            a.push_back(n);
        }
    }
    fin.close();
}
void set_count(VI a)
{
    FOR_MAP(ii, C)
    {
        VI b;
        b.clear();
        b = ii->first;
        int x;
        int true_count = 0;
        int prev = -1;
        if (b.size() <= a.size())
        {
            for (int i = 0; i < b.size(); ++i)
            {
                for (int j = prev + 1; j < a.size(); ++j)
                {
                    if (b[i] == a[j])
                    {
                        true_count++;
                        prev = j;
                        break;
                    }
                }
            }
        }
        if (true_count == b.size())
        {
            ii->second++;
        }
    }
}
void generate_L()
{
    L.clear();
    FOR_MAP(ii, C)
    {
        if (ii->second >= MIN_SUP)
        {
            L[ii->first] = ii->second;
        }
    }
}
