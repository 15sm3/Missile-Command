// state.cpp


#include "headers.h"

#include "state.h"


// Draw each of the objects in the world


void State::draw() 

{
  int i;

  gpuProgram->activate();

  glUniformMatrix4fv( glGetUniformLocation( gpuProgram->id(), "M"), 1, GL_TRUE, &M[0][0] );

  silos[0].draw( gpuProgram );
  
  for (i=0; i<silos.size(); i++)
    silos[i].draw( gpuProgram );

  for (i=0; i<cities.size(); i++)
    cities[i].draw( gpuProgram );

  for (i=0; i<missilesIn.size(); i++)
    missilesIn[i].draw( gpuProgram );

  for (i=0; i<missilesOut.size(); i++)
    missilesOut[i].draw( gpuProgram );

  for (i=0; i<explosions.size(); i++)
    explosions[i].draw( gpuProgram );

  for (i=0; i<contract.size(); i++)
    contract[i].draw( gpuProgram );

  gpuProgram->deactivate();
}


// Update the state of the world after some time interval, deltaT
//
// CHANGE ALL OF THIS FUNCTION


void State::updateState( float deltaT )

{
  int i;

  // Update the time

  currentTime += deltaT;

  // Generate some new missiles.  The rate of missile generation
  // should increase with time.
  //
  // CHANGE THIS

  float missileRate = currentTime/600;
  vec3 source(randIn01(), worldTop, 0);
 

  //increases with time, with a cap of 40% probability after 240 seconds
  if (randIn01() > 0.99 - std::min(missileRate, 0.4f) ) {	// New missile 

    missilesIn.add( Missile( 
           source, // source
			     0.1* (vec3 (randIn01(), 0, 0) - source).normalize(),         // velocity
			     0,                              // destination y
			     vec3( 1,1,0 ) ) );              // colour
  }

  // Look for terminating missiles

  for (i=0; i<missilesIn.size(); i++)
    if (missilesIn[i].hasReachedDestination()) {
      // CHANGE THIS: ADD AN EXPLOSION
      explosions.add(Circle(missilesIn[i].position(), 0.15, 0.02, vec3(1,1,0)));
      missilesIn.remove(i);
      i--;
    }

  for (i=0; i<missilesOut.size(); i++)
    if (missilesOut[i].hasReachedDestination()) {
      // CHANGE THIS: ADD AN EXPLOSION
      explosions.add(Circle(missilesOut[i].position(), 0.15, 0.04, vec3(0,1,1)));
      missilesOut.remove(i);
      i--;
    }

  // Look for terminating explosions

  for (i=0; i<explosions.size(); i++)
    if (explosions[i].radius() >= explosions[i].maxRadius()) {

      // CHANGE THIS: CHECK FOR DESTROYED CITY OR SILO

        for(int j=0;j <silos.size(); j++) {
            if(silos[j].isHit(explosions[i].position(), explosions[i].radius())) {
                silos.remove(j);
            }
        }

        for (int k=0; k<cities.size(); k++) {
            if(cities[k].isHit(explosions[i].position(), explosions[i].radius())) {
              cities.remove(k);
            }
        }

      // Look for incoming missiles that hit an explosion and are
      // destroyed

      // ADD CODE HERE

       for (int l=0; l<missilesIn.size(); l++) {
            if(missilesIn[l].isHit(explosions[i].position(), explosions[i].radius())) {
              missilesIn.remove(l);
            }
        }
      contract.add(explosions[i]);
      explosions.remove(i);
      i--;
    }

    for (i=0; i<contract.size(); i++) {
        if (contract[i].radius() <= 0) {
              contract.remove(i);
                i--;
        }
        }


  // Update all the moving objects

  for (i=0; i<missilesIn.size(); i++)
    missilesIn[i].move( deltaT );

  for (i=0; i<missilesOut.size(); i++)
    missilesOut[i].move( deltaT );

  for (i=0; i<explosions.size(); i++) 
      explosions[i].expand( deltaT );

  for (i=0; i<contract.size(); i++) 
      contract[i].expand( -deltaT/3 );
    
  
}


// Fire a missile

void State::fireMissile( int siloIndex, float x, float y )

{
  const float speed = 0.3;
    
  if (silos[siloIndex].canShoot()) {

    silos[siloIndex].decrMissiles();

    missilesOut.add( 
          Missile( silos[siloIndex].position(),           // source
			    speed * (vec3(x,y,0) - silos[siloIndex].position()).normalize(), // velocity
			    y,		                     // destination y
			    vec3( 0,1,1 ) ) );                     // colour
      
  }
}



// Create the initial state of the world


void State::setupWorld()

{
  // Keep track of the time

  currentTime = 0;

  timeOfIncomingFlight = 6;	// 6 seconds for incoming missile to arrive

  // Seed the random number generator

#ifdef _WIN32
  srand( 12345 );
#else
  struct timeb t;
  ftime( &t );
  srand( t.time );
#endif

  // Silos are added in order left, middle, right so that they have
  // indices 0, 1, and 2 (respectively) in the "silos" array.

  silos.add(  Silo( vec3( 0.1, 0, 0 ) ) );
  silos.add(  Silo( vec3( 0.5, 0, 0 ) ) );
  silos.add(  Silo( vec3( 0.9, 0, 0 ) ) );

  // Cities on the left

  cities.add( City( vec3( 0.2, 0, 0 ) ) );
  cities.add( City( vec3( 0.3, 0, 0 ) ) );
  cities.add( City( vec3( 0.4, 0, 0 ) ) );

  // Cities on the right

  cities.add( City( vec3( 0.6, 0, 0 ) ) );
  cities.add( City( vec3( 0.7, 0, 0 ) ) );
  cities.add( City( vec3( 0.8, 0, 0 ) ) );
}
