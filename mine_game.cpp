#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define FN(i, n) for(int i=0;i<(int)n;++i)
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
#define f first
#define s second
struct VAO {
    GLuint VertexArrayID;
    GLuint VertexBuffer;
    GLuint ColorBuffer;

    GLenum PrimitiveMode;
    GLenum FillMode;
    int NumVertices;
};
struct COLOR {
    float r;
    float g;
    float b;
};
//defining colors
COLOR grey = {168.0/255.0,168.0/255.0,168.0/255.0};
COLOR gold = {218.0/255.0,165.0/255.0,32.0/255.0};
COLOR coingold = {255.0/255.0,223.0/255.0,0.0/255.0};
COLOR red = {255.0/255.0,51.0/255.0,51.0/255.0};
COLOR lightgreen = {57/255.0,230/255.0,0/255.0};
COLOR darkgreen = {51/255.0,102/255.0,0/255.0};
COLOR black = {30/255.0,30/255.0,21/255.0};
COLOR blue = {0,0,1};
COLOR darkbrown = {46/255.0,46/255.0,31/255.0};
COLOR lightbrown = {95/255.0,63/255.0,32/255.0};
COLOR brown1 = {117/255.0,78/255.0,40/255.0};
COLOR brown2 = {134/255.0,89/255.0,40/255.0};
COLOR brown3 = {46/255.0,46/255.0,31/255.0};
COLOR cratebrown = {153/255.0,102/255.0,0/255.0};
COLOR cratebrown1 = {121/255.0,85/255.0,0/255.0};
COLOR cratebrown2 = {102/255.0,68/255.0,0/255.0};
COLOR skyblue2 = {113/255.0,185/255.0,209/255.0};
COLOR skyblue1 = {123/255.0,201/255.0,227/255.0};
COLOR skyblue = {132/255.0,217/255.0,245/255.0};
COLOR lightpink = {255/255.0,122/255.0,173/255.0};
COLOR darkpink = {255/255.0,51/255.0,119/255.0};
COLOR white = {255/255.0,255/255.0,255/255.0};
COLOR score = {117/255.0,78/255.0,40/255.0};
struct game_object
{
  string name;//name of object
  string object_color;//red,black,green
  VAO* object;
  float gravity;
  glm::vec3 center,speed,change,rotation_center,angle;
  COLOR color;
  bool ispresent,is_rotate;
  float height,width,radius;

};
struct GLMatrices {
    glm::mat4 projection;
    glm::mat4 model;
    glm::mat4 view;
    GLuint MatrixID;
} Matrices;
//defining globals
string scoreLabel,endLabel;
GLuint programID;
double mouse_pos_x, mouse_pos_y;
double new_mouse_pos_x, new_mouse_pos_y;
float old_time; // Time in seconds
float cur_time,last_update_time,game_start_timer; // Time in second
int scoreLabel_x,scoreLabel_y,endLabel_x,endLabel_y,timer_x,timer_y,game_timer,zoom_camera,x_change,y_change;
int e_left=-400,e_right=400,e_up=400,e_down=-400;
float speed_x_c=(float)(e_right-e_left)/50;
float speed_y_c=(float)(e_up-e_down)/50;
bool CursorOnScreen=0;
map<string,vector<game_object> > all_objects;
vector<game_object> canon_vector;
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open())
    {
        std::string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

    // Link the program
    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void quit(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
/* Generate VAO, VBOs and return VAO handle */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, GLfloat* vertex_buffer_data, GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
    struct VAO* vao = new struct VAO;
    vao->PrimitiveMode = primitive_mode;
    vao->NumVertices = numVertices;
    vao->FillMode = fill_mode;

    // Create Vertex Array Object
    // Should be done after CreateWindow and before any other GL calls
    glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
    glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
    glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors

    glBindVertexArray (vao->VertexArrayID); // Bind the VAO
    glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
    glVertexAttribPointer(
            0,                  // attribute 0. Vertices
            3,                  // size (x,y,z)
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );

    glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
    glVertexAttribPointer(
            1,                  // attribute 1. Color
            3,                  // size (r,g,b)
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );

    return vao;
}

/* Generate VAO, VBOs and return VAO handle - Common Color for all vertices */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, GLfloat* vertex_buffer_data, GLfloat red, GLfloat green, GLfloat blue, GLenum fill_mode=GL_FILL)
{
    GLfloat* color_buffer_data = new GLfloat [3*numVertices];
    for (int i=0; i<numVertices; i++) {
        color_buffer_data [3*i] = red;
        color_buffer_data [3*i + 1] = green;
        color_buffer_data [3*i + 2] = blue;
    }

    return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}
