#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <vector>
#include <random>
#define DETECT_NONE 0
#define DETECT_MID 1
#define DETECT_SIDE 2
#define DETECT_EDGE 3
#define PI 3.14159

class Bullet : public sf::CircleShape{
private:
    float linVeloMaxBull = 6.0;
    float linVeloX = 0.0;
    float linVeloY = 0.0;
    bool hasShot = false;
public:
    Bullet(const sf::Shape &playerShape) {
        setRadius(10.0);
        setOrigin(sf::Vector2f(getGlobalBounds().width/2,getGlobalBounds().height/2));
        setOutlineColor(sf::Color::White);
        setOutlineThickness(1.0);
        setPosition(playerShape.getPosition());
    }
    ~Bullet() {
    }
    void shootBullet(const sf::Shape &playerShape) {
        linVeloX = std::cos((playerShape.getRotation() + 90.0) * PI / 180.0) * -linVeloMaxBull;
        linVeloY = std::sin((playerShape.getRotation() + 90.0) * PI / 180.0) * -linVeloMaxBull;
        hasShot = true;
    }

    void moveBullet(){
        move(sf::Vector2f(linVeloX,linVeloY));
    }
};

void astroGame(sf::RenderWindow &renderWindow);
bool toDestroy(Bullet currentBull, const sf::Vector2f &windowRes);

