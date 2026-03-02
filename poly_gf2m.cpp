// Updated display method for proper polynomial output formatting
#include <iostream>
#include <vector>

class Polynomial {
public:
    Polynomial(const std::vector<int>& coefficients) : coeffs(coefficients) {}
    
    void display() const {
        for (size_t i = 0; i < coeffs.size(); ++i) {
            if (coeffs[i] != 0) {
                if (i > 0 && coeffs[i] > 0) {
                    std::cout << '+';
                }
                std::cout << coeffs[i];
                if (i > 0) {
                    std::cout << "x^" << i;
                }
            }
        }
        std::cout << std::endl;
    }

private:
    std::vector<int> coeffs;
};

int main() {
    std::vector<int> coeffs = {1, -2, 0, 3}; // Represents 1 - 2x + 3x^3
    Polynomial poly(coeffs);
    poly.display(); // This should output: 1 - 2x + 3x^3
    
    return 0;
}