/* Render the VBOs handled by VAO */
void draw3DObject (struct VAO* vao)
{
    // Change the Fill Mode for this object
    glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

    // Bind the VAO to use
    glBindVertexArray (vao->VertexArrayID);

    // Enable Vertex Attribute 0 - 3d Vertices
    glEnableVertexAttribArray(0);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

    // Enable Vertex Attribute 1 - Color
    glEnableVertexAttribArray(1);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);

    // Draw the geometry !
    glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}
void cursor_enter_callback(GLFWwindow* window, int entered)
{
    cout<<"entered"<<" "<<entered<<endl;
    if (entered) CursorOnScreen = 1 ;
    else CursorOnScreen = 0 ;
}
// Returns the mouse coordinates translated according to our coordinate system
glm::vec3 GetMouseCoordinates(GLFWwindow* window)
{
    double CursorX,CursorY ;
    glfwGetCursorPos(window, &CursorX, &CursorY) ;
    return glm::vec3(CursorX+e_left,CursorY+e_down,0) ;
}
// find angle from A to B : assuming both are normalized vectors
float FindAngle(glm::vec3 A,glm::vec3 B)
{
    float theta = acos(dot(A,B)) ;
    if(cross(A,B)[2] > 0 ) theta *= -1 ;
    return theta ;
}
void mousescroll(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset==-1) {
        zoom_camera /= 1.1; //make it bigger than current size
    }
    else if(yoffset==1){
        zoom_camera *= 1.1; //make it bigger than current size
    }
    if (zoom_camera<=1) {
        zoom_camera = 1;
    }
    if (zoom_camera>=4) {
        zoom_camera=4;
    }
    if(x_change-400.0f/zoom_camera<-400)
        x_change=-400+400.0f/zoom_camera;
    else if(x_change+400.0f/zoom_camera>400)
        x_change=400-400.0f/zoom_camera;
    if(y_change-300.0f/zoom_camera<-300)
        y_change=-300+300.0f/zoom_camera;
    else if(y_change+300.0f/zoom_camera>300)
        y_change=300-300.0f/zoom_camera;
    Matrices.projection = glm::ortho((float)(-400.0f/zoom_camera+x_change), (float)(400.0f/zoom_camera+x_change), (float)(-300.0f/zoom_camera+y_change), (float)(300.0f/zoom_camera+y_change), 0.1f, 500.0f);
}

//Ensure the panning does not go out of the map
void check_pan(){
    if(x_change-400.0f/zoom_camera<-400)
        x_change=-400+400.0f/zoom_camera;
    else if(x_change+400.0f/zoom_camera>400)
        x_change=400-400.0f/zoom_camera;
    if(y_change-300.0f/zoom_camera<-300)
        y_change=-300+300.0f/zoom_camera;
    else if(y_change+300.0f/zoom_camera>300)
        y_change=300-300.0f/zoom_camera;
}

void initKeyboard(){

}

//function to move cannonaim
void move_canon(int u)
{
  cout<<"called"<<endl;
  vector<game_object> &r=all_objects["canon"];
  for(auto &it: r)
  {
    it.center=it.center+glm::vec3(0,u * speed_y_c,0);
    if(it.is_rotate) it.rotation_center += glm::vec3(0,u * speed_y_c,0) ;
  }
}
void RotateCannon(GLFWwindow* window)
{
    if(!CursorOnScreen) return ;
    glm::vec3 Mouse = GetMouseCoordinates(window) ;
    vector<game_object> &Cannon=all_objects["canon"];
    for(auto &it:Cannon) if(it.is_rotate) it.angle = normalize(Mouse - it.rotation_center) ;
}
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
  cout<<"pressed key"<<" "<<key<<endl;
  if (action == GLFW_RELEASE) {
      switch (key) {
          case GLFW_KEY_UP:
              move_canon(1);
              break;
          case GLFW_KEY_DOWN:
              move_canon(-1);
              break;

      }
  }
  else if (action == GLFW_PRESS) {
      switch (key) {
          case GLFW_KEY_ESCAPE:
              quit(window);
              break;
          default:
              break;
      }
  }
  else if(action ==GLFW_REPEAT)
  {
    switch (key) {
        case GLFW_KEY_UP:
            move_canon(1);
            break;
        case GLFW_KEY_DOWN:
            move_canon(-1);
            break;

    }
  }
}

