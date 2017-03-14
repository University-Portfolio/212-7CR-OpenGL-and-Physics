#include "Collider.h"
#include "PhysicsScene.h"
#include "Body.h"
#include <glm.hpp>


Collider::Collider()
{
}

void Collider::Begin()
{
	game_object = Cast<GameObject3D>(GetParent());
	attached_body = game_object->GetComponent<Body>();
}

void Collider::AddToScene(PhysicsScene* physics_scene) 
{
	this->physics_scene = physics_scene;
}

void Collider::DefaultResolution(Body* body, HitInfo& hit_info)
{
	const glm::vec3 correction = hit_info.normal * -hit_info.distance;
	body->velocity += correction;
	//body->angular_velocity += xyz;
	//body->ApplyFriction();

	//Call appropriate functions
	GameObject* body_parent = Cast<GameObject>(body->GetParent());
	if (body_parent)
		body_parent->OnCollide(this, hit_info);
	if (game_object)
		game_object->OnCollide(body->GetCollider(), -hit_info);

	//Apply appropriate force
	if (attached_body)
		attached_body->ApplyForce(body->mass * -correction);
}

bool Collider::DoesBroadPhaseCollide(Collider* collider, glm::vec3 velocity) 
{
	/* Bounding sphere */
	const float distance = glm::distance(GetWorldLocation(), collider->GetWorldLocation() + velocity);

	//Force no collision, if at exactly same location
	if (distance != 0.0f)
		return distance <= bounding_radius + collider->bounding_radius;
	else
		return false;
}