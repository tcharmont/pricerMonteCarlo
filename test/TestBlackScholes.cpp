#include "../src/BlackScholesModel.hpp"
#include "../src/parser.hpp"
#include "../src/OptionBasket.hpp"
#include "../src/OptionAsian.hpp"
#include "../src/OptionPerformance.hpp"
#include "../src/MonteCarlo.hpp"
#include <string>
#include <iostream>

using namespace std;

int main() {

    double T, r, strike, rho;
    PnlVect *spot, *sigma, *divid;
    PnlMat *pnlMat;
    string type;
    int size, nbtt;
    size_t n_samples;

    Param *P = new Parser("../data/basket.dat");

    P->extract("maturity", T);
    P->extract("option size", size);
    P->extract("spot", spot, size);
    P->extract("volatility", sigma, size);
    P->extract("interest rate", r);
	P->extract("correlation", rho);
    if (P->extract("dividend rate", divid, size) == false) {
        divid = pnl_vect_create_from_zero(size);
    }
    P->extract("strike", strike);
    P->extract("sample number", n_samples);
    nbtt = 300;
    PnlRng *rng = pnl_rng_create(PNL_RNG_KNUTH);
    pnlMat = pnl_mat_create_from_zero(nbtt + 1, size);
    PnlVect *trend = pnl_vect_create_from_zero(size);
    BlackScholesModel blackScholesModel(size, r, rho, sigma, spot, trend);
    blackScholesModel.asset(pnlMat, T, nbtt, rng);

    /// Test de BlackScholesModel avec un historique
    pnlMat = pnl_mat_create_from_zero(nbtt + 1, size);
    int t = 2;
    PnlMat *past = pnl_mat_create_from_scalar(t, size, 4);
    blackScholesModel.asset(pnlMat, t, T, nbtt, rng, past);

    pnl_rng_free(&rng);
    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_vect_free(&divid);
    pnl_vect_free(&trend);
    delete P;

    exit(0);

}
