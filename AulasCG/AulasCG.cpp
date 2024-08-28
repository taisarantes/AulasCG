#include <stdio.h>
#include <string.h>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>
#include <time.h>
#include <chrono>

const GLint WIDTH = 800, HEIGHT = 600;

GLuint VAO, VBO, IBO, programa;

// Vertex Shader (GLSL)
static const char* vShader = "						\n\
#version 330										\n\
													\n\
layout(location=0) in vec3 pos;						\n\
uniform mat4 model;									\n\
out vec4 vColor;									\n\
													\n\
void main() {										\n\
	gl_Position = model * vec4(pos, 1.0);			\n\
	vColor = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);	\n\
}";

//Fragment Shader (GLSL)
static const char* fShader = "                  \n\
#version 330                                    \n\
                                                \n\
uniform vec3 triangleColor;                     \n\
in vec4 vColor;                                 \n\
out vec4 color;                                 \n\
                                                \n\
void main(){                                    \n\
  color = vColor;					            \n\
}";

void CreateTriagle() {
	//1. Definir os pontos dos vértices
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,		//Vértice 0 (Preto)
		0.0f, 1.0f, 0.0f,		//Vértice 1 (Verde)
		1.0f, -1.0f, 0.0f,		//Vértice 2 (Vermelho)
		0.0f, 0.0f, 1.0f		//Vértice 3 (Azul)
	};

	GLuint indices[] = {
		0, 1, 2,
		1, 2, 3,
		0, 1, 3,
		0, 2, 3
	};

	//VAO
	glGenVertexArrays(1, &VAO); //Gera um VAO ID
	glBindVertexArray(VAO); //Atribuindo o ID ao VAO

	glGenBuffers(1, &IBO); // Cria o IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); // Coloca o IBO em contexto
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Carregar os dados de vértice para a placa de vídeo
	//Vertex Buffer Object: VBO
	glGenBuffers(1, &VBO); //Gera um VBO ID
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //Transforma o VBO em um Array Buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Copia os dados ao VBO

	//GL_STATIC_DRAW: Os dados do vértice serão carregados uma vez e desenhados várias vezes (por exemplo, o mundo).
	//GL_DYNAMIC_DRAW: Os dados do vértice serão criados uma vez, alterados de tempos em tempos, mas desenhados muitas vezes mais do que isso.
	//GL_STREAM_DRAW : Os dados do vértice serão carregados uma vez e desenhados uma vez.

	//Vertex Attribute Pointer - Atributos dos dados na memória
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //0: shader location | 3: numero de valores de vertice (x,y,Z) | GL_FLOAT: tipo dos valores
	//GL_FALSE: normalizado | 0: pular elemento (cor) | 0: offset
//Vertex Attribute Pointer Enable
	glEnableVertexAttribArray(0); //0: shader location

	//Limpar o Buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Limpar o Vertex Array
	glBindVertexArray(0);
}

void AddShader(GLuint program, const char* shaderCode, GLenum shaderType) {
	//Começamos a compilar cada shader
	//1. Criar um shader
	GLuint _shader = glCreateShader(shaderType);

	//2. Atribui o código do GLSL para o shader
		//2.1 Guarda a variavel localmente (converte char para GLchar*)
	const GLchar* code[1];
	code[0] = shaderCode;

	//2.2 Anexando o código a shader
	glShaderSource(_shader, 1, code, NULL); //1: numero de códigos | NULL: final da string onde encontrar NULL

	//3. Compila o shader
	glCompileShader(_shader); //compilar o shader

	//4. Tratamento de erros de compilação
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glGetShaderiv(_shader, GL_COMPILE_STATUS, &result); //Verifica o resultado
	if (!result) {
		glGetProgramInfoLog(_shader, sizeof(eLog), NULL, eLog);
		printf("Erro ao compilar o %d shader: '%s'\n", shaderType, eLog);
		return;
	}

	//5. Adiciona o shader ao programa (recebido como parâmetro)
	glAttachShader(program, _shader); //adiciona o shader ao programa
}

void CompileShaders() {
	//1.0 Cria o programa
	programa = glCreateProgram(); //Inicia o programa
	if (!programa) {
		printf("Erro ao criar o Shader Program");
		return;
	}

	//2.0 Compila o Vertex Shader
	AddShader(programa, vShader, GL_VERTEX_SHADER); //Adiciona o Vertex Shader ao programa
	//3.0 Compila o Fragment Shader
	AddShader(programa, fShader, GL_FRAGMENT_SHADER); //Adiciona o Vertex Shader ao programa

	//4.0 Cria o link do programa
	glLinkProgram(programa); //Criar o link do programa na GPU

	//5.0 Tratamento de erros
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glGetProgramiv(programa, GL_LINK_STATUS, &result); //Verifica o resultado
	if (!result) {
		glGetProgramInfoLog(programa, sizeof(eLog), NULL, eLog);
		printf("Erro ao linkar o programa: '%s'\n", eLog);
		return;
	}

	//6.0 Validação se o programa está rodando
	glValidateProgram(programa); //Validação do programa
	glGetProgramiv(programa, GL_VALIDATE_STATUS, &result); //Verifica o resultado
	if (!result) {
		glGetProgramInfoLog(programa, sizeof(eLog), NULL, eLog);
		printf("Erro ao validar o programa: '%s'\n", eLog);
		return;
	}
}

int main() {
	/*#########################
	  # Inicialização do GLFW #
	  #########################*/

	if (!glfwInit()) {
		printf("O glfw não inicializou corretamente.");
		glfwTerminate();
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	/*################################
	  # Fim da inicialização do GLFW #
	  ################################*/

	  // Criação da janela
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Computacao Grafica", NULL, NULL);

	// Caso a janela não inicie
	if (!window) {
		printf("A janela não pode ser criada.");
		glfwTerminate();
		return 1;
	}

	// Salvando o valor de Width e Height no sistema mesmo após a alteração de tamanho de janela
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);

	// Colocando a janela em contexto
	glfwMakeContextCurrent(window);

	// Utilizando as funções "experimentais" do glew
	glewExperimental = GL_TRUE;

	// Inicializando o Glew
	if (glewInit() != GLEW_OK) {
		printf("Nao foi possivel inicializar o glew.");
		glfwDestroyWindow(window);
		glfwTerminate();
		return 1;
	}
	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, bufferWidth, bufferHeight);

	// Criar o triângulo
	CreateTriagle();
	CompileShaders();

	// Enquanto a janela não precisa fechar
	while (!glfwWindowShouldClose(window)) {
		// Cria um evento
		glfwPollEvents();


		//glClearColor(r, g, b, 0.0f);
		glClearColor(0.902f, 0.902f, 0.9804f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Desenhar o triangulo
		glUseProgram(programa);
		glBindVertexArray(VAO);

		/*
		* Desenha o Programa em 3D
		*/
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		/*
		* Alterando a cor do triangulo
		*/
		GLuint uniColor = glGetUniformLocation(programa, "triangleColor");

		float r = (float)rand() / RAND_MAX;
		float g = (float)rand() / RAND_MAX;
		float b = (float)rand() / RAND_MAX;
		glUniform3f(uniColor, r, g, b); //atualiza o valor da entrada com a cor vermelha

		//auto t_now = std::chrono::high_resolution_clock::now();
		//float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
		//glUniform3f(uniColor, (sin(time * 4.0f) + 1.0f) / 2.0f, 0.0f, 0.0f);

		//glUniform3f(uniColor, 1.0f, 0.7137f, 0.7569f);
		glm::mat4 model(1.0f);

		GLuint uniModel = glGetUniformLocation(programa, "model");
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

		// glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

