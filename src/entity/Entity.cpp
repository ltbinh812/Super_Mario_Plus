#include "Entity.h"

Entity::Entity(Vector2 pos, Vector2 boxsize)
        :position(pos), prevPosition(pos) ,velocity{0, 0}, boxSize(boxsize) {}

void Entity::addForce(Vector2 force) {
    velocity.x += force.x;
    velocity.y += force.y;
}