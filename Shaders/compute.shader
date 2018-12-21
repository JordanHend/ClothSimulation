#version 430 core

struct Particle
{
	vec4 pos;
	vec4 old_pos;
	vec4 acceleration;
	vec4 accumulated_normal;
};
struct Constraint
{
	uvec4 particles;
	vec4 rest_distance;
};


layout(std140, binding = 0) buffer cloth
{
	Particle particles[ ];	
};

layout(std430, binding = 1) buffer links
{
	Constraint constraints[ ];	
};

layout(std430, binding = 2) buffer ind
{
	uvec4 indices[ ];	
};

uniform vec3 force;
uniform int numParticles;
uniform int numConstraints;
uniform int numTriangles;
/* Some physics constants */
#define DAMPING 0.5 // how much to damp the cloth simulation each frame
#define TIME_STEPSIZE2 0.05 // how large time step each particle takes each frame
#define CONSTRAINT_ITERATIONS 25 
#define THICKNESS 0.20



int numRuns = 0;

void satisfyConstraint(Constraint t)
{

		uint p1ID = t.particles.x;
		uint p2ID = t.particles.y;
		vec3 p1 = particles[p1ID].pos.xyz;
		vec3 p2 = particles[p2ID].pos.xyz;

		vec3 p1_to_p2 = p2 - p1; 

		//Get distance from p1 to p2
		float current_distance = distance(p1, p2);

	
	float t0 =  (t.rest_distance.x / current_distance);

		//How much we need to resolve it by
		vec3 correctionVector = p1_to_p2*(1 - t0);

		//Divide it by two because we're moving both particles evenly
		vec3 correctionVectorHalf = correctionVector*0.5f; 



		if(particles[p1ID].pos.w == 1 )
		{
			particles[p1ID].pos += vec4(correctionVectorHalf, 0);

		}

		if(particles[p2ID].pos.w == 1)
		{
			particles[p2ID].pos -= vec4(correctionVectorHalf, 0);
		}
		
	
}


void TimeStep(uint pID)
{
	Particle p = particles[pID];
	if(particles[pID].pos.w == 1)
	{

	//Move particles position, and update old position.
	vec4 temp = p.pos;
	vec3 temppos = temp.xyz + ((temp.xyz - temp.xyz) * (1 - DAMPING)) + (particles[pID].acceleration.xyz * (TIME_STEPSIZE2));
	particles[pID].pos = vec4(temppos.xyz, temp.w);
	particles[pID].old_pos = temp;



	
	}
	//Reset acceleration
	particles[pID].acceleration = vec4(0,0,0,0);
}

//For wind force
vec3 calculateNormal(uint p1,uint p2,uint p3)
{
		vec3 pos1 = particles[p1].pos.xyz;
		vec3 pos2 = particles[p2].pos.xyz;
		vec3 pos3 = particles[p3].pos.xyz;

		vec3 v1 = pos2 - pos1;
		vec3 v2 = pos3 - pos1;

		return normalize(cross(v1, v2));
}


void addForce(uint pID, vec3 f)
{
	vec4 temp = vec4((f / 1), 0.0);
	particles[pID].acceleration += temp;

}



void addWindForcesForTriangle(uint p1,uint p2,uint p3, vec3 direction)
{

	//Calculate normal, and apply the force in that direction.
	vec3 normal = calculateNormal(p1,p2,p3);
	vec3 d = normalize(normal);

	vec3 force0 = normal * (dot(d, direction));
	addForce(p1, force0);
	addForce(p2, force0);
	addForce(p3, force0);
}



void windForce()
{

	for(uint i = 0; i < numTriangles; i+=3)
		addWindForcesForTriangle(indices[i].x, indices[i].y, indices[i].z, force);


}




void makeNormals()
{

    for( int i=0; i < numTriangles; i++ )
    {
        const uint ia = indices[i].x;
        const uint ib = indices[i].y;
        const uint ic = indices[i].z;

        const vec3 e1 = particles[ia].pos.xyz - particles[ib].pos.xyz;
        const vec3 e2 = particles[ic].pos.xyz - particles[ib].pos.xyz;
        const vec3 no = cross( e1, e2 );

        particles[ia].accumulated_normal += vec4(no, 0);
        particles[ib].accumulated_normal += vec4(no, 0);
        particles[ic].accumulated_normal += vec4(no, 0);
    }


}

//UNFINISHED FUNCTION--> I need a way to do this realistically... hard to make BSP on shader.
//Can get this to work, but too slow
void clothVcloth(uint i, uint j)
{
float d = distance(particles[i].pos.xyz, particles[j].pos.xyz);
	if(d > THICKNESS)
	{
	float amt = d - THICKNESS;
	particles[i].pos += vec4(amt / 2, amt / 2, amt / 2, 0);
	particles[j].pos -= vec4(amt / 2, amt / 2, amt / 2, 0);
	}

}

//Maybe could parallelize this better, but i just want to run it once.
layout (local_size_x = 1) in;
void main()
{
	
	for(unsigned int i = 0; i < numParticles; i++)
	{
		//Reset normal
		particles[i].accumulated_normal = vec4(0,0,0,0);

		//Add gravity
		addForce(i, vec3(0, -0.1, 0));

		//Move particles
		TimeStep(i);


	}

	//Recalculate normals for render pass
	makeNormals();

	//Apply force uniform as a wind force
	windForce();

	//And lastly, handle constraints
	for(unsigned int i = 0; i < numConstraints; i++)
		satisfyConstraint(constraints[i]);
			
	




}
