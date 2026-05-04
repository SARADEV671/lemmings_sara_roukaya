#include "grille.hpp"
#include <iostream>
#include <fstream>

//---------Convertit un caractère de la grille en caractère d'affichage textuel ----------
char affiche_case(char c)
{
    if (c >= 'a' && c <= 'i')
        return '#';
    if (c >= 'j' && c <= 'q')
        return '@';
    if (c >= 'r' && c <= 'z')
        return '~';
    return c;
}
//--------------- ------------------
// Vrai si la case est un sol solide (destructible ou non)
bool est_sol(char c)
{
    return (c >= 'a' && c <= 'q');
}
// Vrai si la case peut être détruite (creuseur, batisseur)
bool est_destructible(char c)
{
    return (c >= 'a' && c <= 'i');
}
// Vrai si la case est un liquide mortel (eau, acide, lave...)
bool est_liquide(char c)
{
    return (c >= 'r' && c <= 'z');
}
// Vrai si le lemming peut entrer dans cette case (air, entrée, sortie
bool est_traversable(char c)
{
    return (c == ' ' || c == '2' || c == '1');
}

//--------le plateau :  lecture  --------
void lire_plateau(grille &g, std::string ch, int &depart_x, int &depart_y, int &ligne, int &colonne, int &max_lem, int &obj_sauvtage, int &chrono, competences &comp)
{
    std::ifstream fic;
    fic.open(ch);
    if (!fic)
    {
        std::cout << "probleme d'ouverture du fichier";
        return;
    }
    fic >>
        ligne >> colonne >> max_lem >> obj_sauvtage >> chrono;
    fic >> comp.nb_normal >> comp.nb_bloqueur >> comp.nb_creuseur >> comp.nb_parachute >> comp.nb_batisseur >> comp.nb_foreur >> comp.nb_grimpeur >> comp.nb_mineur >> comp.nb_bombes;
    std::string temp;
    std::getline(fic, temp);
    std::string ligne_grille;
    for (int i = 0; i < ligne; i++)
    {
        std::getline(fic, ligne_grille);
        for (int j = 0; j < colonne; j++)
        {
            g[i][j] = ligne_grille[j];

            if (ligne_grille[j] == '1') // on cherche les cordonnees du point de depart :
            {
                depart_x = j;
                depart_y = i;
            }
        }
    }
}

// ------------affichage -----------
// compteur : le nombre de lemings afficher dans la grille
void affiche_plateau(grille g, lemings ls, int ligne, int colonne, int compteur)
{
    for (int i = 0; i < ligne; i++)
    {
        for (int j = 0; j < colonne; j++)
        {
            bool est_lemming = false;
            for (int k = 0; k < compteur; k++)
                if (ls[k].vivant && ls[k].y == i && ls[k].x == j)
                    est_lemming = true;
            if (est_lemming)
                std::cout << 'L';
            else
                std::cout << affiche_case(g[i][j]);
        }
        std::cout << std::endl;
    }
}
