#include "DIYPhysicsEngine.h"
//rigid body functions
using namespace std;

typedef CollisionManifold (*fn)(DIYPhysicScene*, PhysicsObject*, PhysicsObject*);

static fn FunctionPointerTable[] =
{
    0,                              DIYPhysicScene::Plane2Sphere,   DIYPhysicScene::Plane2Box,
    DIYPhysicScene::Sphere2Plane,   DIYPhysicScene::Sphere2Sphere,  DIYPhysicScene::Sphere2Box,
    DIYPhysicScene::Box2Plane,      DIYPhysicScene::Box2Sphere,     DIYPhysicScene::Box2Box
};


void DIYPhysicScene::checkForCollisions()
{
    int actor_count = actors.size();

    for (int first_actor = 0;
        first_actor < actor_count - 1;
        ++first_actor)
    {
        for (int second_actor = first_actor + 1;
            second_actor < actor_count;
            ++second_actor)
        {
            PhysicsObject * object1 = actors[first_actor];
            PhysicsObject * object2 = actors[second_actor];
            
            int shapeid1 = object1->_shapeID;
            int shapeid2 = object2->_shapeID;

            int index = (shapeid1 * NUMBERSHAPE) + shapeid2;

            fn collision_function = FunctionPointerTable[index];

            if (collision_function != nullptr)
            {
                CollisionManifold manifold = collision_function(this, object1, object2);

                if (manifold.colliding)
                {
                    //ADDED THIS
                    if ( manifold.first->is_static && manifold.second && manifold.second->is_static )
                    {
                        continue;
                    }

                    Gizmos::add2DCircle(manifold.P, 0.5f, 16, glm::vec4(1, 1, 0, 1));
                    Gizmos::add2DLine(manifold.P, manifold.P + manifold.N * 5.0f, glm::vec4(1, 1, 0, 1));

                    //CHAGED THESE TWO LINES. LOOK AT THEM CLOSELY
                    float inv_mass1 = (!manifold.first->is_static) ? 1.0f / manifold.first->mass : 0;
                    float inv_mass2 = (manifold.second && !manifold.second->is_static) 
                                        ? 1.0f / manifold.second->mass : 0;

                    
                    float inv_moi1 = (!manifold.first->is_static) ? 1.0f / manifold.first->moment_of_inertia : 0;
                    float inv_moi2 = (manifold.second && !manifold.second->is_static)
                                        ? 1.0f / manifold.second->moment_of_inertia : 0;

                    glm::vec2 com1 = manifold.first->position;
                    glm::vec2 com2 = manifold.second ? manifold.second->position : manifold.P;

                    glm::vec2 R_1p = manifold.P - manifold.first->position;
                    R_1p = glm::vec2(-R_1p.y, R_1p.x);

                    glm::vec2 R_2p = manifold.P - (manifold.second ? manifold.second->position : manifold.P);
                    R_2p = glm::vec2(-R_2p.y, R_2p.x);

                    glm::vec2 velocity1 = manifold.first->velocity + manifold.first->angular_velocity * R_1p;
                    
                    glm::vec2 velocity2;
                    if (manifold.second)
                    {
                        velocity2 = manifold.second->velocity + manifold.second->angular_velocity * R_2p;
                    }

                    float R_1p_dot_n = glm::dot(R_1p, manifold.N);
                    float R_2p_dot_n = glm::dot(R_2p, manifold.N);

                    float denom = glm::dot(manifold.N, manifold.N * (inv_mass1 + inv_mass2)) +
                        R_1p_dot_n * R_1p_dot_n * inv_moi1 + R_2p_dot_n * R_2p_dot_n * inv_moi2;

                    float j = (-(1 + manifold.e) * glm::dot(velocity1 - velocity2, manifold.N)) / denom;



                    if (!manifold.first->is_static) //added this if
                    {
                        manifold.first->velocity += (j * inv_mass1) * manifold.N;
                        manifold.first->angular_velocity += glm::dot(R_1p, j * manifold.N) * inv_moi1;
                    }
                                            //Added this second check
                    if (manifold.second && !manifold.second->is_static)
                    {
                        manifold.second->velocity += (-j * inv_mass2) * manifold.N;
                        manifold.second->angular_velocity += glm::dot(R_2p, -j * manifold.N) * inv_moi2;
                    }
                    
                    glm::vec2 tangent(manifold.N.y, -manifold.N.x);

                    float R_1p_dot_t = glm::dot(R_1p, tangent);
                    float R_2p_dot_t = glm::dot(R_2p, tangent);

                    float friction_denom =
                        inv_mass1 + inv_mass2 + (R_1p_dot_t*R_1p_dot_t) * inv_moi1 + (R_2p_dot_t*R_2p_dot_t) * inv_moi2;
                    
                    float friction_j = (-(1.1f) * glm::dot(velocity1 - velocity2, tangent)) / friction_denom;


                    /*if (!manifold.first->is_static) //added this if
                    {
                        manifold.first->velocity += (friction_j * inv_mass1) * tangent;
                        manifold.first->angular_velocity += glm::dot(R_1p, friction_j * tangent) * inv_moi1;
                    }
                                            //Added this second check
                    if (manifold.second && !manifold.second->is_static)
                    {
                        manifold.second->velocity += (-friction_j * inv_mass2) * tangent;
                        manifold.second->angular_velocity += glm::dot(R_2p, -friction_j * tangent) * inv_moi2;
                    }*/

                }
            }
        }
    }
}   
 

