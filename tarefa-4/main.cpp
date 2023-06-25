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

// Assinatura de funções
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
int loadTexture(string path);
vector<GLfloat> loadOBJ(string filepath, int &nVerts);
std::string loadMTL(string filePath);
int setupSprite(vector<GLfloat> vbuffer);
glm::mat4 getModel();

// Constantes
const std::string MODEL = "../models/Suzanne/SuzanneTriTextured";
const GLuint WIDTH = 800;
const GLuint HEIGHT = 800;
const glm::vec3 CAMERA_POS = glm::vec3(0.0, 0.0, 3.0);

// Variáveis de posicionamento
bool rotateX = false;
bool rotateY = false;
bool rotateZ = false;
GLfloat translateX = 0.0f;
GLfloat translateY = 0.0f;
GLfloat translateZ = 0.0f;
float scale = 1.0f;

// Variáveis de iluminação
vector<GLfloat> ka;
vector<GLfloat> ks;
float ns;

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
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "CG Tarefa 4 - Nicole", nullptr, nullptr);
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

    // Compilando e buildando o programa de shader
    Shader shader("../shaders/Phong.vs", "../shaders/Phong.fs");

    // Define arquivos OBJ e MTL
    std::string objPath = MODEL;
    objPath += ".obj";
    std::string mtlPath = MODEL;
    mtlPath += ".mtl";

    // Setando VAO e VBO com textura
    int nVerts;
    std::string modelName = MODEL;
    std::string textureName = loadMTL(mtlPath);
    GLuint texID = loadTexture(textureName);
    vector<GLfloat> vertices = loadOBJ(objPath, nVerts);
    GLuint VAO = setupSprite(vertices);

    glUseProgram(shader.ID);
    glUniform1i(glGetUniformLocation(shader.ID, "tex_buffer"), 0);

    // Propriedades de material
    shader.setVec3("ka", ka[0], ka[1], ka[2]);
    shader.setFloat("kd", 0.5);
    shader.setVec3("ks", ks[0], ks[1], ks[2]);
    shader.setFloat("q", ns);

    // Definindo a fonte de luz pontual
    shader.setVec3("lightPos", -5.0f, 200.0f, 5.0f);
    shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

    // Posição de câmera
    shader.setVec3("cameraPos", CAMERA_POS[0], CAMERA_POS[1], CAMERA_POS[2]);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        // Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funcoes de callback correspondentes
        glfwPollEvents();

        // Definindo as dimensoes da viewport com as mesmas dimensoes da janela da aplicacao
        int width;
        int height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        // Matriz de view -- posicao e orientacao da camera
        glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        shader.setMat4("view", value_ptr(view));

        // Matriz de projecao perspectiva - definindo o volume de visualizacao (frustum)
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        shader.setMat4("projection", glm::value_ptr(projection));

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

vector<GLfloat> loadOBJ(string filepath, int &nVerts)
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
    nVerts = vbuffer.size() / 8;

    return vbuffer;
}

int setupSprite(vector<GLfloat> vbuffer)
{
    GLuint VAO;
    GLuint VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * (sizeof(GLfloat)), vbuffer.data(), GL_STATIC_DRAW);
    glBindVertexArray(VAO);

    // Posição
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);

    // Texture
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Vetor Normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

int loadTexture(const string path)
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

std::string loadMTL(const string filePath)
{

    std::ifstream file(filePath);
    std::string mapKdTextureName = "";
    std::string line;

    if (!file.is_open())
    {
        std::cout << "Failed to open MTL file: " << filePath << std::endl;
        return mapKdTextureName;
    }

    while (std::getline(file, line))
    {
        if (line.length() > 0)
        {

            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "map_Kd")
            {
                iss >> mapKdTextureName;
            }
            else if (prefix == "Ka")
            {
                glm::vec3 temp_ka;
                iss >> temp_ka.x >> temp_ka.y >> temp_ka.z;

                ka.push_back(temp_ka.x);
                ka.push_back(temp_ka.y);
                ka.push_back(temp_ka.z);
            }
            else if (prefix == "Ks")
            {
                glm::vec3 temp_ks;
                iss >> temp_ks.x >> temp_ks.y >> temp_ks.z;

                ks.push_back(temp_ks.x);
                ks.push_back(temp_ks.x);
                ks.push_back(temp_ks.x);
            }
            else if (prefix == "Ns")
            {
                iss >> ns;
            }
        }
    }

    mapKdTextureName.erase(std::remove(mapKdTextureName.begin(), mapKdTextureName.end(), '\r'), mapKdTextureName.end());

    file.close();
    return mapKdTextureName;
}