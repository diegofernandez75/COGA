#include "objetos.h"
#include <vector>

// Funcion para preparar los cubos de la grua con un color base
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

    // Generamos los buffers para que la tarjeta grafica entienda el dibujo
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Configuramos como tiene que leer los vertices y los colores del array
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Desactivamos todo para no manchar otros objetos por error (Paso de seguridad)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Esto es para dibujar el suelo verde por donde se mueve la grua
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

    // Generamos los buffers para que la tarjeta grafica entienda el dibujo
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
    //limpio buffers
    // Desactivamos todo para no manchar otros objetos 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