//plane class functions
PlaneClass::PlaneClass(glm::vec2 normal,float distance)
{
	this->normal = glm::normalize(normal);
	this->distance = distance;
	_shapeID = PLANE;
}

void PlaneClass::makeGizmo()
{
	float lineSegmentLength = 100;
	glm::vec2 centrePoint = normal * distance;
	glm::vec2 parallel = glm::vec2(normal.y,-normal.x); //easy to rotate normal through 90degrees around z
	glm::vec4 colour(1,1,1,1);
	glm::vec2 start = centrePoint + (parallel * lineSegmentLength);
	glm::vec2 end = centrePoint - (parallel * lineSegmentLength);
	Gizmos::add2DLine(start.xy(),end.xy(),colour);
}

//sphere class functions

SphereClass::SphereClass(	glm::vec2 position,glm::vec2 velocity,float radius,float mass,glm::vec4& colour)
	: DIYRigidBody(position,velocity,0,mass)  //call the base class constructor
{
	this->_radius = radius;
	this->colour = colour;
    this->moment_of_inertia = (this->mass * this->_radius * this->_radius) / 2.0f;
	std::cout<<"adding sphere "<<this->position.x<<','<<this->position.y<<std::endl;
	_shapeID = SPHERE;
}

SphereClass::SphereClass(	glm::vec2 position,float angle,float speed,float radius,float mass,glm::vec4& colour)
		: DIYRigidBody(position,glm::vec2(speed * cos(angle),speed * sin(angle)),0,mass)  //call the base class constructor
{
	this->_radius = radius;
	this->colour = colour;
    this->moment_of_inertia = (this->mass * this->_radius * this->_radius) / 2.0f;
	std::cout<<"adding sphere "<<this->position.x<<','<<this->position.y<<std::endl;
	_shapeID = SPHERE;
}

void SphereClass::makeGizmo()
{
	glm::vec2 center = position.xy();
	Gizmos::add2DCircle(center, _radius,30, colour);
}

//box class functions

BoxClass::BoxClass(	glm::vec2 position,glm::vec2 velocity,float rotation,float mass,float width, float height,glm::vec4& colour)
	: DIYRigidBody(position,velocity,rotation,mass)  //call the base class constructor
{
	this->width = width;
	this->height = height;
	this->colour = colour;
    this->is_colliding = false;

    float h = 2 * height;
    float w = 2 * width;
    this->moment_of_inertia = mass * (h * h + w * w) / 12;

	_shapeID = BOX;
}

BoxClass::BoxClass(	glm::vec2 position, float angle, float speed, float rotation, float width, float height, float mass, glm::vec4& colour)
	: DIYRigidBody(position,glm::vec2(speed * cos(angle),speed * sin(angle)),rotation,mass)  //call the base class constructor
{
	this->width = width;
	this->height = height;
    this->colour = colour;

    float h = 2 * height;
    float w = 2 * width;
    this->moment_of_inertia = mass * (h * h + w * w) / 12;

    this->is_colliding = false;

	_shapeID = BOX;
}

void BoxClass::makeGizmo()
{
    //Added all this
    glm::vec4 my_colour(1, 0, 0, 1);
    if (this->is_colliding)
    {
        my_colour = glm::vec4(0, 1, 0, 1);
    }
                                                                
    Gizmos::add2DAABBFilled(position, glm::vec2(width, height), my_colour, &rotationMatrix);
                                                               //  ^ added the my_
}

