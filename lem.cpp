#include <iostream>
#include <array>
#include <string>
#include <fstream>

const int ligne = 10;
const int colonne = 20;
const int max_lemings = 10;
using lignes = std::array<char, colonne>;
using grille = std::array<lignes, ligne>;

//--------la grille lecture  --------
void lire_plateau(grille &g, std::string ch, int &depart_x, int &depart_y)
{
    std::ifstream fic;
    fic.open(ch);
    if (!fic)
    {
        std::cout << "probleme d'ouverture du fichier";
        return;
    }

    std::string ligne_txt;
    for (int i = 0; i < ligne; i++)
    {
        std::getline(fic, ligne_txt);
        for (int j = 0; j < colonne; j++)
        {
            g[i][j] = ligne_txt[j];

            if (ligne_txt[j] == 'S') // on cherche les cordonnÃ©es du point de dÃ©part :
            {
                depart_x = j;
                depart_y = i;
                g[i][j] = ' ';
            }
        }
    }
}
//-------------definition d'un lemming ------------
struct lemming
{
    int y, x;
    int direction; // gauche=-1 ou doite=1
    bool vivant;
    bool sauve;
    std::string action; // 5 action : aucun bloqueur creuseur batisseur parachuteur
    int hauteur_chute;  // pour detecter la mort
    int nb_brique;      // pour batisseur nb_max = 12
};
//------definition d'un ensemble de lemings ------------
using lemings = std::array<lemming, max_lemings>;
// ------------affichage -----------
// compteur : le nombre de lemings afficher dans la grille
void affiche_plateau(grille g, lemings ls, int compteur)
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
                std::cout << g[i][j];
        }
        std::cout << std::endl;
    }
}

//-----------deplacement de lemings --------------------
// detecter si le lemming est dans la sortie  :
bool est_sortie(grille g, lemming &l)
{
    if (g[l.y][l.x] == 'E')
    {
        l.vivant = false;
        l.sauve = true;
        return true;
    }
    return false;
}
// le lemming l est le sujet de l'action
// le tableau de lemming dans le procedure deplacement juste pour detecter les bloqueur :

void deplace(grille &g, lemming &l, lemings &ls, int compteur)
{
    // si le lemming est mort on sort :
    if (!l.vivant || l.y + 1 >= ligne) // mort ou depasse les limites de la grille
        return;
    // si le lemming dans le point de sortie : il est sauve et il mort
    if (est_sortie(g, l))
        return;
    //-----------le lemming tombe ---------------
    if (l.y + 1 < ligne && (g[l.y + 1][l.x] == ' ' || g[l.y + 1][l.x] == 'E'))
    {
        l.y++;
        l.hauteur_chute++;
        est_sortie(g, l);
        return;
    }
    //------------atterisssage : on vient de tomber et il y'a le sol ------------
    // si le lemming est tomber on verifie s'il depasser 3 case et n'est pas un parachuteur :
    if (l.hauteur_chute > 0)
    {
        if (l.hauteur_chute > 3 && l.action != "parachuteur")
        {
            l.vivant = false;
            return;
        }
        l.hauteur_chute = 0;
    }

    /*if(l.hauteur_chute>3 && l.action != "parachuteur")
      {
          l.vivant = false;
          return;
      }
    else
      {
          l.hauteur_chute =0;
      }*/

    // action de lemings : blokeur,parachuteur,batisseur,aucun,creuseur :
    // je commence par qu'elle action est est ce que le fait de commencer par une action va impacter les autres
    //----------1creusuer : il detruit les murs et tombe--------
    if (l.y + 1 < ligne && g[l.y + 1][l.x] == '#' && l.action == "creuseur")
    {
        g[l.y + 1][l.x] = ' ';
        l.y++;
        return;
    }
    //---------2-bloqueur : il bouge pas et bloque les autres a bouger horizentalement :
    if (l.action == "bloqueur")
        return;
    //------ 3-batisseur il construit des escalier vertical :construire+avancer+monte
    int prochain_x = l.x + l.direction;
    if (l.action == "batisseur" && l.nb_brique > 0)
    {
        if (prochain_x >= 0 && prochain_x < colonne && l.y - 1 >= 0 && g[l.y][prochain_x] == ' ' && g[l.y - 1][prochain_x] == ' ')
        {
            g[l.y][prochain_x] = '#';
            l.x = prochain_x;
            l.y--;
            l.nb_brique--;
            if (l.nb_brique == 0)
                l.action = "aucune";
            return;
        }
        else
        {

            l.action = "aucune";
            return;
        }
    }
    // ----------deplacement_horizentale ---------
    if (prochain_x < colonne && prochain_x >= 0)
    {
        bool bloqueur_devant = false;
        for (int k = 0; k < compteur && !bloqueur_devant; k++)
            if (ls[k].vivant && ls[k].action == "bloqueur" && ls[k].y == l.y && ls[k].x == prochain_x)
                bloqueur_devant = true;
        if (!bloqueur_devant && (g[l.y][prochain_x] == ' ' || g[l.y][prochain_x] == 'E'))
        {
            if (g[l.y][prochain_x] == 'E')
            {
                l.vivant = false;
                l.sauve = true;
                return;
            }
            l.x = prochain_x;
        }
        else if (g[l.y][prochain_x] == '#' && l.y - 1 >= 0 && g[l.y - 1][prochain_x] == ' ')
        {
            // marche d'une case : on monte et on avance
            l.x = prochain_x;
            l.y--;
        }

        else
            l.direction *= -1; // demi_tour
    }
    else
        l.direction *= -1; // demi_tour
}
//----------deplacement de tt les lemings------ :
void deplace_tt(grille &g, lemings &ls, int compteur)
{
    for (int k = 0; k < compteur; k++)
        deplace(g, ls[k], ls, compteur);
}

