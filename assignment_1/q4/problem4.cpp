// This code is inspired from the below source. The source had been modified to suite the HW needs.
// https://urldefense.com/v3/__https://en.cppreference.com/w/cpp/numeric/random/poisson_distribution__;!!Nmw4Hv0!yTMSaSg32zBImzxIQwyUt74utn6j6NHx_yDVSKhrx0g8_WcKqT2iRv_Jep4fvaz-iLuE01ohbpxT3FrEHL7vO1q72BFaoPHswu1D8yzkrXB50OyX$ 
// Accessed on Sep 16 2023

#include <iostream>
#include <random>

//Find the poisson's sequence
void gen_posson_seq(int seq_len, int dist_mean, int poiss_rand_seq[])
{
    std::random_device rd; //insert the seed
    std::mt19937 gen(rd());
 
    std::poisson_distribution<> d(dist_mean); 

    for (int i=0; i<seq_len; ++i) //generate the sequence
        poiss_rand_seq[i] = d(gen); //sample from the distribution
}

//Print the content of the array
void print_seq(int seq_len, int poiss_rand_seq[]){
    std::cout << "\nThe sequence of the generated poisson random numbers are:\n";
    for (int i=0; i<seq_len; ++i)
        std::cout << poiss_rand_seq[i] << ',';
}

//Main function
int main()
{
    int seq_length = 3; //decide the size of the returned sequence
    int distribution_mean = 5; //decide the mean value of the poisson distribution
    int poiss_rand_seq[seq_length]; //declare the random numbers array
    bool printEN=true; //if set to 'true', the poiss_rand_seq content will be printed.

    //start the program
    std::cout << "Finding a poisson random sequence of length " << seq_length << " with a mean of " << distribution_mean << "...\n";
    gen_posson_seq(seq_length, distribution_mean, poiss_rand_seq); //storing the random sequence in the array
    if (printEN) //print the array's content
        print_seq(seq_length, poiss_rand_seq);
    return 0;
}