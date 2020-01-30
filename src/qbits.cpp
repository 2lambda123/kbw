#include "../include/qbits.hpp"
#include <random>

Qbits::Qbits(size_t seed) {
    std::srand(seed);
    qbits[Index()] = 1;
}

void Qbits::x(size_t idx) {
    map qbits_tmp{};
    for (auto &i : qbits) {
        auto j = i.first;
        j.flip(idx);
        qbits_tmp[j] = i.second; 
    }
    qbits.swap(qbits_tmp);
}

void Qbits::y(size_t idx) {
    map qbits_tmp{};
    for (auto &i : qbits) {
        auto j = i.first;
        j.flip(idx);
        if (i.first.is_one(idx)) {
            qbits_tmp[j] = i.second*-1i;
        } else {
            qbits_tmp[j] = i.second*1i;
        }
    }
    qbits.swap(qbits_tmp);
}

void Qbits::z(size_t idx) {
    for (auto &i : qbits) {
        if (i.first.is_one(idx)) {
            qbits[i.first] *= -1;
        }
    }
}

void Qbits::h(size_t idx) {
    map qbits_tmp{};
    for (auto &i : qbits) {
        if (i.first.is_one(idx)) {
            qbits_tmp[i.first] -= i.second/std::sqrt(2);
        } else {
            qbits_tmp[i.first] += i.second/std::sqrt(2);
        }
        if (std::abs(qbits_tmp[i.first]) < 1e-10) {
            qbits_tmp.erase(i.first);
        }
        auto j = i.first;
        j.flip(idx);
        qbits_tmp[j] += i.second/std::sqrt(2);
        if (std::abs(qbits_tmp[j]) < 1e-10) {
            qbits_tmp.erase(j);
        }
    }
    qbits.swap(qbits_tmp);
}

void Qbits::s(size_t idx) {
    for (auto &i : qbits) {
        if (i.first.is_one(idx)) {
            qbits[i.first] *= 1i;
        }
    }
}

void Qbits::sd(size_t idx) {
    for (auto &i : qbits) {
        if (i.first.is_one(idx)) {
            qbits[i.first] *= -1i;
        }
    }
}

void Qbits::t(size_t idx) {
    for (auto &i : qbits) {
        if (i.first.is_one(idx)) {
            qbits[i.first] *= std::exp(1i*M_PI);
        }
    }
}

void Qbits::td(size_t idx) {
    for (auto &i : qbits) {
        if (i.first.is_one(idx)) {
            qbits[i.first] *= std::exp(-1i*M_PI);
        }
    }
}

void Qbits::cnot(size_t ctrl, size_t target) {
    map qbits_tmp{};

    for (auto &i : qbits) {
        if (i.first.is_one(ctrl)) {
            auto j = i.first;
            j.flip(target);
            qbits_tmp[j] = i.second; 
        }
    }

    qbits.swap(qbits_tmp);
}

int Qbits::measure(size_t idx) {
    double p = 0;

    for (auto &i : qbits) {
        if (i.first.is_zero(idx)) {
            p += std::pow(std::abs(i.second), 2);
        }
    }
    
    auto result = p != 0 and 
                  (double(std::rand()) / double(RAND_MAX) <= p)?
                  0 : 1;
    
    p = result == 0? std::sqrt(p) : std::sqrt(1-p);

    map qbits_tmp{};

    for (auto &i : qbits) {
        if (i.first.is_zero(idx) xor result) {
            qbits_tmp[i.first] = i.second/p;
        }
    }

    qbits.swap(qbits_tmp);
    return result;
}

std::ostream& operator<<(std::ostream &os, Qbits q) {
    for (auto &i : q.qbits) {
        os << i.first << ' ' << i.second << std::endl;
    }
    return os;
}