 #include "common.h"

//----------------------------------------------------------------------------

class sliceobj
{
public:

  void init(std::vector<glm::vec3> &points, std::vector<glm::vec3> &normals, std::vector<glm::vec4> &colors, std::vector<glm::vec3> &texcoords);

  void draw();

private:

  int ortho_start, ortho_num; //indexing geometry
  int persp_start, persp_num; //indexing geometry

};

//----------------------------------------------------------------------------

void sliceobj::init(std::vector<glm::vec3> &points, std::vector<glm::vec3> &normals, std::vector<glm::vec4> &colors, std::vector<glm::vec3> &texcoords)
{
  cout << endl << endl;
  cout << "initializing sliceobj starting at: " << endl;
  cout << "points:    "  << points.size()  << endl;
  cout << "normals:   "  << normals.size() << endl;
  cout << "colors:    "  << colors.size()  << endl;
  cout << "texcoords: "  << texcoords.size() << endl;
  cout << endl << endl;


  ortho_start = points.size();



  //this should be two triangles, which span the entire screen.

  //I am excited to implement this on the GPU for the first time - some of the earlier
  //iterations of voraldo included logic similar to this, but entirely CPU based

  //
  // A---------------B
  // |          .    |
  // |       .       |
  // |    .          |
  // |               |
  // C---------------D
  //


  //diagonal runs from C to B

  //we are at 0 in z
  //A is -1, 1
  //B is  1. 1
  //C is -1,-1
  //D is  1,-1

  points.push_back(glm::vec3(-1, 1, 0));  //A
  points.push_back(glm::vec3(-1,-1, 0));  //C
  points.push_back(glm::vec3( 1, 1, 0));  //B

  points.push_back(glm::vec3( 1, 1, 0));  //B
  points.push_back(glm::vec3(-1,-1, 0));  //C
  points.push_back(glm::vec3( 1,-1, 0));  //D


  texcoords.push_back(glm::vec3(-1, 1, 0));  //A
  texcoords.push_back(glm::vec3(-1,-1, 0));  //C
  texcoords.push_back(glm::vec3( 1, 1, 0));  //B

  texcoords.push_back(glm::vec3( 1, 1, 0));  //B
  texcoords.push_back(glm::vec3(-1,-1, 0));  //C
  texcoords.push_back(glm::vec3( 1,-1, 0));  //D


  normals.push_back(glm::vec3(0,0,1));
  normals.push_back(glm::vec3(0,0,1));
  normals.push_back(glm::vec3(0,0,1));

  normals.push_back(glm::vec3(0,0,1));
  normals.push_back(glm::vec3(0,0,1));
  normals.push_back(glm::vec3(0,0,1));



  //these are passed to gl_PointCoord untouched, so that they just cover the screen.
  // The main goal from this step is to have fragments that cover the screen, once
  //  i.e. create a pixel shader type of environment

  // we will need resolution passed in, or to find out if there is GLSL built in resolution
  // The fragcoord locations are done in (floating point type) rounded-to-the-nearest-integer
  //   pixels.









  ortho_num = points.size() - ortho_start;






  persp_start = points.size();

  float divisions = 25.0f;
  float z = 0.0f;
  float depth = 13.0f;

  for(float x = -1.0f; x <= 1.0f; x += 2.0f/divisions)
    for(float y = -1.0f; y <= 1.0f; y += 2.0f/divisions)
    {

      z = 0;

      //this scheme is a little simpler than the sphere thing I was considering

      if((x+1.0f/divisions) > 0)
        z += -exp(x-4);
      else
        z += -exp(-x-4);


      if((y+1.0f/divisions) > 0)
        z += -exp(y-4);
      else
        z += -exp(-y-4);


      points.push_back(glm::vec3(x, y+2.0f/divisions, z));  //A
      points.push_back(glm::vec3(x,y, z));  //C
      points.push_back(glm::vec3(x+2.0f/divisions, y+2.0f/divisions, z));  //B

      points.push_back(glm::vec3(x+2.0f/divisions, y+2.0f/divisions, z));  //B
      points.push_back(glm::vec3(x,y, z));  //C
      points.push_back(glm::vec3(x+2.0f/divisions,y, z));  //D



      texcoords.push_back(glm::vec3(x, y+2.0f/divisions, 0));  //A
      texcoords.push_back(glm::vec3(x,y, 0));  //C
      texcoords.push_back(glm::vec3(x+2.0f/divisions, y+2.0f/divisions, 0));  //B

      texcoords.push_back(glm::vec3(x+2.0f/divisions, y+2.0f/divisions, 0));  //B
      texcoords.push_back(glm::vec3(x,y, 0));  //C
      texcoords.push_back(glm::vec3(x+2.0f/divisions,y, 0));  //D



      normals.push_back(glm::normalize(glm::vec3(x, y+2.0f/divisions, z+depth)));  //A
      normals.push_back(glm::normalize(glm::vec3(x,y, z+depth)));  //C
      normals.push_back(glm::normalize(glm::vec3(x+2.0f/divisions, y+2.0f/divisions, z+depth)));  //B

      normals.push_back(glm::normalize(glm::vec3(x+2.0f/divisions, y+2.0f/divisions, z+depth)));  //B
      normals.push_back(glm::normalize(glm::vec3(x,y, z+depth)));  //C
      normals.push_back(glm::normalize(glm::vec3(x+2.0f/divisions,y, z+depth)));  //D





    }

  persp_num = points.size() - persp_start;














  cout << endl << endl;
  cout << "initializing sliceobj finished at: " << endl;
  cout << "points:    "  << points.size()  << endl;
  cout << "normals:   "  << normals.size() << endl;
  cout << "colors:    "  << colors.size()  << endl;
  cout << "texcoords: "  << texcoords.size() << endl;
  cout << endl << endl;

}

//----------------------------------------------------------------------------

void sliceobj::draw()
{
  glDrawArrays(GL_TRIANGLES, ortho_start, ortho_num);
  // glDrawArrays(GL_TRIANGLES, persp_start, persp_num);
}

//----------------------------------------------------------------------------
