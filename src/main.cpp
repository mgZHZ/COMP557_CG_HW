/**
 COMP557 Assignment1
 Hanzhi Zhang
 260908416
 */

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"
#include "Motion.h"
#include "DAGNode.h"

#include "glm/gtx/string_cast.hpp"

using namespace std;

GLFWwindow *window; // Main application window
string RES_DIR = ""; // Where data files live
shared_ptr<Program> prog;
shared_ptr<Program> prog2; // for drawing with colours
shared_ptr<Program> prog3; // for drawing axis with colours

shared_ptr<Shape> shape_teapot;
shared_ptr<Shape> shape_sphere;
shared_ptr<Shape> shape_cube;


shared_ptr<Motion> motion;

vector<string> playlist = {"0019_AdvanceBollywoodDance001.bvh", "Cyrus_Take6.bvh", "OptiTrack-IITSEC2007.bvh"};
int play = 0;


GLuint vao;
GLuint posBufID; // position buffer for drawing a line loop
GLuint aPosLocation = 0; // location set in col_vert.glsl (or can be queried)
const GLuint NumVertices = 4;
GLfloat vertices[NumVertices][3] = {
					{ -1, -1,  0 },
					{  1, -1,  0 },
					{  1,  1,  0 },
					{ -1,  1,  0 } };

//GLuint x_aPosLocation = 0;
//const GLuint axis_NumVertices = 2;
GLfloat ox_vertices[2][3] = {{  0,  0,  0 }, { 1,  0,  0 } };
GLfloat oy_vertices[2][3] = {{  0,  0,  0 }, { 0,  1,  0 } };
GLfloat oz_vertices[2][3] = {{  0,  0,  0 }, { 0,  0,  1 } };

//unit line
GLfloat unitline[2][3] = {{0,0,0},{1, 1, 1}};

int fc = 0;        // frame counter
int step = 4;
int stepkeeper = 0;
int sign = 1;
//int

int counter = 0;

static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

static void init()
{
	GLSL::checkVersion();

	// Check how many texture units are supported in the vertex shader
	int tmp;
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &tmp);
	cout << "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS = " << tmp << endl;
	// Check how many uniforms are supported in the vertex shader
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &tmp);
	cout << "GL_MAX_VERTEX_UNIFORM_COMPONENTS = " << tmp << endl;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &tmp);
	cout << "GL_MAX_VERTEX_ATTRIBS = " << tmp << endl;

	// Set background color.
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);

	// Initialize mesh.
    shape_teapot = make_shared<Shape>();
    shape_teapot->loadMesh(RES_DIR + "teapot.obj");
    shape_teapot->init();
    
    shape_sphere = make_shared<Shape>();
    shape_sphere->loadMesh(RES_DIR + "sphere.obj");
    shape_sphere->init();
    
    shape_cube = make_shared<Shape>();
    shape_cube->loadMesh(RES_DIR + "cube.obj");
    shape_cube->init();
    
    
    // load bvh.
    motion = make_shared<Motion>();
    cout << "playing: " << playlist[play] << endl;
    motion->loadBVH(RES_DIR + playlist[play]);

	
	// Initialize the GLSL programs.
	prog = make_shared<Program>();
	prog->setVerbose(true);
	prog->setShaderNames(RES_DIR + "nor_vert.glsl", RES_DIR + "nor_frag.glsl");
	prog->init();
	prog->addUniform("P");
	prog->addUniform("MV");
	prog->addAttribute("aPos");
	prog->addAttribute("aNor");
	prog->setVerbose(false);
	
    prog2 = make_shared<Program>();
	prog2->setVerbose(true);
	prog2->setShaderNames(RES_DIR + "col_vert.glsl", RES_DIR + "col_frag.glsl");
	prog2->init();
	prog2->addUniform("P");
	prog2->addUniform("MV");
	prog2->addUniform("col");
	prog2->addAttribute("aPos");
	prog2->setVerbose(false);
    
    prog3 = make_shared<Program>();
    prog3->setVerbose(true);
    prog3->setShaderNames(RES_DIR + "col_vert.glsl", RES_DIR + "col_frag.glsl");
    prog3->init();
    prog3->addUniform("P");
    prog3->addUniform("MV");
    prog3->addUniform("col");
    prog3->addAttribute("aPos");
    prog3->setVerbose(false);
    
	
	// If there were any OpenGL errors, this will print something.
	// You can intersperse this line in your code to find the exact location
	// of your OpenGL error.
	GLSL::checkError(GET_FILE_LINE);

	// Create a buffers for doing some line drawing