//------------nb_lemings_sauvÃ©_________________
int compter_sauves(lemings ls, int nb)
{
    int sauve = 0;
    for (int k = 0; k < nb; k++)
        if (ls[k].sauve)
            sauve++;
    return sauve;
}
// nb_lemings_vivant------
int compter_vivant(lemings ls, int nb)
{
    int vivants = 0;
    for (int k = 0; k < nb; k++)
        if (ls[k].vivant)
            vivants++;
    return vivants;
}
//------------victoir -------------
void victoir(int sauves, int nb)
{
    int min = nb / 2; // normalement le nb_sera dans le fichier
    if (sauves >= min)
        std::cout << "VICTOIR! " << sauves << "/" << nb << "lemings sauve!\n";
    else
        std::cout << "DEFAITE! " << sauves << "/" << nb << "lemings sauve!\n";
}
//-----------verifier si tt est mort -------------
bool tt_mort(lemings ls, int nb)
{
    for (int k = 0; k < nb; k++)
        if (ls[k].vivant)
            return false;
    return true;
}
//----------fonction principale------------------
int main()
{
    lemings ls;
    grille g;
    std::string ch = "./plateau.txt";
    int x_depart = -1;
    int y_depart = -1;
    int nb = 4;
    lire_plateau(g, ch, x_depart, y_depart);
    for (int k = 0; k < max_lemings; k++)
    {
        ls[k] = {y_depart, x_depart, 1, false, false, "aucune", 0, 12};
    }
    int compteur = 0;
    int tour = 0;
    bool fini = false;
    while (!fini)
    {
        // on créee un lemming tt les 3 tours :

        if (tour % 3 == 0 && compteur < nb)
        {
            ls[compteur].vivant = true;
            compteur++;
        }
        // affichage des sauves et vivant et tour
        std::cout << "Tour : " << tour << " |sauves: " << compter_sauves(ls, nb) << " | Vivants : " << compter_vivant(ls, nb) << "\n";
        affiche_plateau(g, ls, compteur);
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
        {
            /*int index;
            std::string action;
            std::cout << "index du lemming et action de la forme suivante (ex :0 bloqueur)?";
            std::cin >> index >> action;
            if (index >= 0 && index < compteur && ls[index].vivant)
                ls[index].action = action;*/
            int index;
            std::string action;
            std::cout << "Actions possible : bloqueur creuseur batisseur parachuteur  : exemple : index action : 0 bloqueur";
            std::cin >> index >> action;
            if (index >= 0 && index < compteur && ls[index].vivant)
            {
                ls[index].action = action;
                if (action == "batisseur")
                    ls[index].nb_brique = 12;
                // std::cout << "Lemming " << index << " action = " << ls[index].action << "\n";
            }
            else
                std::cout << "Index invalide ou lemming mort!\n";
        }

        deplace_tt(g, ls, compteur);
        if (tt_mort(ls, compteur) && nb == compteur)
        {
            victoir(compter_sauves(ls, nb), nb);
            fini = true;
        }

        tour++;
    }
    return 0;
}