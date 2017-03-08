#ifndef Scene2Defined
#define Scene2Defined

#include "vec2.hpp"
#include <vector>
#include <string>

#define _NB_QUADTREE_X 10
#define _NB_QUADTREE_Y 40
#define _NB_ZONE_X 2
#define _NB_ZONE_Y 10
#define _NB_ZONE_Y_COUNT 100

namespace tipe {

	class Scene;

	struct Particle {
		vec2 pos;
		vec2 speed;
		vec2 acc;
		vec2 lpos;
		vec2 lcollision;
	};
	
	struct Plan {
		vec2 norm;
		vec2 dir;
		double d;
		double dmax1;
		double dmax2;
		vec2 A;
		vec2 B;
	};
	
	struct ThreadParam {
		Scene* s;
		int i;
		int j;
	};

	class Scene {
		public:
			Scene();
			
			void save(std::string str);
			void saveMeasures(std::string str);
			void load(std::string str);
			
			void switchShowParticles();
			void setDTexp(int exp);
			void setDT(double dt);
			void setRadius(double r);
			void setMass(double m);
			void setMeasurement(int n, double i);
			void setGravity(double g);
			void addPlane(vec2 A, vec2 B);
			void addSquare(vec2 C, vec2 S);
			void setZone(double w, double h);
			void resetMax();
			void printInfo();
			
			void clear();
			void generate(int n1);
			void fill(double T);
			void computeStep();
			
			void draw();
			void plot(int n);
			
			void resolve(int i, int j);
			
		protected:
			void collideSphere(Particle& p1, Particle& p2);
			void collidePlane(Particle& p, Plan& plan);
			void computeMeasures();
			void computeRegr();
			
		private:
			struct Measure {
				double t;
				double kb;
				double r;
			};
			
			double mTime;
			double mDT0;
			int mDTexp;
			double mDT;
			int mSteps;
			
			double mDistLibre;
			int mDistLibreNb;
			
			Particle* mParticles;
			int mParticleNumber;
			double mTemperature;
			
			double mPression;
			double mQtMvt;
			double mPressionPart;
			int mPressionNb;
			
			double mRegressionA;
			double mRegressionB;
			
			double mKbSum;
			double mKb;
			int mKbNb;
			
			double mRSum;
			double mR;
			int mRNb;
			
			std::vector< Measure > mMeasures;
			double mMaxKb;
			double mMinKb;
			double mMaxR;
			double mMinR;
			
			
			double mTZones[_NB_ZONE_X][_NB_ZONE_Y];
			double mTZonesNb[_NB_ZONE_X][_NB_ZONE_Y];
			double mTZonedMax;
			int mNbZones[_NB_ZONE_Y_COUNT];
			int mNbZonedMax;
			
			double mHeight;
			double mWidth;
			double mParticleRadius;
			double mParticleMass;
			bool mShowParticles;
			double mGravity;
			
			std::vector<Plan> mPlanes;
			
			Particle* mQuadtree[_NB_QUADTREE_X+2][_NB_QUADTREE_Y+2][500];
			int mQuadtreeNb[_NB_QUADTREE_X+2][_NB_QUADTREE_Y+2];
			sf::Thread* mThreads[_NB_QUADTREE_X+2][_NB_QUADTREE_Y+2];
			bool mThreadsMustCompute[_NB_QUADTREE_X+2][_NB_QUADTREE_Y+2];
			int mThreadsRemaining;
			sf::Mutex mMutex;

	};
		
};

#endif