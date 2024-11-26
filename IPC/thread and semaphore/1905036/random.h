
//random number generators using poisson distribution
#define ld long double
int generate_random_number(ld lambda) {
    ld L = exp(-lambda);
    ld p = 1.0;
    int k = 0;

    do {
        k++;
        p *= ((ld) rand() / RAND_MAX);
    } while (p > L);

    int sample = (k-1)%20+1;
    return sample;

}