/* Executed for character input (like in text boxes) */
void keyboardChar(GLFWwindow* window, unsigned int key)
{
    switch (key) {
        case 'Q':
        case 'q':
            quit(window);
            break;
        default:
            break;
    }
}

int mouse_clicked=0;
int right_mouse_clicked=0;
double mouse_x,mouse_y;
double mouse_x_old,mouse_y_old;

void mouse_click(){

}

void mouse_release(GLFWwindow* window, int button){

}

/* Executed when a mouse button is pressed/released */
void mouseButton (GLFWwindow* window, int button, int action, int mods)
{

}
void reshapeWindow (GLFWwindow* window, int width, int height)
{
    int fbwidth=width, fbheight=height;
    /* With Retina display on Mac OS X, GLFW's FramebufferSize
       is different from WindowSize */
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);
    GLfloat fov = 90.0f;
    glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);
    Matrices.projection = glm::ortho(-400.0f/zoom_camera, 400.0f/zoom_camera, -300.0f/zoom_camera, 300.0f/zoom_camera, 0.1f, 500.0f);
}
void CreateCircle(string name,COLOR color,glm::vec3 centre,float radius,int parts,bool fill)
{
  GLfloat vertex_buffer_data[parts*9];
  GLfloat color_buffer_data[parts*9];
  int i,j;
  float angle=(2*M_PI/parts);
  float current_angle = 0;
  VAO* circle;
  FN(i,parts)
  {
     FN(j,3)
     {
       color_buffer_data[i*9+j*3]=color.r;
        color_buffer_data[i*9+j*3+1]=color.g;
         color_buffer_data[i*9+j*3+2]=color.b;
       }
        vertex_buffer_data[i*9]=0;
        vertex_buffer_data[i*9+1]=0;
        vertex_buffer_data[i*9+2]=0;
         vertex_buffer_data[i*9+3]=radius*cos(current_angle);
         vertex_buffer_data[i*9+4]=radius*sin(current_angle);
         vertex_buffer_data[i*9+5]=0;
         vertex_buffer_data[i*9+6]=radius*cos(current_angle+angle);
         vertex_buffer_data[i*9+7]=radius*sin(current_angle+angle);
          vertex_buffer_data[i*9+8]=0;
          current_angle+=angle;
        }
          if(fill) circle= create3DObject(GL_TRIANGLES, (parts*9)/3, vertex_buffer_data, color_buffer_data, GL_FILL);
          else circle=create3DObject(GL_TRIANGLES, (parts*9)/3, vertex_buffer_data, color_buffer_data, GL_LINE);
          game_object GO={};
          GO.color = color;
          GO.name=name;
        //  GO.name = name;
          GO.object = circle;
          GO.center=centre;
          GO.radius=radius;
          GO.speed=glm::vec3(0,0,0);
          if(name=="canon2")GO.is_rotate=1; else GO.is_rotate=0;
          canon_vector.push_back(GO);
}
void createcanon(string name,COLOR colorA,COLOR colorB,glm::vec3 centre,float radius1,float radius2)
{
  glm::vec3 c1=centre + glm::vec3(-1*radius1,0,0);
  CreateCircle("canon1",colorA,c1,radius1,120,1);
  glm::vec3 c2=centre + glm::vec3(radius2,0,0);
  CreateCircle("canon2",colorB,c2,radius2,120,1);
  for(auto &it:canon_vector) if(it.name=="canon2") it.rotation_center=c1;
  all_objects["canon"] = canon_vector ;
}
void createRectangle (string name, COLOR colorA, COLOR colorB, COLOR colorC, COLOR colorD, glm::vec3 centre, float height, float width, string component)
{
    // GL3 accepts only Triangles. Quads are not supported
    float w=width/2,h=height/2;
    GLfloat vertex_buffer_data [] = {
        -w,-h,0, // vertex 1
        -w,h,0, // vertex 2
        w,h,0, // vertex 3

        w,h,0, // vertex 3
        w,-h,0, // vertex 4
        -w,-h,0  // vertex 1
    };

    GLfloat color_buffer_data [] = {
        colorA.r,colorA.g,colorA.b, // color 1
        colorB.r,colorB.g,colorB.b, // color 2
        colorC.r,colorC.g,colorC.b, // color 3

        colorC.r,colorC.g,colorC.b, // color 4
        colorD.r,colorD.g,colorD.b, // color 5
        colorA.r,colorA.g,colorA.b // color 6
    };

    // create3DObject creates and returns a handle to a VAO that can be used later
    VAO *rectangle = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
    game_object GO={};
    GO.color = colorA;
    GO.name = name;
    GO.object = rectangle;
    GO.center=centre;
    GO.height=height;
    GO.width=width;
    GO.speed=glm::vec3(0,0,0);
    //r.push_back(GO);
}
void draw (GLFWwindow* window)
{
    //game_timer=(int)(90-(glfwGetTime()-game_start_timer));
    Matrices.projection = glm::ortho((float)e_left,(float) e_right, (float)e_down, (float) e_up, 0.1f, 500.0f);
    // clear the color and depth in the frame buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram (programID);
    Matrices.view = glm::lookAt(glm::vec3(0,0,1), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane

    // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
    //  Don't change unless you are sure!!
    glm::mat4 MVP ;
    glm::mat4 VP = Matrices.projection * Matrices.view;
    RotateCannon(window);
    for(auto it: all_objects)
    {

    for(auto it2: it.s )
    {
      Matrices.model = glm::mat4(1.0f) * glm::translate (it2.center);
      if(it2.is_rotate)
        {
            Matrices.model = glm::translate (it2.rotation_center*(float)-1 ) * Matrices.model ;
            float theta = FindAngle(normalize(it2.center - it2.rotation_center),it2.angle) ;
            Matrices.model = glm::rotate(theta, glm::vec3(0,0,1)) * Matrices.model ;
            Matrices.model = glm::translate (it2.rotation_center) * Matrices.model ;
        }
        MVP = VP * Matrices.model; // MVP = p * V * M
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DObject(it2.object);
      }
    }
}



GLFWwindow* initGLFW (int width, int height)
{
    GLFWwindow* window; // window desciptor/handle

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval( 1 );

    /* --- register callbacks with GLFW --- */

    /* Register function to handle window resizes */
    /* With Retina display on Mac OS X GLFW's FramebufferSize
       is different from WindowSize */
    glfwSetFramebufferSizeCallback(window, reshapeWindow);
    glfwSetWindowSizeCallback(window, reshapeWindow);

    /* Register function to handle window close */
    glfwSetWindowCloseCallback(window, quit);

    /* Register function to handle keyboard input */
    glfwSetKeyCallback(window, keyboard);      // general keyboard input
    glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling

    /* Register function to handle mouse click */
    glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks
    glfwSetScrollCallback(window, mousescroll); // mouse scroll

    return window;
}
void initGL (GLFWwindow* window, int width, int height)
{
  //creating objects
  //createRectangle("cannonpower1",cratebrown2,cratebrown2,cratebrown2,cratebrown2,glm::vec3(-3,0,0),1,2,"background");
  //createRectangle("cannonpower1",darkbrown,darkbrown,darkbrown,darkbrown,glm::vec3(-1.6,0,0),0.2,0.8,"background");
  createcanon("canon",black,brown3,glm::vec3(e_left+50,0,0),25,10);
  // Create and compile our GLSL program from the shaders
  programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
  // Get a handle for our "MVP" uniform
  Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


  reshapeWindow (window, width, height);

  // Background color of the scene
  glClearColor (0.3f, 0.3f, 0.3f, 0.0f); // R, G, B, A
  glClearDepth (1.0f);

  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LEQUAL);

  cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
  cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
  cout << "VERSION: " << glGetString(GL_VERSION) << endl;
  cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

int main (int argc, char** argv)
{
    int width = 800;
    int height = 600;
    scoreLabel="SCORE";
    scoreLabel_x=175;
    scoreLabel_y=250;
    timer_y=250;
    timer_x=20;
    endLabel="";
    endLabel_x=-160;
    GLFWwindow* window = initGLFW(width, height);
    glfwSetCursorEnterCallback(window, cursor_enter_callback);
    initGL (window, width, height);



    double last_update_time = glfwGetTime(), current_time;

    glfwGetCursorPos(window, &mouse_pos_x, &mouse_pos_y);

    game_start_timer=glfwGetTime();
    old_time = glfwGetTime();
    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {

        cur_time = glfwGetTime(); // Time in seconds
        // OpenGL Draw commands
        draw(window);
        old_time=cur_time;

        // Swap Frame Buffer in double buffering
        glfwSwapBuffers(window);

        // Poll for Keyboard and mouse events
        glfwPollEvents();

        // Control based on time (Time based transformation like 5 degrees rotation every 0.5s)
        current_time = glfwGetTime(); // Time in seconds
        if ((current_time - last_update_time) >= 0.5) { // atleast 0.5s elapsed since last frame
            // do something every 0.5 seconds ..
            last_update_time = current_time;
        }
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
