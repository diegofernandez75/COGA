#include "camara.h"
#include <cmath> //  Para usar sin() y cos()

// Configuracion inicial de la camara al comenzar
CamaraGrua::CamaraGrua(int modoInicial) {
    modoActual = modoInicial;
    anguloHorizontal = 180.0f; // Empieza desde atras de la grúa
    anguloVertical = 89.9f;   // Empieza mirando desde arriba (cenital)
}

// Metodo para alternar entre las diferentes vistas (1, 2 o 3)
void CamaraGrua::setModo(int nuevoModo) {
    modoActual = nuevoModo;
}

// Rotar la cámara (sumar delta de ángulos)
void CamaraGrua::rotar(float deltaHorizontal, float deltaVertical) {
    anguloHorizontal += deltaHorizontal;
    anguloVertical += deltaVertical;
    // Límites para evitar que la cámara de la vuelta completa sobre el eje vertical
    if (anguloVertical > 89.9f) anguloVertical = 89.9f;
    if (anguloVertical < 5.0f) anguloVertical = 5.0f;
}

// La función principal que calcula dónde está la cámara y hacia dónde mira
glm::mat4 CamaraGrua::GetViewMatrix(float gruaPx, float gruaPy, float gruaPz, float gruaAngulo) {
    glm::mat4 view;

    if (modoActual == 1) {
        // Primera persona: la cámara está encima de la cabina mirando hacia donde va la grúa
        glm::vec3 posCam(
            gruaPx + 3.0f * (float)sin(glm::radians(gruaAngulo)),
            gruaPy + 4.0f,
            gruaPz + 3.0f * (float)cos(glm::radians(gruaAngulo))
        );
        glm::vec3 target(
            gruaPx + 25.0f * (float)sin(glm::radians(gruaAngulo)),
            1.0f,
            gruaPz + 25.0f * (float)cos(glm::radians(gruaAngulo))
        );
        view = glm::lookAt(posCam, target, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else if (modoActual == 2) {
        // Tercera persona: la cámara sigue a la grúa desde atrás a distancia fija
        glm::vec3 posCam(
            gruaPx - 20.0f * (float)sin(glm::radians(gruaAngulo)),
            gruaPy + 10.0f,
            gruaPz - 20.0f * (float)cos(glm::radians(gruaAngulo))
        );
        glm::vec3 target(
            gruaPx + (float)sin(glm::radians(gruaAngulo)),
            10.0f,
            gruaPz + (float)cos(glm::radians(gruaAngulo))
        );
        view = glm::lookAt(posCam, target, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else {
        // Cenital : cámara dinámica que sigue a la grúa y rota con las flechas
        float dist = 60.0f;
        glm::vec3 posRelative(
            dist * cos(glm::radians(anguloVertical)) * sin(glm::radians(anguloHorizontal)),
            dist * sin(glm::radians(anguloVertical)),
            dist * cos(glm::radians(anguloVertical)) * cos(glm::radians(anguloHorizontal))
        );
        glm::vec3 posCam = glm::vec3(gruaPx, gruaPy, gruaPz) + posRelative;
        glm::vec3 target(gruaPx, gruaPy, gruaPz);
        view = glm::lookAt(posCam, target, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    return view;
}