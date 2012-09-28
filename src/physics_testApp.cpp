#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/Perlin.h"
#include "cinder/gl/gl.h"

#include "MSACore.h"
#include "MSAObjCPointer.h"
#include "MSAPhysics3D.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace msa;

class physics_testApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );
    void keyDown( KeyEvent event );
    void resize( ResizeEvent event );
	void update();
	void draw();
    void addRandomParticle();
    void addRandomSpring();
    
    msa::physics::World3D   myWorld;
    
    Perlin          mPerlin;
    float			mSeed;
	float			mOctaves;
	float			mTime;
    float           mFrequency;
    
    CameraPersp     mCam;
    
    Vec2f           netSize;
    
};

void physics_testApp::resize( ResizeEvent event )
{
	mCam.lookAt( Vec3f( 10.0f, -10.0f, 25.0f ), Vec3f( 10.0f, -10.0f, 0.0f ) );
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	gl::setMatrices( mCam );
}

void physics_testApp::setup()
{
    
    gl::enableAlphaBlending();
    
    myWorld.setGravity(Vec3f(0, -0.1, 0));
	myWorld.setSectorCount(1);
	myWorld.setDrag(0.95f);
    
    // Particles
    
    netSize = Vec2f(20,20);
    
    for (int y = 0; y < netSize.y; y++) {
        for (int x = 0; x < netSize.x; x++) {
            physics::Particle3D *p = myWorld.makeParticle(Vec3f(x, -y, 0));
            p->setMass(0);
            if (y == 0) p->makeFixed();
        }
    }
    
    // Springs
    
    for (int y = 0; y < netSize.y; y++)
    {
        for (int x = 0; x < netSize.x; x++)
        {
            if (y < netSize.y-1 && x < netSize.x-1) {
                physics::Particle3D *a = myWorld.getParticle((y * netSize.x) + x);
                physics::Particle3D *b = myWorld.getParticle((y * netSize.x) + x + 1);
                physics::Particle3D *c = myWorld.getParticle(((y + 1) * netSize.x) + x);
                myWorld.makeSpring(a, b, 1.3, 1);
                myWorld.makeSpring(a, c, 1.3, 1);
            }
            
            if (y < netSize.y-1 && x == netSize.x-1) {
                physics::Particle3D *a = myWorld.getParticle((y * netSize.x) + x);
                physics::Particle3D *c = myWorld.getParticle(((y + 1) * netSize.x) + x);
                myWorld.makeSpring(a, c, 1.3, 1);
            }
            
            if (y == netSize.y-1 && x < netSize.x-1) {
                physics::Particle3D *a = myWorld.getParticle((y * netSize.x) + x);
                physics::Particle3D *b = myWorld.getParticle((y * netSize.x) + x + 1);
                myWorld.makeSpring(a, b, 1.38, 1);
            }
        }
    }
    
    mSeed = clock();
	mOctaves = 4;
	mTime = 0.1f;
    mFrequency = 0.2;
    
    mPerlin = Perlin( mOctaves, mSeed );

}

void physics_testApp::mouseDown( MouseEvent event )
{
    addRandomSpring();
}

void physics_testApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ){
		case 's': addRandomSpring();	break;
		default:						break;
	}
}

void physics_testApp::update()
{
    mTime += 0.2f;
    
    for (int x = 0; x < netSize.x; x++)
    {
        physics::Particle3D *a = myWorld.getParticle(x);
        a->moveTo(Vec3f(x, 0, mPerlin.noise(mTime*mFrequency, x*mFrequency) * 5));
    }
    
    myWorld.update();
}

void physics_testApp::draw()
{
	gl::clear( ci::Color( 0, 0, 0 ) );
    
    // Draw Springs
    
    glColor4f(1, 1, 1, 0.3);
    for(int i=0; i<myWorld.numberOfSprings(); i++) {
        msa::physics::Spring3D *spring = (msa::physics::Spring3D *) myWorld.getSpring(i);
        msa::physics::Particle3D *a = spring->getOneEnd();
        msa::physics::Particle3D *b = spring->getTheOtherEnd();
        glPushMatrix();
        glLineWidth(1);
        gl::drawLine(a->getPosition(), b->getPosition());
        glPopMatrix();
    }
    
    // Draw Particles

    for(int i=0; i<myWorld.numberOfParticles(); i++) {
        msa::physics::Particle3D *p = myWorld.getParticle(i);
        glColor4f(1, 1, 1, 1);
        glBegin(GL_POINTS);
        glVertex3f(p->getPosition());
        glEnd();
    }
    
}

void physics_testApp::addRandomSpring() {
	msa::physics::Particle3D *a = myWorld.getParticle((int)randFloat(0, myWorld.numberOfParticles()));
	msa::physics::Particle3D *b = myWorld.getParticle((int)randFloat(0, myWorld.numberOfParticles()));
	myWorld.makeSpring(a, b, randFloat(0.005, 0.1), randFloat(10, 250));
}


CINDER_APP_BASIC( physics_testApp, RendererGl )
