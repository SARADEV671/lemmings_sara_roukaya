#include "jeu.hpp"
#include <iostream>
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
            std::cout << "Plus de batisseurs !" << std::endl;
        else if (action == "parachuteur" && comp.nb_parachute == 0)
            std::cout << "Plus de parachuteurs !" << std::endl;
        else if (action == "foreur" && comp.nb_foreur == 0)
            std::cout << "Plus de foreur !" << std::endl;
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