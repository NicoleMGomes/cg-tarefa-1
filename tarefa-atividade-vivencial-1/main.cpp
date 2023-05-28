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

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

int setupShader();
int setupGeometry();
bool loadOBJ(
    const char *path,
    std::vector<glm::vec3> &out_vertices,
    std::vector<glm::vec2> &out_uvs,
    std::vector<glm::vec3> &out_normals);

const GLuint WIDTH = 1000, HEIGHT = 1000;

const GLchar *vertexShaderSource = "#version 330\n"
                                   "layout (location = 0) in vec3 position;\n"
                                   "layout (location = 1) in vec3 color;\n"
                                   "uniform mat4 model;\n"
                                   "out vec4 finalColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "gl_Position = model * vec4(position, 1.0);\n"
                                   "finalColor = vec4(color, 1.0);\n"
                                   "}\0";

const GLchar *fragmentShaderSource = "#version 330\n"
                                     "in vec4 finalColor;\n"
                                     "out vec4 color;\n"
                                     "void main()\n"
                                     "{\n"
                                     "color = finalColor;\n"
                                     "}\n\0";

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
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "CG Tarefa 1 - Nicole", nullptr, nullptr);
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

    // Definindo as dimensoes da viewport com as mesmas dimensoes da janela da aplicacao
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Compilando e buildando o programa de shader
    GLuint shaderID = setupShader();

    // Gerando um buffer simples
    GLuint VAO = setupGeometry();

    glUseProgram(shaderID);

    glm::mat4 model = glm::mat4(1); // matriz identidade;
    GLint modelLoc = glGetUniformLocation(shaderID, "model");

    model = glm::rotate(model, /*(GLfloat)glfwGetTime()*/ glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));

    glEnable(GL_DEPTH_TEST);

    // Loop da aplicacao - "game loop"
    while (!glfwWindowShouldClose(window))
    {
        // Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funcoes de callback correspondentes
        glfwPollEvents();

        // Limpa o buffer de cor
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // cor de fundo
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLineWidth(10);
        glPointSize(20);

        float angle = (GLfloat)glfwGetTime();

        model = glm::mat4(1);
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

        glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));
        // Chamada de desenho - drawcall
        // Poligono Preenchido - GL_TRIANGLES

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 360000);

        // Chamada de desenho - drawcall
        // CONTORNO - GL_LINE_LOOP

        glDrawArrays(GL_POINTS, 0, 360000);
        glBindVertexArray(0);

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

int setupShader()
{
    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Checando erros de compilacao (exibicao via log no terminal)
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Checando erros de compilacao (exibicao via log no terminal)
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // Linkando os shaders e criando o identificador do programa de shader
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Checando por erros de linkagem
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

bool loadOBJ(
    const char *path,
    std::vector<glm::vec3> &out_vertices,
    std::vector<glm::vec2> &out_uvs,
    std::vector<glm::vec3> &out_normals)
{
    std::ifstream file(path);

    if (!file)
    {
        std::cerr << "Failed to open file: " << path << std::endl;
        return false;
    }

    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v")
        {
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if (type == "vt")
        {
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        }
        else if (type == "vn")
        {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (type == "f")
        {
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            char slash;

            for (int i = 0; i < 3; ++i)
            {
                iss >> vertexIndex[i] >> slash >> uvIndex[i] >> slash >> normalIndex[i];
                vertexIndices.push_back(vertexIndex[i]);
                uvIndices.push_back(uvIndex[i]);
                normalIndices.push_back(normalIndex[i]);
            }
        }
    }

    // Populate the output vectors using the indices
    for (unsigned int i = 0; i < vertexIndices.size(); ++i)
    {
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];

        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        glm::vec2 uv = temp_uvs[uvIndex - 1];
        glm::vec3 normal = temp_normals[normalIndex - 1];

        out_vertices.push_back(vertex);
        out_uvs.push_back(uv);
        out_normals.push_back(normal);
    }

    file.close();

    return true;
}

int setupGeometry()
{
    std::vector<glm::vec3> vert;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;

    loadOBJ("/Users/i560750/Developer/Unisinos/CG/cg-tarefa-1/models/Suzanne/suzanneTri.obj", vert, uvs, normals);

    GLfloat vertices[vert.size() * 6];
    int size = 0;

    for (int i = 0; i < vert.size(); i++)
    {
        vertices[size] = vert[i].x;
        vertices[size + 1] = vert[i].y;
        vertices[size + 2] = vert[i].z;
        //Define cor randomica
        vertices[size + 3] = rand()%2;
        vertices[size + 4] = rand()%2;
        vertices[size + 5] = rand()%2;
        size += 6;
    }

    GLuint VBO, VAO;

    // Geracao do identificador do VBO
    glGenBuffers(1, &VBO);

    // Faz a conexao (vincula) do buffer como um buffer de array
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Envia os dados do array de floats para o buffer da OpenGl
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Geracao do identificador do VAO (Vertex Array Object)
    glGenVertexArrays(1, &VAO);

    // Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vertices
    // e os ponteiros para os atributos
    glBindVertexArray(VAO);

    // Atributo posicao (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    // Atributo cor (r, g, b)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Observe que isso e permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vertice
    // atualmente vinculado - para que depois possamos desvincular com seguranca
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Desvincula o VAO (e uma boa pratica desvincular qualquer buffer ou array para evitar bugs medonhos)
    glBindVertexArray(0);

    return VAO;
}
