#include <SFML/Graphics.hpp>
#include<cmath>
#include<iostream>
 
class GravitySource
{
    sf::Vector2f pos;
    long double strength;
    sf::CircleShape s;

public:
    GravitySource(long double pos_x, long double pos_y, long double strength) {
        pos.x = pos_x;
        pos.y = pos_y;
        this->strength = strength;
 
        s.setPosition(pos);
        s.setFillColor(sf::Color::White);
        s.setRadius(10);
    }
 
    void render(sf::RenderWindow& wind) {
        wind.draw(s);
    }
 
    sf::Vector2f get_pos() {
        return pos;
    }
 
    long double get_strength() {
        return strength;
    }
};
 
 
 
class Particle
{
    sf::Vector2<long double> pos;
    sf::Vector2<long double> vel;
    sf::Vector2<long double> acc;
    sf::CircleShape s;
    long double scale;
    const long double G{6.6743}; //upsized by 10^20
 
public:
    Particle(long double pos_x, long double pos_y, long double vel_x, long double vel_y) {
        pos.x = pos_x;
        pos.y = pos_y;
 
        vel.x = vel_x;
        vel.y = vel_y;
        scale = 175/pos_x;
    }
 
    void initialize_acceleration(GravitySource &s) {
        long double sunMass{s.get_strength()}; 
        sf::Vector2<long double> rel_pos{pos.x - s.get_pos().x, pos.y - s.get_pos().y};
        long double acceleration_x{G * sunMass/pow(magnitude(pos), 3)};
        acceleration_x *= pos.x;
            
        long double acceleration_y{G * sunMass/pow(magnitude(pos), 3)};
        acceleration_y *= pos.y;
        acc = {-acceleration_x, -acceleration_y};
        this->s.setPosition(scale_down(s));
        this->s.setFillColor(sf::Color::White);
        this->s.setRadius(8);
        
    }
    void render(sf::RenderWindow& wind, GravitySource &s) {
        this->s.setPosition(scale_down(s));
        wind.draw(this->s);
    }
 
    void set_color(sf::Color col) {
        s.setFillColor(col);
    }
    
    void update_physics(GravitySource& s, sf::Color color) {
        constexpr int time_step = 1000;
        for (int i = 0; i < 500; i++) {
            vel = sf::Vector2<long double>{vel.x + acc.x * time_step, vel.y + acc.y * time_step};
            pos = sf::Vector2<long double>{pos.x + vel.x * time_step, pos.y +  vel.y * time_step};
            sf::Vector2<long double> rel_pos{pos.x - s.get_pos().x, pos.y- s.get_pos().y};
            //Switch position and velocity to get regular euler
            
            // formula is G*sunMass/|distance between sun and comet|^3 * x or y (rcos(theta) or rsin(theta));
            long double acceleration_x =
            G * s.get_strength()/pow(magnitude(rel_pos), 3);
            acceleration_x *= rel_pos.x;
            
            long double acceleration_y =
            G * s.get_strength()/pow(magnitude(rel_pos), 3);
            acceleration_y *= rel_pos.y;

            acc = {-acceleration_x, -acceleration_y};
        }
        set_color(color);
    }
    private:

    long double magnitude(sf::Vector2<long double> p) {
        return sqrt(pow(p.x,2) + pow(p.y,2));
    }

    sf::Vector2f scale_down(GravitySource &s) {
        return sf::Vector2f{static_cast<float>((pos.x- s.get_pos().x) * scale + s.get_pos().x), static_cast<float>((pos.y - s.get_pos().y) * scale + s.get_pos().y)};
    }
 
};
sf::Color map_val_to_color(float value) {
    if (value < 0.0f) value = 0;
    if (value > 1.0f) value = 1;
 
    int r = 0, g = 0, b = 0;
 
    // 0 - 0.5 interp blue to green
    // 0.5 - 1 interp green to red
 
    if (value < 0.5f)
    {
        b = 255 * (1.0f - 2 * value);
        g = 255 * 2 * value;
    }
    else {
        g = 255 * (2.0f - 2 * value);
        r = 255 * (2 * value - 1);
    }
 
    return sf::Color(r, g, b);
}
 
 
 
float randomFloat() {
    return (float)(rand()) / (float)(rand());
}

int main() {
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(1600, 1000)), "My Program");
    window.setFramerateLimit(60);
 
    GravitySource source(800, 300, 1.989 * 1e10);
 
    Particle particle(800 + static_cast<long double>(226.6192)*pow(10,6), 300, 5.2893, 29.6647);
    particle.initialize_acceleration(source);
 
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
            }
            window.clear();
            particle.update_physics(source, map_val_to_color(randomFloat()));
            
            source.render(window);
            particle.render(window, source);
            //draw calls
            window.display();
            
        }

    }
 
 
 
    return 0;
}
