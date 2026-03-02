// poly_gf2m.cpp
// Реализация класса для работы с полиномами над конечными полями GF(2^m)

#include <iostream>
#include <vector>
#include <algorithm>

// Класс для представления полинома над GF(2) и GF(2^m)
class PolynomialGF2m {
private:
    std::vector<int> coefficients;  // Коэффициенты полинома

    // Вспомогательный приватный метод для нормализации коэффициентов
    // (удаление старших нулевых коэффициентов)
    void normalize() {
        // Удаляем старшие нулевые коэффициенты для нормализации
        while (!coefficients.empty() && coefficients.back() == 0) {
            coefficients.pop_back();
        }
        // Если вектор пуст, добавляем один ноль
        if (coefficients.empty()) {
            coefficients.push_back(0);
        }
    }

public:
    // Конструктор: инициализирует полином с заданными коэффициентами
    // coefficients: вектор коэффициентов [a0, a1, a2, ...] представляет a0 + a1*x + a2*x^2 + ...
    PolynomialGF2m(std::vector<int> coeffs) : coefficients(coeffs) {
        // Вызываем метод нормализации для удаления старших нулей
        normalize();
    }

    // Сложение двух полиномов в GF(2)
    // В GF(2) сложение - это операция XOR (исключающее или)
    PolynomialGF2m operator+(const PolynomialGF2m& other) const {
        std::vector<int> result(std::max(coefficients.size(), other.coefficients.size()), 0);
        
        // Складываем коэффициенты поэлементно с использованием XOR
        for(size_t i = 0; i < result.size(); ++i) {
            int a = (i < coefficients.size()) ? coefficients[i] : 0;
            int b = (i < other.coefficients.size()) ? other.coefficients[i] : 0;
            result[i] = a ^ b;  // XOR операция для GF(2)
        }
        return PolynomialGF2m(result);
    }

    // Умножение двух полиномов в GF(2)
    // Результат имеет степень deg(p1) + deg(p2)
    PolynomialGF2m operator*(const PolynomialGF2m& other) const {
        // Если один из полиномов нулевой, результат - ноль
        if (isZero() || other.isZero()) {
            return PolynomialGF2m(std::vector<int>{0});
        }
        
        // Размер результата: сумма степеней + 1
        std::vector<int> result(coefficients.size() + other.coefficients.size() - 1, 0);
        
        // Умножаем каждый коэффициент первого полинома на каждый коэффициент второго
        for(size_t i = 0; i < coefficients.size(); ++i) {
            for(size_t j = 0; j < other.coefficients.size(); ++j) {
                // В GF(2) умножение коэффициентов - это AND операция
                // Результат добавляем с XOR (так как в GF(2) нет "переноса")
                result[i + j] ^= (coefficients[i] & other.coefficients[j]);
            }
        }
        return PolynomialGF2m(result);
    }

    // Деление полинома на другой полином с остатком (деление "столбиком")
    // Возвращает пару (частное, остаток)
    // ВАЖНО: остаток = делимое mod делитель
    std::pair<PolynomialGF2m, PolynomialGF2m> divide(const PolynomialGF2m& divisor) const {
        if (divisor.isZero()) {
            throw std::invalid_argument("Делитель не может быть нулевым!");
        }
        
        std::vector<int> quotient;
        // Копируем делимое в переменную остатка
        PolynomialGF2m remainder = *this;
        
        // Выполняем деление "столбиком"
        // Пока степень остатка >= степени делителя
        while (!remainder.isZero() && remainder.degree() >= divisor.degree()) {
            // Вычисляем сдвиг (разность степеней)
            int shift = remainder.degree() - divisor.degree();
            
            // Добавляем коэффициент в частное
            quotient.push_back(1);  // В GF(2) коэффициент всегда 1 или 0
            
            // Создаём одночлен для сдвинутого делителя: x^shift
            std::vector<int> shiftVector(shift + 1, 0);
            shiftVector[shift] = 1;
            PolynomialGF2m xShift(shiftVector);
            
            // Вычитаем (сдвинутый делитель) из остатка
            // В GF(2) вычитание эквивалентно сложению (XOR)
            remainder = remainder + (divisor * xShift);
        }
        
        // Разворачиваем коэффициенты частного (они накапливались в обратном порядке)
        std::reverse(quotient.begin(), quotient.end());
        
        if (quotient.empty()) {
            quotient.push_back(0);
        }
        
        return std::make_pair(PolynomialGF2m(quotient), remainder);
    }

