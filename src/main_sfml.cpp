#include <SFML/Graphics.hpp>
#include "type.hpp"
#include "grille.hpp"
#include "lemming.hpp"
#include "jeu.hpp"

int main()
{
    // variables de la grille
    grille g;
    int depart_x = 0, depart_y = 0;
    int ligne = 10, colonne = 20;
    int max_lem = 10, obj_sauvetage = 5, chrono = 700;
    competences comp = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    // chargement du niveau
    lire_plateau(g, "../niveau/simple.txt", depart_x, depart_y,
                 ligne, colonne, max_lem, obj_sauvetage, chrono, comp);

    // initialisation des lemmings
    lemings ls;
    for (int k = 0; k < max_lem; k++)
        ls[k] = {depart_y, depart_x, 1, false, false, "aucune", 0, 12};
    int compteur = 0;
    int tour = 0;
    sf::Clock clock;

    // création de la fenêtre
    const int TAILLE_CASE = 32;
    const int HAUTEUR_HUD = 60;
    sf::RenderWindow fenetre(sf::VideoMode(colonne * TAILLE_CASE, ligne * TAILLE_CASE + HAUTEUR_HUD), "Lemmings");
    // tuile lemming
    sf::Texture texture_lem_droite;
    texture_lem_droite.loadFromFile("../images/lemming_anim.png");
    sf::Texture texture_lem_gauche;
    texture_lem_gauche.loadFromFile("../images/rotated_lemming_anim.png");
    sf::Sprite spr_lemming(texture_lem_droite);
    // tuile mur
    sf::Texture texture_mur;
    texture_mur.loadFromFile("../images/map.png");
    sf::Sprite mur(texture_mur);
    mur.setTextureRect(sf::IntRect(50, 130, 41, 32));
    // tuile entree
    sf::Texture texture_entree;
    texture_entree.loadFromFile("../images/lemming_trapdoors.png");
    sf::Sprite entree(texture_entree);
    entree.setTextureRect(sf::IntRect(0, 64, 41, 32));
    // tuile sortie
    sf::Texture texture_sortie;
    texture_sortie.loadFromFile("../images/lemming_doors.png");
    sf::Sprite sortie(texture_sortie);
    sortie.setTextureRect(sf::IntRect(0, 64, 41, 32));
    // barre des competences
    sf::Texture texture_boutons;
    texture_boutons.loadFromFile("../images/buttons.png");
    sf::Sprite spr_bouton(texture_boutons);
    // curseur
    sf::Texture texture_cursor;
    texture_cursor.loadFromFile("../images/cursor.png");
    sf::Sprite spr_cursor(texture_cursor);
    spr_cursor.setTextureRect(sf::IntRect(0, 87, 12, 12));
    spr_cursor.setScale(2.f, 2.f);
    // fenetre.setMouseCursorVisible(false);
    //  la boucle principale
    while (fenetre.isOpen())
    {
        sf::Event evenement;
        // écoute les événements
        while (fenetre.pollEvent(evenement))
            // détecte la fermeture
            if (evenement.type == sf::Event::Closed)
                fenetre.close();

        // --- logique : avancer d'un tour toutes les 0.3 secondes ---
        if (clock.getElapsedTime().asSeconds() > 0.3f)
        {
            if (tour % 3 == 0 && compteur < max_lem)
            {
                ls[compteur].vivant = true;
                compteur++;
            }
            deplace_tt(g, ls, ligne, colonne, compteur);
            tour++;
            clock.restart();
        }
        // --- affichage ---
        fenetre.clear();

        // dessiner la grille
        for (int i = 0; i < ligne; i++)
        {
            for (int j = 0; j < colonne; j++)
            {
                if (g[i][j] == 'a') // mur destructible
                {
                    mur.setPosition(j * TAILLE_CASE, i * TAILLE_CASE);
                    fenetre.draw(mur);
                }
                else if (g[i][j] == '2') // sortie
                {
                    sortie.setPosition(j * TAILLE_CASE, i * TAILLE_CASE);
                    fenetre.draw(sortie);
                }
                else if (g[i][j] == '1') // entrée
                {
                    entree.setPosition(j * TAILLE_CASE, i * TAILLE_CASE);
                    fenetre.draw(entree);
                }
            }
        }
        // dessiner les lemmings
        for (int k = 0; k < compteur; k++)
        {
            if (ls[k].vivant)
            {
                if (ls[k].direction == 1)
                    spr_lemming.setTexture(texture_lem_droite);
                else
                    spr_lemming.setTexture(texture_lem_gauche);

                if (ls[k].action == "aucune")
                    spr_lemming.setTextureRect(sf::IntRect(0, 0, 16, 16));
                else if (ls[k].action == "creuseur")
                    spr_lemming.setTextureRect(sf::IntRect(0, 16, 16, 16));
                else if (ls[k].action == "parachuteur")
                    spr_lemming.setTextureRect(sf::IntRect(0, 32, 16, 16));
                else if (ls[k].action == "bloqueur")
                    spr_lemming.setTextureRect(sf::IntRect(0, 48, 16, 16));
                else if (ls[k].action == "foreur")
                    spr_lemming.setTextureRect(sf::IntRect(0, 64, 16, 16));
                else if (ls[k].action == "batisseur")
                    spr_lemming.setTextureRect(sf::IntRect(0, 80, 16, 16));
                else
                    spr_lemming.setTextureRect(sf::IntRect(0, 0, 16, 16));

                spr_lemming.setScale(2.f, 2.f);
                spr_lemming.setPosition(ls[k].x * TAILLE_CASE, ls[k].y * TAILLE_CASE);
                fenetre.draw(spr_lemming);
            }
        }

        // dessiner la barre des competences
        int y_hud = ligne * TAILLE_CASE + 10;
        std::string actions[] = {"bloqueur", "creuseur", "batisseur", "parachuteur", "foreur"};
        int coords_x[] = {80, 0, 16, 64, 48};
        int coords_y[] = {0, 32, 32, 32, 32};
        for (int i = 0; i < 5; i++)
        {
            spr_bouton.setTextureRect(sf::IntRect(coords_x[i], coords_y[i], 15, 23));
            spr_bouton.setScale(2.f, 2.f);
            spr_bouton.setPosition(10 + i * 40, y_hud);
            fenetre.draw(spr_bouton);
        }

        // curseur
        sf::Vector2i pos_souris = sf::Mouse::getPosition(fenetre);
        spr_cursor.setPosition(pos_souris.x, pos_souris.y);
        fenetre.draw(spr_cursor);

        fenetre.display();
    }

    return 0;
}

/*Une spritesheet, c'est quoi ?Imagine une feuille de papier avec plein de petits dessins collés dessus, organisés en grille. C'est exactement ça une spritesheet.*/
// sf::Texture — charge l'image en mémoire
// texture.loadFromFile("map.png");
/*Un sprite c'est simplement un objet qu'on peut dessiner à l'écran.
Il a 3 informations :
quoi dessiner → la texture (setTexture)
quel morceau → le découpage (setTextureRect)
où le dessiner → la position dans la fenêtre (setPosition)*/
// sf::Sprite — c'est ce qu'on dessine
// Le sprite est "connecté" à la texture.