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
    return ((c >= 'a' && c <= 'i') || ((c == '/' || c == '\\')));
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
// determiner les escalier
bool est_escalier(char c)
{
    return (c == '/' || c == '\\');
}

//--------le plateau :  lecture  --------
//--------le plateau :  lecture  --------
// ------------affichage -----------
// compteur : le nombre de lemings afficher dans la grille
//--------le plateau : lecture securisee --------
// Renvoie 'true' si le fichier est valide, 'false' s'il y a une erreur
bool lire_plateau(grille &g, std::string ch, int &depart_x, int &depart_y, int &ligne, int &colonne, int &max_lem, int &obj_sauvtage, int &chrono, competences &comp)
{
    std::ifstream fic(ch);
    if (!fic)
    {
        std::cout << "ERREUR  : probleme d'ouverture du fichier -> " << ch << std::endl;
        return false;
    }

    // 1. Lecture avec verification de type (plante si on met des lettres au lieu de chiffres)
    if (!(fic >> ligne >> colonne >> max_lem >> obj_sauvtage >> chrono))
    {
        std::cout << "ERREUR : format de la 1ere ligne incorrect (nombres entiers attendus)." << std::endl;
        return false;
    }
    if (ligne > max_ligne || colonne > max_colonne)
    {
        std::cout << "ERREUR : grille trop grande." << std::endl;
        return false;
    }

    if (!(fic >> comp.nb_normal >> comp.nb_bloqueur >> comp.nb_creuseur >> comp.nb_parachute >> comp.nb_batisseur >> comp.nb_foreur >> comp.nb_grimpeur >> comp.nb_mineur >> comp.nb_bombes))
    {
        std::cout << "ERREUR : format des competences incorrect sur la 2eme ligne." << std::endl;
        return false;
    }

    // 2. Verifications logiques de base
    if (ligne <= 0 || colonne <= 0)
    {
        std::cout << "ERREUR : dimensions de la grille invalides (" << ligne << "x" << colonne << ")." << std::endl;
        return false;
    }

    if (max_lem <= 0)
    {
        std::cout << "ERREUR : le nombre total de lemmings doit etre positif." << std::endl;
        return false;
    }
    if (obj_sauvtage < 0 || obj_sauvtage > max_lem)
    {
        std::cout << "ERREUR : objectif de sauvetage incoherent (" << obj_sauvtage << " sur " << max_lem << " lemmings)." << std::endl;
        return false;
    }
    if (chrono <= 0)
    {
        std::cout << "ERREUR : le chrono doit etre superieur a 0." << std::endl;
        return false;
    }

    // Vider le tampon après la lecture des entiers
    std::string temp;
    std::getline(fic, temp);

    // Initialiser le départ à une valeur invalide (-1) pour verifier a la fin si on a bien trouve le '1'
    depart_x = -1;
    depart_y = -1;

    // 3. Lecture de la grille ligne par ligne
    std::string ligne_grille;
    for (int i = 0; i < ligne; i++)
    {
        // On verifie si le fichier s'arrete brutalement avant la fin prevue
        if (!std::getline(fic, ligne_grille))
        {
            std::cout << "ERREUR : le fichier s'arrete a la ligne " << i << " alors que " << ligne << " lignes etaient prevues !" << std::endl;
            return false;
        }

        // On verifie si la ligne dessinée est plus petite que 'colonne' (pour eviter un "Out of bounds")
        // Note: il faut convertir ligne_grille.length() en int pour comparer avec colonne.
        int longueur = ligne_grille.length();
        if (longueur < colonne)
        {
            std::cout << "ERREUR : la ligne " << i + 1 << " du dessin est trop courte (" << ligne_grille.length() << " caracteres au lieu de " << colonne << ")." << std::endl;
            return false;
        }

        for (int j = 0; j < colonne; j++)
        {
            g[i][j] = ligne_grille[j];

            if (ligne_grille[j] == '1') // on cherche le point de depart
            {
                depart_x = j;
                depart_y = i;
            }
        }
    }

    // 4. Verification de la presence de l'entree
    if (depart_x == -1 || depart_y == -1)
    {
        std::cout << "ERREUR : Aucun point d'entree ('1') n'a ete trouve sur la grille !" << std::endl;
        return false;
    }

    return true; 
}

void affiche_plateau(const grille &g, const lemings &ls, int ligne, int colonne, int compteur)
{
    for (int i = 0; i < ligne; i++)
    {
        for (int j = 0; j < colonne; j++)
        {

            bool est_lemming = false;
            for (int k = 0; k < compteur; k++)
            {
                if (ls[k].vivant && ls[k].x == j && ls[k].y == i)
                    est_lemming = true;
            }

            if (est_lemming)
                std::cout << 'L';
            else
                std::cout << affiche_case(g[i][j]);
        }
        std::cout << std::endl;
    }
}