    // Вычисление наибольшего общего делителя (НОД) двух полиномов
    // Используется алгоритм Евклида: gcd(a, b) = gcd(b, a mod b)
    PolynomialGF2m gcd(const PolynomialGF2m& other) const {
        PolynomialGF2m a = *this;
        PolynomialGF2m b = other;
        
        // Алгоритм Евклида: применяем до тех пор, пока b не равен нулю
        while (!b.isZero()) {
            // Получаем остаток от деления a на b (это a mod b)
            PolynomialGF2m temp = a;
            a = b;
            // b = temp mod b
            b = temp.divide(b).second;  // .second содержит остаток
        }
        
        return a;
    }

    // Вычисление производной полинома
    // Производная используется в коррекционных кодах (например, коды Гоппа)
    // d/dx(a0 + a1*x + a2*x^2 + ...) = a1 + 2*a2*x + 3*a3*x^2 + ...
    // В GF(2) коэффициент при четной степени равен нулю
    PolynomialGF2m derivative() const {
        if (coefficients.size() <= 1) {
            return PolynomialGF2m(std::vector<int>{0});
        }
        
        std::vector<int> deriv;
        // Коэффициент при x^(i-1) в производной = i * coefficients[i]
        for (size_t i = 1; i < coefficients.size(); ++i) {
            // В GF(2) умножение на нечётное число дает то же число, на чётное - ноль
            if (i % 2 == 1) {
                deriv.push_back(coefficients[i]);
            } else {
                deriv.push_back(0);
            }
        }
        
        return PolynomialGF2m(deriv);
    }

    // Проверка неприводимости полинома
    // Полином неприводим, если его нельзя разложить на произведение полиномов меньшей степени
    bool isIrreducible() const {
        // Упрощённая проверка (полная реализация требует более сложной логики)
        if (degree() <= 1) {
            return false;  // Полиномы степени 0 и 1 не считаются неприводимыми
        }
        
        // В общем случае требуется проверка с использованием результантов или других методов
        return true;  // Заглушка для полной реализации
    }

    // Пошаговое деление "столбиком" с выводом каждого шага
    // Используется в учебном режиме для демонстрации алгоритма деления
    void stepByStepDivision(const PolynomialGF2m& divisor) const {
        std::cout << "\n========== ПОШАГОВОЕ ДЕЛЕНИЕ СТОЛБИКОМ ==========" << std::endl;
        std::cout << "Делимое:  ";
        display();
        std::cout << "Делитель: ";
        divisor.display();
        std::cout << "================================================\n" << std::endl;
        
        PolynomialGF2m remainder = *this;
        std::vector<int> quotient;
        int step = 1;
        
        // Выполняем деление пока степень остатка >= степени делителя
        while (!remainder.isZero() && remainder.degree() >= divisor.degree()) {
            std::cout << "--- Шаг " << step << " ---" << std::endl;
            std::cout << "Текущий остаток: ";
            remainder.display();
            
            // Вычисляем сдвиг
            int shift = remainder.degree() - divisor.degree();
            std::cout << "Степень остатка: " << remainder.degree() 
                      << ", Степень делителя: " << divisor.degree() << std::endl;
            std::cout << "Сдвиг: " << shift << " (вычитаем x^" << shift << " * делитель)" << std::endl;
            
            // Добавляем в частное
            quotient.push_back(1);
            
            // Создаём x^shift
            std::vector<int> shiftVector(shift + 1, 0);
            shiftVector[shift] = 1;
            PolynomialGF2m xShift(shiftVector);
            
            // Вычитаем делитель * x^shift из остатка
            PolynomialGF2m toSubtract = divisor * xShift;
            std::cout << "Вычитаем: ";
            toSubtract.display();
            
            remainder = remainder + toSubtract;  // XOR в GF(2)
            
            std::cout << "Новый остаток: ";
            remainder.display();
            std::cout << std::endl;
            
            step++;
        }
        
        // Разворачиваем частное
        std::reverse(quotient.begin(), quotient.end());
        
        std::cout << "================================================" << std::endl;
        std::cout << "РЕЗУЛЬТАТ:" << std::endl;
        std::cout << "Частное:  ";
        if (quotient.empty()) {
            std::cout << "0" << std::endl;
        } else {
            PolynomialGF2m q(quotient);
            q.display();
        }
        std::cout << "Остаток:  ";
        remainder.display();
        std::cout << "================================================\n" << std::endl;
    }

