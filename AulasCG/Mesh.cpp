#include "Mesh.h"

Mesh::Mesh() {
	VAO = 0;
	VBO = 0;
	IBO = 0;
}

Mesh::~Mesh() {
	if (VAO != 0) glDeleteBuffers(1, &VBO);
	if (VAO != 0) glDeleteBuffers(1, &IBO);
	if (VAO != 0) glDeleteBuffers(1, &VAO);
}

void Mesh::CreateMesh(GLfloat* vertices, unsigned int numOfVertices, GLuint* indices, unsigned int numOfIndices) {
	Mesh::numOfIndices = numOfIndices;

	//VAO
	glGenVertexArrays(1, &VAO); //Gera um VAO ID
	glBindVertexArray(VAO); //Atribuindo o ID ao VAO

	glGenBuffers(1, &IBO); // Cria o IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); // Coloca o IBO em contexto
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numOfIndices, indices, GL_STATIC_DRAW);

	//Carregar os dados de vértice para a placa de vídeo
	//Vertex Buffer Object: VBO
	glGenBuffers(1, &VBO); //Gera um VBO ID
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //Transforma o VBO em um Array Buffer
	glBufferData(GL_ARRAY_BUFFER, numOfVertices, vertices, GL_STATIC_DRAW); //Copia os dados ao VBO

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

void Mesh::RenderMesh() {
	glBindVertexArray(VAO);

	/*
	* Desenha o Programa em 3D
	*/
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}