// poly_gf2m.hpp

#ifndef POLY_GF2M_HPP
#define POLY_GF2M_HPP

#include <vector>
#include <stdexcept>
#include <iostream>
#include <bitset>

template <typename T> class PolyGF2m {
public:
    // Constructor
    PolyGF2m(const std::vector<T>& coeffs) : coefficients(coeffs) {
        // Remove leading zeros
        while (!coefficients.empty() && coefficients.back() == 0) {
            coefficients.pop_back();
        }
    }

    // Polynomial addition
    PolyGF2m operator+(const PolyGF2m& other) const {
        size_t maxSize = std::max(coefficients.size(), other.coefficients.size());
        std::vector<T> result(maxSize);

        for (size_t i = 0; i < maxSize; ++i) {
            T a = (i < coefficients.size()) ? coefficients[i] : 0;
            T b = (i < other.coefficients.size()) ? other.coefficients[i] : 0;
            result[i] = a ^ b;  // GF(2) addition
        }
        return PolyGF2m(result);
    }

    // Polynomial multiplication
    PolyGF2m operator*(const PolyGF2m& other) const {
        std::vector<T> result(coefficients.size() + other.coefficients.size() - 1, 0);

        for (size_t i = 0; i < coefficients.size(); ++i) {
            for (size_t j = 0; j < other.coefficients.size(); ++j) {
                result[i + j] ^= (coefficients[i] & other.coefficients[j]);  // GF(2) multiplication
            }
        }
        return PolyGF2m(result);
    }

    // Polynomial division with remainder
    std::pair<PolyGF2m, PolyGF2m> operator/(const PolyGF2m& divisor) const {
        if (divisor.isZero()) throw std::invalid_argument("Divisor cannot be zero");
        std::vector<T> quotient;
        PolyGF2m remainder = *this;
        size_t divisorDegree = divisor.degree();

        while (remainder.degree() >= divisorDegree) {
            T leadingCoefficient = remainder.coefficients.back();
            size_t diffDegree = remainder.degree() - divisorDegree;
            std::vector<T> term(diffDegree + 1);
            term.back() = leadingCoefficient;
            quotient.push_back(leadingCoefficient);
            remainder = remainder + (divisor * PolyGF2m(term));  // Update remainder
        }
        return std::make_pair(PolyGF2m(quotient), remainder);
    }

    // Compute GCD
    PolyGF2m gcd(const PolyGF2m& other) const {
        PolyGF2m a = *this;
        PolyGF2m b = other;
        while (!b.isZero()) {
            auto remainder = a % b;
            a = b;
            b = remainder;
        }
        return a;
    }

    // Derivative of the polynomial
    PolyGF2m derivative() const {
        if (coefficients.empty()) return PolyGF2m(std::vector<T>{0});
        std::vector<T> deriv;
        for (size_t i = 1; i < coefficients.size(); ++i) {
            deriv.push_back(coefficients[i] * i);
        }
        return PolyGF2m(deriv);
    }

    // Check irreducibility
    bool isIrreducible() const {
        // Implement irreducibility check logic
        return true; // Placeholder, actual logic needed
    }

    // Step-by-step division mode (for debugging)
    void stepByStepDivision(const PolyGF2m& divisor) const {
        // Implement step-by-step division logic (for debugging)
    }

    // Utility functions
    bool isZero() const { return coefficients.empty(); }
    size_t degree() const { return coefficients.size() - 1; }

private:
    std::vector<T> coefficients;
};

#endif // POLY_GF2M_HPP