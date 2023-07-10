# Apriori-algorithm-using-OpenMP
Parallel and Distributed Project

The **APRIORI ALGORITHM**, a well-known Association Rule Mining algorithm, aids in locating numerous frequent item groupings in the database. The user establishes the parameters for the search for these item sets in terms of the amount of support, which is measured by the proportion of transactions in which an itemset appears, as well as the confidence level, which is measured by the proportion of transactions involving an itemset in which another itemset also appears. The problem with this algorithm is that it involves a lot of iteration, and as a result, its efficiency drops off precipitously as the size or dimension of the dataset increases. Through the use of openMP threads, our project has become more time and resource efficient. We use data decomposition to divide the transaction database into multiple parts, and each of these parts is then taken by a separate thread. This allows each thread to determine the support count of all of the candidate itemsets for all of the transactions that are associated with that specific thread.

### Function Description:
-	void C1(string file_name) - This function generates the first candidate list.
-	void output(structure T) - It prints all the candidates or the frequency list along with their count.
-	void L1() - it generates the first frequency list.
-	void generate_C() - It generates all the candidates of size k from frequency list of size k-1.
-	bool check_compatibility(VI a,VI b) - This function checks if the two frequency itemsets are same or not.
-	void prune() - It removes all the candidates of size k whose subsets are not present in the frequent itemsets of size k-1.
-	void scan_D(string file_name) - It scans the database and calculates set_count, i.e, calculating the support count for each transaction
-	void set_count(VI a) - It increments the support count of the candidates if exists in the database transaction. it is done for all the candidates.
-	void generate_L() - It finds the frequent itemsets of size k by removing all the candidate item sets whose support count is less than the min support count.

### Architecture:

![apriori](https://github.com/keshav-06/Apriori-algorithm-using-OpenMP/assets/87660591/4f4d2683-ea09-415e-ab00-2a15042dfd75)

### Parallelising Serial Code -
Since the implementation of Apriori algorithm include 9 functions, it is very important to recognize the part of the code to be parallelized. The Apriori algorithm is itself an iterative algorithm and each iteration depends on the values obtained by the previous one, i.e. there exists a real loop dependency between the iterations. Since each iteration take significant amount of time, each iteration needs to be parallelized. From the previous studies, it has been found that about 90% of the time is consumed in scanning the transactions in the database to find out whether a particular candidate is present in the transaction or not. The function which scansthe database if scan_D() . It also calls set_count to calculate the support for each candidate which also contributes to the large amount of the time spent. 
I use data decomposition technique to divide our transaction data into the number of threads. Since the transactions are divided into the number of threads, now for every candidate the scanning and calculating the set_count is done parallelly by different threads and leading toward the speedup. After the set_count is calculated by different threads for each candidate, all the set_counts for each candidate are added and hence final set_count for every candidate is found. 
These counts for all the threads are added in the critical section. 
The critical section in the program is implemented using Barrier Synchronization. barrier synchronization has been used to add the calculated set_count values and find the final support for each candidate. A barrier for a group of threads in a section of code means that any thread /process must stop at this point and cannot proceed until all other threads/processes reach this barrier. So, in our code, until all the threads calculate the set_count for each candidate,the summation of set_counts cannot be done.

![flow](https://github.com/keshav-06/Apriori-algorithm-using-OpenMP/assets/87660591/abe55d89-4618-461d-9f43-4e5e9d162f2d)

The above picture shows the flow of executuion.

## Conclusion
Thus, the parallel implementation of the Apriori algorithm reduces the time taken for the algorithm to scan the database. This increases the efficiency of the algorithm and also makes it more usable for real time applications. 
