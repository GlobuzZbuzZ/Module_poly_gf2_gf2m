#ifndef GF_FIELD_HPP
#define GF_FIELD_HPP

#include <stdexcept>
#include <cmath>

// Function to check if a number is a prime power (p^k)
bool is_prime_power(int n) {
    if (n < 1) return false;
    for (int p = 2; p <= std::sqrt(n); ++p) {
        int power = 0;
        int temp = n;
        while (temp % p == 0) {
            temp /= p;
            power++;
        }
        if (temp == 1 && power > 0) {
            return true;
        }
    }
    return false;
}

class GFField {
public:
    GFField(int dimension) : dimension(dimension) {
        if (!is_prime_power(dimension)) {
            throw std::invalid_argument("Dimension must be a prime power (p^k).");
        }
        // Further initialization if needed
    }

    int get_dimension() const {
        return dimension;
    }

private:
    int dimension;
};

#endif // GF_FIELD_HPP
