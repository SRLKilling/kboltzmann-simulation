#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <GL/glu.h>
#include "scene.hpp"

#include <iostream>
#include <cstdio>
#include <ctime>
#include <cstdlib>


/***

	Unites :
	- Masse : e-27 kg
	- Longueur : nm
	- Temps : ns
	- Quantitée de mouvement : e-27 N.ns
	- Force : e-27 N
	- Pression : e-18 N/m
	
	Constantes :
	- K = kb * e23     (K = 1.3806488)
	
	
	- p * s = 1/2 * N * m * <v2> = Ec
	- Ec = N * Kb * T
	
	- n = N / Na
	- R = p * s / n * T
		= P * S * Na / N * T
		= P * S * na / N * T
	
	Formules :
	- 1/2 * M * v^2 = 3/2 * K * T
	- P * S = n * R * T
	
**/



int main(int argc, char **argv)
{
	srand(time(0));
	double speed = 10.0;

    sf::Window window(sf::VideoMode(512, 680), "GP");
	glMatrixMode(GL_PROJECTION);
	gluPerspective(70.0, (double)512/680, 0.01, 1000.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glLineWidth(4.0);
	glPointSize(2.0);
	
	
	tipe::Scene* mScene = new tipe::Scene();
	
		// Hydrogen
			mScene->setRadius(0.025e-9);
			mScene->setMass(1.660538*1.00794e-27);
			mScene->setDTexp(-17);
			speed = 50.0;
			
		// Xenon
			// mScene->setRadius(0.108);
			// mScene->setMass(1.660538*131.293);
			// mScene->setDTexp(-6);
			// speed = 30.0;
	
		mScene->generate(4000);
		mScene->setZone(10e-9, 40e-9);
		mScene->setGravity(1.0e15);
		
		if(argc > 1) mScene->load( std::string(argv[1]) );
		else mScene->fill(273);
	
	
	int mViewNum = 0;
	
	bool mDragging = false;
	bool mPlaying = false;
	bool mDisplay = true;
	mScene->setDT(speed);
	double zoom = 4e7;
	
	sf::Event event;
	int lastX = sf::Mouse::getPosition(window).x, lastY = sf::Mouse::getPosition(window).y;
	int width = 1024;
	int height = 680;
	double x = 0.0;
	double y = 0.0;
	
	sf::Clock c;
	
	while(window.isOpen())  {
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
				
			else if (event.type == sf::Event::Resized) {
				width = event.size.width;
				height = event.size.height;
				glViewport(0, 0, width, height);
			}
			
			else if (event.type == sf::Event::MouseButtonPressed) {
				if(event.mouseButton.button == sf::Mouse::Left) {
					mDragging = true;
					lastX = event.mouseButton.x;
					lastY = event.mouseButton.y;
				}
			}
			else if (event.type == sf::Event::MouseMoved) {
				if(mDragging) {
					x -= (lastX - event.mouseMove.x)/(1000*zoom);
					y += (lastY - event.mouseMove.y)/(1000*zoom);
					lastX = event.mouseMove.x;
					lastY = event.mouseMove.y;
				}
			}
			else if (event.type == sf::Event::MouseButtonReleased) {
				if(event.mouseButton.button == sf::Mouse::Left)  mDragging = false;
			}
			
			else if (event.type == sf::Event::MouseWheelMoved) {
				if(event.mouseWheel.delta > 0) zoom *= event.mouseWheel.delta * 1.1;
				else zoom /= -event.mouseWheel.delta * 1.1;
				// zoom =  zoom + (zoom - 1.0) event.mouseWheel.delta * 0.8;
			}
			
			else if(event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Space) mPlaying = !mPlaying;
				if (event.key.code == sf::Keyboard::Add) { speed += 2.0; mScene->setDT(speed); }
				if (event.key.code == sf::Keyboard::Subtract) { speed -= 2.0; mScene->setDT(speed); }
				if (event.key.code == sf::Keyboard::Left) mScene->computeStep();
				if (event.key.code == sf::Keyboard::Num1) { mViewNum = 0; }
				if (event.key.code == sf::Keyboard::Num2) { mViewNum = 1; }
				if (event.key.code == sf::Keyboard::Num3) { mViewNum = 3; }
				if (event.key.code == sf::Keyboard::Num4) { mViewNum = 4; }
				if (event.key.code == sf::Keyboard::P) { mScene->switchShowParticles(); }
				if (event.key.code == sf::Keyboard::D) { mDisplay = !mDisplay; }
				if (event.key.code == sf::Keyboard::R) { mScene->resetMax(); x = y = 0.0; double zoom = 4e8; }
				if (event.key.code == sf::Keyboard::S) { std::string file; std::cin >> file; mScene->save(file); }
				if (event.key.code == sf::Keyboard::L) { std::string file; std::cin >> file; mScene->load(file); }
				if (event.key.code == sf::Keyboard::M) { std::string file; std::cin >> file; mScene->saveMeasures(file); }
			}
		}
		
		if(mPlaying || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			mScene->computeStep();
		}
		
		if(mDisplay) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			// glOrtho(-1.0, 1.0, -1.0, 1.0, 0.0, 1.0);
			if(mViewNum == 0) glOrtho(-1.0, 1.0, -1.0, 1.0, 0.0, 1.0);
			else glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
			
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			if(mViewNum == 0) {
				glTranslatef(x, y, 0.0);
				glScalef(zoom, zoom, zoom);
				mScene->draw();
			} else {
				mScene->plot(mViewNum);
			}
			
			window.display();
		}
		
		if(c.getElapsedTime().asMilliseconds() >= 10000) {
			if(mPlaying) mScene->printInfo();
			c.restart();
		}
	}
	
    return 0;
}