bool BoxClass::isPointOver(glm::vec2 point)
{
    glm::vec2 rel_point = point - position;

    float ct = cosf(-rotation2D);
    float st = sinf(-rotation2D);

    rel_point = glm::vec2(ct * rel_point.x - st * rel_point.y,
                          st * rel_point.x + ct * rel_point.y);


    bool result = rel_point.x > -width && rel_point.x < width
                && rel_point.y > -height && rel_point.y < height;


    return result;
}

DIYRigidBody::DIYRigidBody(	glm::vec2 position,glm::vec2 velocity,float rotation,float mass) 
{
	std::cout<<"adding rigid body "<<position.x<<','<<position.y<<std::endl;
	this->position = position;
	this->velocity = velocity;
	this->rotation2D = rotation;
    
    //ADDED THIS
    this->is_static = false;

    //added these 3 lines
    this->angular_velocity = 0;
    this->total_torque = 0;
    this->total_force = glm::vec2();

	this->mass = mass;
    this->dynamic_friction = 0.2f;
	colour = glm::vec4(1,1,1,1); //white by default
}

void DIYRigidBody::applyForceAtPoint(glm::vec2 force, glm::vec2 point)
{
    total_force += force;
    glm::vec2 cm_to_point = point - this->position;

    glm::vec2 perp_to_point(-cm_to_point.y, cm_to_point.x);
    total_torque += glm::dot(perp_to_point, force);
}


void DIYRigidBody::applyForce(glm::vec2 force)
{
    applyForceAtPoint(force, this->position);
}
void DIYRigidBody::applyForceToActor(DIYRigidBody* actor2, glm::vec2 force)
{
	actor2->applyForce(force);
	applyForce(-force);
}

void DIYRigidBody::collisionResponse(glm::vec2 collisionPoint)
{
	// for now we'll just set the velocity to zero
	collisionPoint = collisionPoint;
}

void DIYRigidBody::update(glm::vec2 gravity,float timeStep)
{
    if (this->is_static)
    {
        return;
    }

    //removed  * timeStep
	applyForce(gravity * mass);

    glm::vec2 friction_force = 
        -this->mass * this->dynamic_friction * this->velocity;
    applyForce(friction_force);
       
    float friction_torque = 
        (-moment_of_inertia * this->dynamic_friction * angular_velocity);
    total_torque += friction_torque;

    glm::vec2 acceleration = (total_force / mass);
    glm::vec2 delta_v = acceleration * timeStep;

    float angular_accel = total_torque / moment_of_inertia;
    float delta_w = angular_accel * timeStep;

	oldPosition = position; //keep our old position for collision response
	

    velocity += delta_v;
    position += velocity * timeStep;

    rotation2D += angular_velocity * timeStep;
    angular_velocity += delta_w;

	rotationMatrix = glm::rotate(rotation2D,glm::vec3(0.0f,0.0f,1.0f)); 

    total_force = glm::vec2();
    total_torque = 0;
}

void DIYRigidBody::debug()
{
	cout<<"position "<<position.x<<','<<position.y<<endl;
}

//scene functions

void DIYPhysicScene::addActor(PhysicsObject* object)
{
	actors.push_back(object);
}
	
void DIYPhysicScene::removeActor(PhysicsObject* object)
{
	auto item = std::find(actors.begin(), actors.end(), object);
	if(item < actors.end())
	{
		actors.erase(item);
	}
}

//added these two functions
void DIYPhysicScene::addJoint(Joint* object)
{
    joints.push_back(object);
}

void DIYPhysicScene::removeJoint(Joint* object)
{
    auto item = std::find(joints.begin(), joints.end(), object);
    if (item < joints.end())
    {
        joints.erase(item);
    }
}


void DIYPhysicScene::upDate()
{
	bool runPhysics = true;
	int maxIterations = 10; //emergency count to stop us repeating for ever in extreme situations

	for(auto actorPtr:actors)
	{
		actorPtr->update(gravity,timeStep);
    }

    //ADDED THIS FOR LOOP
    for (auto jointPtr : joints)
    {
        jointPtr->Update(timeStep);

    }


    if (collisionEnabled)
    {
        checkForCollisions();
    }

	maxIterations--;
}

void DIYPhysicScene::debugScene()
{
	int count = 0;
	for (auto actorPtr:actors)
	{
		cout<<count<<" : ";
		actorPtr->debug();
		count++;
	}
}

void DIYPhysicScene::upDateGizmos()
{
    for (auto actorPtr : actors)
    {
        actorPtr->makeGizmo();
    }

    //ADDED THIS FOR LOOP
    for (auto jointPtr : joints)
    {
        jointPtr->DrawGizmo();
    }
}

