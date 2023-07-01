#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader/Shader.h>
#include <GLFW/glfw3.h>

class Object
{
public:
    string path;
    GLuint texID;
    GLuint vao;
    int nVerts;

    Object(string value)
    {
        path = value;
    }

    string getObjPath()
    {
        std::string objPath = path;
        objPath.append(".obj");
        return objPath;
    }

    string getMtlPath()
    {
        std::string mtlPath = path;
        mtlPath.append(".mtl");
        return mtlPath;
    }
};