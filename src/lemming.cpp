#include "lemming.hpp"
#include "grille.hpp"

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
