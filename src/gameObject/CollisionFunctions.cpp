#include "GameObject.hpp"
#include "../game/Game.hpp"

void GameObject::defaultCollisionFunction()
{
    // do nothing if the object doesn't have collision
    if (!hasCollision) return;

    // collide with other game objects
    defaultHandleCollisionsWithGameObjects();
    // handle everything else AFTER other entities, to avoid being pushed into walls/oob

    // world borders
    bool flag = defaultCollideWithWorldBorders();
    // return if the object was moved to another scene
    if (flag) return;

    // walls
    defaultHandleSideCollisionsWithWalls();
    defaultHandleCornerCollisionsWithWalls();
}


void GameObject::playerCollisionFunction()
{
    if (!hasCollision) return;
    // collide with other game objects
    defaultHandleCollisionsWithGameObjects();
    // handle everything else AFTER other entities, to avoid being pushed into walls

    // world borders
    playerCollideWithWorldBorders();

    // walls
    defaultHandleSideCollisionsWithWalls();
    defaultHandleCornerCollisionsWithWalls();
}



void GameObject::stationaryItemCollisionFunction()
{
    // do nothing if the object doesn't have collision, such as when it's being held
    if (!hasCollision) return;

    // collide with other game objects
    itemsHandleCollisionsWithGameObjects();
    // handle everything else AFTER other entities, to avoid being pushed into walls/oob

    // world borders
    bool flag = defaultCollideWithWorldBorders();
    // return if the object was moved to another scene
    if (flag) return;

    // walls
    defaultHandleSideCollisionsWithWalls();
    defaultHandleCornerCollisionsWithWalls();
}




void GameObject::defaultHandleCollisionsWithGameObjects()
{
    // get the vector of game objects in the current level
    std::vector<std::shared_ptr<GameObject>> gameObjects = game->currLevel->gameObjects;

    // iterate through all the other game objects to find the distance between them
    for (int i = 0; i < gameObjects.size(); i++)
    {
        std::shared_ptr<GameObject> other = gameObjects[i];
        // doesn't collide with itself or objects that have no collision
        if (idx == i || !other->has_collision()) continue;

        // find the displacement between the two objects
        Vector2 disp = other->get_pos() - pos;
        // break up objects directly on each other
        if (disp == Vector2_Zero) disp = Vector2_One;

        // d is the magnitude of the displacement,
        // r is the sum of the radii of the two objects
        float d = disp.length(), r = radius + other->get_radius();

        // if d < r, the objects are too close to each other and need to be pushed apart
        if ( d < r )
        {
            // player will not collide with held object
            if (type != Player || !other->is_held()) {
                // normalise the displacement to find the direction the objects need to move
                disp.normalise();

                // knock objects apart
                float sideLen = game->currLevel->cell_sideLen;
                velocity += disp * (-0.266667 * sideLen);
                acceleration = velocity * -1.5f;

                Vector2 newVel = other->get_velocity() + (disp * (0.266667 * sideLen));
                other->set_velocity(newVel);
                other->set_acceleration(newVel * -1.5f);

                // objects damage each other
                if (!other->is_enemy()) {
                    hp -= other->get_damage();
                    other->set_HP( other->get_hp() - damage );
                }
            }
        }
    }
}


void GameObject::itemsHandleCollisionsWithGameObjects()
{
    // get the vector of game objects in the current level
    std::vector<std::shared_ptr<GameObject>> gameObjects = game->currLevel->gameObjects;

    // iterate through all the other game objects to find the distance between them
    for (int i = 0; i < gameObjects.size(); i++)
    {
        std::shared_ptr<GameObject> other = gameObjects[i];
        // doesn't collide with itself or objects that have no collision
        if (idx == i || !other->has_collision()) continue;
        // held item won't collide with player, other items won't collide with held item
        if ((is_held() && other->get_type() == Player) || other->is_held()) continue;

        // find the displacement between the two objects
        Vector2 disp = other->get_pos() - pos;
        // break up objects directly on each other
        if (disp == Vector2_Zero) disp = Vector2_One;

        // d is the magnitude of the displacement,
        // r is the sum of the radii of the two objects
        float d = disp.length(), r = radius + other->get_radius();

        // if d < r, the objects are too close to each other and need to be pushed apart
        if ( d < r )
        {
            if (other->is_item()) {
                if (other->get_type() == type) {
                    // add the other stack's item to this stack's, as far as is possible
                    // items store their count in their hp variable
                    
                    int spaceAvailable = max_hp - hp; // how many more items can be held

                    int otherHP = other->get_hp();
                    // number of items being transfered
                    int transfer = Min(otherHP, spaceAvailable);

                    // transfer the items
                    hp += transfer;
                    other->set_HP(otherHP - transfer);

                // not the same item type, attempt to craft with the other item stack
                } game->craftTwoItems(gameObjects[idx], other);
            }

            // regular collision behaviour
            // normalise the displacement to find the direction the objects need to move
            disp.normalise();

            // if the item is moving, damage the enemy it hit
            if (other->is_enemy()) {
                // make sure that it is moving fast enough
                if (velocity.length() > 35.0f && timer <= 0.0f) {
                    // calculate the damage by multiplying the speed by the number of items in the stack
                    int dam = ceilToInt(hp * moveSpeed);
                    // damage the enemy
                    other->set_HP(other->get_hp() - dam);
                }
                timer = 0.75f;
            }
            // knock objects apart
            float sideLen = game->currLevel->cell_sideLen;
            velocity += disp * (-0.266667 * sideLen);
            acceleration = velocity * -1.5f;

            Vector2 newVel = other->get_velocity() + (disp * (0.266667 * sideLen));
            other->set_velocity(newVel);
            other->set_acceleration(newVel * -1.5f);
        }
    }
}

