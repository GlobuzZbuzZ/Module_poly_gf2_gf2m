// gf_field.hpp
// Finite Field Base Class with Prime Validation

#ifndef GF_FIELD_HPP
#define GF_FIELD_HPP

#include <stdexcept>
#include <cmath>

class GF_Field {
public:
    GF_Field(int prime): prime_(prime) {
        if (prime_ <= 1 || !isPrime(prime_)) {
            throw std::invalid_argument("The field must be defined by a prime number.");
        }
    }

    int getPrime() const { return prime_; }

private:
    int prime_;

    bool isPrime(int num) {
        if (num <= 1) return false;
        for (int i = 2; i <= std::sqrt(num); i++) {
            if (num % i == 0) return false;
        }
        return true;
    }
};

#endif // GF_FIELD_HPP