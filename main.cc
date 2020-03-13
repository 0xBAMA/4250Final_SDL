#include "resources/scene.h"

//DEBUG STUFF

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{

  bool show_high_severity         = true;
  bool show_medium_severity       = true;
  bool show_low_severity          = true;
  bool show_notification_severity = true;

  if(severity == GL_DEBUG_SEVERITY_HIGH && show_high_severity)
    fprintf( stderr, "        GL CALLBACK: %s type = 0x%x, severity = GL_DEBUG_SEVERITY_HIGH, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, message );

  if(severity == GL_DEBUG_SEVERITY_MEDIUM && show_medium_severity)
    fprintf( stderr, "        GL CALLBACK: %s type = 0x%x, severity = GL_DEBUG_SEVERITY_MEDIUM, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, message );

  if(severity == GL_DEBUG_SEVERITY_LOW && show_low_severity)
    fprintf( stderr, "        GL CALLBACK: %s type = 0x%x, severity = GL_DEBUG_SEVERITY_LOW, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, message );

  if(severity == GL_DEBUG_SEVERITY_NOTIFICATION && show_notification_severity)
    fprintf( stderr, "        GL CALLBACK: %s type = 0x%x, severity = GL_DEBUG_SEVERITY_NOTIFICATION, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, message );
}

//----------------------------------------------------------------------------
//the geometry
Scene scene;

int frame_count = 0;

//other globals
float t = 0.0;
float tilt = 0.0;
float yoffset = 0.0;

float texture_scale = 1.0;


glm::vec3 location = glm::vec3(0.5,0.5,-4);
glm::vec2 rotation = glm::vec2(0,0);


glm::vec3 texture_offset = glm::vec3(0,0,0);


//----------------------------------------------------------------------------

void display()
{
  // frame_count++;
  // scene.set_frame_count(frame_count);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // display functions
  scene.draw();

  glutSwapBuffers();
  glutPostRedisplay();
}

//----------------------------------------------------------------------------


float scale = 1.0f;

void keyboard(unsigned char key, int x, int y)
{
  switch (key) {

    case 033:
      exit(EXIT_SUCCESS);
      break;

    case '=':   //+
      scale *= 0.9;
      glUniform1fv(glGetUniformLocation(scene.get_draw_shader(), "scale"), 1, &scale);
      cout << scale << endl;
      break;

    case '-':   //-
      scale /= 0.9;
      glUniform1fv(glGetUniformLocation(scene.get_draw_shader(), "scale"), 1, &scale);
      cout << scale << endl;
      break;


    case 'f':
      glutFullScreenToggle(); //specific to freeglut
      break;

    case 'g':
      scene.compute();
      // THIS SHOULD ENSURE COHERENCY - 3/12/20 not sure if this is true? maybe only neccesary to prevent the display function from using the wrong data?
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
      break;

//movement
    case 'w':
      texture_offset += glm::vec3(0.005,0,0);
      break;

    case 's':
      texture_offset -= glm::vec3(0.005,0,0);
      break;

    case 'a':
      texture_offset += glm::vec3(0,0.005,0);
      break;

    case 'd':
      texture_offset -= glm::vec3(0,0.005,0);
      break;

    case 'q':
      texture_offset += glm::vec3(0,0,0.005);
      break;

    case 'e':
      texture_offset -= glm::vec3(0,0,0.005);
      break;

    case 'c':
      texture_scale*=0.9;
      glUniform1fv(glGetUniformLocation(scene.get_draw_shader(), "uniform_scale"), 1, &texture_scale);
      break;

    case 'v':
      texture_scale/=0.9;
      glUniform1fv(glGetUniformLocation(scene.get_draw_shader(), "uniform_scale"), 1, &texture_scale);
      break;

    case 'u':
      // location += glm::vec3(0,0.05,0);
      yoffset += 0.1;
      break;

    case 'i':
      // location -= glm::vec3(0,0.1,0);
      yoffset -= 0.1;
      break;

//rotation
    case 't':
      // rotation += glm::vec2(0,0.1);
      tilt += 0.1;
      break;

    case 'y':
      // rotation -= glm::vec2(0,0.1);
      tilt -= 0.1;
      break;

    case ',':
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      break;

    case '.':
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      break;

    case 'z':
      t+=0.08;
      break;

    case 'x':
      // yoffset = 2.0f;  tilt = -0.4f;
      yoffset = 1.55f;  tilt = -0.3f;
      break;
  }

  glutPostRedisplay();
}

//----------------------------------------------------------------------------

void timer(int)
{

  t+=0.0001;
  glUniform1fv(glGetUniformLocation(scene.get_draw_shader(), "t"), 1, &t);


  location = glm::vec3(0.5,0.5,0.5) + glm::vec3(5*cos(t), yoffset, 5*sin(t));
  rotation = glm::vec2(0,0) + glm::vec2(t+JonDefault::twopi/4,tilt);




  glUniform3fv(glGetUniformLocation(scene.get_draw_shader(), "location"), 1, glm::value_ptr(location));
  glUniform2fv(glGetUniformLocation(scene.get_draw_shader(), "rotation"), 1, glm::value_ptr(rotation));
  glUniform3fv(glGetUniformLocation(scene.get_draw_shader(), "texture_offset"), 1, glm::value_ptr(texture_offset));
  glUniform1fv(glGetUniformLocation(scene.get_draw_shader(), "uniform_scale"), 1, &texture_scale);


	glutPostRedisplay();
	glutTimerFunc(1000.0/60.0, timer, 0);
}

//----------------------------------------------------------------------------

int main(int argc, char **argv)
{
  // printf("\033[H\033[J"); //clear screen

  glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);  //doesn't look as good
  // glutInitDisplayMode(GLUT_MULTISAMPLE | GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

  glutInitContextVersion( 4, 5 );
	glutInitContextProfile( GLUT_CORE_PROFILE );

  glutInitWindowSize(720,480);
  glutCreateWindow("Window");
  glutFullScreen();

  glewInit();

  //DEBUG
  glEnable              ( GL_DEBUG_OUTPUT );
  glDebugMessageCallback( MessageCallback, 0 );

  cout << endl << endl << " GL_DEBUG_OUTPUT ENABLED " << endl;


  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  // glutMouseFunc( mouse );
  // glutIdleFunc( idle );
  glutTimerFunc(1000.0/60.0, timer, 0);

  scene.init();

//ENTER MAIN LOOP
  glutMainLoop();

  return(EXIT_SUCCESS);

}
