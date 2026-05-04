#include <iostream>
#include <array>
#include <string>
#include <fstream>

const int max_ligne = 100;
const int max_colonne = 200;
const int max_lemmings = 100;
using lignes = std::array<char, max_colonne>;
using grille = std::array<lignes, max_ligne>;

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
//------------competences -------------
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
using lemings = std::array<lemming, max_lemmings>;
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

//-----------deplacement de lemings --------------------
// detecter si le lemming est dans la sortie  :
bool est_sortie(grille g, lemming &l)
{
    if (g[l.y][l.x] == '2')
    {
        l.vivant = false;
        l.sauve = true;
        return true;
    }
    return false;
}
// le lemming l est le sujet de l'action
// le tableau de lemming dans le procedure deplacement juste pour detecter les bloqueur :
// action de lemings : blokeur,parachuteur,batisseur,aucun,creuseur :
void deplace(grille &g, lemming &l, int ligne, int colonne, lemings &ls, int compteur)
{
    // si le lemming est mort on sort :
    if (!l.vivant || l.y + 1 >= ligne) // mort ou depasse les limites de la grille
        return;
    // si le lemming dans le point de sortie : il est sauve et il mort
    if (est_sortie(g, l))
        return;
    //-----------le lemming tombe ---------------
    char case_dessous = g[l.y + 1][l.x];
    if (est_traversable(case_dessous) || est_liquide(case_dessous))
    {
        l.y++;
        l.hauteur_chute++;
        if (est_liquide(g[l.y][l.x]))
        {
            l.vivant = false;
            return;
        }
        est_sortie(g, l);
        return;
    }
    //------------atterisssage : on vient de tomber et il y'a le sol ------------
    // si le lemming est tomber on verifie s'il depasser 3 case et n'est pas un parachuteur :
    if (l.hauteur_chute > 0)
    {
        if (l.hauteur_chute > 6 && l.action != "parachuteur")
        {
            l.vivant = false;
            return;
        }
        l.hauteur_chute = 0;
    }
    // est est ce que le fait de commencer par une action va impacter les autres?????
    //----------1creusuer : il detruit les cases destructubles (a-i) et tombe--------
    if (l.action == "creuseur" && l.y + 1 < ligne - 1 && est_destructible(g[l.y + 1][l.x]))
    {
        g[l.y + 1][l.x] = ' ';
        l.y++;
        est_sortie(g, l);
        return;
    }
    //---------2-bloqueur : il bouge pas et bloque les autres a bouger horizentalement :
    if (l.action == "bloqueur")
        return;
    //------ 3-batisseur il construit des escalier vertical :construire+avancer+monte
    int prochain_x = l.x + l.direction;
    if (l.action == "batisseur" && l.nb_brique > 0)
    {
        if (prochain_x >= 0 && prochain_x < colonne && l.y - 1 >= 0 && est_traversable(g[l.y][prochain_x]) && est_traversable(g[l.y - 1][prochain_x]))
        {
            g[l.y][prochain_x] = 'a';
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
    // 4 foreur : creuse horizontalement (a-i uniquement)
    if (l.action == "foreur")
    {
        if (prochain_x >= 0 && prochain_x < colonne && est_destructible(g[l.y][prochain_x]))
        {
            g[l.y][prochain_x] = ' ';
            l.x = prochain_x;
            est_sortie(g, l);
            return;
        }
        else
            l.action = "aucune"; // béton ou bord : redevient normal
    }
    // ----------deplacement_horizentale ---------
    if (prochain_x >= 0 && prochain_x < colonne)
    {
        bool bloqueur_devant = false;
        for (int k = 0; k < compteur && !bloqueur_devant; k++)
            if (ls[k].vivant && ls[k].action == "bloqueur" && ls[k].y == l.y && ls[k].x == prochain_x)
                bloqueur_devant = true;
        char case_devant = g[l.y][prochain_x];
        // case vide ou sortie ;
        if (!bloqueur_devant && est_traversable(case_devant))
        {

            l.x = prochain_x;
            est_sortie(g, l);
        }
        else if (!bloqueur_devant && est_sol(case_devant) && l.y - 1 >= 0 && est_traversable(g[l.y - 1][prochain_x]))// vérifier que la case au-dessus de la destination est libre
        {
            // marche d'une case : on monte et on avance
            l.x = prochain_x;
            l.y--;
            if (est_sortie(g, l))
                return;
        }

        else
            l.direction *= -1; // demi_tour
    }
    else
        l.direction *= -1; // demi_tour
}
//----------deplacement de tt les lemings------ :
void deplace_tt(grille &g, lemings &ls, int ligne, int colonne, int compteur)
{
    for (int k = 0; k < compteur; k++)
        deplace(g, ls[k], ligne, colonne, ls, compteur);
}

//------------nb_lemings_sauvee_________________
int compter_sauves(lemings ls, int max_lem)
{
    int sauve = 0;
    for (int k = 0; k < max_lem; k++)
        if (ls[k].sauve)
            sauve++;
    return sauve;
}
// nb_lemings_vivant------
int compter_vivant(lemings ls, int max_lem)
{
    int vivants = 0;
    for (int k = 0; k < max_lem; k++)
        if (ls[k].vivant)
            vivants++;
    return vivants;
}
//------------victoir -------------
void victoir(int sauves, int max_lem, int obj_sauvtage)
{
    // obj_sauvetage  normalement le nb_sera telecharger du fichier texte !
    if (sauves >= obj_sauvtage)
        std::cout
            << "VICTOIR! " << sauves << "/" << max_lem << "lemings sauve!" << std::endl;
    else
        std::cout << "DEFAITE! " << sauves << "/" << max_lem << "lemings sauve!" << std::endl;
}
//-----------verifier si tt est mort -------------
bool tt_mort(lemings ls, int max_lem)
{
    for (int k = 0; k < max_lem; k++)
        if (ls[k].vivant)
            return false;
    return true;
}
void assigner_competence(lemings &ls, int compteur, competences &comp)
{
    int index;
    std::string action;
    bool valide = false;
    do
    {
        std::cout << "Actions dispo :  bloqueur(" << comp.nb_bloqueur << ")"
                  << " creuseur(" << comp.nb_creuseur << ")"
                  << " batisseur(" << comp.nb_batisseur << ")"
                  << " foreur(" << comp.nb_foreur << ")"
                  << " parachuteur(" << comp.nb_parachute << ")  format exemple : 0 bloquer (index action)" << std::endl;
        std::cin >> index >> action;
        if (index < 0 || index >= compteur || !ls[index].vivant)
        {
            std::cout << "Index invalide ou lemming mort !" << std::endl;
        }
        else if (action == "bloqueur" && comp.nb_bloqueur == 0)
            std::cout << "Plus de bloqueurs " << std::endl;
        else if (action == "creuseur" && comp.nb_creuseur == 0)
            std::cout << "Plus de creuseurs !" << std::endl;
        else if (action == "batisseur" && comp.nb_batisseur == 0)
            std::cout << "Plus de batisseurs !"<<std::endl;
        else if (action == "parachuteur" && comp.nb_parachute == 0)
            std::cout << "Plus de parachuteurs !"<<std::endl;
        else if (action == "foreur" && comp.nb_foreur == 0)
            std::cout << "Plus de foreur !"<<std::endl;
        else
            valide = true;

    } while (!valide);
    // assigner et décrémenter
    ls[index].action = action;
    if (action == "bloqueur")
        comp.nb_bloqueur--;
    if (action == "creuseur")
        comp.nb_creuseur--;
    if (action == "batisseur")
    {
        comp.nb_batisseur--;
        ls[index].nb_brique = 12; // on la remet a 12 pour donner a un batisseur un stock de brique frais
    }
    if (action == "parachuteur")
        comp.nb_parachute--;
    if (action == "foreur")
        comp.nb_foreur--;
}
//----------fonction principale------------------
int main()
{
    lemings ls;
    grille g;
    std::string ch = "./plateau.txt";
    int x_depart = -1;
    int y_depart = -1;
    // int nb = 4;
    int max_lem = 10;
    int obj_sauvtage = max_lem / 2;
    int chrono = 700;
    int ligne = 10;
    int colonne = 20;
    competences comp = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    lire_plateau(g, ch, x_depart, y_depart, ligne, colonne, max_lem, obj_sauvtage, chrono, comp);
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
        // affichage des sauves et vivant et tour
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
        bool plus_de_lemmings = (compteur >= max_lem);
        if (tt_mort(ls, compteur) && plus_de_lemmings)
        {
            victoir(compter_sauves(ls, compteur), max_lem, obj_sauvtage);
            fini = true;
        }

        tour++;
    }
    return 0;
}