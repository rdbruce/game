#pragma once

#include "Entity.hpp"
#include "AStarPathfinding.hpp"

// velocity functions

// gives wolf velocity in the direction of the player
void wolfVelocityFunc( std::shared_ptr<GameObject> w );
// a jumping attack for the wolf
void wolfJumpAttack( std::shared_ptr<GameObject> w );
// moves to the centre of the map without collision until it gets into the map
void wolfSpawningFunc( std::shared_ptr<GameObject> w );


// adds object's acceleration to their velocity
void defaultVelocityFunc( std::shared_ptr<GameObject> o );
// adds acceleration to velocity, until the object comes to a stop
void deccelerateVelocityFunc( std::shared_ptr<GameObject> o );

// draws a unit vector to the player and moves towards it
void moveDirectlyToPlayer( std::shared_ptr<GameObject> o );




// position functions

void playerPositionFunc( std::shared_ptr<GameObject> p );

// adds velocity to current position
void defaultPositionFunc( std::shared_ptr<GameObject> o );
// trees don't move, snap their position to the position they spawned
void fallingTreePositionFunc( std::shared_ptr<GameObject> t );

// holds the item stack a constant distance away from the player's centre
void heldItemPositionFunc( std::shared_ptr<GameObject> item );
// adds velocity to the position. when the velocity reaches zero, set acceleration to zero.
void thrownItemPositionFunc( std::shared_ptr<GameObject> item );


// animation scripts

// chooses an animation set depending on which direction the player is facing,
// chooses one image from the selected set based on how much the animation has progressed
LTexture * playerAnimationScript( std::shared_ptr<GameObject> p );
// rotates the tree image, using linear interpolation
LTexture * treeFallingAnimations( std::shared_ptr<GameObject> t );

// just returns the object's brush member. for objects with no animation
LTexture * noAnimation( std::shared_ptr<GameObject> o );


// pathfinding!!!!

// finds the best path to the player
void pathfindToPlayer( std::shared_ptr<GameObject> o, int searchDepth );

void wolfUpdatePathfinding( std::shared_ptr<GameObject> w );
void wolfWalkToNextCell( std::shared_ptr<GameObject> w );



// functions that make switching between these functions easier

// switches a wolf into it's jump attack
void wolfEnterJumpAttack( std::shared_ptr<GameObject> w );