#ifndef OBJETOS_H
#define OBJETOS_H

#include <glad/glad.h>

// Prepara un cubo con el color que le pasemos (r, g, b)
void inicializarCuboColor(float r, float g, float b, unsigned int &VAO, unsigned int &VBO);
// Crea un plano en el suelo para el escenario
void inicializarPlano(float r, float g, float b, unsigned int &VAO, unsigned int &VBO);

#endif
