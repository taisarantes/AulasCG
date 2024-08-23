#include <stdio.h>
#include <string.h>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>

//Int do GL que leva em consideracao a int da placa, 128bit, 64bit, ...
const GLint WIDTH = 800, HEIGHT = 600;

GLuint VAO, VBO, shaderProgram;


//Vertex Shader
static const char* vShader = "                  \n\
#version 330                                    \n\
                                                \n\
layout(location=0) in vec2 pos;                 \n\
                                                \n\
void main(){                                    \n\
  gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);   \n\
}";

//Fragment Shader
static const char* fShader = "                  \n\
#version 330                                    \n\
                                                \n\
uniform vec3 triangleColor;                     \n\
                                                \n\
out vec4 color;                                 \n\
                                                \n\
void main(){                                    \n\
  color = vec4(triangleColor, 1.0);             \n\
}";

void CreateTriagle() {
    //1. Definir os pontos dos vértices
    GLfloat vertices[] = {
        -1.0f, -1.0f, //Vértice 1 (x, y)
        1.0f, -1.0f,  //Vértice 2 (x, y)
        0.0f, 1.0f,   //Vértice 3 (x, y)
    };

    //VAO
    glGenVertexArrays(1, &VAO); //Gera um VAO ID
    glBindVertexArray(VAO); //Atribuindo o ID ao VAO

    //Carregar os dados de vértice para a placa de vídeo
    //Vertex Buffer Object: VBO
    glGenBuffers(1, &VBO); //Gera um VBO ID
    glBindBuffer(GL_ARRAY_BUFFER, VBO); //Transforma o VBO em um Array Buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Copia os dados ao VBO

    //GL_STATIC_DRAW: Os dados do vértice serão carregados uma vez e desenhados várias vezes (por exemplo, o mundo).
    //GL_DYNAMIC_DRAW: Os dados do vértice serão criados uma vez, alterados de tempos em tempos, mas desenhados muitas vezes mais do que isso.
    //GL_STREAM_DRAW : Os dados do vértice serão carregados uma vez e desenhados uma vez.

    //Vertex Attribute Pointer - Atributos dos dados na memória
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0); //0: shader location | 2: numero de valores de vertice (x,y) | GL_FLOAT: tipo dos valores
    //GL_FALSE: normalizado | 0: pular elemento (cor) | 0: offset
    //Vertex Attribute Pointer Enable
    glEnableVertexAttribArray(0); //0: shader location

    //Limpar o Buffer
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
    shaderProgram = glCreateProgram(); //Inicia o programa
    if (!shaderProgram) {
        printf("Erro ao criar o Shader Program");
        return;
    }

    //2.0 Compila o Vertex Shader
    AddShader(shaderProgram, vShader, GL_VERTEX_SHADER); //Adiciona o Vertex Shader ao programa
    //3.0 Compila o Fragment Shader
    AddShader(shaderProgram, fShader, GL_FRAGMENT_SHADER); //Adiciona o Vertex Shader ao programa

    //4.0 Cria o link do programa
    glLinkProgram(shaderProgram); //Criar o link do programa na GPU

    //5.0 Tratamento de erros
    GLint result = 0;
    GLchar eLog[1024] = { 0 };
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result); //Verifica o resultado
    if (!result) {
        glGetProgramInfoLog(shaderProgram, sizeof(eLog), NULL, eLog);
        printf("Erro ao linkar o programa: '%s'\n", eLog);
        return;
    }

    //6.0 Validação se o programa está rodando
    glValidateProgram(shaderProgram); //Validação do programa
    glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &result); //Verifica o resultado
    if (!result) {
        glGetProgramInfoLog(shaderProgram, sizeof(eLog), NULL, eLog);
        printf("Erro ao validar o programa: '%s'\n", eLog);
        return;
    }
}

int main()
{
    if (!glfwInit()) {
        printf("Não funcionou o GLFW");
        return 1;
    }

    //Limita o programa a funcionar apenas com o OpenGL3, nem maior nem menor
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //Escolhe o perfil de funcoes do OpenGL, nesse caso para o core
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //Habilita o modo de compatibilidade com funcoes antigas
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Computacao Grafica", NULL, NULL);
    if (!window) {
        printf("A janela nao pode ser criada");
        //Limpa a memoria da placa de video
        glfwTerminate();
        return 1;
    }

    int bufferWIDTH, bufferHEIGTH;
    glfwGetFramebufferSize(window, &bufferWIDTH, &bufferHEIGTH);
    //Mostra qual a janela que esta em contexto, que sera desenhado as coisas
    glfwMakeContextCurrent(window);

    //Libera as bibliotecas experimentas da plava de video
    glewExperimental = GL_TRUE;
    //Inia o Glew
    if (glewInit() != GLEW_OK) {
        printf("Glew nao foi iniciado");
        //Destroi a janela
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    //Cria a viewport indo de 0,0 até o tamanho atual da tela
    glViewport(0, 0, bufferWIDTH, bufferHEIGTH);

    CreateTriagle();
    CompileShaders();

    //Mante o while até a que a janela deva fechar
    while (!glfwWindowShouldClose(window)) {
        //Get + Handle user input events
        glfwPollEvents();

        //Clear Window (100% = 255)
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //Desenhar o triangulo
        glUseProgram(shaderProgram); //Busca o programa que está com o shader (triangulo)
        glBindVertexArray(VAO); //Bind o VAO
        GLint uniColor = glGetUniformLocation(shaderProgram, "triangleColor"); //procura a entrada chamada triangleColor
        //float r = (float)rand()/RAND_MAX;
        //float g = (float)rand()/RAND_MAX;
        //float b = (float)rand()/RAND_MAX;
        //glUniform3f(uniColor, r, g, b); //atualiza o valor da entrada com a cor vermelha

        //auto t_now = std::chrono::high_resolution_clock::now();
        //float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
        //glUniform3f(uniColor, (sin(time * 4.0f) + 1.0f) / 2.0f, 0.0f, 0.0f);

        glUniform3f(uniColor, 0.98f, 0.78f, 0.01f); //atualiza o valor da entrada com a cor vermelha
        glDrawArrays(GL_TRIANGLES, 0, 3); //Desenha um triangulo | 0: Array index | 2: Número de pontos (vértices)
        glBindVertexArray(0);
        glUseProgram(0); //remove o programa da memória

        //Aplica as mudancas em buffer na tela
        glfwSwapBuffers(window);
    }
}
