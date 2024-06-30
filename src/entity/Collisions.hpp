#pragma once

#include "Entity.hpp"

// detects collisions with world borders, walls, and other game objects
void defaultCollisionFunction( std::shared_ptr<GameObject> o );

// similar to default, but lets the player load different areas when walking off the map
void playerCollisionFunction( std::shared_ptr<GameObject> p );


// for an item sitting still, collide with non-items normally, but when colliding
// with another item, combine stacks, if possible
void stationaryItemCollisionFunction( std::shared_ptr<GameObject> item );



// pushes apart gameobjects that are too close to itself
void defaultHandleCollisionsWithGameObjects( std::shared_ptr<GameObject> o );
// pushes apart objects that are too close to itself,
// or combine stacks with other items
void itemsHandleCollisionsWithGameObjects( std::shared_ptr<GameObject> item );




// makes sure the object cannot walk out of bounds
bool defaultCollideWithWorldBorders( std::shared_ptr<GameObject> o );
// loads different areas based on where the player walks
void playerCollideWithWorldBorders( std::shared_ptr<GameObject> p );

// checks if the hitbox is within a wall, and pushes it out
void defaultHandleSideCollisionsWithWalls( std::shared_ptr<GameObject> o );
void defaultHandleCornerCollisionsWithWalls( std::shared_ptr<GameObject> o );