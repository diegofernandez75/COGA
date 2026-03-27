#ifndef CAMARA_GRUA_H
#define CAMARA_GRUA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CamaraGrua {
public:
    int modoActual; // 1 = 1ª persona, 2 = 3ª persona, 3 = Cenital
    float anguloHorizontal, anguloVertical; // Angulos para rotar con las flechas (orbital)

    CamaraGrua(int modoInicial = 3);

    // pasamos dónde está la grúa para que calcule la vista
    glm::mat4 GetViewMatrix(float gruaPx, float gruaPy, float gruaPz, float gruaAngulo);
    
    void setModo(int nuevoModo);
    void rotar(float deltaHorizontal, float deltaVertical); // Funcion para rotar la vista
};

#endif