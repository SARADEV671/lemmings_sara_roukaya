#pragma once
#include "type.hpp"

bool est_sortie(grille g, lemming &l);
void deplace(grille &g, lemming &l, int ligne, int colonne, lemings &ls, int compteur);
void deplace_tt(grille &g, lemings &ls, int ligne, int colonne, int compteur);