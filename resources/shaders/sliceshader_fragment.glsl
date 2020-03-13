#version 450 core

in vec3 position;
in vec4 color;
in vec3 normal;
in vec3 texcoord;

out vec4 fcolor;

// uniform sampler3D tex;
uniform layout(rgba8) image3D current;  // this shader always reads from current
uniform layout(rgba8) image3D next;
uniform float scale;


uniform vec3 texture_offset;
uniform float uniform_scale;




// #define NUM_STEPS 2000
#define NUM_STEPS 600

#define MIN_DISTANCE 0.0
#define MAX_DISTANCE 1000.0

uniform float t;




uniform vec2 rotation;
uniform vec3 location;

//thanks to Neil Mendoza via http://www.neilmendoza.com/glsl-rotation-about-an-arbitrary-axis/
mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}





// hit() code adapted from:
//
//    Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
//    "An Efficient and Robust Ray-Box Intersection Algorithm"
//    Journal of graphics tools, 10(1):49-54, 2005



double t0 = MIN_DISTANCE;
double t1 = MAX_DISTANCE;

double tmin, tmax; //global scope, set in hit() to tell min and max parameters

vec3 gorg = vec3(0);
vec3 gdir = vec3(0);

bool hit(vec3 ray_org, vec3 dir)
{
  //the bounding box is from 000 to 111 or maybe -1 instead of 0
  // vec3 min = vec3(-1,-1,-1);
  vec3 min = vec3(0,0,0);
  vec3 max = vec3(1,1,1);

  int sign[3];

  vec3 inv_direction = vec3(1/dir.x, 1/dir.y, 1/dir.z);

	sign[0] = (inv_direction[0] < 0)?1:0;
  sign[1] = (inv_direction[1] < 0)?1:0;
	sign[2] = (inv_direction[2] < 0)?1:0;

  vec3 bbox[2] = {min,max};

  tmin = (bbox[sign[0]][0] - ray_org[0]) * inv_direction[0];
  tmax = (bbox[1-sign[0]][0] - ray_org[0]) * inv_direction[0];

  double tymin = (bbox[sign[1]][1] - ray_org[1]) * inv_direction[1];
  double tymax = (bbox[1-sign[1]][1] - ray_org[1]) * inv_direction[1];

  if ( (tmin > tymax) || (tymin > tmax) )
    return false;
  if (tymin > tmin)
    tmin = tymin;
  if (tymax < tmax)
    tmax = tymax;

  double tzmin = (bbox[sign[2]][2] - ray_org[2]) * inv_direction[2];
  double tzmax = (bbox[1-sign[2]][2] - ray_org[2]) * inv_direction[2];

  if ( (tmin > tzmax) || (tzmin > tmax) )
    return false;
  if (tzmin > tmin)
    tmin = tzmin;
  if (tzmax < tmax)
    tmax = tzmax;
  return ( (tmin < t1) && (tmax > t0) );

  return true;
}

vec4 get_color_for_pixel()
{
  // return vec4(tmax-tmin,0,1,1);

  float current_t = float(tmax); //start at the farthest point into the texture
  vec4 t_color = vec4(0.396,0.396,0.4,1.0);
  float step = float((tmax-tmin))/NUM_STEPS;

  if(step < 0.001) step = 0.001;




  vec3 texture_scale = uniform_scale * vec3(0.5,1.0,1.0);



  vec4 new_read, old_read;

  // old_read = new_read = texture(current,texture_scale*(gorg+current_t*gdir+texture_offset)); //this is with the sampler

  //----------------------------------------------------------------------------

  int sx, sy, sz;

  sz = int(imageSize(current).x * (texture_scale*(gorg+current_t*gdir+texture_offset)).x);
  sy = int(imageSize(current).y * (texture_scale*(gorg+current_t*gdir+texture_offset)).y);
  sx = int(imageSize(current).z * (texture_scale*(gorg+current_t*gdir+texture_offset)).z);

  ivec3 samp = ivec3(sx, sy, sz);
  old_read = new_read = imageLoad(current,samp);  //this is with the image

  //----------------------------------------------------------------------------

  for(int i = 0; i < NUM_STEPS; i++)
  {
    if(current_t>=tmin)
    {
      current_t -= step;

      old_read = new_read;


      // new_read = texture(current,texture_scale*(gorg+current_t*gdir+texture_offset)); //this is with the sampler

      //----------------------------------------------------------------------------

      sx = int(imageSize(current).x * (texture_scale*(gorg+current_t*gdir+texture_offset)).x);
      sy = int(imageSize(current).y * (texture_scale*(gorg+current_t*gdir+texture_offset)).y);
      sz = int(imageSize(current).z * (texture_scale*(gorg+current_t*gdir+texture_offset)).z);

      samp = ivec3(sx, sy, sz);
      new_read = imageLoad(current,samp); //this is with the image

      //----------------------------------------------------------------------------


      // it's a over b, where a is the new sample and b is the current color, t_color
      t_color.rgb = new_read.rgb * new_read.a + t_color.rgb * t_color.a * ( 1 - new_read.a );
      t_color.a = new_read.a + t_color.a * ( 1 - new_read.a );

    }
    else
    {

    }
  }
  return t_color;
}

void main()
{

  fcolor = color;
  fcolor = vec4(0.396,0.396,0.4,1.0);

  mat4 rot = rotationMatrix(vec3(0,1,0),rotation.x) * rotationMatrix(vec3(1,0,0),rotation.y);


  gorg = (rot * vec4(scale * position,0)).xyz+location;
  gdir = (rot * vec4(normal,0.0)).xyz;

  if(hit(gorg,gdir))
    fcolor = get_color_for_pixel();  //this assumes tmin, tmax defined by hit(org,dir)
  else
    discard;


  // the ray origin will be determined by the location of the viewer,
  // specified in the value of location, and for each pixel a displacement
  // will be figured out using the texture coordinate

  // the ray direction will be determined by using the value of direction,
  // potentially displaced by up and right, to do something like perspective
  // projection - another thing to consider is you might scale the contribution
  // of left and right based on the squared distance of the pixel's xy value
  // from the center, i.e. modeling fisheye effects.


}
