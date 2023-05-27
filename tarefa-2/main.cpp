#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Prototipo da funcao de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Prototipos das funcoes
int setupShader();
int setupGeometry();

// Dimensoes da janela (pode ser alterado em tempo de execucao)
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Codigo fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar* vertexShaderSource = "#version 330\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"uniform mat4 model;\n"
"out vec4 finalColor;\n"
"void main()\n"
"{\n"
//...pode ter mais linhas de codigo aqui!
"gl_Position = model * vec4(position, 1.0);\n"
"finalColor = vec4(color, 1.0);\n"
"}\0";

//Codigo fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar* fragmentShaderSource = "#version 330\n"
"in vec4 finalColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = finalColor;\n"
"}\n\0";

bool rotateX=false, rotateY=false, rotateZ=false;
GLfloat translateX=0.0f, translateY=0.0f, translateZ=0.0f;
float scale = 1.0f;

int main()
{
	// Inicializacao da GLFW
	glfwInit();

	//Muita atencao aqui: alguns ambientes nao aceitam essas configuracoes
	//Voce deve adaptar para a versao do OpenGL suportada por sua placa
	//Sugestao: comente essas linhas de codigo para desobrir a versao e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Criacao da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "CG Tarefa 1 - Nicole", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da funcao de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d funcoes da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// Obtendo as informacoes de versao
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensoes da viewport com as mesmas dimensoes da janela da aplicacao
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();

	// Gerando um buffer simples, com a geometria de um triangulo
	GLuint VAO = setupGeometry();

	glUseProgram(shaderID);

	glm::mat4 model = glm::mat4(1); //matriz identidade;
	GLint modelLoc = glGetUniformLocation(shaderID, "model");

	model = glm::rotate(model, /*(GLfloat)glfwGetTime()*/glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));

	glEnable(GL_DEPTH_TEST);

	// Loop da aplicacao - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funcoes de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo
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

		//Translação
		model = glm::translate(model, glm::vec3(translateX, translateY, translateZ));

		//Escala
		model = glm::scale(model, glm::vec3(scale, scale, scale));

		glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));
		// Chamada de desenho - drawcall
		// Poligono Preenchido - GL_TRIANGLES
		
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Chamada de desenho - drawcall
		// CONTORNO - GL_LINE_LOOP
		
		glDrawArrays(GL_POINTS, 0, 36);
		glBindVertexArray(0);

		//Adiciona segundo cubo
		model = glm::translate(model, glm::vec3(2, 0, 0));

		glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glDrawArrays(GL_POINTS, 0, 36);
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

// Funcao de callback de teclado - so pode ter uma instancia (deve ser estatica se estiver dentro de uma classe) 
// chamada sempre que uma tecla for pressionada ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
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

	//Escala com E (aumenta) e Q (diminui)
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

//Esta funcao esta basntante hardcoded - objetivo e compilar e "buildar" um programa de
// shader simples e unico neste exemplo de codigo
// O codigo fonte do vertex e fragment shader esta nos arrays vertexShaderSource e
// fragmentShader source no inicio deste arquivo
// A funcao retorna o identificador do programa de shader
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
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
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
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Linkando os shaders e criando o identificador do programa de shader
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Checando por erros de linkagem
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

// Esta funcao esta bastante harcoded - objetivo e criar os buffers que armazenam a 
// geometria de um triangulo
// Apenas atributo coordenada nos vertices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A funcao retorna o identificador do VAO
int setupGeometry()
{
	// Aqui setamos as coordenadas x, y e z do triangulo e as armazenamos de forma
	// sequencial, ja visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do vertice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO �nico ou em VBOs separados
	GLfloat vertices[] = {

        //x    y    z    r    g    b
		//Base
		//virada para -y
		-0.5, -0.5, -0.5, 1.0, 1.0, 0.0,
		-0.5, -0.5,  0.5, 0.0, 1.0, 1.0,
		 0.5, -0.5, -0.5, 1.0, 0.0, 1.0,

		 -0.5, -0.5,  0.5, 1.0, 1.0, 0.0,
		  0.5, -0.5,  0.5, 0.0, 1.0, 1.0,
		  0.5, -0.5, -0.5, 1.0, 0.0, 1.0,

		 //lado a
		 //virado pro -z
		 //triangulo 1a
         -0.5, -0.5, -0.5, 1.0, 1.0, 0.0,
		 -0.5,  0.5, -0.5, 0.0, 1.0, 0.0,
		  0.5, -0.5, -0.5, 0.0, 1.0, 1.0,

		//triangulo 2a
          0.5,  0.5, -0.5, 0.0, 1.0, 0.0,
		 -0.5,  0.5, -0.5, 0.0, 1.0, 1.0,
		  0.5, -0.5, -0.5, 1.0, 1.0, 0.0,

 		// //lado b
		//virado para -x
		//triangulo 1b
		-0.5, -0.5, -0.5, 1.0, 0.0, 1.0,
		-0.5,  0.5, -0.5, 0.0, 1.0, 0.0,
		-0.5, -0.5,  0.5, 1.0, 0.0, 1.0,

 		//triangulo 2b
		-0.5, 0.5,  0.5, 1.0, 1.0, 1.0,
		-0.5, -0.5, 0.5, 0.0, 0.0, 1.0,
		-0.5, 0.5, -0.5, 1.0, 1.0, 1.0,

 		// //lado c
		//virado para +x
		//triangulo 1c
		0.5, -0.5, -0.5, 0.0, 1.0, 1.0,
		0.5,  0.5, -0.5, 1.0, 0.0, 0.0,
		0.5, -0.5,  0.5, 1.0, 0.0, 1.0,

 		//triangulo 2c
		0.5, 0.5,  0.5, 0.0, 1.0, 1.0,
		0.5, -0.5, 0.5, 0.0, 1.0, 1.0,
		0.5, 0.5, -0.5, 1.0, 1.0, 1.0,

		//lado d
		 //virado pro +z
		 //triangulo 1a
         -0.5, -0.5, 0.5, 1.0, 1.0, 1.0,
		 -0.5,  0.5, 0.5, 0.0, 0.0, 1.0,
		  0.5, -0.5, 0.5, 0.0, 1.0, 1.0,

		//triangulo 2a
          0.5,  0.5, 0.5, 0.0, 1.0, 1.0,
		 -0.5,  0.5, 0.5, 0.0, 1.0, 1.0,
		  0.5, -0.5, 0.5, 1.0, 0.0, 1.0,

		//Topo 
		//virada para +y
		-0.5, 0.5, -0.5, 1.0, 1.0, 0.0,
		-0.5, 0.5,  0.5, 0.0, 1.0, 1.0,
		 0.5, 0.5, -0.5, 1.0, 0.0, 1.0,

		 -0.5, 0.5,  0.5, 1.0, 0.0, 1.0,
		  0.5, 0.5,  0.5, 1.0, 1.0, 0.0,
		  0.5, 0.5, -0.5, 1.0, 0.0, 1.0,
	};

	GLuint VBO, VAO;

	//Geracao do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conexao (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Geracao do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vertices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localizacao no shader * (a localizacao dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se esta normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 
	
	//Atributo posicao (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Observe que isso e permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vertice 
	// atualmente vinculado - para que depois possamos desvincular com seguranca
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (e uma boa pratica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}
