#include "BlackScholesModel.hpp"
#include <cmath>
#include <iostream>

using namespace std;

BlackScholesModel::BlackScholesModel(int size, double r, double rho, PnlVect *sigma, PnlVect *spot) {
    size_ = size;
    r_ = r;
    rho_ = rho;
    sigma_ = sigma;
    spot_ = spot;
}

void BlackScholesModel::asset(PnlMat *path, double T, int nbTimeSteps, PnlRng *rng) {
    /*Cholesky*/
    PnlMat *gamma = pnl_mat_create_from_scalar(size_, size_, rho_);
    PnlMat *identity = pnl_mat_create(size_, size_);
    pnl_mat_set_id(identity);
    pnl_mat_mult_scalar(identity, 1 - rho_);
    pnl_mat_plus_mat(gamma, identity);
    pnl_mat_chol(gamma);
    pnl_mat_set_row(path, spot_, 0);
    for (int i = 1; i <= nbTimeSteps; i++) {
        PnlVect *G_i = pnl_vect_create(size_);
        pnl_vect_rng_normal(G_i, size_, rng);
        for (int d = 0; d < size_; d++) {
            double sigma_d = pnl_vect_get(sigma_, d);
            PnlVect *L_d = pnl_vect_create(size_);
            pnl_mat_get_row(L_d, gamma, d);
            double path_t_d = pnl_mat_get(path, i - 1, d) * exp((r_ - pow(sigma_d, 2) / 2) * T / nbTimeSteps +
                                                                sigma_d * sqrt(T / nbTimeSteps) *
                                                                pnl_vect_scalar_prod(L_d, G_i));
            pnl_mat_set(path, i, d, path_t_d);
        }
    }
}

void BlackScholesModel::asset(PnlMat *path, double t, double T, int nbTimeSteps, PnlRng *rng, const PnlMat *past) 
{
    /// Decomposition de Cheloesky
    PnlMat *gamma = pnl_mat_create_from_scalar(size_, size_, rho_);
    PnlMat *identity = pnl_mat_create(size_, size_);
    pnl_mat_set_id(identity);
    pnl_mat_mult_scalar(identity, 1 - rho_);
    pnl_mat_plus_mat(gamma, identity);
    pnl_mat_chol(gamma);
    
    /// Récupérer les spots de past et remplir les premières lignes de path
    double nbDatesaRecuperer = round(t);
    PnlVect* tempRow = pnl_vect_create(size_);
    for (int i = 0; i <= nbDatesaRecuperer; i++) 
    {
        pnl_mat_get_row(tempRow, past, i);
        pnl_mat_set_row(path, tempRow, i);
    }

    /// Récupérer les valeurs S à la date t
    PnlVect* spots_t = pnl_vect_create(size_);
    pnl_mat_get_row(spots_t, past, ceil(t));

    /// Simuler le reste du chemin
   double previousDate = t;
   for (int i = nbDatesaRecuperer+1; i <= nbTimeSteps; i++) 
    {
        PnlVect *G_i = pnl_vect_create(size_);
        pnl_vect_rng_normal(G_i, size_, rng);
        for (int d = 0; d < size_; d++) 
	{
            double sigma_d = pnl_vect_get(sigma_, d);
            PnlVect *L_d = pnl_vect_create(size_);
            pnl_mat_get_row(L_d, gamma, d);
            double lastSpot = (i == nbDatesaRecuperer+1)? GET(spots_t, d) : pnl_mat_get(path, i-1, d);
            double timeInterval = (((double)i) * T/(double)nbTimeSteps) - previousDate;
            double path_t_d = lastSpot * exp((r_ - pow(sigma_d,2) / 2) * timeInterval +
                                                                sigma_d * sqrt(timeInterval) *
                                                                pnl_vect_scalar_prod(L_d, G_i));
            pnl_mat_set(path, i, d, path_t_d);
            previousDate = (double)i * T / (double)nbTimeSteps;
        }
    }
}

void BlackScholesModel::shiftAsset(PnlMat *shift_path, const PnlMat *path, int d, double h, double t, double timestep)
{

}