bool GameObject::defaultCollideWithWorldBorders()
{
    Vector2Int map = get_mapDimensions();
    if (hitbox.x < 0.0f) {
        auto lvl = game->currLevel->left;
        if (lvl != nullptr) {
            Vector2Int newMap = lvl->gridDimensions * lvl->cell_sideLen;
            Vector2 newPos(newMap.x-hitbox.w-2, newMap.y/2);
            game->moveEntityToLevel(game->currLevel->gameObjects[idx], lvl, newPos);
            return true; // true for moving to another scene
        } else {
            hitbox.x = 0.0f;
            pos.x = hitbox.x + hitbox.w/2;
        }
    } else if (hitbox.x > map.x-hitbox.w) {
        auto lvl = game->currLevel->right;
        if (lvl != nullptr) {
            Vector2Int newMap = lvl->gridDimensions * lvl->cell_sideLen;
            Vector2 newPos(hitbox.w, newMap.y/2);
            game->moveEntityToLevel(game->currLevel->gameObjects[idx], lvl, newPos);
            return true; // true for moving to another scene
        } else {
            hitbox.x = map.x-hitbox.w;
            pos.x = hitbox.x + hitbox.w/2;
        }
    }
    if (hitbox.y < 0.0f) {
        auto lvl = game->currLevel->above;
        if (lvl != nullptr) {
            Vector2Int newMap = lvl->gridDimensions * lvl->cell_sideLen;
            Vector2 newPos(newMap.x/2, newMap.y-hitbox.h-2);
            game->moveEntityToLevel(game->currLevel->gameObjects[idx], lvl, newPos);
            return true; // true for moving to another scene
        } else {
            hitbox.y = 0.0f;
            pos.y = hitbox.y + hitbox.h/2;
        }
    } else if (hitbox.y > map.y-hitbox.h) {
        auto lvl = game->currLevel->below;
        if (lvl != nullptr) {
            Vector2Int newMap = lvl->gridDimensions * lvl->cell_sideLen;
            Vector2 newPos(newMap.x/2, hitbox.h);
            game->moveEntityToLevel(game->currLevel->gameObjects[idx], lvl, newPos);
            return true; // true for moving to another scene
        } else {
            hitbox.y = map.y-hitbox.h;
            pos.y = hitbox.y + hitbox.h/2;
        }
    }
    return false;
}


void GameObject::playerCollideWithWorldBorders()
{
    Vector2Int map = get_mapDimensions();
    if (hitbox.x < 0.0f) {
        Scene *lvl = game->currLevel->left;
        if (lvl != nullptr) {
            Vector2Int newMap = lvl->gridDimensions * lvl->cell_sideLen;
            Vector2 newPos(newMap.x-hitbox.w-2, newMap.y/2);
            game->movePlayerToLevel(lvl, newPos);
            return;
        } else {
            hitbox.x = 0.0f;
            pos.x = hitbox.x + hitbox.w/2;
        }
    } else if (hitbox.x > map.x-hitbox.w) {
        Scene *lvl = game->currLevel->right;
        if (lvl != nullptr) {
            Vector2Int newMap = lvl->gridDimensions * lvl->cell_sideLen;
            Vector2 newPos(hitbox.w, newMap.y/2);
            game->movePlayerToLevel(lvl, newPos);
            return;
        } else {
            hitbox.x = map.x-hitbox.w;
            pos.x = hitbox.x + hitbox.w/2;
        }
    }
    if (hitbox.y < 0.0f) {
        Scene *lvl = game->currLevel->above;
        if (lvl != nullptr) {
            Vector2Int newMap = lvl->gridDimensions * lvl->cell_sideLen;
            Vector2 newPos(newMap.x/2, newMap.y-hitbox.h-2);
            game->movePlayerToLevel(lvl, newPos);
            return;
        } else {
            hitbox.y = 0.0f;
            pos.y = hitbox.y + hitbox.h/2;
        }
    } else if (hitbox.y > map.y-hitbox.h) {
        Scene *lvl = game->currLevel->below;
        if (lvl != nullptr) {
            Vector2Int newMap = lvl->gridDimensions * lvl->cell_sideLen;
            Vector2 newPos(newMap.x/2, hitbox.h);
            game->movePlayerToLevel(lvl, newPos);
            return;
        } else {
            hitbox.y = map.y-hitbox.h;
            pos.y = hitbox.y+ hitbox.h/2;
        }
    }
}

