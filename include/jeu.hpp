#pragma once
#include "type.hpp"

int  compter_sauves(lemings ls, int max_lem);
int  compter_vivant(lemings ls, int max_lem);
bool tt_mort(lemings ls, int max_lem);
void victoir(int sauves, int max_lem, int obj_sauvtage);
void assigner_competence(lemings &ls, int compteur, competences &comp);