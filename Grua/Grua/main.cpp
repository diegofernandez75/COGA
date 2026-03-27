#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

#include "lecturaShader_0_9.h"
#include "objetos.h"
#include "camara.h"

// Tamaño de la ventana
const unsigned int SCR_WIDTH  = 1200;
const unsigned int SCR_HEIGHT = 800;

/// 

// Tiempo entre frames para movimiento independiente de FPS
float lapsoTime = 0.0f; 
float lastTime = 0.0f;
float rotacionRuedas = 0.0f; // Rotacion acumulada de las ruedas

// Estructura de cada parte de la grua
// px,py,pz = posicion | angulo_trans = angulo de giro | velocidad = solo la base la usa
// sx,sy,sz = escala en cada eje | VAO = identificador del buffer de vertices
typedef struct {
    float px, py, pz;
    float angulo_trans;
    float velocidad;
    float sx, sy, sz;
    unsigned int VAO;
    glm::vec3 color; // Añadido segun requisitos profesionales
} objeto;

// Instanciamos cámara
CamaraGrua miCamara(3);

// Medidas objetos del pdf transformacion (ahora con colores)
objeto baseGrua       = { 0.0f,  1.5f, 0.5f, 0.0f, 0.0f, 10.0f, 2.0f, 4.0f, 0, glm::vec3(1.0f, 0.0f, 0.0f) };
objeto cabinaGrua     = { 5.0f,  1.0f, 0.0f, 0.0f, 0.0f,  2.0f, 3.0f, 4.0f, 0, glm::vec3(1.0f, 0.0f, 0.0f) };
objeto articulacion   = {-0.4f,  0.4f, 0.0f,35.0f, 0.0f,  1.0f, 1.0f, 1.0f, 0, glm::vec3(0.0f, 0.0f, 1.0f) };
objeto brazo          = { 0.0f,  3.0f, 0.0f, 0.0f, 0.0f,  0.5f, 6.0f, 0.5f, 0, glm::vec3(0.0f, 1.0f, 1.0f) };

// RUEDAS (Posicion relativa a la base)
objeto ruedaFL = { 4.0f, -0.7f,  2.2f, 0.0f, 0.0f, 1.5f, 1.5f, 0.4f, 0, glm::vec3(0.05f, 0.05f, 0.05f) };
objeto ruedaFR = { 4.0f, -0.7f, -2.2f, 0.0f, 0.0f, 1.5f, 1.5f, 0.4f, 0, glm::vec3(0.05f, 0.05f, 0.05f) };
objeto ruedaBL = {-4.0f, -0.7f,  2.2f, 0.0f, 0.0f, 1.5f, 1.5f, 0.4f, 0, glm::vec3(0.05f, 0.05f, 0.05f) };
objeto ruedaBR = {-4.0f, -0.7f, -2.2f, 0.0f, 0.0f, 1.5f, 1.5f, 0.4f, 0, glm::vec3(0.05f, 0.05f, 0.05f) };

// Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    // GLFW y config version OpenGL 3.3 core
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Grua Interactiva OpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Error al crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    // Activar test de profundidad para que los objetos se tapen correctamente
    glEnable(GL_DEPTH_TEST);

    // Cargar shaders (vertex + fragment). Esperan los uniforms: model, view, projection
    // Ajustado para que funcione y no de errores de rutas
    GLuint shaderProgram = setShaders("Grua/shader.vert", "Grua/shader.frag");
    if (shaderProgram == 0) {
        // Fallback en caso de que el directorio de trabajo sea directamente donde están los shaders
        shaderProgram = setShaders("shader.vert", "shader.frag");
    }

    // Crear VAO y VBO para cada pieza de la grua con su color
    unsigned int vboBase;
    inicializarCuboColor(1.0f, 0.0f, 0.0f, baseGrua.VAO,     vboBase);   // rojo

    unsigned int vboCabina;
    inicializarCuboColor(1.0f, 0.0f, 0.0f, cabinaGrua.VAO,   vboCabina); // rojo

    unsigned int vboArt;
    inicializarCuboColor(0.0f, 0.0f, 1.0f, articulacion.VAO, vboArt);    // azul

    unsigned int vboBrazo;
    inicializarCuboColor(0.0f, 1.0f, 1.0f, brazo.VAO,        vboBrazo);  // cian

    unsigned int vboRueda;
    inicializarCuboColor(0.2f, 0.2f, 0.2f, ruedaFL.VAO, vboRueda); // Gris

    ruedaFR.VAO = ruedaFL.VAO;
    ruedaBL.VAO = ruedaFL.VAO; 
    ruedaBR.VAO = ruedaFL.VAO;

    // Dos colores para el suelo modular infinito (PLANOS)
    unsigned int vaoSueloA, vboSueloA;
    inicializarPlano(0.0f, 1.0f, 0.0f, vaoSueloA, vboSueloA); // verde claro

    unsigned int vaoSueloB, vboSueloB;
    inicializarPlano(0.0f, 0.8f, 0.0f, vaoSueloB, vboSueloB); // verde oscuro

    // Localizaciones de los uniforms en el shader
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc  = glGetUniformLocation(shaderProgram, "view");
    unsigned int projLoc  = glGetUniformLocation(shaderProgram, "projection");

    // Bucle principal de renderizado
    while (!glfwWindowShouldClose(window))
    {
        // Calcular lapsoTime para movimiento suave
        float currentTime = (float)glfwGetTime();
        lapsoTime  = currentTime - lastTime;
        lastTime  = currentTime;

        processInput(window);

        // Matriz de proyeccion en perspectiva (45 grados de FOV)
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            1.0f, 200.0f
        );

        // Calcular la matriz de vista segun el modo de camara
        glm::mat4 view = miCamara.GetViewMatrix(baseGrua.px, baseGrua.py, baseGrua.pz, baseGrua.angulo_trans);
        
        // Limpiar pantalla
        glClearColor(0.5f, 0.8f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // --- Suelo modular infinito (centrado en la grúa) ---
        int gridRange = 25; // Cantidad de celdas a renderizar alrededor
        int centerI = (int)std::floor((baseGrua.px + 1.0f) / 2.0f);
        int centerJ = (int)std::floor((baseGrua.pz + 1.0f) / 2.0f);

        for (int i = centerI - gridRange; i <= centerI + gridRange; ++i)
        {
            for (int j = centerJ - gridRange; j <= centerJ + gridRange; ++j)
            {
                glm::mat4 modelFloor = glm::mat4(1.0f);
                modelFloor = glm::translate(modelFloor, glm::vec3((float)i * 2.0f, 0.0f, (float)j * 2.0f));
                modelFloor = glm::scale(modelFloor, glm::vec3(2.0f, 1.0f, 2.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelFloor));

                if ((i + j) % 2 == 0) glBindVertexArray(vaoSueloA); 
                else                  glBindVertexArray(vaoSueloB);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }

        // --- Grua jerarquica ---
        // Todas las piezas hijas heredan la transformacion de la base

        // 1. BASE: se traslada a su posicion y rota segun el angulo de direccion
        glm::mat4 modelBase = glm::mat4(1.0f);
        modelBase = glm::translate(modelBase, glm::vec3(baseGrua.px, baseGrua.py, baseGrua.pz));
        modelBase = glm::rotate(modelBase, glm::radians(baseGrua.angulo_trans - 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        // Guardamos el estado de la base para que las hijas partan de aqui
        glm::mat4 stackBase = modelBase;

        glm::mat4 baseDraw = glm::scale(modelBase, glm::vec3(baseGrua.sx, baseGrua.sy, baseGrua.sz));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(baseDraw));
        glBindVertexArray(baseGrua.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 2. CABINA: hija de la base, gira sobre el eje Y (torreta horizontal)
        glm::mat4 modelCabina = stackBase;
        modelCabina = glm::translate(modelCabina, glm::vec3(cabinaGrua.px, cabinaGrua.py, cabinaGrua.pz));
        modelCabina = glm::rotate(modelCabina, glm::radians(cabinaGrua.angulo_trans), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 cabinaDraw = glm::scale(modelCabina, glm::vec3(cabinaGrua.sx, cabinaGrua.sy, cabinaGrua.sz));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cabinaDraw));
        glBindVertexArray(cabinaGrua.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 3. ARTICULACION: hija de la base, gira sobre el eje Z (sube y baja el brazo)
        glm::mat4 modelArt = stackBase;
        modelArt = glm::translate(modelArt, glm::vec3(articulacion.px, articulacion.py, articulacion.pz));
        modelArt = glm::rotate(modelArt, glm::radians(articulacion.angulo_trans), glm::vec3(0.0f, 0.0f, 1.0f));

        // Guardamos el estado de la articulacion para que el brazo parta de aqui
        glm::mat4 stackArt = modelArt;

        glm::mat4 artDraw = glm::scale(modelArt, glm::vec3(articulacion.sx, articulacion.sy, articulacion.sz));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(artDraw));
        glBindVertexArray(articulacion.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 4. BRAZO: hijo de la articulacion, gira sobre el eje X (inclina el brazo)
        glm::mat4 modelBrazo = stackArt;
        modelBrazo = glm::translate(modelBrazo, glm::vec3(brazo.px, brazo.py, brazo.pz));
        modelBrazo = glm::rotate(modelBrazo, glm::radians(brazo.angulo_trans), glm::vec3(1.0f, 0.0f, 0.0f));

        glm::mat4 brazoDraw = glm::scale(modelBrazo, glm::vec3(brazo.sx, brazo.sy, brazo.sz));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(brazoDraw));
        glBindVertexArray(brazo.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 5. RUEDAS: hijas de la base, rotan segun la velocidad
        objeto* ruedas[] = {&ruedaFL, &ruedaFR, &ruedaBL, &ruedaBR};
        for (int i = 0; i < 4; ++i) {
            glm::mat4 mRueda = stackBase;
            mRueda = glm::translate(mRueda, glm::vec3(ruedas[i]->px, ruedas[i]->py, ruedas[i]->pz));
            mRueda = glm::rotate(mRueda, glm::radians(rotacionRuedas), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotacion rodaje
            mRueda = glm::scale(mRueda, glm::vec3(ruedas[i]->sx, ruedas[i]->sy, ruedas[i]->sz));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mRueda));
            glBindVertexArray(ruedas[i]->VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Liberar todos los buffers de GPU antes de cerrar
    glDeleteVertexArrays(1, &baseGrua.VAO);     glDeleteBuffers(1, &vboBase);
    glDeleteVertexArrays(1, &cabinaGrua.VAO);   glDeleteBuffers(1, &vboCabina);
    glDeleteVertexArrays(1, &articulacion.VAO); glDeleteBuffers(1, &vboArt);
    glDeleteVertexArrays(1, &brazo.VAO);        glDeleteBuffers(1, &vboBrazo);
    glDeleteVertexArrays(1, &ruedaFL.VAO);      glDeleteBuffers(1, &vboRueda);
    glDeleteVertexArrays(1, &vaoSueloA);        glDeleteBuffers(1, &vboSueloA);
    glDeleteVertexArrays(1, &vaoSueloB);        glDeleteBuffers(1, &vboSueloB);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // W acelera, X frena o marcha atras (progresivo con *lapsoTime)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) baseGrua.velocidad += 10.0f * lapsoTime;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) baseGrua.velocidad -= 10.0f * lapsoTime;

    // A gira a la izquierda, D a la derecha (Solo si hay movimiento > 0.1)
    if (std::abs(baseGrua.velocidad) > 0.1f) {
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) baseGrua.angulo_trans += 45.0f * lapsoTime;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) baseGrua.angulo_trans -= 45.0f * lapsoTime;
    }

    // K y L controlan la elevacion de la articulacion/brazo
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) articulacion.angulo_trans += 45.0f * lapsoTime;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) articulacion.angulo_trans -= 45.0f * lapsoTime;

    // Límites para evitar que el brazo dé la vuelta completa (Fisica realista)
    if (articulacion.angulo_trans > 80.0f) articulacion.angulo_trans = 80.0f;
    if (articulacion.angulo_trans <  0.0f) articulacion.angulo_trans =  0.0f;

    // Limitar velocidad maxima para que no se dispare
    if (baseGrua.velocidad >  50.0f) baseGrua.velocidad =  50.0f;
    if (baseGrua.velocidad < -50.0f) baseGrua.velocidad = -50.0f;

    // Friccion: la grua va frenando sola si no se pulsa nada
    baseGrua.velocidad *= 0.99f;

    // Actualizar rotacion de las ruedas segun velocidad
    rotacionRuedas += baseGrua.velocidad * 100.0f * lapsoTime;

    // Mover la base en la direccion que apunta segun su angulo
    baseGrua.px += baseGrua.velocidad * (float)sin(glm::radians(baseGrua.angulo_trans)) * lapsoTime;
    baseGrua.pz += baseGrua.velocidad * (float)cos(glm::radians(baseGrua.angulo_trans)) * lapsoTime;

    // Teclas 1, 2, 3 cambian el modo de camara
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) miCamara.setModo(1);
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) miCamara.setModo(2);
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) miCamara.setModo(3);

    // Control de rotacion de camara (solo modo 3 cenital) con las flechas
    if (miCamara.modoActual == 3) {
        float rotSpeed = 50.0f * lapsoTime;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  miCamara.rotar(-rotSpeed, 0.0f);
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) miCamara.rotar(rotSpeed, 0.0f);
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    miCamara.rotar(0.0f, rotSpeed);
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  miCamara.rotar(0.0f, -rotSpeed);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}