void GameObject::defaultHandleSideCollisionsWithWalls()
{
    int sideLen = game->currLevel->cell_sideLen;
    Vector2Int numPoints(hitbox.w/sideLen, hitbox.h/sideLen);
    Vector2 step((float)hitbox.w/(numPoints.x+1), (float)hitbox.h/(numPoints.y+1));

    // top side move down
    for (int i = 1; i <= numPoints.x; i++) {
        // position of the current collision node
        Vector2 node(hitbox.x+(step.x*i), hitbox.y);
        // cell of the collision node
        Vector2Int cell(node.x/sideLen, node.y/sideLen);
        // if it's a barrier, set the position to just outside the barrier cell
        if (game->is_barrier(cell)) {
            hitbox.y = (cell.y+1)*sideLen; pos.y = hitbox.y + hitbox.h/2;
            // if the enetity does damage, damage the barrier it hit
            if (velocity.y != 0.0f) {
                game->damageCell(cell, damage);
                acceleration.y = velocity.y = 0.0f;
            }
        }
    }
    // bottom side, move up
    for (int i = 1; i <= numPoints.x; i++) {
        // position of the current collision node
        Vector2 node(hitbox.x+(step.x*i), hitbox.y+hitbox.h);
        // cell of the collision node
        Vector2Int cell(node.x/sideLen, node.y/sideLen);
        // if it's a barrier, set the position to just outside the barrier cell
        if (game->is_barrier(cell)) {
            hitbox.y = cell.y*sideLen - hitbox.h; pos.y = hitbox.y + hitbox.h/2;
            // if the enetity does damage, damage the barrier it hit
            if (velocity.y != 0.0f) {
                game->damageCell(cell, damage);
                acceleration.y = velocity.y = 0.0f;
            }
        }
    }
    // left side, move right
    for (int i = 1; i <= numPoints.y; i++) {
        // position of the current collision node
        Vector2 node(hitbox.x, hitbox.y+(step.y*i));
        // cell of the collision node
        Vector2Int cell(node.x/sideLen, node.y/sideLen);
        // if it's a barrier, set the position to just outside the barrier cell
        if (game->is_barrier(cell)) {
            hitbox.x = (cell.x+1)*sideLen; pos.x = hitbox.x + hitbox.w/2;
            // if the enetity does damage, damage the barrier it hit
            if (velocity.x != 0.0f) {
                game->damageCell(cell, damage);
                acceleration.x = velocity.x = 0.0f;
            }
        }
    }
    // right side, move left
    for (int i = 1; i <= numPoints.y; i++) {
        // position of the current collision node
        Vector2 node(hitbox.x+hitbox.w, hitbox.y+(step.y*i));
        // cell of the collision node
        Vector2Int cell(node.x/sideLen, node.y/sideLen);
        // if it's a barrier, set the position to just outside the barrier cell
        if (game->is_barrier(cell)) {
            hitbox.x = cell.x*sideLen - hitbox.w; pos.x = hitbox.x + hitbox.w/2;
            // if the enetity does damage, damage the barrier it hit
            if (velocity.x != 0.0f) {
                game->damageCell(cell, damage);
                acceleration.x = velocity.x = 0.0f;
            }
        }
    }
}