CollisionManifold DIYPhysicScene::Sphere2Sphere(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
    SphereClass * first_sphere = (SphereClass*)first;
    SphereClass * second_sphere = (SphereClass*)second;

    //get the vector from the first to the second sphere
    glm::vec2 delta = second_sphere->position - first_sphere->position;
    //the length of the delta is the distance
    float distance = glm::length(delta);
    float raddii_sum = first_sphere->_radius + second_sphere->_radius;

    CollisionManifold result = {};
    result.first = first_sphere;
    result.second = second_sphere;
    result.colliding = false;

    if (distance < raddii_sum)
    {
        
        glm::vec2 collision_normal = glm::normalize(delta);
        
                                                    //added this multiply
        float intersection = (raddii_sum - distance) * 0.5f;

        //added the following 3 if checks
        if (first_sphere->is_static || second_sphere->is_static)
            intersection *= 2;

        //glm::vec2 rel_vel = 

        if ( !first_sphere->is_static )
            first_sphere->position  -= intersection * collision_normal;
        
        if (!second_sphere->is_static)
            second_sphere->position += intersection * collision_normal;
        
        result.e = 0.95f;
        result.N = collision_normal;
        result.P = first_sphere->position + collision_normal * first_sphere->_radius;

        result.colliding = true;
    }

    return result;
}



CollisionManifold DIYPhysicScene::Sphere2Plane(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
    SphereClass* sphere = (SphereClass*)first;
    PlaneClass* plane = (PlaneClass*)second;

    float perpendicular_distance = 
        glm::dot(sphere->position, plane->normal) - plane->distance;

    CollisionManifold result = {};
    result.colliding = false;
    result.first = sphere;
    result.second = nullptr;

    if (perpendicular_distance < sphere->_radius)
    {
        float intersection = sphere->_radius - perpendicular_distance;
        sphere->position += plane->normal * intersection;
        
        result.colliding = true;
        result.N = plane->normal;
        result.e = 0.75f;
        result.P = sphere->position - plane->normal * sphere->_radius;
    }

    return result;
}

CollisionManifold DIYPhysicScene::Plane2Sphere(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
    return Sphere2Plane(scene, second, first);
}

void BuildBoxPoints(BoxClass* box, glm::vec2* points)
{
    points[0] =
        (box->rotationMatrix *
        glm::vec4(-box->width, -box->height, 0, 1)).xy()
        + box->position;
    points[1] =
        (box->rotationMatrix *
        glm::vec4(-box->width, box->height, 0, 1)).xy()
        + box->position;
    points[2] =
        (box->rotationMatrix *
        glm::vec4(box->width, box->height, 0, 1)).xy()
        + box->position;
    points[3] =
        (box->rotationMatrix *
        glm::vec4(box->width, -box->height, 0, 1)).xy()
        + box->position;
}


CollisionManifold DIYPhysicScene::Box2Box(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
    BoxClass* first_box = (BoxClass*)first;
    BoxClass* second_box = (BoxClass*)second;

    glm::vec2 points[8];

    BuildBoxPoints(first_box, points);
    BuildBoxPoints(second_box, points + 4);

    glm::vec2* first_points = points;
    glm::vec2* second_points = points + 4;

    CollisionManifold result = {};
    result.colliding = false;
    result.first = first_box;
    result.second = second_box;
    result.e = 0.75f;
    
    glm::vec2 smallest_normal;
    float smallest_overlap = FLT_MAX;

    for (int box_index = 0;
        box_index < 2;
        ++box_index)
    {
        for (int point_index = 0;
            point_index < 3;
            ++point_index)
        {
            glm::vec2 edge_vector = first_points[point_index] - first_points[point_index + 1];
            edge_vector = glm::normalize(edge_vector);
            glm::vec2 perp_vector(edge_vector.y, -edge_vector.x);

            float first_min = FLT_MAX, first_max = -FLT_MAX;
            float second_min = FLT_MAX, second_max = -FLT_MAX;

            glm::vec2 first_min_point, first_max_point;
            glm::vec2 second_min_point, second_max_point;

            for (int check_index = 0;
                check_index < 4;
                ++check_index)
            {
                float first_projected = glm::dot(first_points[check_index], perp_vector);

                if (first_projected < first_min)
                {
                    first_min = first_projected;
                    first_min_point = first_points[check_index];
                }
                if (first_projected > first_max)
                {
                    first_max = first_projected;
                    first_max_point = first_points[check_index];
                }

                float second_projected = glm::dot(second_points[check_index], perp_vector);

                if (second_projected < second_min)
                {
                    second_min = second_projected;
                    second_min_point = second_points[check_index];
                }
                if (second_projected > second_max)
                {
                    second_max = second_projected;
                    second_max_point = second_points[check_index];
                }
            }

            if (first_min > second_max || second_min > first_max)
            {
                return result;
            }
            else
            {
                float max1_min0 = second_max - first_min;
                float max0_min1 = first_max - second_min;

                float overlap = glm::min(max1_min0, max0_min1);

                if (overlap < smallest_overlap)
                {
                    result.N = perp_vector;
                    //result.P = 
                }
            }
        }

        first_points = points + 4;
        second_points = points;
    }

    //if we made it this far and did not return yet, that means they are colliding
    
    first_box->is_colliding = true;
    second_box->is_colliding = true;

    return result;
}

