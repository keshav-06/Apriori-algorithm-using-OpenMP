#include <bits/stdc++.h>
#include <omp.h>
using namespace std;
// macros
#define structure map<vector<int>, int>
#define FOR_MAP(ii, T) for(structure::iterator (ii)=(T).begin();(ii)!=(T).end();(ii)++)
#define FOR_next_MAP(jj, ii, T) for(structure::iterator (jj)=(ii);(jj)!=(T).end();(jj)++)
#define VI vector<int>
int MIN_SUP;
structure C;
structure L;
void C1(string);
void L1();
void generate_C();
void generate_L();
void output(structure);
void scan_D(int, int, string);
void prune();
bool check_compatibility(VI, VI);
void set_count(VI, structure *);
int main(int argc, char const *argv[])
{
    int l;
    string file_name;
    cout << "Enter the minimum support count: " << endl;
    cin >> MIN_SUP;
    cout << "Enter transactions: " << endl;
    cin >> l;
    cout << "Enter the input_file name" << endl;
    cin >> file_name;
    int threads;
    cout << "Enter the number of threads: " << endl;
    cin >> threads;
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
                break;
            prune();
            if (C.size() == 0)
                break;
            scan_D(l, threads, file_name);
            generate_L();
            if (L.size() == 0)
                break;
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

/* Generating the first candidate list */
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
        if (n == -1)
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

/* Prints all the candidates or frequency list along with their count */
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
/* Generating the first frequency list */
void L1()
{
    FOR_MAP(ii, C)
    {
        if (ii->second >= MIN_SUP)
        {
            L[ii->first] = ii->second;
        }
    }
}
/* Generating all the cadidates of size k from frequency list of size k-1 */
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
/* Checking if any two frequency item sets are same or not */
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
/* Removing all the candidates of size k whose subsets of size k1 are not in the frequency list of size k-1 */
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
/* For going to a particular line in the file */
FILE *GotoLine(FILE *file, unsigned int num)
{

    int count = 0;
    char buf[100];
    do
    {
        if (count == num)
            break;
        fgets(buf, 100, file);
        count++;
    } while (1);
    return file;
}
/* Scanning the database and calling set_count for each transaction */
void scan_D(int l, int t, string file_name)
{
    int p = l / t; // no of transactions per database
    FILE *f[t];
    int i;
    for (i = 0; i < t; i++)
    {

        f[i] = fopen(file_name.c_str(), "r"); // each thread opens its own file
        if (f[i] == NULL)
        {
            cout << "Input file opening error\n";
            exit(0);
        }

        f[i] = GotoLine(f[i], i * p); // each file has a poiner after p lines for each thread
    }
    int n;
    VI a;
    int count = 0;
    int nthreads, tid, procs, maxt, inpar, dynamic, nested;
    structure temp;
    temp.clear();
    FOR_MAP(ii, C)
    {
        temp[ii->first] = ii->second;
    }
    int max = omp_get_thread_limit();
    omp_set_dynamic(max);
    omp_set_num_threads(t);

/* Dividing the transactions among the threads */
#pragma omp parallel firstprivate(temp) private(count, n, a)
    {
        if (!omp_in_parallel())
        {
            cout << "not parallel" << endl;
            exit(0);
            
        }
#pragma omp master
        {
            /* Get environment information */
            procs = omp_get_num_procs();
            nthreads = omp_get_num_threads();
            maxt = omp_get_max_threads();
            inpar = omp_in_parallel();
            dynamic = omp_get_dynamic();
            nested = omp_get_nested();

            /* Print environment information */
            printf("Number of processors = %d\n", procs);
            printf("Number of threads = %d\n", nthreads);
            printf("Max threads = %d\n", maxt);
            printf("In parallel? = %d\n", inpar);
            printf("Dynamic threads enabled? = %d\n", dynamic);
            printf("Nested parallelism supported? = %d\n", nested);
        }
        count = 0;
        char buf[1000];
        int x;

        int tid = omp_get_thread_num();
        int lev = omp_get_level();
        do
        {
            fgets(buf, 100, f[tid]);

            string s = buf;
            istringstream iss(s);
            a.clear();
            while (iss)
            {
                string sub;
                iss >> sub;
                stringstream convert(sub);
                convert >> x;
                if (x == -1 && a.size() > 0)
                {
                    set_count(a, &temp);
                    count++;
                    break;
                }
                a.push_back(x);
            };
        } while (count < p);
#pragma omp flush(a)
#pragma omp barrier
/* Each thread calculates its local count for all the candidates and at the en
d they are added to get final count */
#pragma omp critical
        {
            FOR_MAP(ii, temp)
            {
                C[ii->first] += ii->second;
            }
            temp.clear();
        }
    }

    for (i = 0; i < t; i++)
        fclose(f[i]);
}
/* Incrementing the count of the candidate if it exists in the transaction, it is
done for all candidates */
void set_count(VI a, structure *temp)
{
    FOR_MAP(ii, (*temp))
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
                for (int j = prev + 1; j < a.size(); ++j) // Number of iterations of inner loop can be decreased based on the previous iteration result
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
/* Finding the frequent item-sets of size k by removing all candidate itemsets having their count less than minimum support count */
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
