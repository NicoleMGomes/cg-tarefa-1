#include <iostream>
#include <string>
#include <assert.h>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image/stb_image.h>
#include <shader/Shader.h>

#define MODEL "../models/Suzanne/SuzanneTriTextured"

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
int loadTexture(string path);
int loadSimpleOBJ(string filepath, int &nVerts, glm::vec3 color);
glm::mat4 getModel();
std::string readMapKdFromMTL(string filePath);

const GLuint WIDTH = 1000, HEIGHT = 1000;

bool rotateX = false, rotateY = false, rotateZ = false;
GLfloat translateX = 0.0f, translateY = 0.0f, translateZ = 0.0f;
float scale = 1.0f;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Criacao da janela GLFW
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "CG Tarefa 3 - Nicole", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Fazendo o registro da funcao de callback para a janela GLFW
    glfwSetKeyCallback(window, key_callback);

    // GLAD: carrega todos os ponteiros d funcoes da OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    // Obtendo as informacoes de versao
    const GLubyte *renderer = glGetString(GL_RENDERER); /* get renderer string */
    const GLubyte *version = glGetString(GL_VERSION);   /* version as a string */
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    // Define arquivos OBJ e MTL
    std::string objPath = MODEL;
    objPath += ".obj";
    std::string mtlPath = MODEL;
    mtlPath += ".mtl";

    // Compilando e buildando o programa de shader
    Shader shader("../shaders/sprite.vs", "../shaders/sprite.fs");

    std::string modelName = MODEL;
    std::string textureName = readMapKdFromMTL(mtlPath);
    GLuint texID = loadTexture(textureName);
    int nVerts;
    GLuint VAO = loadSimpleOBJ(objPath, nVerts, glm::vec3(0, 0, 0));

    glUseProgram(shader.ID);
    glUniform1i(glGetUniformLocation(shader.ID, "tex_buffer"), 0);

    glm::mat4 projection = glm::mat4(1); // matriz identidade
    projection = glm::ortho(-3.0, 3.0, -3.0, 3.0, -1.0, 1.0);
    GLint projLoc = glGetUniformLocation(shader.ID, "projection");
    glUniformMatrix4fv(projLoc, 1, false, glm::value_ptr(projection));

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        // Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funcoes de callback correspondentes
        glfwPollEvents();

        // Definindo as dimensoes da viewport com as mesmas dimensoes da janela da aplicacao
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        // Limpa o buffer de cor
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // cor de fundo
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLineWidth(10);
        glPointSize(20);

        glm::mat4 model = getModel();
        GLint modelLoc = glGetUniformLocation(shader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));

        // Ativando o primeiro buffer de textura (0) e conectando ao identificador gerado
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, nVerts);
        // glDrawArrays(GL_POINTS, 0, points);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0); // unbind da textura

        // Troca os buffers da tela
        glfwSwapBuffers(window);
    }
    // Pede pra OpenGL desalocar os buffers
    glDeleteVertexArrays(1, &VAO);
    // Finaliza a execucao da GLFW, limpando os recursos alocados por ela
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_X && action == GLFW_PRESS)
    {
        rotateX = true;
        rotateY = false;
        rotateZ = false;
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS)
    {
        rotateX = false;
        rotateY = true;
        rotateZ = false;
    }

    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
    {
        rotateX = false;
        rotateY = false;
        rotateZ = true;
    }

    // Escala com E (aumenta) e Q (diminui)
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        scale += 0.1f;
    }

    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    {
        scale -= 0.1f;
    }

    // Translação WASD para os eixos x e z, IJ para o eixo y
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
        case GLFW_KEY_W:
            translateY += 0.5f;
            break;
        case GLFW_KEY_A:
            translateX -= 0.5f;
            break;
        case GLFW_KEY_S:
            translateY -= 0.5f;
            break;
        case GLFW_KEY_D:
            translateX += 0.5f;
            break;
        case GLFW_KEY_I:
            translateZ += 0.5f;
            break;
        case GLFW_KEY_J:
            translateZ -= 0.5f;
            break;
        default:
            break;
        }
    }
}

