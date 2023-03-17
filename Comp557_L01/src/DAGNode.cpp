/**
 COMP557 Assignment1
 Hanzhi Zhang
 260908416
 */
#include "DAGNode.h"
#include "MatrixStack.h"
#include "Program.h"

#include <iostream>
#include <cassert>

#include "GLSL.h"
#include "Shape.h"


#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>


using namespace std;
class Shape;

shared_ptr<Shape> OBJnode;

 
//string RES_DIR = "";


DAGNode::DAGNode()
{
}

DAGNode::~DAGNode()
{
}

void DAGNode::init()
{
}

//
//void DAGNode::draw(const std::shared_ptr<Program> progr, const shared_ptr<MatrixStack> P, const std::shared_ptr<MatrixStack> MV, std::string shape, std::string RES_DIR ) const
//{
//
//    OBJnode = make_shared<Shape>();
//    OBJnode->loadMesh(RES_DIR + shape);
//    OBJnode->init();
//
//    glUniformMatrix4fv(progr->getUniform("P"), 1, GL_FALSE, &P->topMatrix()[0][0]);
//    glUniformMatrix4fv(progr->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);
//    OBJnode->draw(progr);
//
//}
void DAGNode::draw(const std::shared_ptr<Program> prog, const shared_ptr<MatrixStack> P, const std::shared_ptr<MatrixStack> MV,
                   std::string RES_DIR, int play ) const{
    
    
    prog->bind();
    MV->pushMatrix();
    MV->scale(0.5, 0.5, 0.5);
    
    if(play == 0){
        MV->scale(0.5, 0.5, 0.5);
    }
    else{
        MV->scale(1.5);
    }
    
    string ashape= "teapot.obj";
    
    if (name == "Head"){
        MV->scale(1.2, 1.2, 1.2);
        ashape = "sphere.obj";
    }
    else if ( parent != NULL and parent->name == "Head"){
        double t = glfwGetTime();
        MV->rotate( (float) t, 0, 1, 0 );
        ashape = "teapot.obj";
    }
    else if ( name == "Site" or name == "EndSite"){
        
        MV->scale(0.5);
        ashape = "sphere.obj";
    }
    else{
        MV->scale(0.5);
        ashape = "cube.obj";
    }
    
    OBJnode = make_shared<Shape>();
    OBJnode->loadMesh(RES_DIR + ashape);
    OBJnode->init();
    
    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P->topMatrix()[0][0]);
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);
    OBJnode->draw(prog);

    
    MV->popMatrix();
    prog->unbind();
        
    
    
    
    
    
    
    
}
