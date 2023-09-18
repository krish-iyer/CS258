// This code is inspired from the below source. The source had been modified to suite the HW needs.
// https://urldefense.com/v3/__https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution__;!!Nmw4Hv0!yTMSaSg32zBImzxIQwyUt74utn6j6NHx_yDVSKhrx0g8_WcKqT2iRv_Jep4fvaz-iLuE01ohbpxT3FrEHL7vO1q72BFaoPHswu1D8yzkrRFw0Osg$ 
// Accessed on Sep 16 2023

#include <iostream>
#include <random>

//Find the uniform's sequence
void gen_uniform_seq(int seq_len, int start, int end, int uni_rand_seq[])
{
    std::random_device rd;  //insert the seed
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(start, end);

    for (int i=0; i<seq_len; ++i) //generate the sequence
        uni_rand_seq[i] = distrib(gen); //sample from the distribution 
}

//Print the content of the array
void print_seq(int seq_len, int uni_rand_seq[]){
    std::cout << "\nThe sequence of the generated uniform random numbers are:\n";
    for (int i=0; i<seq_len; ++i)
        std::cout << uni_rand_seq[i] << ',';
    std::cout << "\n";
}

//Main function
int main()
{
    int seq_length = 10; //decide the size of the returned sequence
    const int distribution_start = 1; //decide the start value of the uniform distribution
    int distribution_end = 5; //decide the end value of the uniform distribution
    int uniform_rand_seq[seq_length]; //declare the random numbers array
    bool printEN=true; //if set to 'true', the uniform_rand_seq content will be printed.

    //start the program
    std::cout << "Finding a uniform random sequence of length " << seq_length << " with an interval of [" << distribution_start << "," << distribution_end << "]...\n";
    gen_uniform_seq(seq_length, distribution_start, distribution_end, uniform_rand_seq); //storing the random sequence in the array
    if (printEN) //print the array's content
        print_seq(seq_length, uniform_rand_seq);
    return 0;
}