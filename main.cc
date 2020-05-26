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

bool frame_upd8=false;

//other globals
float t = 0.0f;
float tilt = 0.0f;
float yoffset = 0.0f;

float scale = 1.0f;
float texture_scale = 1.0f;


glm::vec3 location = glm::vec3(0.5,0.5,-4);
glm::vec2 rotation = glm::vec2(0,0);


glm::vec3 texture_offset = glm::vec3(0,0,0);


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
      frame_upd8 = !frame_upd8;
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

}



int main(int argc, char **argv)
{
  // printf("\033[H\033[J"); //clear screen

  // glutInit(&argc, argv);
  // glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);  //doesn't look as good
  // glutInitDisplayMode(GLUT_MULTISAMPLE | GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

  // glutInitContextVersion( 4, 5 );
	// glutInitContextProfile( GLUT_CORE_PROFILE );

  // glutInitWindowSize(720,480);
  // glutCreateWindow("Window");
  // glutFullScreen();





  cout << endl << "Creating OpenGL window ...";

  if(SDL_Init( SDL_INIT_EVERYTHING ) != 0)
  {
      printf("Error: %s\n", SDL_GetError());
  }

  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
  SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );

  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 8);

  // GL 4.5 + GLSL 450
  const char* glsl_version = "#version 450";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5 );

  //this is how you query the screen resolution
  SDL_DisplayMode dm;
  SDL_GetDesktopDisplayMode(0, &dm);

  //pulling these out because I'm going to try to span the whole screen with
  //the window, in a way that's flexible on different resolution screens
  int total_screen_width = dm.w;
  int total_screen_height = dm.h;

  SDL_Window * window;
  SDL_GLContext GLcontext;

  window = SDL_CreateWindow( "OpenGL Window", 0, 0, total_screen_width, total_screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS );
  GLcontext = SDL_GL_CreateContext( window );

  SDL_GL_MakeCurrent(window, GLcontext);
  SDL_GL_SetSwapInterval(1); // Enable vsync -- questionable utility



  if (glewInit() != GLEW_OK)
  {
      fprintf(stderr, "Failed to initialize OpenGL loader!\n");
  }

  //DEBUG
  glEnable              ( GL_DEBUG_OUTPUT );
  glDebugMessageCallback( MessageCallback, 0 );

  cout << endl << endl << " GL_DEBUG_OUTPUT ENABLED " << endl;


  // glutDisplayFunc(display);
  // glutKeyboardFunc(keyboard);
  // glutMouseFunc( mouse );
  // glutIdleFunc( idle );
  // glutTimerFunc(1000.0/60.0, timer, 0);

  SDL_GL_SwapWindow( window );

  cout << " done." << endl;

  scene.init();


//ENTER MAIN LOOP
  // glutMainLoop();


  bool exit = false;
  while(!exit)
  {

  //take input
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
        exit = true;
      if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
        exit = true;
      if (event.type == SDL_KEYUP  && event.key.keysym.sym == SDLK_ESCAPE)
        exit = true;

      if(event.type == SDL_KEYDOWN)
      {
        //switch based on key press
        switch( event.key.keysym.sym )
        {
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
		frame_upd8=!frame_upd8;
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
      }
    }

  //clear and draw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);





  //MESSY
    t+=0.0001;
    glUniform1fv(glGetUniformLocation(scene.get_draw_shader(), "t"), 1, &t);

    location = glm::vec3(0.5,0.5,0.5) + glm::vec3(5*cos(t), yoffset, 5*sin(t));
    rotation = glm::vec2(0,0) + glm::vec2(t+JonDefault::twopi/4,tilt);

    glUniform3fv(glGetUniformLocation(scene.get_draw_shader(), "location"), 1, glm::value_ptr(location));
    glUniform2fv(glGetUniformLocation(scene.get_draw_shader(), "rotation"), 1, glm::value_ptr(rotation));
    glUniform3fv(glGetUniformLocation(scene.get_draw_shader(), "texture_offset"), 1, glm::value_ptr(texture_offset));
    glUniform1fv(glGetUniformLocation(scene.get_draw_shader(), "uniform_scale"), 1, &texture_scale);


    scene.draw();
	  
	  if(frame_upd8)
	  {
	    scene.compute();
            // THIS SHOULD ENSURE COHERENCY - 3/12/20 not sure if this is true? maybe only neccesary to prevent the display function from using the wrong data?
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);  
	  }

    SDL_GL_SwapWindow(window);                      //swap the double buffers to display

  }







  return(EXIT_SUCCESS);

}
