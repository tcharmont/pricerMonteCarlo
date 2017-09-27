#include "HedgingPortfolio.hpp"
#include <cmath>
#include <iostream>


HedgingPortfolio::HedgingPortfolio(int H, MonteCarlo *mCarlo) {
    this->H = H;
    monteCarlo = mCarlo;
}

HedgingPortfolio::~HedgingPortfolio() {
    delete monteCarlo;
}

double HedgingPortfolio::hedgingPAndL(PnlVect *result, PnlMat *path) {
    /// Initialisation
    PnlVect *delta_past = pnl_vect_create_from_zero(monteCarlo->mod_->size_);
    PnlVect *delta_current = pnl_vect_create_from_zero(monteCarlo->mod_->size_);
    PnlVect *delta_temp = pnl_vect_create_from_zero(monteCarlo->mod_->size_);

    PnlVect *path_i = pnl_vect_create_from_zero(monteCarlo->mod_->size_);
    double prix, ic;
    // PnlMat *past_0 = pnl_mat_create(1, monteCarlo->mod_->size_);
    PnlMat *past_i = pnl_mat_create(1, monteCarlo->mod_->size_);
    //pnl_mat_extract_subblock(past_0, path, 0, 1, 0, monteCarlo->mod_->size_);

    //pnl_mat_extract_subblock(past_i, path, 0, 1, 0, monteCarlo->mod_->size_);
    pnl_mat_get_row(path_i, path, 0);
    pnl_mat_set_row(past_i,path_i,0);

    std::cout << "*** start init ***" << std::endl;

    /// Calcul du prix initial de l'option
    monteCarlo->price(prix, ic);

    /// Calcul des deltas initiaux
    monteCarlo->delta(past_i, 0, delta_past);

    std::cout << "*** start calcul ***" << std::endl;

    /// Complétion de la matrice result
    pnl_vect_set(result, 0, prix - pnl_vect_scalar_prod(delta_past, path_i));
    for (int i = 1; i <= H; i++) {
        std::cout << " i = " << i << std::endl;
        //pnl_mat_extract_subblock(past_i, path, 0, i, 0, monteCarlo->mod_->size_);
	if ( i%(H/monteCarlo->opt_->nbTimeSteps()) == 0 ){
	  pnl_mat_add_row(past_i,past_i->m,path_i);
	  std::cout<<"je suis là"<<std::endl;
	}
	
        monteCarlo->delta(past_i, i * monteCarlo->opt_->maturity() / H, delta_current);
        pnl_vect_clone(delta_temp, delta_past);
        pnl_vect_clone(delta_past, delta_current);
        pnl_vect_minus_vect(delta_current, delta_temp);
        pnl_mat_get_row(path_i, path, i);
        LET(result, i) = GET(result, i - 1) * exp(monteCarlo->mod_->r_ * monteCarlo->opt_->maturity() / H) -
                         (pnl_vect_scalar_prod(delta_current, path_i));
    }

    std::cout << "*** end ***" << std::endl;

    double payoff = monteCarlo->opt_->payoff(path);
    //pnl_mat_print(path);
    std::cout << "payoff : " << payoff << std::endl;
    double pAngLResult = GET(result, H) + pnl_vect_scalar_prod(delta_past, path_i) - payoff;

    /// Free the memory
    pnl_vect_free(&delta_current);
    pnl_vect_free(&delta_past);
    pnl_vect_free(&delta_temp);
    pnl_vect_free(&path_i);

    return pAngLResult;
}
