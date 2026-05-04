#include <SFML/Graphics.hpp>

int main()
{
    // je cree un fenetre de taille 800*600 et de titre lemming
    sf::RenderWindow fenetre(sf::VideoMode(800, 600), "Lemmings")
    while(window.isOpen()) // debut de la boucle du jeux

    sf::Event evenement; // je cree un evenement 
    while(window.pollEvent(evenement))  //regarde s'il ya un evenement et le mettre dans evenement
        if(event.type == sf::Event.closed)// si le type d'evenement est une fermuteure de fenetere 
            window.close(); // je ferme le programme 

    window.clear(sf::Color::Black);// apres chaque tour en redessine le fentre en noir 

}
