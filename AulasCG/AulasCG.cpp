#include <iostream>
#include <stdio.h>
#include <GL\glew.h>
#include <GLFW\glfw3.h>

const GLint WIDTH = 800, HEIGHT = 600; 

int main(){
    glfwInit();  

    if (!glfwInit()) {
        printf("Não foi possível iniciar o GLFW.");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(WIDTH,HEIGHT, "Computação Gráfica", NULL, NULL); 

    if (!window) {
        printf("Não foi possível iniciar a janela.");
        glfwTerminate();
        return 1;
    }

    GLint bufferWidth, bufferHight;

    glfwGetFramebufferSize(window, &bufferWidth, &bufferHight);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();
    if (glewInit() != GLEW_OK) {
        printf("Não foi possível iniciar o glew.");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glViewport(0, 0, bufferWidth, bufferHight);
    while (!glfwWindowShouldClose(window)) {
        // cria um evento
        glfwPollEvents();

        // define a cor do fundo
        glClearColor(0.7f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
    }

}

