#pragma once
#include <array>
#include <string>

const int max_ligne = 100;
const int max_colonne = 200;
const int max_lemmings = 100;
using lignes = std::array<char, max_colonne>;
using grille = std::array<lignes, max_ligne>;

struct lemming
{
    int y, x;
    int direction;
    bool vivant;
    bool sauve;
    std::string action;
    int hauteur_chute;
    int nb_brique;
};
struct competences
{
    int nb_normal;
    int nb_bloqueur;
    int nb_creuseur;
    int nb_parachute;
    int nb_batisseur;
    int nb_foreur;
    int nb_grimpeur;
    int nb_mineur;
    int nb_bombes;
};
using lemings = std::array<lemming, max_lemmings>;