//	glGenVertexArrays(1, &vao);
//	glBindVertexArray(vao);                                                         // Set the buffer as the active array
//	glGenBuffers(1, &posBufID);                                                     // Allocate memory for the triangle
//	glBindBuffer(GL_ARRAY_BUFFER, posBufID);                                        // Set the buffer as the active array
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),	vertices, GL_STATIC_DRAW);      // Fill the buffer with data
//	glEnableVertexAttribArray(aPosLocation);                                        // Enable the vertex array
//	glVertexAttribPointer(aPosLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

static void reset(int i){
    
    cout << "switch to " << i << " " << playlist[i] << endl;
    
    play = i;
    fc = 0;        // frame counter
    step = 4;
    stepkeeper = 0;
    sign = 1;
    
    init();
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    // esc
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    //space: (start and stop) the motion
    if(key == GLFW_KEY_SPACE  && action == GLFW_PRESS) {

        if(stepkeeper == 0){
            cout << "freez!" << endl;
            stepkeeper = step;
            step = 0;
        }
        else{
            cout << "unfreez" << endl;
            step = stepkeeper;
            stepkeeper = 0;
        }
    }

    //enter: forward or backward the motion, (incrementing or decrementing frames)
    if(key ==  GLFW_KEY_ENTER  && action == GLFW_PRESS) {
        cout << "reverse!" << endl;
        sign *= -1;
    }
    
    //up: increase the stepping rate (make it within numFrames)
    if(key ==  GLFW_KEY_UP  && action == GLFW_PRESS) {

        cout << "<<<<< faster!" << endl;
        
        if (stepkeeper == 0 ){//and// step+1 < motion->numFrames){
            step += 1;
        }
        else if (step == 0 ){// and stepkeeper+1 < motion->numFrames) {
            stepkeeper += 1;
        }
    }
    
    //down: decrease the stepping rate (make sure always greater than 1)
    if(key ==  GLFW_KEY_DOWN  && action == GLFW_PRESS) {

        cout << " /// s l o w d o w n " << endl;
        if (stepkeeper == 0 and step-1 >= 1){  step -= 1;  }
        else if (step == 0 and stepkeeper-1 >= 1){ stepkeeper -= 1; }
    }
    
    
    if(key ==  GLFW_KEY_1  && action == GLFW_PRESS) {  reset(0);  }
    
    if(key ==  GLFW_KEY_2  && action == GLFW_PRESS) {  reset(1);  }
    
    if(key ==  GLFW_KEY_3  && action == GLFW_PRESS) {  reset(2);  }
}




