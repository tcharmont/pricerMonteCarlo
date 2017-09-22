#include <iostream>
#include <string>
#include "../src/parser.hpp"
#include "../src/BlackScholesModel.hpp"
#include "../src/MonteCarlo.hpp"
#include "../src/OptionAsian.hpp"
#include "../src/OptionBasket.hpp"
#include "../src/OptionPerformance.hpp"

#include "pnl/pnl_matrix.h"

using namespace std;

int main(int argc, char **argv)
{
    double T, t, r, strike, corr;
    PnlVect *spot, *sigma, *payoffCoeff;
    string type;
    int size, nbTimeSteps;
    size_t n_samples; /* ATTENTION : type long donné en entrée ! */

    char *infile = argv[1];
    //Param *P = new Parser(infile);

    //P->extract("option type", type);
    //P->extract("maturity", T);
    //P->extract("option size", size);
    //P->extract("spot", spot, size);
    //P->extract("volatility", sigma, size);
    //P->extract("interest rate", r);
    //P->extract("correlation", corr);
    //P->extract("payoff coefficients", payoffCoeff,size);

    //if (P->extract("dividend rate", divid, size) == false)
    //{
    //    divid = pnl_vect_create_from_zero(size);
    //}
    //P->extract("strike", strike);
    //P->extract("sample number", n_samples);
    //P->extract("timestep number", nbTimeSteps);

    //P->print();

    type = "basket";
    strike =100;
    T = 3.0;
    t = 3.0;
    size = 40;
    spot = pnl_vect_create_from_scalar(size, 100.0);
    sigma = pnl_vect_create_from_scalar(size, 0.2);
    r = 0.04879;
    corr = 0.0;
    payoffCoeff = pnl_vect_create_from_scalar(size, 0.025);
    n_samples = 1;
    nbTimeSteps = 100;

    PnlMat* history = pnl_mat_create_from_file(infile);
    //pnl_mat_print(history);


	/* Création de l'option en fonction du type */

	Option * opt;

    if(type.compare("asian") == 0)
        opt = new OptionAsian(T, nbTimeSteps, size, payoffCoeff, strike);
    if(type.compare("basket") == 0)
        opt = new OptionBasket(T, nbTimeSteps, size, payoffCoeff, strike);
    if(type.compare("performance") == 0)
        opt = new OptionPerformance(T, nbTimeSteps, size, payoffCoeff);


    BlackScholesModel* bsmod = new BlackScholesModel(size, r, corr, sigma, spot);

	/* Pas de temps */
    double fdStep = 0.0;
	/* Générateur aléatoire */
	PnlRng* rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	MonteCarlo monteCarlo(bsmod, opt, rng, fdStep, (int)n_samples); /* n_samples */

	double prix;
	double ic;
    std::cout << "_____ MonteCarlo Computation_____"<< std::endl;
    monteCarlo.price(history, t , prix, ic);
    std::cout << prix << " | " << ic << std::endl;

    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_vect_free(&payoffCoeff);
  //pnl_mat_free(&history);
  //delete P;

    exit(0);
}