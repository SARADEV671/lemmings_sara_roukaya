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
// determiner les escalier
bool est_escalier(char c)
{
    return (c == '/' || c == '\\');
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

//-------------definition d'un lemming ------------
struct lemming
{
    int y, x;
    int direction; // gauche=-1 ou doite=1
    bool vivant;
    bool sauve;
    std::string action; // 6 action : aucun bloqueur creuseur batisseur parachuteur foreur
    int hauteur_chute;  // pour detecter la mort
    int nb_brique;      // pour batisseur nb_max = 12
};
//------definition d'un ensemble de lemings ------------
using lemings = std::array<lemming, max_lemmings>;
// ------------affichage -----------
// compteur : le nombre de lemings afficher dans la grille
void affiche_plateau(const grille &g, lemings ls, int ligne, int colonne, int compteur)
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
bool est_sortie(const grille &g, lemming &l)
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
    if (!l.vivant)
        return;
    if (l.y + 1 >= ligne)
    {
        l.vivant = false;
        return;
    }
    // si le lemming dans le point de sortie : il est sauve et il mort
    if (est_sortie(g, l))
        return;
    //-----------le lemming tombe ---------------
    char case_dessous = g[l.y + 1][l.x];
    if (est_traversable(case_dessous) || est_liquide(case_dessous))
    {
        if (l.action == "creuseur")
            l.action = "aucune";
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
        if (l.action == "parachuteur")
            l.action = "aucune";
    }
    // est est ce que le fait de commencer par une action va impacter les autres?????
    //----------1creusuer : il detruit les cases destructubles (a-i) et tombe--------
    if (l.action == "creuseur")
    {
        char dessous = g[l.y + 1][l.x];

        if (l.y + 1 < ligne && est_destructible(dessous))
        {
            g[l.y + 1][l.x] = ' ';
            l.y++;
            est_sortie(g, l);
        }
        else if (dessous == ' ')
        {
            // vide → il tombe et arrête de creuser
            l.action = "aucune";
            l.y++;
        }
        else
        {
            // bloc indestructible
            l.action = "aucune";
        }

        return;
    }
    //---------2-bloqueur : il bouge pas et bloque les autres a bouger horizentalement :
    if (l.action == "bloqueur")
        return;
    //------ 3-batisseur il construit des escalier vertical :construire+avancer+monte
    int prochain_x = l.x + l.direction;
    if (l.action == "batisseur" && l.nb_brique > 0)
    {
        if (prochain_x >= 0 && prochain_x < colonne && l.y - 1 >= 0 && g[l.y][prochain_x] == ' ' && est_traversable(g[l.y - 1][prochain_x]))
        {
            if (l.direction == 1)
                g[l.y][prochain_x] = '/';
            else
                g[l.y][prochain_x] = '\\';

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
    if (l.action == "foreur" && prochain_x >= 0 && prochain_x < colonne)
    {
        char devant = g[l.y][prochain_x];
        if (est_liquide(devant))
        {
            l.vivant = false;
            return;
        }
        if (est_destructible(devant))
        {
            g[l.y][prochain_x] = ' ';

            l.x = prochain_x;
            est_sortie(g, l);
            return;
        }
        if (est_traversable(devant))
        {
            l.action = "aucune";
            l.x = prochain_x;
            est_sortie(g, l);
            return;
        }

        l.action = "aucune";
        return;
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
            if (est_sortie(g, l))
                return;
        }
        // grimper un escalier
        else if (!bloqueur_devant && est_escalier(case_devant) && l.y - 1 >= 0 && est_traversable(g[l.y - 1][prochain_x])) // vérifier que la case au-dessus de la destination est libre
        {
            // un escalier il monte une case :
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
int compter_sauves(const lemings &ls, int max_lem)
{
    int sauve = 0;
    for (int k = 0; k < max_lem; k++)
        if (ls[k].sauve)
            sauve++;
    return sauve;
}
// nb_lemings_vivant------
int compter_vivant(const lemings &ls, int max_lem)
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
bool tt_mort(const lemings &ls, int max_lem)
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
            std::cout << "Plus de batisseurs !" << std::endl;
        else if (action == "parachuteur" && comp.nb_parachute == 0)
            std::cout << "Plus de parachuteurs !" << std::endl;
        else if (action == "foreur" && comp.nb_foreur == 0)
            std::cout << "Plus de foreur !" << std::endl;
        else if (action != "bloqueur" && action != "creuseur" && action != "batisseur" && action != "parachuteur" && action != "foreur")
            std::cout << "Action inconnue : " << action << " !" << std::endl;
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
    std::string ch = "./niveau/simple.lem";
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