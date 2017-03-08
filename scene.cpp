#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>
#include "scene.hpp"
#include "vec3.hpp"

#include <cstdio>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <random>

#define _KB 1.3806488e-23

namespace tipe {

	void resolveCollision(ThreadParam p);

	Scene::Scene() {
		mTime = 0.0;
		mDT = 0.001;
		mSteps = 0;
		mPression = 1.0;
		mPressionPart = 1.0;
		mPressionNb = 1;
	
		mHeight = 1.0;
		mWidth = 1.0;
		mParticleMass = 1.0;
		mParticleRadius = 0.01;
		mShowParticles = true;
		mGravity = 0.0;
		mTemperature = 0.0;
		
		mDistLibre = 0.0;
		mDistLibreNb = 0;
		
		mKb = 0.0;
		mKbSum = 0.0;
		mKbNb = 0;
		mR = 0.0;
		mRSum = 0.0;
		mRNb = 0;
		
		mMaxKb = 0.0;
		mMinKb = 0.0;
		mMaxR = 0.0;
		mMinR = 0.0;
		
		for(int i = 0; i < _NB_ZONE_X; ++i) { for(int j = 0; j < _NB_ZONE_Y; ++j) {
			mTZones[i][j] = 0.0;
			mTZonesNb[i][j] = 1;
		}}
		mTZonedMax = 0.0000001;
		
		for(int i = 0; i < _NB_ZONE_Y_COUNT; ++i) {
			mNbZones[i] = 0;
		}
		mNbZonedMax = 1;
		
		for(int i = 0; i < _NB_QUADTREE_X+2; ++i) {
			for(int j = 0; j < _NB_QUADTREE_Y+2; ++j) {
				mQuadtreeNb[i][j] = 0;
				
				ThreadParam p; p.s = this; p.i = i; p.j = j;
				mThreads[i][j] = new sf::Thread(resolveCollision, p);
				mThreads[i][j]->launch();
			}
		}
	}
	
	void Scene::clear() {
		delete[] mParticles;
		mParticleNumber = 0;
		
		mTime = 0.0;
		mDT = 0.001;
		mSteps = 0;
		mTemperature = 0.0;
		
		mDistLibre = 0.0;
		mDistLibreNb = 0;
	}
	
	void Scene::load(std::string str) {
		clear();
		
		FILE* f = fopen(str.c_str(), "rb");
		fread(&mTime, sizeof(int), 1, f);
		fread(&mParticleNumber, sizeof(int), 1, f);
		mParticles = new Particle[mParticleNumber];
		fread(mParticles, sizeof(Particle), mParticleNumber, f);
		fclose(f);
	}
	
	void Scene::save(std::string str) {
		FILE* f = fopen(str.c_str(), "wb");
		fwrite(&mTime, sizeof(int), 1, f);
		fwrite(&mParticleNumber, sizeof(int), 1, f);
		fwrite(mParticles, sizeof(Particle), mParticleNumber, f);
		fclose(f);
	}
	
	void Scene::saveMeasures(std::string str) {
		std::ofstream fkb((str+"_kb").c_str(), std::ios::out);
		std::ofstream fr((str+"_r").c_str(), std::ios::out);
		std::ofstream ft((str+"_t").c_str(), std::ios::out);
		fkb << mMeasures.size();
		fr << mMeasures.size();
		ft << mMeasures.size();
		for(int i = 0; i < mMeasures.size(); ++i) {
			fkb << mMeasures[i].kb << std::endl;
			fr << mMeasures[i].r << std::endl;
			ft << mMeasures[i].t << std::endl;
		}
	}
	
	
	
	
	
	
	/************************************************************************
	****************** GAZ GENERATION **************************************/
	
	void Scene::generate(int n) {
		clear();
		
		mParticleNumber = n;
		mParticles = new Particle[mParticleNumber];
	}
	
	
	#define EPSILON 0.01
	double randD(double a, double b) {
		return a + rand()*(b-a)/RAND_MAX;
	}
	
