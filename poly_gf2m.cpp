// poly_gf2m.cpp

#include <iostream>
#include <vector>

class PolynomialGF2m {
public:
    // Constructor
    PolynomialGF2m(const std::vector<int>& coefficients) : coefficients(coefficients) {}

    // Addition of two polynomials
    PolynomialGF2m operator+(const PolynomialGF2m& other) {
        std::vector<int> result(std::max(coefficients.size(), other.coefficients.size()), 0);
        for(size_t i = 0; i < result.size(); ++i) {
            int a = (i < coefficients.size()) ? coefficients[i] : 0;
            int b = (i < other.coefficients.size()) ? other.coefficients[i] : 0;
            result[i] = a ^ b; // XOR for GF(2)
        }
        return PolynomialGF2m(result);
    }

    // Multiplication of two polynomials
    PolynomialGF2m operator*(const PolynomialGF2m& other) {
        std::vector<int> result(coefficients.size() + other.coefficients.size() - 1, 0);
        for(size_t i = 0; i < coefficients.size(); ++i) {
            for(size_t j = 0; j < other.coefficients.size(); ++j) {
                result[i + j] ^= (coefficients[i] & other.coefficients[j]); // GF(2) multiplication
            }
        }
        return PolynomialGF2m(result);
    }

    // Display polynomial
    void display() const {
        for(int i = coefficients.size() - 1; i >= 0; --i) {
            if(coefficients[i] != 0) {
                std::cout << coefficients[i];
                if(i != 0) std::cout << "x^" << i;
                if(i > 0) std::cout << " + ";
            }
        }
        std::cout << std::endl;
    }

private:
    std::vector<int> coefficients;
};

// Example usage
int main() {
    PolynomialGF2m p1({1, 0, 1});  // x^2 + 1
    PolynomialGF2m p2({1, 1});      // x + 1
    PolynomialGF2m sum = p1 + p2;
    PolynomialGF2m product = p1 * p2;

    std::cout << "Sum: ";
    sum.display();
    std::cout << "Product: ";
    product.display();

    return 0;
}