#include "../src/OptionBasket.hpp"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv) {

    double maturity = 4;
    int nbTimeSteps = 1;
    int size = 1;
    double strike = 10;
    PnlVect *payoffCoeff = pnl_vect_create(size);

    /// Test sur OptionBasket
    OptionBasket optionBasket(maturity, nbTimeSteps, size, payoffCoeff, strike);
    cout << (optionBasket.strike() == 10) << endl;

    return 0;
}