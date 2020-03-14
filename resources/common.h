#ifndef COMMON_H
#define COMMON_H

#include <random>
#include <vector>
#include <iostream>
using std::cout;
using std::endl;

#define POINT_SPRITE_PATH "resources/textures/height/sphere_small.png"

#define WATER_HEIGHT_TEXTURE "resources/textures/height/wave_height.png"
#define WATER_NORMAL_TEXTURE "resources/textures/normal/wave_norm.png"
#define WATER_COLOR_TEXTURE "resources/textures/water_color.png"

#define ROCK_NORMAL_TEXTURE "resources/textures/normals/rock_norm.png"
#define ROCK_HEIGHT_TEXTURE "resources/textures/height/rock_height.png"


//************************************************


// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

//SDL includes - windowing, gl context, system info
#include <SDL.h>
//allows you to run OpenGL inside of SDL2
#include <SDL_opengl.h>

// GLUT
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

// Shader Compilation
#include "shaders/Shader.h"

//**********************************************

#include "../resources/LodePNG/lodepng.h"
// Good, simple png library

#include "../resources/perlin.h"
//perlin noise generation

//**********************************************

// #define GLM_MESSAGES
// #define GLM_SWIZZLE
#define GLM_FORCE_SWIZZLE
#define GLM_SWIZZLE_XYZW
#include "glm/glm.hpp" //general vector types
// #include "glm/gtc/swizzle.hpp"    //swizzling
#include "glm/gtc/matrix_transform.hpp" // for glm::ortho
#include "glm/gtc/type_ptr.hpp" //to send matricies gpu-side
#include "glm/gtx/transform.hpp"

//**********************************************

namespace JonDefault{

    glm::mat4 view = glm::lookAt(
        glm::vec3(3.0f, 3.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // glm::mat4 proj = glm::perspective(glm::radians(65.0f), 1080.0f / 1920.0f, 0.25f, 6.0f);
    glm::mat4 proj = glm::perspective(glm::radians(65.0f), 1920.0f / 1080.0f, 0.25f, 6.0f);

    double twopi = 2*3.1415926535;

}

//******************************************************************************
//  Function: planetest
//
//  NOTE: This comes up with some frequency. I'll be carrying this along for a while.
//
//  Purpose:
//      Return true if the test point is below the plane. Return false if the
//      test point is below the plane. Above and below are determined with
//      respect to the normal specified by plane_norm. This is used to confirm
//      that computed normals are actually pointing outwards, by testing a
//      point that is known to be inside the shape against the computed normal.
//      If the point is below the plane specified by plane_norm and plane_point,
//      we know that that normal will be valid. Otherwise, it needs to be
//      inverted.
//
//  Parameters:
//      plane_point - the point from which the normal will originate
//      plane_norm - what direction is 'up'?
//      test_point - you want to know if this is above or below the plane
//
//  Preconditions:
//      plane_norm must be given as a nonzero vector
//
//  Postconditions:
//      true or false is returned to tell the user the restult of their query
//      TRUE IF YOUR TEST POINT IS BELOW THE PLANE
//
//******************************************************************************

bool planetest(glm::vec3 plane_point, glm::vec3 plane_norm, glm::vec3 test_point)
{
  double result, a, b, c, x, x1, y, y1, z, z1;

  a  =  plane_norm.x;   b  =  plane_norm.y;  c  =  plane_norm.z;
  x  =  test_point.x;   y  =  test_point.y;  z  =  test_point.z;
  x1 = plane_point.x;   y1 = plane_point.y;  z1 = plane_point.z;

  //equation of a plane is:
    // a (x-x1) + b (y-y1) + c (z-z1) = 0;

  result = a * (x-x1) + b * (y-y1) + c * (z-z1);

  return (result < 0) ? true:false;
}

//**********************************************

//function: capsule sdf
float capsdf(glm::vec3 p, glm::vec3 a, glm::vec3 b, float r)
{
  glm::vec3 ab = b-a;
  glm::vec3 ap = p-a;

  float t = glm::dot(ab,ap)/glm::dot(ab,ab);

  t = glm::clamp(t, 0.0f, 1.0f);

  glm::vec3 c = a + (t * ab);

  return glm::length(p-c) - r;
}


#endif