CollisionManifold DIYPhysicScene::Box2Plane(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
    BoxClass* box = (BoxClass*)first;
    PlaneClass* plane = (PlaneClass*)second;

    glm::vec2 points[4];
    BuildBoxPoints(box, points);

    CollisionManifold result = {};
    result.colliding = false;
    result.e = 0.75f;
    result.first = box;
    result.second = nullptr;
    
    float lowest_distance = 0;

    for ( int point_index = 0; point_index < 4; ++point_index )
    {
        float distance = glm::dot(plane->normal, points[point_index]) - plane->distance;

        if (distance < 0)
        {
            result.colliding = true;
            result.N = plane->normal;

            if (distance < lowest_distance)
            {
                lowest_distance = distance;
                result.P = points[point_index] - plane->normal * lowest_distance;
            }
        }
    }

    if (result.colliding)
    {
        box->position -= plane->normal * lowest_distance;
    }

    return result;
}

CollisionManifold DIYPhysicScene::Plane2Box(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
    return Box2Plane(scene, second, first);
}

CollisionManifold DIYPhysicScene::Box2Sphere(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
    BoxClass* box = (BoxClass*)first;
    SphereClass* sphere = (SphereClass*)second;

    glm::vec2 vector_to_circle = sphere->position - box->position;

    float sin_theta = sinf(-box->rotation2D);
    float cos_theta = cosf(-box->rotation2D);

    vector_to_circle = glm::vec2(cos_theta * vector_to_circle.x - sin_theta * vector_to_circle.y,
                                 sin_theta * vector_to_circle.x + cos_theta * vector_to_circle.y);

    float dist_sq = 0;
    if (vector_to_circle.x > box->width ) // if the circle is to the right
    {
        float dist = vector_to_circle.x - box->width;
        dist_sq += dist * dist;
    }
    else if (vector_to_circle.x < -box->width) //else if the circle is to the left
    {
        float dist = vector_to_circle.x + box->width;
        dist_sq += dist * dist;
    }

    if (vector_to_circle.y > box->height ) // if the circle is above
    {
        float dist = vector_to_circle.y - box->height;
        dist_sq += dist * dist;
    }
    else if (vector_to_circle.y < -box->height) // else if the circle is below
    {
        float dist = vector_to_circle.y + box->height;
        dist_sq += dist * dist;
    }

    if ((sphere->_radius * sphere->_radius) > dist_sq)
    {
        box->is_colliding = true;
        
        CollisionManifold result = {};
        return result;

    }

    CollisionManifold result = {};
    return result;
}

CollisionManifold DIYPhysicScene::Sphere2Box(DIYPhysicScene* scene, PhysicsObject* first, PhysicsObject* second)
{
    return Box2Sphere(scene, second, first);
}

SpringJoint::SpringJoint(DIYRigidBody* a_bodyA, DIYRigidBody* a_bodyB,
                            float a_k, float a_d, float a_resting_distance)
{
    this->bodyA = a_bodyA;
    this->bodyB = a_bodyB;
    this->k = a_k;
    this->d = a_d;
    this->resting_distance = a_resting_distance;
}

void SpringJoint::Update(float delta_time)
{
    if (!bodyA || !bodyB)
    {
        return;
    }

    glm::vec2 diff_vector = bodyA->position - bodyB->position;

    float distance = glm::length(diff_vector);

    glm::vec2 force = -k * (distance - resting_distance) 
                                * glm::normalize(diff_vector);

    if ( !bodyA->is_static )
        bodyA->applyForce( force - this->d * bodyA->velocity );
    
    if ( !bodyB->is_static )
        bodyB->applyForce( -force - this->d * bodyB->velocity );
}

void SpringJoint::DrawGizmo()
{
    Gizmos::add2DLine(bodyA->position, bodyB->position, glm::vec4(0, 0.8f, 0.8f, 1));
}
