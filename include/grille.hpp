#pragma once
#include "type.hpp"

char affiche_case(char c);
bool est_sol(char c);
bool est_destructible(char c);
bool est_liquide(char c);
bool est_traversable(char c);

void lire_plateau(grille &g, std::string ch, int &depart_x, int &depart_y,
                  int &ligne, int &colonne, int &max_lem,
                  int &obj_sauvtage, int &chrono, competences &comp);

void affiche_plateau(grille g, lemings ls, int ligne, int colonne, int compteur);