#version 450 core

uniform layout(rgba8) image3D current;
uniform layout(rgba8) image3D next;

//electron head is yellow
bool is_electron_head(ivec4 i)
{
	return i.r > 253 && i.g > 253 && i.b < 2 && i.a > 253;
}

//electron tail is cyan
bool is_electron_tail(ivec4 i)
{
	return i.r < 2 && i.g > 253 && i.b > 253 && i.a > 253;
}

//conductor is black
bool is_conductor(ivec4 i)
{
	return i.r < 2 && i.g < 2 && i.b < 2 && i.a > 253;
}





void main()
{

	vec3 offsets[8];

	int my_index = gl_VertexID;

	// integer division/mod to get index
	ivec3 sample_location = ivec3(my_index%512, (my_index/512)%256, my_index/(256*512));


// == conductor

	ivec4 data[19];

	//the location of voxel being considered
	data[0] = ivec4(imageLoad(current,sample_location)*vec4(256));

	//neighborhood for the wireworld simulation
	int p = 1;
	int n = -1;
	int e_count = 0;









	//we consider everything but the corners
	if(is_conductor(data[0]))
	{
		data[ 1] = ivec4(imageLoad(current,sample_location+ivec3(p,0,0))*vec4(256));   // +x  0  0
		data[ 2] = ivec4(imageLoad(current,sample_location+ivec3(n,0,0))*vec4(256));   // -x  0  0
		data[ 3] = ivec4(imageLoad(current,sample_location+ivec3(0,p,0))*vec4(256));   // 0  +y  0
		data[ 4] = ivec4(imageLoad(current,sample_location+ivec3(0,n,0))*vec4(256));   // 0  -y  0
		data[ 5] = ivec4(imageLoad(current,sample_location+ivec3(0,0,p))*vec4(256));   // 0  0  +z
		data[ 6] = ivec4(imageLoad(current,sample_location+ivec3(0,0,n))*vec4(256));   // 0  0  -z
		data[ 7] = ivec4(imageLoad(current,sample_location+ivec3(p,p,0))*vec4(256));   // +x +y  0
		data[ 8] = ivec4(imageLoad(current,sample_location+ivec3(p,n,0))*vec4(256));   // +x -y  0
		data[ 9] = ivec4(imageLoad(current,sample_location+ivec3(n,p,0))*vec4(256));   // -x +y  0
		data[10] = ivec4(imageLoad(current,sample_location+ivec3(n,n,0))*vec4(256));   // -x -y  0
		data[11] = ivec4(imageLoad(current,sample_location+ivec3(p,0,p))*vec4(256));   // +x 0  +z
		data[12] = ivec4(imageLoad(current,sample_location+ivec3(p,0,n))*vec4(256));   // +x 0  -z
		data[13] = ivec4(imageLoad(current,sample_location+ivec3(n,0,p))*vec4(256));   // -x 0  +z
		data[14] = ivec4(imageLoad(current,sample_location+ivec3(n,0,n))*vec4(256));   // -x 0  -z
		data[15] = ivec4(imageLoad(current,sample_location+ivec3(0,p,p))*vec4(256));   // 0 +y  +z
		data[16] = ivec4(imageLoad(current,sample_location+ivec3(0,p,n))*vec4(256));   // 0 +y  -z
		data[17] = ivec4(imageLoad(current,sample_location+ivec3(0,n,p))*vec4(256));   // 0 -y  +z
		data[18] = ivec4(imageLoad(current,sample_location+ivec3(0,n,n))*vec4(256));   // 0 -y  -z

		//do I have 1 or 2 electron neighbors?

		e_count = 0;

		for(int i = 1; i < 19; i++)
		{
			if(is_electron_head(data[i]))
			{
				e_count++;
			}
		}

		if(e_count == 1 || e_count == 2)
		{
			imageStore(next, sample_location, ivec4(255,255,0,255));	//goes to electron head
		}
		else
		{
			imageStore(next,sample_location, ivec4(0,0,0,255));		//conductor to conductor
		}



	}
	else if(is_electron_head(data[0])) //goes to electron tail cyan
	{
		imageStore(next,sample_location, ivec4(0,255,255,255));
		// imageStore(current,sample_location, ivec4(0,255,255,255));
	}
	else if(is_electron_tail(data[0])) //goes back to conductor
	{
		imageStore(next,sample_location, ivec4(0,0,0,255));
		// imageStore(current,sample_location, ivec4(0,0,0,255));
	}
}