	void Scene::fill(double T) {
		std::default_random_engine gen;
		std::normal_distribution<double> distrib(2 * _KB * T / mParticleMass, 100.0);
		printf("Vit Moyenne : %f m/s\n", sqrt(2 * _KB * T / mParticleMass) );
		
		for(int i = 0; i < mParticleNumber; ++i) {
			Particle& p = mParticles[i];
			p.pos.x = randD(-mWidth/2.0  +mParticleRadius, mWidth/2.0  -mParticleRadius);
			p.pos.y = randD(-mHeight/2.0 +mParticleRadius, mHeight/2.0 -mParticleRadius);
			p.lpos = p.pos;
			p.lcollision = p.pos;
			
			double v2 = distrib(gen);
			while(v2 <= 0) v2 = distrib(gen);
			double v = sqrt( v2 );
			double theta = randD(0.0, 2*3.1415);
			p.speed.x = v * cos(theta);
			p.speed.y = v * sin(theta);	
		}
	}
	
	
	
	
	
	
	
	
	
	
	
	/************************************************************************
	****************** PARAM SETTERS ****************************************/
	
	void Scene::setDTexp(int exp) { mDTexp = exp; mDT0 = 1; while(exp != 0) { mDT0 /= 10.0; exp += 1; }}
	void Scene::setDT(double dt) { mDT = dt * mDT0; }
	void Scene::setGravity(double g) { mGravity = g; }
	void Scene::switchShowParticles() { mShowParticles = !mShowParticles; }
	void Scene::addPlane(vec2 A, vec2 B) {
		Plan p;
		p.A = A; p.B = B;
		
		p.dir = normalized(B-A);
		p.dmax1 = -(p.dir.x * A.x + p.dir.y * A.y);
		p.dmax2 = -(p.dir.x * B.x + p.dir.y * B.y);
		
		p.norm = normalized( vec2(-p.dir.y, p.dir.x) );
		p.d = -(p.norm.x * A.x + p.norm.y * A.y);
		mPlanes.push_back(p);
	}
	void Scene::addSquare(vec2 C, vec2 S) {
		S *= 0.5; vec2 S_ = vec2(-S.x, S.y);
		addPlane(C + S, C + S_);
		addPlane(C + S_, C - S);
		addPlane(C - S, C - S_);
		addPlane(C - S_, C + S);
	}
	void Scene::setZone(double w, double h) {
		mWidth = w;
		mHeight = h;
		addSquare(vec2(0.0, 0.0), vec2(w, h));
	}
	void Scene::resetMax() {
		mTZonedMax = 0.0000001;
		mNbZonedMax = 1;
		mDistLibreNb = 0;
		mDistLibre = 0.0;
		
		mKbNb = 0;
		mKb = 0.0;
		mKbSum = 0.0;
		mRNb = 0;
		mR = 0.0;
		mRSum = 0.0;
	}
	
	
	void Scene::setRadius(double r) {
		mParticleRadius = r;
	}
	void Scene::setMass(double m) {
		mParticleMass = m;
	}
	
	void Scene::printInfo() {
	
		computeMeasures();
		double V = mWidth * mHeight;
		double Na = 6.022e23;
		mR = (mPression * V * Na) / (mParticleNumber * mTemperature);
		mRSum += mR;
		++mRNb;
		
		Measure m;
		m.t = mTime;
		m.kb = mKb;
		m.r = mR;
		if(m.kb >= mMaxKb) mMaxKb = m.kb;
		if(m.kb <= mMinKb) mMinKb = m.kb;
		if(m.r >= mMaxR) mMaxR = m.r;
		if(m.r <= mMinR) mMinR = m.r;
		
		
		printf("----------------------------------------------------\n\n");
		printf("Temps : %e s         Etapes : %i     (dt : %f e %i s)\n\n", mTime, mSteps, mDT/mDT0, mDTexp);
		printf("Temperature : %f K\nPression : %e N/m\nLibre parcours moyen : %e m\nNombre de particues : %i \n\n", mTemperature, mPression, mDistLibre/mDistLibreNb, mParticleNumber);
		printf("Constantes :\n  - Kb = %e K/J    (inst : %e) - rouge\n  - R = %e J/K.mol    (inst : %e) - bleu\n\n", mKbSum/(double)mKbNb, mKb, mRSum/(double)mRNb, mR);
		mSteps = 0;
		
		mMeasures.push_back( m );
	}
	
	
	
	
	
	
	
	
	
	
	
	
	/************************************************************************
	****************** DRAWING *********************************************/
	
