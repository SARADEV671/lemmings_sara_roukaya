#include <iostream>
#include "type.hpp"
#include "grille.hpp"
#include "lemming.hpp"
#include "jeu.hpp"

//----------fonction principale------------------
int main()
{
    lemings ls;
    grille g;
    std::string ch = "../niveau/simple.txt";
    int x_depart = -1;
    int y_depart = -1;
    // int nb = 4;
    int max_lem = 10;
    int obj_sauvtage = max_lem / 2;
    int chrono = 700;
    int ligne = 10;
    int colonne = 20;
    competences comp = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    if (!lire_plateau(g, ch, x_depart, y_depart, ligne, colonne, max_lem, obj_sauvtage, chrono, comp))
        return 1;
    for (int k = 0; k < max_lemmings; k++)
    {
        ls[k] = {y_depart, x_depart, 1, false, false, "aucune", 0, 12};
    }
    int compteur = 0;
    int tour = 0;
    bool fini = false;
    while (!fini)
    {
        // on créee un lemming tt les 3 tours :
        if (tour % 3 == 0 && compteur < max_lem)
        {
            ls[compteur].vivant = true;
            compteur++;
        }

        std::cout
            << "Tour : " << tour << " |sauves: " << compter_sauves(ls, max_lem) << " | Vivants : " << compter_vivant(ls, max_lem) << " | Objectif : " << obj_sauvtage << "/" << max_lem << std::endl;
        affiche_plateau(g, ls, ligne, colonne, compteur);
        //-------jouer ------
        char choix_joueur;
        std::cout << "taper (n = avancer , q=quitter, competence (c)";
        std::cin >> choix_joueur;
        while (choix_joueur != 'n' && choix_joueur != 'q' && choix_joueur != 'c')
        {
            std::cout << "choix invalide ressaye : ";
            std::cin >> choix_joueur;
        }
        if (choix_joueur == 'q')
            fini = true;
        else if (choix_joueur == 'c')
            assigner_competence(ls, compteur, comp);

        deplace_tt(g, ls, ligne, colonne, compteur);
        // affichage des sauves et vivant et tour
        bool plus_de_lemmings = (compteur >= max_lem);
        if (plus_de_lemmings && tt_mort(ls, max_lem))
        {
            victoir(compter_sauves(ls, max_lem), max_lem, obj_sauvtage);
            fini = true;
        }

        tour++;
    }
    return 0;
}