    // Вывод полинома в удобном математическом формате
    void display() const {
        if (isZero()) {
            std::cout << "0" << std::endl;
            return;
        }

        bool first = true;
        
        // Выводим коэффициенты в порядке от старшей степени к младшей
        for(int i = (int)coefficients.size() - 1; i >= 0; --i) {
            if(coefficients[i] != 0) {
                // Добавляем символ "+" перед членом (кроме первого)
                if (!first) {
                    std::cout << " + ";
                }
                first = false;
                
                // Выводим в зависимости от степени
                if(i == 0) {
                    // Свободный член - выводим только коэффициент
                    std::cout << coefficients[i];
                } else if(i == 1) {
                    // Для x^1 выводим просто x
                    std::cout << "x";
                } else {
                    // Для x^n выводим x^n
                    std::cout << "x^" << i;
                }
            }
        }
        std::cout << std::endl;
    }

    // Проверка, является ли полином нулевым
    bool isZero() const {
        if (coefficients.empty()) return true;
        for (int coeff : coefficients) {
            if (coeff != 0) return false;
        }
        return true;
    }

    // Получить степень полинома
    // Степень нулевого полинома = -∞ (здесь возвращаем -1)
    int degree() const {
        if (isZero()) return -1;
        return coefficients.size() - 1;
    }
};

// ============== ПРИМЕР ИСПОЛЬЗОВАНИЯ ==============
int main() {
    std::cout << "========== ДЕМОНСТРАЦИЯ МОДУЛЯ POLY_GF2M ==========" << std::endl;
    
    // Создаём тестовые полиномы
    // p1 = x^2 + 1 (коэффициенты: 1, 0, 1 для степеней 0, 1, 2)
    PolynomialGF2m p1({1, 0, 1});
    
    // p2 = x + 1 (коэффициенты: 1, 1 для степеней 0, 1)
    PolynomialGF2m p2({1, 1});
    
    std::cout << "\np1 = ";
    p1.display();
    
    std::cout << "p2 = ";
    p2.display();
    
    // Тест сложения полиномов
    std::cout << "\n--- ОПЕРАЦИЯ: СЛОЖЕНИЕ ---" << std::endl;
    PolynomialGF2m sum = p1 + p2;
    std::cout << "p1 + p2 = ";
    sum.display();
    
    // Тест умножения полиномов
    std::cout << "\n--- ОПЕРАЦИЯ: УМНОЖЕНИЕ ---" << std::endl;
    PolynomialGF2m product = p1 * p2;
    std::cout << "p1 * p2 = ";
    product.display();
    
    // Тест производной
    std::cout << "\n--- ОПЕРАЦИЯ: ПРОИЗВОДНАЯ ---" << std::endl;
    std::cout << "dp1/dx = ";
    p1.derivative().display();
    
    // Тест НОД
    std::cout << "\n--- ОПЕРАЦИЯ: НОД ---" << std::endl;
    std::cout << "НОД(p1, p2) = ";
    p1.gcd(p2).display();
    
    // Тест пошагового деления
    std::cout << "\n--- ОПЕРАЦИЯ: ПОШАГОВОЕ ДЕЛЕНИЕ ---" << std::endl;
    p1.stepByStepDivision(p2);

    return 0;
}