glm::mat4 getModel()
{

    float angle = (GLfloat)glfwGetTime();

    glm::mat4 model = glm::mat4(1);

    if (rotateX)
    {
        model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
    }
    else if (rotateY)
    {
        model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else if (rotateZ)
    {
        model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    // Translação
    model = glm::translate(model, glm::vec3(translateX, translateY, translateZ));

    // Escala
    model = glm::scale(model, glm::vec3(scale, scale, scale));

    return model;
}

int loadSimpleOBJ(string filepath, int &nVerts, glm::vec3 color)
{
    vector<glm::vec3> vertices;
    vector<GLuint> indices;
    vector<glm::vec2> texCoords;
    vector<glm::vec3> normals;
    vector<GLfloat> vbuffer;

    ifstream inputFile;
    inputFile.open(filepath.c_str());
    if (inputFile.is_open())
    {
        char line[100];
        string sline;

        while (!inputFile.eof())
        {
            inputFile.getline(line, 100);
            sline = line;

            string word;

            istringstream ssline(line);
            ssline >> word;

            if (word == "v")
            {
                glm::vec3 v;
                ssline >> v.x >> v.y >> v.z;

                vertices.push_back(v);
            }
            if (word == "vt")
            {
                glm::vec2 vt;
                ssline >> vt.s >> vt.t;

                texCoords.push_back(vt);
            }
            if (word == "vn")
            {
                glm::vec3 vn;
                ssline >> vn.x >> vn.y >> vn.z;
                normals.push_back(vn);
            }
            if (word == "f")
            {
                string tokens[3];

                ssline >> tokens[0] >> tokens[1] >> tokens[2];

                for (int i = 0; i < 3; i++)
                {
                    // Recuperando os indices de v
                    int pos = tokens[i].find("/");
                    string token = tokens[i].substr(0, pos);
                    int index = atoi(token.c_str()) - 1;
                    indices.push_back(index);

                    vbuffer.push_back(vertices[index].x);
                    vbuffer.push_back(vertices[index].y);
                    vbuffer.push_back(vertices[index].z);
                    vbuffer.push_back(color.r);
                    vbuffer.push_back(color.g);
                    vbuffer.push_back(color.b);

                    // Recuperando os indices de vts
                    tokens[i] = tokens[i].substr(pos + 1);
                    pos = tokens[i].find("/");
                    token = tokens[i].substr(0, pos);
                    index = atoi(token.c_str()) - 1;
                    vbuffer.push_back(texCoords[index].s);
                    vbuffer.push_back(texCoords[index].t);

                    // Recuperando os indices de vns
                    tokens[i] = tokens[i].substr(pos + 1);
                    index = atoi(tokens[i].c_str()) - 1;
                    vbuffer.push_back(normals[index].x);
                    vbuffer.push_back(normals[index].y);
                    vbuffer.push_back(normals[index].z);
                }
            }
        }
    }
    else
    {
        cout << "Problema ao encontrar o arquivo " << filepath << endl;
    }

    inputFile.close();
    GLuint VBO, VAO;
    nVerts = vbuffer.size() / 11; // 3 pos + 3 cor + 3 normal + 2 texcoord

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(GLfloat), vbuffer.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    // Atributo cor (r, g, b)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Atributo coordenada de textura (s, t)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // Atributo normal do vertice (x, y, z)
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid *)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

int loadTexture(string path)
{
    GLuint texID;

    // Gera o identificador da textura na mem�ria
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    // Ajusta os par�metros de wrapping e filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Carregamento da imagem
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        if (nrChannels == 3) // jpg, bmp
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else // png
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texID;
}

std::string readMapKdFromMTL(string filePath)
{

    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cout << "Failed to open MTL file: " << filePath << std::endl;
        return "";
    }

    std::string mapKdTextureName = "";
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        if (line.substr(0, 6) == "map_Kd")
        {
            mapKdTextureName = line.substr(7);
            break;
        }
    }

    mapKdTextureName.erase(std::remove(mapKdTextureName.begin(), mapKdTextureName.end(), '\r'), mapKdTextureName.end());

    file.close();
    return mapKdTextureName;
}