void GameObject::defaultHandleCornerCollisionsWithWalls()
{
    /*
    for each of the four corners: find the cell that the corner lies in
    if that cell is a barrier, then find the displacement
    between the hitbox corner itself, and the corner of the cell nearest to the 
    entity's centre. 

    this displacement vector represents how far into the barrier the entity is. 
    instead of just pushing the game object out with this vector directly, 
    find the smaller of the two terms, then push the object out ONLY by that term.

    so, if the x component of the displacement is 0.5, and the y component is -3,
    |0.5| < |-3|, so ONLY move the entity along the x axis by 0.5
    this will get the entity out of the wall hitbox, without causing it to interact
    with corners in an unintuitive way
    */

    /*
    Object::findCell returns indices of said cell. 
    findCell * sideLen is equal to the coordinates of the top left corner of the cell
    (findCell + (1, 1)) * sideLen is the coordinates of the bottom right corner

    'pos' member represents the position of the hitbox's top left corner. 'size' 
    represents the dimensions of the hitbox. pos + (size.x, size.y) is the position
    of the bottom right corner of the hitbox
    */

   int sideLen = game->currLevel->cell_sideLen;

    // top left
    Vector2 disp = Vector2_Zero;
    Vector2Int cell(hitbox.x/sideLen, hitbox.y/sideLen);
    if (game->is_barrier(cell)) {
        // displacement bettwen TOP LEFT corner of hitbox and BOTTOM RIGHT corner of cell
        disp = ((Vector2(cell.x,cell.y)+Vector2_One)*sideLen)-Vector2(hitbox.x,hitbox.y);
    }
    if (disp != Vector2_Zero) {
        bool flag = false;
        if (absf(disp.x)<absf(disp.y)) {
            hitbox.x += disp.x; pos.x = hitbox.x + hitbox.w/2;
            if (velocity.x != 0.0f) {
                flag = true;
                velocity.x = acceleration.x = 0.0f;
            }
        } else {
            hitbox.y += disp.y; pos.y = hitbox.y + hitbox.h/2;
            if (velocity.y != 0.0f) {
                flag = true;
                velocity.y = acceleration.y = 0.0f;
            }
        }
        if (flag) game->damageCell(cell, damage);
    }
    // top right
    disp = Vector2_Zero;
    cell = Vector2Int((hitbox.x+hitbox.w)/sideLen, hitbox.y/sideLen);
    if (game->is_barrier(cell)) {
        // displacement bettwen TOP LEFT corner of hitbox and BOTTOM RIGHT corner of cell
        disp = (Vector2(cell.x,cell.y+1)*sideLen)-Vector2(hitbox.x+hitbox.w,hitbox.y);
    }
    if (disp != Vector2_Zero) {
        bool flag = false;
        if (absf(disp.x)<absf(disp.y)) {
            hitbox.x += disp.x; pos.x = hitbox.x + hitbox.w/2;
            if (velocity.x != 0.0f) {
                flag = true;
                velocity.x = acceleration.x = 0.0f;
            }
        } else {
            hitbox.y += disp.y; pos.y = hitbox.y + hitbox.h/2;
            if (velocity.y != 0.0f) {
                flag = true;
                velocity.y = acceleration.y = 0.0f;
            }
        }
        if (flag) game->damageCell(cell, damage);
    }
    // bottom left
    disp = Vector2_Zero;
    cell = Vector2Int(hitbox.x/sideLen, (hitbox.y+hitbox.h)/sideLen);
    if (game->is_barrier(cell)) {
        // displacement bettwen TOP LEFT corner of hitbox and BOTTOM RIGHT corner of cell
        disp = (Vector2(cell.x+1,cell.y)*sideLen)-Vector2(hitbox.x,hitbox.y+hitbox.h);
    }
    if (disp != Vector2_Zero) {
        bool flag = false;
        if (absf(disp.x)<absf(disp.y)) {
            hitbox.x += disp.x; pos.x = hitbox.x + hitbox.w/2;
            if (velocity.x != 0.0f) {
                flag = true;
                velocity.x = acceleration.x = 0.0f;
            }
        } else {
            hitbox.y += disp.y; pos.y = hitbox.y + hitbox.h/2;
            if (velocity.y != 0.0f) {
                flag = true;
                velocity.y = acceleration.y = 0.0f;
            }
        }
        if (flag) game->damageCell(cell, damage);
    }
    // bottom right
    disp = Vector2_Zero;
    cell = Vector2Int((hitbox.x+hitbox.w)/sideLen, (hitbox.y+hitbox.h)/sideLen);
    if (game->is_barrier(cell)) {
        // displacement bettwen TOP LEFT corner of hitbox and BOTTOM RIGHT corner of cell
        disp = (Vector2(cell.x,cell.y)*sideLen)-Vector2(hitbox.x+hitbox.w,hitbox.y+hitbox.h);
    }
    if (disp != Vector2_Zero) {
        bool flag = false;
        if (absf(disp.x)<absf(disp.y)) {
            hitbox.x += disp.x; pos.x = hitbox.x + hitbox.w/2;
            if (velocity.x != 0.0f) {
                flag = true;
                velocity.x = acceleration.x = 0.0f;
            }
        } else {
            hitbox.y += disp.y; pos.y = hitbox.y + hitbox.h/2;
            if (velocity.y != 0.0f) {
                flag = true;
                velocity.y = acceleration.y = 0.0f;
            }
        }
        if (flag) game->damageCell(cell, damage);
    }
}