	vec3 tslToRgb(float h, float s, float l) {
		int i;
		float f, p, q, t;
		
		if(s == 0) return vec3(l, l, l);
		
		h *= 6;
		i = floor( h );
		f = h - i;
		p = l * (1.0 - s);
		q = l * (1.0 - s * f);
		t = l * (1.0 - s * (1-f));
		
		switch(i) {
			case 0: return vec3(l, t, p); break;
			case 1: return vec3(q, l, p); break;
			case 2: return vec3(p, l, t); break;
			case 3: return vec3(p, q, l); break;
			case 4: return vec3(t, p, l); break;
			default: case 5: return vec3(l, p, q); break;
		}
	}
	
	void Scene::draw() {
		
		// Zones de température
		glBegin(GL_QUADS);
			double x = mWidth/_NB_ZONE_X;
			double y = mHeight/_NB_ZONE_Y;
			for(int i = 0; i < _NB_ZONE_X; ++i) {
				for(int j = 0; j < _NB_ZONE_Y; ++j) {
					float coef = mTZones[i][j]/mTZonedMax;
					vec3 color = tslToRgb(2.0/3.0 * (1.0 - coef), 1.0, 1.0);
					glColor4f(color.x, color.y, color.z, 0.2);
					
					glVertex2f(-mWidth/2.0 + i*x,     -mHeight/2.0 + j*y);
					glVertex2f(-mWidth/2.0 + (i+1)*x, -mHeight/2.0 + j*y);
					glVertex2f(-mWidth/2.0 + (i+1)*x, -mHeight/2.0 + (j+1)*y);
					glVertex2f(-mWidth/2.0 + i*x,     -mHeight/2.0 + (j+1)*y);
				}
			}
		glEnd();
	
		// Particules
		if(mShowParticles) {
			glBegin(GL_POINTS);
			for(int i = 0; i < mParticleNumber; ++i) {
				Particle p = mParticles[i];
				// glColor3f(1.0, 0.0, 0.0);
				glColor3f(0.0, 0.0, 0.0);
				glVertex2f(p.pos.x, p.pos.y);
			}
			glEnd();
		}
		
		// Plans
		glBegin(GL_LINES);
			glColor3f(0.0, 0.0, 0.0);
			for(int i = 0; i < mPlanes.size(); ++i) {
				glVertex2f(mPlanes[i].A.x, mPlanes[i].A.y);
				glVertex2f(mPlanes[i].B.x, mPlanes[i].B.y);
			}
		glEnd();
		
		// glBegin(GL_QUADS);
			// float coef = mTemperature/8.0;
			// printf("%f\n", mTemperature);
			// vec3 color = tslToRgb(2.0/3.0 * (1.0 - coef), 1.0, 1.0);
			// glColor4f(color.x, color.y, color.z, 0.2);
			// glVertex2f(-1.5, -0.5);
			// glVertex2f(-2.0, -0.5);
			// glVertex2f(-2.0, 0.5);
			// glVertex2f(-1.5, 0.5);
		// glEnd();
			
		
		
		// Repartition par altitude
		glLineWidth(20.0);
		glBegin(GL_LINE_STRIP);
			y = mHeight/_NB_ZONE_Y_COUNT;
			for(int i = 0; i < _NB_ZONE_Y_COUNT; ++i) {
				double coef = 1.0 - (double)mNbZones[i]/mNbZonedMax;
				glColor3f(coef, coef, coef);
				glVertex2f(mWidth/2.0 + mHeight/10.0, -mHeight/2.0 + i*y);
			}
			glVertex2f(mWidth/2.0 + mHeight/10.0, mHeight/2.0);
		glEnd();
		glLineWidth(2.0);
		
		
		// Quadtree
		// glColor3f(0.0, 0.0, 0.0);
		// glBegin(GL_LINES);
			// x = mWidth/_NB_QUADTREE_X;
			// y = mHeight/_NB_QUADTREE_Y;
			// for(int i = 0; i < _NB_QUADTREE_Y; ++i) {
				// glVertex2f(-mWidth/2.0 - 1.0, -mHeight/2.0 + i*y);
				// glVertex2f(mWidth/2.0, -mHeight/2.0 + i*y);
			// } for(int j = 0; j < _NB_QUADTREE_X; ++j) {
				// glVertex2f(-mWidth/2.0 + j*x, +mHeight/2.0 + 1.0);
				// glVertex2f(-mWidth/2.0 + j*x, -mHeight/2.0 - 1.0);
			// }
			// glVertex2f(mWidth/2.0, -mHeight/2.0 - 1.0);
			// glVertex2f(mWidth/2.0, mHeight/2.0 + 1.0);
			
			// glVertex2f(-mWidth/2.0 - 1.0, mHeight/2.0);
			// glVertex2f(mWidth/2.0, mHeight/2.0);
		// glEnd();
	}
	
