#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Function to generate random numbers between 0 and 1
double rand_0_to_1() {
    return (double) rand() / RAND_MAX;
}

// Function to generate random numbers following the Poisson distribution
int poisson_random(double lambda) {
    double L = exp(-lambda);
    double p = 1.0;
    int k = 0;

    do {
        k++;
        p *= rand_0_to_1();
    } while (p > L);

    return k - 1;
}

int main() {
    const int num_samples = 10; // Change this value to the desired number of random samples
    const double mean = 5.0;    // Change this value to the desired mean of the Poisson distribution

    // Seed the random number generator using the system time
    srand(time(NULL));

    printf("Random numbers following a Poisson distribution with mean %.2f:\n", mean);

    for (int i = 0; i < num_samples; i++) {
        int sample = poisson_random(mean);
        printf("%d ", sample);
    }

    printf("\n");

    return 0;
}