int main(int argc, char ** argv) {
    const sf::Time MS_PER_FRAME = sf::milliseconds(16);
    int counterList = 0;
    const int counterListMax = 1;
    bool listHasChanged = false;
    bool selectChosen = false;

    sf::Time startTime;
    sf::RenderWindow renderWindow(sf::VideoMode(640,480), "SFML Astro Clone");

    sf::Event event;

    sf::Clock clock;

    sf::Font font;
    if(!font.loadFromFile("OpenSans-Regular.ttf")) {
        std::cout << "Error loading font." << std::endl;
    }

    sf::Text titleText;

    titleText.setFont(font);
    titleText.setString("Astro Clone");
    titleText.setCharacterSize(renderWindow.getSize().x/8);
    titleText.setFillColor(sf::Color::White);
    titleText.setOrigin(titleText.getGlobalBounds().width/2,titleText.getGlobalBounds().height/2);
    titleText.setPosition(renderWindow.getSize().x/2,renderWindow.getSize().y/4);

    sf::Text titleStartText;
    titleStartText.setFont(font);
    titleStartText.setString("Start");
    titleStartText.setCharacterSize(renderWindow.getSize().x/16);
    titleStartText.setFillColor(sf::Color::Red);
    titleStartText.setOrigin(titleStartText.getGlobalBounds().width/2,titleStartText.getGlobalBounds().height/2);
    titleStartText.setPosition(renderWindow.getSize().x/2,renderWindow.getSize().y/4 * 2);

    sf::Text titleQuitText;
    titleQuitText.setFont(font);
    titleQuitText.setString("Quit");
    titleQuitText.setCharacterSize(renderWindow.getSize().x/16);
    titleQuitText.setFillColor(sf::Color::White);
    titleQuitText.setOrigin(titleQuitText.getGlobalBounds().width/2,titleQuitText.getGlobalBounds().height/2);
    titleQuitText.setPosition(renderWindow.getSize().x/2,renderWindow.getSize().y/4 * 3);

    std::vector<sf::Text> listText = {titleStartText,titleQuitText};
    while(renderWindow.isOpen()) {
        //Events
        listHasChanged = false;
        selectChosen = false;
        startTime = clock.getElapsedTime();
        while(renderWindow.pollEvent(event)) {
            if(event.type == sf::Event::EventType::Closed) {
                renderWindow.close();
            }
            if(event.type == sf::Event::KeyPressed) {
                if(event.key.code == sf::Keyboard::W) {
                    if(counterList > 0) {
                        counterList--;
                    }
                    else {
                        counterList = listText.size() - 1;
                    }
                    listHasChanged = true;
                }
                else if(event.key.code == sf::Keyboard::S) {
                    if(counterList < listText.size() - 1) {
                        counterList++;
                    }
                    else {
                        counterList = 0;
                    }
                    listHasChanged = true;
                }
                if(event.key.code == sf::Keyboard::Return) {
                    selectChosen = true;
                }
            }
        }
        //Logic
        if (listHasChanged) {
            std::cout << counterList << std::endl;
            for(int i = 0; i < listText.size();i++) {
                listText.at(i).setFillColor(sf::Color::White);
            }
            listText.at(counterList).setFillColor(sf::Color::Red);
        }
        if (selectChosen) {
            if(counterList == 0) {
                astroGame(renderWindow);
            }
            else if(counterList == 1) {
                renderWindow.close();
            }
        }
        //Render
        renderWindow.clear();
        renderWindow.draw(titleText);
        renderWindow.draw(listText.at(0));
        renderWindow.draw(listText.at(1));
        renderWindow.display();
        sf::sleep(startTime + MS_PER_FRAME - clock.getElapsedTime());
    }
}
void astroGame(sf::RenderWindow &renderWindow) {

    bool isActive = true;
    const sf::Time MS_PER_FRAME = sf::milliseconds(16);

    const float borderSizeGap = 10.0;
    const float borderSpace = 0.01;

    sf::Vector2f windowDim = sf::Vector2f(renderWindow.getSize());

    //playerInfo
    float linVeloX = 0.0;
    float linVeloY = 0.0;
    const float linVeloAcceMax = 2.0;
    const float linVeloAcceLimit = 0.50;

    float rotVelo = 0.0;
    const float rotAcceMax = 3.0;

    //ammoInfo
    float linVeloKnockBackPercent = 0.5;
    std::vector<Bullet> bulletVect;

    sf::Time shootDelay = sf::milliseconds(500);
    sf::Time shootDeltaTime;

    //Astroids

    sf::Time startTime;

    sf::Event event;

    sf::Clock clock;

    //shapes
    sf::ConvexShape playerLSide;
    playerLSide.setPointCount(3);
    playerLSide.setPoint(0, sf::Vector2f(10,0));
    playerLSide.setPoint(1, sf::Vector2f(0,20));
    playerLSide.setPoint(2, sf::Vector2f(10, 10));
    playerLSide.setOrigin(sf::Vector2f(10,10));
    playerLSide.setPosition(20,20);

    sf::ConvexShape playerRSide;
    playerRSide.setPointCount(3);
    playerRSide.setPoint(0, sf::Vector2f(10,0));
    playerRSide.setPoint(1, sf::Vector2f(20,20));
    playerRSide.setPoint(2, sf::Vector2f(10,10));
    playerRSide.setOrigin(sf::Vector2f(10,10));
    playerRSide.setPosition(renderWindow.getSize().x/2,renderWindow.getSize().y/2);

    sf::CircleShape playerMainCircle;
    playerMainCircle.setRadius(5.0);
    playerMainCircle.setFillColor(sf::Color::Blue);
    playerMainCircle.setOrigin(sf::Vector2f(playerMainCircle.getGlobalBounds().width/2,playerMainCircle.getGlobalBounds().height/2));
    playerMainCircle.setPosition(sf::Vector2f(renderWindow.getSize().x/2,renderWindow.getSize().y/2));

    std::cout << "awoo" << std::endl;
    shootDeltaTime = clock.getElapsedTime();
    while(renderWindow.isOpen()) {
        startTime = clock.getElapsedTime();
        rotVelo = 0.0;
        while(renderWindow.pollEvent(event)) {
            if(event.type == sf::Event::EventType::Closed) {
                renderWindow.close();
            }
            if(event.type == sf::Event::EventType::KeyPressed && shootDelay <= (clock.getElapsedTime() - shootDeltaTime)) {
                if(event.key.code == sf::Keyboard::Space && bulletVect.size() < 10) {
                    Bullet bulletItem(playerMainCircle);
                    bulletVect.push_back(bulletItem);
                    bulletVect.at(bulletVect.size()-1).shootBullet(playerMainCircle);
                    shootDeltaTime = clock.getElapsedTime();
                }
            }
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            rotVelo += rotAcceMax;
        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            rotVelo -= rotAcceMax;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            linVeloX = std::cos((playerMainCircle.getRotation() + 90.0) * PI / 180.0) * -linVeloAcceMax;
            linVeloY = std::sin((playerMainCircle.getRotation() + 90.0) * PI / 180.0) * -linVeloAcceMax;
        }

        for (int i = bulletVect.size() - 1; i >= 0; i--){
            if(toDestroy(bulletVect.at(i),windowDim)){
                bulletVect.at(i) = bulletVect.back();
                bulletVect.pop_back();
                std::cout << "erased " << i << std::endl;
            }
        }

        if(playerMainCircle.getPosition().x < -borderSizeGap) {
            playerMainCircle.setPosition(sf::Vector2f(renderWindow.getSize().x + borderSizeGap - borderSpace, playerMainCircle.getPosition().y));
        }
        if(playerMainCircle.getPosition().x > renderWindow.getSize().x + borderSizeGap) {
            playerMainCircle.setPosition(sf::Vector2f(-borderSizeGap + borderSpace, playerMainCircle.getPosition().y));
        }

        if(playerMainCircle.getPosition().y < -borderSizeGap) {
            playerMainCircle.setPosition(sf::Vector2f(playerMainCircle.getPosition().x, renderWindow.getSize().y + borderSizeGap - borderSpace));
        }
        if(playerMainCircle.getPosition().y > renderWindow.getSize().y + borderSizeGap) {
            playerMainCircle.setPosition(sf::Vector2f(playerMainCircle.getPosition().x, -borderSizeGap + borderSpace));
        }

        playerMainCircle.rotate(rotVelo);
        playerMainCircle.move(sf::Vector2f(linVeloX,linVeloY));

        playerLSide.setPosition(playerMainCircle.getPosition());
        playerRSide.setPosition(playerMainCircle.getPosition());
        playerLSide.setRotation(playerMainCircle.getRotation());
        playerRSide.setRotation(playerMainCircle.getRotation());

        for(int i = 0; i < bulletVect.size();i++) {
            bulletVect.at(i).moveBullet();
        }

        renderWindow.clear();
        renderWindow.draw(playerMainCircle);
        renderWindow.draw(playerRSide);
        renderWindow.draw(playerLSide);

        for(int i = 0; i < bulletVect.size();i++) {
            renderWindow.draw(bulletVect.at(i));
        }
        renderWindow.display();
        sf::sleep(startTime + MS_PER_FRAME - clock.getElapsedTime());
    }
}

bool toDestroy(Bullet currentBull, const sf::Vector2f &windowRes) {
    if(currentBull.getPosition().x >= windowRes.x || currentBull.getPosition().x <= 0) {
        return true;
    }
    else if(currentBull.getPosition().y >= windowRes.y || currentBull.getPosition().y <= 0) {
        return true;
    }
    return false;
}