	void Scene::plot(int n) {
		glColor3f(0.0, 0.0, 0.0);
		glBegin(GL_LINE_STRIP);
			glVertex2f(0.1, 0.9);
			glVertex2f(0.1, 0.1);
			glVertex2f(0.9, 0.1);
		glEnd();
		
		if(n == 1) {
			glBegin(GL_LINE_STRIP);
				double x = 0.8/_NB_ZONE_Y_COUNT;
				for(int i = 0; i < _NB_ZONE_Y_COUNT; ++i) {
					glVertex2f(0.1 + x*i, 0.1 + 0.8*(double)mNbZones[i]/mNbZonedMax);
				}
			glEnd();
			
			computeRegr();
			glColor3f(1.0, 0.0, 0.0);
			glBegin(GL_LINE_STRIP);
				for(int i = 0; i < _NB_ZONE_Y_COUNT; ++i) {
					glVertex2f(0.1 + x*i, 0.1 + 0.8*exp(mRegressionA*( (double)i * mHeight/_NB_ZONE_Y_COUNT ) + mRegressionB)/mNbZonedMax);
				}
			glEnd();
		} else if(n == 2) {
			glBegin(GL_LINE_STRIP);
				double x = 0.8/_NB_ZONE_Y_COUNT;
				for(int i = 0; i < _NB_ZONE_Y_COUNT; ++i) {
					if(mNbZones[i] != 0)
					glVertex2f(0.1 + x*i, 0.9 + 0.8*(log(mNbZones[i]) - log(mNbZonedMax)) / mHeight);
				}
			glEnd();
		
			computeRegr();
			glColor3f(1.0, 0.0, 0.0);
			glBegin(GL_LINE_STRIP);
				for(int i = 0; i < _NB_ZONE_Y_COUNT; ++i) {
					glVertex2f(0.1 + x*i, 0.9 + 0.8*  mRegressionA*( (double)i * mHeight/_NB_ZONE_Y_COUNT ) / mHeight );
				}
			glEnd();
		} else if(n == 3) {
			glColor3f(0.0, 1.0, 0.0);
			double k = mKbSum/(double)mKbNb;
			glBegin(GL_LINES);
				glVertex2f(0.1, 0.1 + 0.8*(k - mMinKb)/(mMaxKb - mMinKb));
				glVertex2f(0.9, 0.1 + 0.8*(k - mMinKb)/(mMaxKb - mMinKb));
			glEnd();
			
			glColor3f(1.0, 0.0, 0.0);
			glBegin(GL_LINES);
				glVertex2f(0.1, 0.1 + 0.8*(1.38e-23 - mMinKb)/(mMaxKb - mMinKb));
				glVertex2f(0.9, 0.1 + 0.8*(1.38e-23 - mMinKb)/(mMaxKb - mMinKb));
			glEnd();
			
			glColor3f(0.0, 0.0, 1.0);
			glBegin(GL_LINE_STRIP);
				for(int i = 0; i < mMeasures.size(); ++i) {
					glVertex2f(0.1 + 0.8*mMeasures[i].t/mTime, 0.1 + 0.8* (mMeasures[i].kb - mMinKb)/(mMaxKb - mMinKb) );
				}
			glEnd();
			
		} else if(n == 4) {
			mMinR = 5.0;
			
			double r = mRSum/(double)mRNb;
			glColor3f(0.0, 1.0, 0.0);
			glBegin(GL_LINES);
				glVertex2f(0.1, 0.1 + 0.8*(r - mMinR)/(mMaxR - mMinR));
				glVertex2f(0.9, 0.1 + 0.8*(r - mMinR)/(mMaxR - mMinR));
			glEnd();
			
			glColor3f(1.0, 0.0, 0.0);
			glBegin(GL_LINES);
				glVertex2f(0.1, 0.1 + 0.8*(8.314 - mMinR)/(mMaxR - mMinR));
				glVertex2f(0.9, 0.1 + 0.8*(8.314 - mMinR)/(mMaxR - mMinR));
			glEnd();
			
			glColor3f(0.0, 0.0, 1.0);
			glBegin(GL_LINE_STRIP);
				for(int i = 0; i < mMeasures.size(); ++i) {
					glVertex2f(0.1 + 0.8*mMeasures[i].t/mTime, 0.1 + 0.8* (mMeasures[i].r - mMinR)/(mMaxR - mMinR) );
				}
			glEnd();
		}
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	/************************************************************************
	****************** STEP COMPUTATION  ************************************/
	
	void Scene::computeStep() {
		mTime += mDT;
		++mSteps;
		
		//***********************************************************************//
		//**  Initializing Measures
		
		for(int i = 0; i < _NB_ZONE_X; ++i) { for(int j = 0; j < _NB_ZONE_Y; ++j) {
			mTZones[i][j] = 0.0;
			mTZonesNb[i][j] = 0;
			// mTZonedMax = 0.00001;
		}}
		for(int i = 0; i < _NB_ZONE_Y_COUNT; ++i) {
			mNbZones[i] = 0;
			// mNbZonedMax = 1;
		}
		for(int i = 0; i < _NB_QUADTREE_X+2; ++i) { for(int j = 0; j < _NB_QUADTREE_Y+2; ++j) {
			mQuadtreeNb[i][j] = 0;
		}}
		
		mTemperature = 0;
		
		
		//***********************************************************************//
		//**   Computation
		
		for(int i = 0; i < mParticleNumber; ++i) {
			Particle& p = mParticles[i];
			p.lpos = p.pos;
			
			p.acc.y = -mGravity;
			p.speed = p.speed + p.acc * mDT;
			p.pos = p.pos + p.speed * mDT;
			
			// Quadtree
			int nquadx = _NB_QUADTREE_X * (p.pos.x+mWidth/2.0)/mWidth;
			int nquady = _NB_QUADTREE_Y * (p.pos.y+mHeight/2.0)/mHeight;
			if(nquadx < 0 || nquadx >= _NB_QUADTREE_X || nquady < 0 || nquady > _NB_QUADTREE_Y) { // Out of zone
				nquadx = (nquadx < 0) ? _NB_QUADTREE_X : ((nquadx >= _NB_QUADTREE_X) ? _NB_QUADTREE_X + 1 : nquadx);
				nquady = (nquady < 0) ? _NB_QUADTREE_Y : ((nquady >= _NB_QUADTREE_Y) ? _NB_QUADTREE_Y + 1 : nquady);
			}
			int n = mQuadtreeNb[nquadx][nquady];
			mQuadtree[nquadx][nquady][n] = &mParticles[i];
			++mQuadtreeNb[nquadx][nquady];	
		}
		
		
		//***********************************************************************//
		//**  Resolving collisions
		
		mQtMvt = 0.0;
		
			//sf::Clock clock;
		mThreadsRemaining = (_NB_QUADTREE_X + 2) * (_NB_QUADTREE_Y + 2);
		for(int i = 0; i < _NB_QUADTREE_X + 2; ++i) {
			for(int j = 0; j < _NB_QUADTREE_Y + 2; ++j) {
				mThreadsMustCompute[i][j] = true;
			}
		}
			// sf::Time t1 = clock.restart();
		
		while(mThreadsRemaining != 0) { }
			// sf::Time t2 = clock.restart();
			// printf("Temps de lancement : %i\nTemps d'execution :%i\n\n", t1.asMicroseconds(), t2.asMicroseconds());
		
		mQtMvt *= mParticleMass;
		mPressionPart += mQtMvt / (2* (mWidth + mHeight) * mDT);
		++mPressionNb;
		
		
		//***********************************************************************//
		//**  Taking Measures
		
		for(int i = 0; i < mParticleNumber; ++i) {
			Particle& p = mParticles[i];
			
			// Correct if still out of zone
			int nquadx = _NB_QUADTREE_X * (p.pos.x+mWidth/2.0)/mWidth;
			int nquady = _NB_QUADTREE_Y * (p.pos.y+mHeight/2.0)/mHeight;
			if(nquadx < 0 || nquadx >= _NB_QUADTREE_X || nquady < 0 || nquady > _NB_QUADTREE_Y) {
				// printf("Fuck :/\n");
				p.pos.x = randD(-mWidth/2.0+mParticleRadius, mWidth/2.0-mParticleRadius); p.pos.y = randD(-mHeight/2.0 +mParticleRadius, mHeight/2.0 -mParticleRadius); p.lpos = p.pos;
			}
			
			// Temperature
			int nx = _NB_ZONE_X * (p.pos.x+mWidth/2.0)/mWidth;
			int ny = _NB_ZONE_Y * (p.pos.y+mHeight/2.0)/mHeight;
			mTZones[nx][ny] += p.speed.sqlen();
			mTemperature += p.speed.sqlen();
			++mTZonesNb[nx][ny];
			
			// Altitude
			int nyc = _NB_ZONE_Y_COUNT * (p.pos.y+mHeight/2.0)/mHeight;
			++mNbZones[nyc];
		}
		
		
		
		//***********************************************************************//
		//**  Finishing Measures
		for(int i = 0; i < _NB_ZONE_X; ++i) { for(int j = 0; j < _NB_ZONE_Y; ++j) {
			if(mTZonesNb[i][j] != 0) mTZones[i][j] *= 0.5 * mParticleMass / _KB / mTZonesNb[i][j];
			if(mTZones[i][j] >= mTZonedMax) mTZonedMax = mTZones[i][j];
			
		}}
		for(int i = 0; i < _NB_ZONE_Y_COUNT; ++i) {
			if(mNbZones[i] >= mNbZonedMax) mNbZonedMax = mNbZones[i];
		}
		mTemperature /= mParticleNumber;
		mTemperature *= 0.5 * mParticleMass / _KB;
	}
	
	
	
	
	
	
	
	
	
	/************************************************************************
	****************** MEASURES  ********************************************/
	
	void Scene::computeRegr() {
		int n = 0;
		double Y = 0; for(int i = 0; i < _NB_ZONE_Y_COUNT; ++i) { 
			if(mNbZones[i] != 0) { Y += log(mNbZones[i]); ++n; }
		} Y /= n;
		double X = (n + 1) * mHeight / (2.0 * _NB_ZONE_Y_COUNT);
		
		double cov = 0.0; double var = 0.0;
		for(int i = 0; i < _NB_ZONE_Y_COUNT; ++i) {
			if(mNbZones[i] != 0) {
				cov += (i * mHeight/_NB_ZONE_Y_COUNT - X) * (log(mNbZones[i]) - Y);
				var += (i * mHeight/_NB_ZONE_Y_COUNT - X) * (i * mHeight/_NB_ZONE_Y_COUNT - X);
			}
		}
		
		mRegressionA = cov/var;
		mRegressionB = Y - mRegressionA * X;
	}
		
		
	void Scene::computeMeasures() {
		computeRegr();
		
		mKb = - mParticleMass * mGravity / (mRegressionA * mTemperature);
		mKbSum += mKb;
		++mKbNb;
		
		if(mPressionNb == 0) mPression = 0;
		else mPression = mPressionPart / (double)mPressionNb;
		mPressionPart = 0.0;
		mPressionNb = 0;
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	/************************************************************************
	****************** COLLISION ********************************************/
	
	void resolveCollision(ThreadParam p) {
		p.s->resolve(p.i, p.j);
	}
	
	
	void Scene::resolve(int i, int j) {
		while(true) {
			while(!mThreadsMustCompute[i][j]) { sf::sleep(sf::microseconds(10)); }
		
			for(int k = 0; k < mQuadtreeNb[i][j]; ++k) {
				// if(j == _NB_QUADTREE_Y - 1 || j == _NB_QUADTREE_Y + 1)	collidePlane(*mQuadtree[i][j][k], mPlanes[0]);
				if(j >= _NB_QUADTREE_Y - 1)								collidePlane(*mQuadtree[i][j][k], mPlanes[0]);
				if(i == 0 || i == _NB_QUADTREE_X)						collidePlane(*mQuadtree[i][j][k], mPlanes[1]);
				if(j == 0 || j == _NB_QUADTREE_Y)						collidePlane(*mQuadtree[i][j][k], mPlanes[2]);
				if(i == _NB_QUADTREE_X - 1 || i == _NB_QUADTREE_X + 1)	collidePlane(*mQuadtree[i][j][k], mPlanes[3]);
				
				for(int l = k+1; l < mQuadtreeNb[i][j]; ++l) {
					collideSphere(*mQuadtree[i][j][k], *mQuadtree[i][j][l]);
				}
			}
			
			mMutex.lock();
				mThreadsMustCompute[i][j] = false;
				--mThreadsRemaining;
				// printf("Thread finished ! %i -> %i\n", mThreadsRemaining+1, mThreadsRemaining);
			mMutex.unlock();
		}
	}
	
	void Scene::collideSphere(Particle& p1, Particle& p2) {
		vec2 D = p1.pos - p2.pos;
		if(D.length() <= 2.0 * mParticleRadius) { // Test de la collision : distance entre les boules plus petite que 2 rayons
		
			// Trouver le temps exact de contact
			double mTime;
			{
				vec2 globalSpeed = p1.speed - p2.speed;
			
				double a = globalSpeed.sqlen();
				double b = 2.0 * (globalSpeed.x * D.x + globalSpeed.y  * D.y);
				double c = D.sqlen() - 4 * mParticleRadius * mParticleRadius;
					
				double delta = b*b - 4*a*c;
				mTime = (-b-sqrt(delta))/(2*a);
			}
			
			// Retour à la position de contact
			p1.pos -= p1.speed * (mDT - mTime);
			p2.pos -= p2.speed * (mDT - mTime);
			
			mDistLibre += (p1.pos - p1.lcollision).length() + (p2.pos - p2.lcollision).length();
			p1.lcollision = p1.pos; p2.lcollision = p2.pos;
			mDistLibreNb += 2;
			
			
			// Adaptation des vitesses
			{
				vec2 n = normalized(D);
				
				double v1n = dot(p1.speed, n);
				double v2n = dot(p2.speed, n);
				double V = dot(p1.speed + p2.speed, n) / 2.0;
				
				p1.speed += n * 2* (V - v1n);
				p2.speed += n * 2* (V - v2n);
			}
			
			// Resimulation depuis le point de contact
			p1.pos += p1.speed * (mDT - mTime);
			p2.pos += p2.speed * (mDT - mTime);
			
			
		}
	}
	
	
	void Scene::collidePlane(Particle& p, Plan& plan) {
		vec2 norm = plan.norm;
		double d = plan.d;
		
		if( (dot(norm, p.pos) + d) * (dot(norm, p.lpos) + d) <= 0   // Changement de cote
			|| fabs(dot(norm, p.pos) + d) <= mParticleRadius ) { // Distance plus petite que le rayon = coupure
			
			// Est ce qu'on est sur le plan fini ?
			if(plan.dmax2 >= dot(plan.dir, p.pos) || dot(plan.dir, p.pos) >= plan.dmax1) return;
			
			// Trouver le temps exact de contact
			double mTime;
			{
				double A = dot(norm, p.speed);
				double B = dot(norm, p.lpos) +d;
				double C = norm.length() * mParticleRadius;
				
				double t1 = (-B - C)/A;
				double t2 = (-B + C)/A;
				if(t1 <= t2) mTime = t1;
				else mTime = t2;
			}
			
			// Retour à la position de contact
			p.pos -= p.speed * (mDT - mTime);
			vec2 qtmvt = p.speed;
			
			// Adaptation des vitesses
			{
				vec2 speedNorm = norm * dot(norm, p.speed);
				p.speed = (p.speed - speedNorm * 2);
			}
			qtmvt = p.speed - qtmvt;
			mQtMvt += qtmvt.length();
			
			// Resimulation depuis le point de contact
			p.pos += p.speed * (mDT - mTime);
			
		}
	}

};