// P, MV, Vertices, color
static void draw_line(shared_ptr<Program> progr, shared_ptr<MatrixStack> P, shared_ptr<MatrixStack> MV,
                      GLfloat vertices[2][3], int r, int g, int b){
//    glBindVertexArray(vao);
    GLuint vbo;                                                             // a buffer

    glGenVertexArrays(1, &vbo);
    glBindVertexArray(vbo);
    glGenBuffers(1, &posBufID);                                             // Allocate memory for the triangle
    glBindBuffer(GL_ARRAY_BUFFER, posBufID);
    glBufferData(GL_ARRAY_BUFFER, 24, vertices, GL_STATIC_DRAW);



    glEnableVertexAttribArray(aPosLocation);
    glVertexAttribPointer(aPosLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    // baiweizhi tiaoyanse
    glUniformMatrix4fv(progr->getUniform("P"), 1, GL_FALSE, &P->topMatrix()[0][0]);
    glUniformMatrix4fv(progr->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);
    glUniform3f(progr->getUniform("col"), r, g, b);
    
    // load shuju
    glBindVertexArray(vbo);
    
    //hua
    glDrawArrays(GL_LINES, 0, 2);
    
    
    glDeleteBuffers(1, &posBufID);
}

//prog3->bind();
//MV->pushMatrix();
//MV->rotate( (float) t, 0, 1, 0 ); blablabla

// P, MV, Vertices, color
static void draw_axis(shared_ptr<Program> progr, shared_ptr<MatrixStack> P, shared_ptr<MatrixStack> MV, float s){
    MV -> scale(s);

    draw_line(progr, P, MV, ox_vertices, 1, 0, 0);
    draw_line(progr, P, MV, oy_vertices, 0, 1, 0);
    draw_line(progr, P, MV, oz_vertices, 0, 0, 1);
}
//MV->popMatrix();
//prog3->unbind();



//static void drawShape(shared_ptr<Program> progr, shared_ptr<MatrixStack> P, shared_ptr<MatrixStack> MV, shared_ptr<Shape> shape){
//
//    glUniformMatrix4fv(progr->getUniform("P"), 1, GL_FALSE, &P->topMatrix()[0][0]);
//    glUniformMatrix4fv(progr->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);
//    shape->draw(prog);
//
//}

//static void drawJoint(shared_ptr<Program> progr, shared_ptr<MatrixStack> P, shared_ptr<MatrixStack> MV, DAGNode* joint){
//    //}, map<string, glm::mat4> storage){
//
//    prog->bind();
//    MV->pushMatrix();
//    MV->scale(0.5, 0.5, 0.5);
//
//    if(play == 0){
//        MV->scale(0.5, 0.5, 0.5);
//    }
//    else{
//        MV->scale(1.5);
//    }
//
//    shared_ptr<Shape> OBJshape = shape_sphere;
//    string ashape= "teapot.obj";
//
//    if (joint->name == "Head"){
//        MV->scale(1.2, 1.2, 1.2);
//
////        OBJshape = shape_sphere;
//        ashape = "sphere.obj";
//    }
//    else if ( joint->parent != NULL and joint->parent->name == "Head"){
//        double t = glfwGetTime();
//        MV->rotate( (float) t, 0, 1, 0 );
//
////        OBJshape = shape_teapot;
//        ashape = "teapot.obj";
//    }
//    else if ( joint->name == "Site" or joint->name == "EndSite"){
//
//        MV->scale(0.5);
////        OBJshape = shape_sphere;
//        ashape = "sphere.obj";
//    }
//    else{
//        MV->scale(0.5);
////        OBJshape = shape_cube;
//        ashape = "cube.obj";
//    }
//
////    drawShape(prog, P, MV, OBJshape);
////    joint->draw(prog, P, MV, ashape, RES_DIR);
//
//
//    MV->popMatrix();
//    prog->unbind();
//
//}


static glm::mat4 doOTR(shared_ptr<MatrixStack> MV, DAGNode* c, int i){
    
    MV -> pushMatrix();
    MV -> loadIdentity();
    
    MV->translate(c->offset[0]/100, c->offset[1]/100, c->offset[2]/100);
    
    vector<string> channel = c->channels;

    int channelsize = channel.size();

    if( channelsize > 0 ){

        for (int j = 0; j < channelsize; j++){

            // channel j of frame i can be accessed with data[i*numChannels+j]
            float compute = motion->data[c->channelDataStartIndex + i*motion->numChannels+j];

            if(channel[j] == "Xposition"){   MV->translate(compute/100, 0, 0);  }

            else if (channel[j] == "Yposition"){  MV->translate(0, compute/100, 0);  }

            else if (channel[j] == "Zposition"){  MV->translate(0, 0, compute/100);  }


            else if (channel[j] == "Xrotation"){  MV->rotate(compute*M_PI/180, 1, 0, 0);  }

            else if (channel[j] == "Yrotation"){  MV->rotate(compute*M_PI/180, 0, 1, 0);  }

            else if (channel[j] == "Zrotation"){  MV->rotate(compute*M_PI/180, 0, 0, 1);  }
        }
    }
    glm::mat4 A = MV->topMatrix();
    MV -> popMatrix();

    return A;
}



//static vector<DAGNode*> bfs(DAGNode* root) {
static void drawBVH(shared_ptr<Program> progr, shared_ptr<MatrixStack> P, shared_ptr<MatrixStack> MV, DAGNode* root, int i) {

    vector<DAGNode*> Q;
    vector<DAGNode*> abody;
    
    map<string, glm::mat4> storage;
    
    Q.push_back(root);

    while (!Q.empty()) {
        DAGNode* curr = Q.front();
        Q.erase(Q.begin());
        abody.push_back(curr);
                
        // the root
        if (curr->name == "Hips"){
            storage[curr->name] = doOTR(MV, curr, i);
        }
        else{ // the kid
            
            MV -> pushMatrix();
            MV -> loadIdentity();
            
            MV -> multMatrix(storage[curr->parent->name]);
            MV -> multMatrix(doOTR(MV, curr, i));

            storage[curr->name] = MV -> topMatrix();
            
            MV->popMatrix();
        }
        
        progr -> bind();
        MV->pushMatrix();
                
        if(curr->parent != NULL){

            glm::mat4 from = storage[curr->name];
            glm::mat4 to = storage[curr->parent->name];

            GLfloat lmeat[2][3] = { { to[3][0], to[3][1], to[3][2]},  { from[3][0], from[3][1], from[3][2]} };
            draw_line(progr, P, MV, lmeat, 1, 0, 0);
        }
        
        MV -> multMatrix(storage[curr->name]);
        
        if(play==0){ draw_axis(progr, P, MV, 0.07); }   // draw an axis at the joint's position
        else{ draw_axis(progr, P, MV, 0.12); }          // draw an axis at the joint's position
        
        if(play != 0){  MV -> scale(0.7, 0.7, 0.7);  }
        
        curr->draw(prog, P, MV, RES_DIR, play );
        
        MV->popMatrix();
        progr->unbind();
        
        
        vector <DAGNode*> childs = curr->children;
        // BFS
        for (DAGNode* c : childs){
            Q.push_back(c);
        }
    }
}

static void render()
{
	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	float aspect = width/(float)height;
	glViewport(0, 0, width, height);

	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Create matrix stacks.
	auto P = make_shared<MatrixStack>();
	auto MV = make_shared<MatrixStack>();
	// Apply projection.
	P->pushMatrix();
	P->multMatrix(glm::perspective((float)(45.0*M_PI/180.0), aspect, 0.01f, 100.0f));
	// Apply camera transform.
	MV->pushMatrix();
	MV->translate(glm::vec3(0, 0, -5));
    
    MV->translate(glm::vec3(-0.4, -0.35, 0));

    MV->rotate(0.1, 1, 0, 0);
    MV->rotate(-0.2, 0, 1, 0);
    
    //Draw Axis.
    prog3->bind();
    MV->pushMatrix();
    draw_axis(prog3, P, MV,1);
    MV->popMatrix();
    prog3->unbind();
    
    
    //Draw tea pot
//    prog->bind();
//    MV->pushMatrix();
//
//    MV->scale(0.2, 0.2, 0.2);
//    MV->translate(0,1,0);
//    MV->rotate(M_PI, 0, 1, 0);
//
//    drawShape(prog, P, MV, shape_teapot);
//
//    MV->popMatrix();
//    prog->unbind();
//
    
    if(play==0){    // zhe gemen beiduizhe wone, baiyan
        MV->translate(glm::vec3(1.3, 0, 0));
        MV->scale(1.8);
        MV->rotate(M_PI, 0, 1, 0);
    }
    
    //Draw BVH. use prog3
    prog3->bind();
    MV->pushMatrix();
    MV->translate(glm::vec3(0.4, -0.2, 0));

    drawBVH(prog3, P, MV, motion->root, fc);
        
    MV->popMatrix();
    prog3->unbind();
    
	// Pop matrix stacks.
	MV->popMatrix();
	P->popMatrix();
    
    // frame counter increment/reset
    if (fc+step >= motion->numFrames  and  sign == 1){
        // two ways of reset
        // fc -= motion->numFrames;
        fc = 0;
    }
    else if(fc-step < 0 and sign == -1){
        fc = motion->numFrames-1 ;
    }
    else{
        fc += sign*step;
    }
    
	GLSL::checkError(GET_FILE_LINE);
}

int main(int argc, char **argv)
{
	if(argc < 2) {
		cout << "Please specify the resource directory." << endl;
		return 0;
	}
	RES_DIR = argv[1] + string("/");

	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
    
	// https://en.wikipedia.org/wiki/OpenGL
    // hint to use OpenGL 4.1 on all paltforms
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(640, 480, "Hanzhi Zhang 260908416", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	glGetError(); // A bug in glewInit() causes an error that we can safely ignore.
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
    
    
	// Initialize scene.
	init();
	// Loop until the user closes the window.

//    cout << "num channel" << motion->numChannels << endl;
    
	while(!glfwWindowShouldClose(window)) {
        
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
