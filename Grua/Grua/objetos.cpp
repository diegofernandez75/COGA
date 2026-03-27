#include "objetos.h"
#include <vector>

void inicializarCuboColor(float r, float g, float b, unsigned int &VAO, unsigned int &VBO) {
    float vertices[] = {
        // posiciones         // colores originales 
        -0.5f, -0.5f, -0.5f,  r, g, b, 
         0.5f, -0.5f, -0.5f,  r, g, b, 
         0.5f,  0.5f, -0.5f,  r, g, b, 
         0.5f,  0.5f, -0.5f,  r, g, b, 
        -0.5f,  0.5f, -0.5f,  r, g, b, 
        -0.5f, -0.5f, -0.5f,  r, g, b, 

        -0.5f, -0.5f,  0.5f,  r, g, b, 
         0.5f, -0.5f,  0.5f,  r, g, b, 
         0.5f,  0.5f,  0.5f,  r, g, b, 
         0.5f,  0.5f,  0.5f,  r, g, b, 
        -0.5f,  0.5f,  0.5f,  r, g, b, 
        -0.5f, -0.5f,  0.5f,  r, g, b, 

        -0.5f,  0.5f,  0.5f,  r, g, b, 
        -0.5f,  0.5f, -0.5f,  r, g, b, 
        -0.5f, -0.5f, -0.5f,  r, g, b, 
        -0.5f, -0.5f, -0.5f,  r, g, b, 
        -0.5f, -0.5f,  0.5f,  r, g, b, 
        -0.5f,  0.5f,  0.5f,  r, g, b, 

         0.5f,  0.5f,  0.5f,  r, g, b, 
         0.5f,  0.5f, -0.5f,  r, g, b, 
         0.5f, -0.5f, -0.5f,  r, g, b, 
         0.5f, -0.5f, -0.5f,  r, g, b, 
         0.5f, -0.5f,  0.5f,  r, g, b, 
         0.5f,  0.5f,  0.5f,  r, g, b, 

        -0.5f, -0.5f, -0.5f,  r*0.8f, g*0.8f, b*0.8f, 
         0.5f, -0.5f, -0.5f,  r*0.8f, g*0.8f, b*0.8f, 
         0.5f, -0.5f,  0.5f,  r*0.8f, g*0.8f, b*0.8f, 
         0.5f, -0.5f,  0.5f,  r*0.8f, g*0.8f, b*0.8f, 
        -0.5f, -0.5f,  0.5f,  r*0.8f, g*0.8f, b*0.8f, 
        -0.5f, -0.5f, -0.5f,  r*0.8f, g*0.8f, b*0.8f, 

        -0.5f,  0.5f, -0.5f,  r*1.1f, g*1.1f, b*1.1f, 
         0.5f,  0.5f, -0.5f,  r*1.1f, g*1.1f, b*1.1f, 
         0.5f,  0.5f,  0.5f,  r*1.1f, g*1.1f, b*1.1f, 
         0.5f,  0.5f,  0.5f,  r*1.1f, g*1.1f, b*1.1f, 
        -0.5f,  0.5f,  0.5f,  r*1.1f, g*1.1f, b*1.1f, 
        -0.5f,  0.5f, -0.5f,  r*1.1f, g*1.1f, b*1.1f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void inicializarPlano(float r, float g, float b, unsigned int &VAO, unsigned int &VBO) {
    float vertices[] = {
        // posiciones (XZ Plano) // colores
        -0.5f, 0.0f, -0.5f,      r, g, b,
         0.5f, 0.0f, -0.5f,      r, g, b,
         0.5f, 0.0f,  0.5f,      r, g, b,
         0.5f, 0.0f,  0.5f,      r, g, b,
        -0.5f, 0.0f,  0.5f,      r, g, b,
        -0.5f, 0.0f, -0.5f,      r, g, b
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Atributo 0: Posicion
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Atributo 1: Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
