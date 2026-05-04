#include <SFML/Graphics.hpp>
#include <array>
#include <string>
#include <fstream>
const int ligne = 10;
const int colonne = 20;
const int TAILLE_CASE = 32; // chaque case fait 32x32 pixels
using lignes = std::array<char, colonne>;
using grille = std::array<lignes, ligne>;

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
const int max_lemings = 10;
using lemings = std::array<lemming, max_lemings>;

void lire_plateau(grille &g, std::string ch, int &depart_x, int &depart_y)
{
    std::ifstream fic;
    fic.open(ch);
    std::string ligne_txt;
    for (int i = 0; i < ligne; i++)
    {
        std::getline(fic, ligne_txt);
        for (int j = 0; j < colonne; j++)
        {
            g[i][j] = ligne_txt[j];
            if (ligne_txt[j] == 'S')
            {
                depart_x = j;
                depart_y = i;
                g[i][j] = ' ';
            }
        }
    }
}

bool est_sortie(grille &g, lemming &l)
{
    if (g[l.y][l.x] == 'E')
    {
        l.vivant = false;
        l.sauve = true;
        return true;
    }
    return false;
}

int main()
{
    // --- chargement grille ---
    grille g;
    int depart_x = 0, depart_y = 0;
    lire_plateau(g, "./plateau.txt", depart_x, depart_y);
    lemings ls;
    ls[0] = {depart_y, depart_x, 1, true, false, "aucune", 0, 12};
    // --- chargement spritesheet ---
    sf::Texture texture_lemming;
    texture_lemming.loadFromFile("./images/lemming_anim.png");
    sf::Sprite lemming(texture_lemming);

    // --- frame de marche : ligne 0, colonne 0 de la spritesheet ---
    // chaque frame fait 16x16 pixels dans la spritesheet
    lemming.setTextureRect(sf::IntRect(0, 0, 16, 16));
    lemming.setScale(2.f, 2.f); // on agrandit x2 pour mieux voir

    // position du lemming = position de départ
    lemming.setPosition(depart_x * TAILLE_CASE, depart_y * TAILLE_CASE);

    // --- tuile mur ---
    sf::Texture texture_mur;
    texture_mur.loadFromFile("./images/map.png");
    sf::Sprite mur(texture_mur);
    mur.setTextureRect(sf::IntRect(50, 130, 32, 32));

    // --- tuile sortie ---
    sf::Texture texture_sortie;
    texture_sortie.loadFromFile("./images/lemming_doors.png");
    /*sf::Sprite sortie(texture_sortie);
    sortie.setTextureRect(sf::IntRect(0, 0, 41, 32));
    sortie.setScale(float(TAILLE_CASE) / 40, float(TAILLE_CASE) / 30);*/
    sf::Sprite sortie(texture_sortie);
    sortie.setTextureRect(sf::IntRect(0, 64, 41, 32));
    sortie.setScale(float(TAILLE_CASE) / 41, float(TAILLE_CASE) / 32);

    // --- tuile entree ---
    sf::Texture texture_entree;
    texture_entree.loadFromFile("./images/lemming_trapdoors.png");
    sf::Sprite entree(texture_entree);
    // entree.setTextureRect(sf::IntRect(0, 120, 39, 30)); // première trappe
    entree.setTextureRect(sf::IntRect(0, 64, 41, 25));
    entree.setScale(2.f, 2.f);

    // --- fenetre ---
    sf::RenderWindow window(sf::VideoMode(colonne * TAILLE_CASE, ligne * TAILLE_CASE), "Lemmings");
    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                window.close();
        // --- mise a jour logique ---
        if (clock.getElapsedTime().asSeconds() > 0.3f)
        {
            deplace(g, ls[0], ls, 1); // ta fonction existante
            clock.restart();
        }

        // --- mise a jour position sprite ---
        lemming_sprite.setPosition(ls[0].x * TAILLE_CASE, ls[0].y * TAILLE_CASE);

        window.clear(sf::Color(139, 90, 43));

        // --- afficher la grille ---
        for (int i = 0; i < ligne; i++)
        {
            for (int j = 0; j < colonne; j++)
            {
                if (g[i][j] == '#')
                {
                    mur.setPosition(j * TAILLE_CASE, i * TAILLE_CASE);

                    window.draw(mur);
                }
                else if (g[i][j] == 'E')
                {
                    sortie.setPosition(j * TAILLE_CASE, i * TAILLE_CASE);
                    window.draw(sortie);
                }
            }
        }

        // --- afficher le lemming ---
        window.draw(entree);
        window.draw(lemming);

        window.display();
    }
    return 0;
}