#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


/*
CONTROLES DEL PROGRAMA:

ESC  Cerrar la aplicación


CAMARAS:
0 Cámara libre orbital
1 Cámara desde el Sol mirando hacia los planetas interiores
2 Cámara desde la Luna enfocando la Tierra
3 Cámara desde Saturno mirando al Sol

MOVIMIENTO:
Flechas izquierda/derecha: rotación horizontal
Flechas arriba/abajo: zoom (acercar/alejar)
W / S: inclinación vertical de la cámara

EXTRAS:
ESPACIO Pausar / reanudar el movimiento del sistema solar
letra o Mostrar / ocultar las órbitas de los planetas
*/


#define PI 3.14159265359f

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

// Índices del array de cuerpos celestes
#define IDX_SOL      0
#define IDX_MERCURIO 1
#define IDX_VENUS    2
#define IDX_TIERRA   3
#define IDX_LUNA     4
#define IDX_MARTE    5
#define IDX_JUPITER  6
#define IDX_SATURNO  7
#define IDX_URANO    8
#define IDX_NEPTUNO  9
#define IDX_ISS      10
#define NUM_CUERPOS  11

// --- ESTRUCTURA BASE ---
typedef struct { unsigned int vao, vbo, ebo; int numIndices; } FiguraGrafica;

// --- ESTRUCTURA CUERPO CELESTE (Punto 1) ---
typedef struct {
    float px, py, pz;
    float anguloRotacion, anguloTraslacion;
    float velRotacion, velTraslacion;
    float escala;
    glm::vec3 color;
    unsigned int vao; int numIndices;
} CuerpoCeleste;

// --- SHADERS ---
const char* vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 model; uniform mat4 view; uniform mat4 projection;\n"
"void main() { gl_Position = projection * view * model * vec4(aPos, 1.0); }\0";

const char* fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec3 colorPlaneta;\n"
"void main() { FragColor = vec4(colorPlaneta, 1.0); }\0";

// Lógica Temporal 
void actualizarMovimiento(CuerpoCeleste* cuerpo, float lapsoTime) {
    cuerpo->anguloRotacion += cuerpo->velRotacion * lapsoTime;
    cuerpo->anguloTraslacion += cuerpo->velTraslacion * lapsoTime;
    if (cuerpo->anguloRotacion > 2.0f * PI) cuerpo->anguloRotacion -= 2.0f * PI;
    if (cuerpo->anguloTraslacion > 2.0f * PI) cuerpo->anguloTraslacion -= 2.0f * PI;
}


// Creación de geometría

FiguraGrafica crearEsfera(float radio, int sectores, int anillos) {
    FiguraGrafica fig;
    int numVertices = (sectores + 1) * (anillos + 1); fig.numIndices = sectores * anillos * 6;
    float* vertices = (float*)malloc(numVertices * 3 * sizeof(float));
    unsigned int* indices = (unsigned int*)malloc(fig.numIndices * sizeof(unsigned int));
    int vIdx = 0;
    for (int i = 0; i <= anillos; ++i) {
        float v = (float)i / (float)anillos; float phi = v * PI;
        for (int j = 0; j <= sectores; ++j) {
            float u = (float)j / (float)sectores; float theta = u * 2.0f * PI;
            vertices[vIdx++] = radio * cosf(theta) * sinf(phi);
            vertices[vIdx++] = radio * cosf(phi);
            vertices[vIdx++] = radio * sinf(theta) * sinf(phi);
        }
    }
    int iIdx = 0;
    for (int i = 0; i < anillos; ++i) {
        for (int j = 0; j < sectores; ++j) {
            int p1 = i * (sectores + 1) + j; int p2 = p1 + sectores + 1;
            indices[iIdx++] = p1; indices[iIdx++] = p2; indices[iIdx++] = p1 + 1;
            indices[iIdx++] = p1 + 1; indices[iIdx++] = p2; indices[iIdx++] = p2 + 1;
        }
    }
    glGenVertexArrays(1, &fig.vao); glGenBuffers(1, &fig.vbo); glGenBuffers(1, &fig.ebo);
    glBindVertexArray(fig.vao); glBindBuffer(GL_ARRAY_BUFFER, fig.vbo);
    glBufferData(GL_ARRAY_BUFFER, numVertices * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fig.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, fig.numIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); glEnableVertexAttribArray(0); glBindVertexArray(0);
    free(vertices); free(indices); return fig;
}

FiguraGrafica crearOrbita(int puntos) {
    FiguraGrafica fig; fig.numIndices = puntos;
    float* vertices = (float*)malloc(puntos * 3 * sizeof(float));
    for (int i = 0; i < puntos; ++i) {
        float theta = 2.0f * PI * (float)i / (float)puntos;
        vertices[i * 3 + 0] = cosf(theta); vertices[i * 3 + 1] = 0.0f; vertices[i * 3 + 2] = sinf(theta);
    }
    glGenVertexArrays(1, &fig.vao); glGenBuffers(1, &fig.vbo);
    glBindVertexArray(fig.vao); glBindBuffer(GL_ARRAY_BUFFER, fig.vbo);
    glBufferData(GL_ARRAY_BUFFER, puntos * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); glEnableVertexAttribArray(0); glBindVertexArray(0);
    free(vertices); return fig;
}

// Compilación de shaders

unsigned int compilarShaders() {
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); glCompileShader(vertexShader);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); glCompileShader(fragmentShader);
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader); glAttachShader(shaderProgram, fragmentShader); glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader); glDeleteShader(fragmentShader);
    return shaderProgram;
}


// Inicialización del array de cuerpos celestes

void inicializarCuerpos(CuerpoCeleste* c, unsigned int vao, int numIndices) {
    //                px      py    pz    angRot angTras velRot velTras escala  color
    c[IDX_SOL] = { 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.5f,  0.0f,  2.0f,  glm::vec3(1.0f,  0.8f,  0.0f),  vao, numIndices };
    c[IDX_MERCURIO] = { 3.0f,  0.0f, 0.0f, 0.0f, 0.0f, 2.0f,  2.0f,  0.3f,  glm::vec3(0.6f,  0.5f,  0.4f),  vao, numIndices };
    c[IDX_VENUS] = { 5.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.5f,  1.5f,  0.5f,  glm::vec3(0.9f,  0.7f,  0.4f),  vao, numIndices };
    c[IDX_TIERRA] = { 7.0f,  0.0f, 0.0f, 0.0f, 0.0f, 2.0f,  1.0f,  0.6f,  glm::vec3(0.1f,  0.4f,  1.0f),  vao, numIndices };
    c[IDX_LUNA] = { 1.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,  3.0f,  0.2f,  glm::vec3(0.75f, 0.75f, 0.75f), vao, numIndices };
    c[IDX_MARTE] = { 9.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.8f,  0.8f,  0.4f,  glm::vec3(1.0f,  0.3f,  0.1f),  vao, numIndices };
    c[IDX_JUPITER] = { 15.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.0f,  0.4f,  1.5f,  glm::vec3(0.8f,  0.6f,  0.4f),  vao, numIndices };
    c[IDX_SATURNO] = { 20.0f, 0.0f, 0.0f, 0.0f, 0.0f, 4.5f,  0.3f,  1.3f,  glm::vec3(0.9f,  0.8f,  0.5f),  vao, numIndices };
    c[IDX_URANO] = { 25.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.0f,  0.2f,  1.0f,  glm::vec3(0.5f,  0.8f,  1.0f),  vao, numIndices };
    c[IDX_NEPTUNO] = { 30.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.2f,  0.15f, 1.0f,  glm::vec3(0.2f,  0.3f,  0.9f),  vao, numIndices };
    c[IDX_ISS] = { 0.8f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  5.0f,  0.08f, glm::vec3(0.9f,  0.9f,  0.9f),  vao, numIndices };
}


// Actualizar todos los cuerpos

void actualizarTodos(CuerpoCeleste* c, float deltaTime) {
    for (int i = 0; i < NUM_CUERPOS; i++)
        actualizarMovimiento(&c[i], deltaTime);
}

// Controles de cámara y estado 
void procesarInput(GLFWwindow* window, int* modoCamara,
    float* camDistancia, float* camAnguloH, float* camAnguloV,
    int* pausado, int* mostrarOrbitas,
    int* teclaEspacioAntes, int* teclaOAntes,
    float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, 1);

    // Selección de cámara
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) *modoCamara = 0;
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) *modoCamara = 1;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) *modoCamara = 2;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) *modoCamara = 3;

    // Pausa con ESPACIO (toggle, solo dispara una vez por pulsación)
    int espacioAhora = glfwGetKey(window, GLFW_KEY_SPACE);
    if (espacioAhora == GLFW_PRESS && *teclaEspacioAntes == GLFW_RELEASE)
        *pausado = !(*pausado);
    *teclaEspacioAntes = espacioAhora;

    // Mostrar/ocultar órbitas con O (toggle, solo dispara una vez por pulsación)
    int oAhora = glfwGetKey(window, GLFW_KEY_O);
    if (oAhora == GLFW_PRESS && *teclaOAntes == GLFW_RELEASE)
        *mostrarOrbitas = !(*mostrarOrbitas);
    *teclaOAntes = oAhora;

    // Movimiento de cámara libre (solo modo 0)
    if (*modoCamara == 0) {
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) *camAnguloH -= 50.0f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) *camAnguloH += 50.0f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) *camDistancia -= 40.0f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) *camDistancia += 40.0f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) *camAnguloV += 30.0f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) *camAnguloV -= 30.0f * deltaTime;
        if (*camDistancia < 5.0f) *camDistancia = 5.0f;
    }
}

// Cálculo de la vista según modo de cámara 

glm::mat4 calcularVista(int modoCamara, float camDistancia, float camAnguloH, float camAnguloV, CuerpoCeleste* c) {
    glm::vec3 posTierra = glm::vec3(cosf(c[IDX_TIERRA].anguloTraslacion) * c[IDX_TIERRA].px, 0.0f, -sinf(c[IDX_TIERRA].anguloTraslacion) * c[IDX_TIERRA].px);
    glm::vec3 posLuna = posTierra + glm::vec3(cosf(c[IDX_LUNA].anguloTraslacion) * c[IDX_LUNA].px, 0.0f, -sinf(c[IDX_LUNA].anguloTraslacion) * c[IDX_LUNA].px);
    glm::vec3 posSaturno = glm::vec3(cosf(c[IDX_SATURNO].anguloTraslacion) * c[IDX_SATURNO].px, 0.0f, -sinf(c[IDX_SATURNO].anguloTraslacion) * c[IDX_SATURNO].px);

    glm::mat4 view = glm::mat4(1.0f);
    if (modoCamara == 0) { // Cámara libre orbital
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -camDistancia));
        view = glm::rotate(view, glm::radians(camAnguloV), glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::rotate(view, glm::radians(camAnguloH), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else if (modoCamara == 1) { // Desde el Sol mirando hacia los planetas interiores
        view = glm::lookAt(glm::vec3(0.0f, 2.5f, 4.0f), glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else if (modoCamara == 2) { // Desde la Luna mirando a la Tierra
        view = glm::lookAt(posLuna + glm::vec3(0.0f, 0.3f, 0.0f), posTierra, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else if (modoCamara == 3) { // Desde Saturno mirando al Sol
        view = glm::lookAt(posSaturno + glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    return view;
}


// Dibujo de órbitas
void dibujarOrbitas(int uModel, int uColor, FiguraGrafica moldeOrbita, glm::vec3 posTierra, float pxLuna) {
    glUniform3f(uColor, 0.3f, 0.3f, 0.3f);
    glBindVertexArray(moldeOrbita.vao);
    float radios[] = { 3.0f, 5.0f, 7.0f, 9.0f, 15.0f, 20.0f, 25.0f, 30.0f };
    for (int i = 0; i < 8; i++) {
        glm::mat4 mOrb = glm::scale(glm::mat4(1.0f), glm::vec3(radios[i]));
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(mOrb)); glDrawArrays(GL_LINE_LOOP, 0, moldeOrbita.numIndices);
    }
    // Órbita de la Luna alrededor de la Tierra
    glm::mat4 mOrbLuna = glm::translate(glm::mat4(1.0f), posTierra);
    mOrbLuna = glm::scale(mOrbLuna, glm::vec3(pxLuna));
    glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(mOrbLuna)); glDrawArrays(GL_LINE_LOOP, 0, moldeOrbita.numIndices);
}

// Dibujo de todos los cuerpos celestes
void dibujarSistema(int uModel, int uColor, CuerpoCeleste* c) {
    glBindVertexArray(c[IDX_SOL].vao);
    glm::mat4 model, mTierra;

    // Sol
    model = glm::rotate(glm::mat4(1.0f), c[IDX_SOL].anguloRotacion, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(c[IDX_SOL].escala));
    glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(uColor, c[IDX_SOL].color.r, c[IDX_SOL].color.g, c[IDX_SOL].color.b);
    glDrawElements(GL_TRIANGLES, c[IDX_SOL].numIndices, GL_UNSIGNED_INT, (void*)0);

    // Tierra (referencia jerárquica para Luna e ISS)
    mTierra = glm::rotate(glm::mat4(1.0f), c[IDX_TIERRA].anguloTraslacion, glm::vec3(0.0f, 1.0f, 0.0f));
    mTierra = glm::translate(mTierra, glm::vec3(c[IDX_TIERRA].px, 0.0f, 0.0f));
    model = glm::rotate(mTierra, c[IDX_TIERRA].anguloRotacion, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(c[IDX_TIERRA].escala));
    glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(uColor, c[IDX_TIERRA].color.r, c[IDX_TIERRA].color.g, c[IDX_TIERRA].color.b);
    glDrawElements(GL_TRIANGLES, c[IDX_TIERRA].numIndices, GL_UNSIGNED_INT, (void*)0);

    // Luna (órbita relativa a la Tierra)
    model = glm::rotate(mTierra, c[IDX_LUNA].anguloTraslacion, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(c[IDX_LUNA].px, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(c[IDX_LUNA].escala));
    glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(uColor, c[IDX_LUNA].color.r, c[IDX_LUNA].color.g, c[IDX_LUNA].color.b);
    glDrawElements(GL_TRIANGLES, c[IDX_LUNA].numIndices, GL_UNSIGNED_INT, (void*)0);

    // ISS (órbita relativa a la Tierra)
    model = glm::rotate(mTierra, c[IDX_ISS].anguloTraslacion, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(c[IDX_ISS].px, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(c[IDX_ISS].escala));
    glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(uColor, c[IDX_ISS].color.r, c[IDX_ISS].color.g, c[IDX_ISS].color.b);
    glDrawElements(GL_TRIANGLES, c[IDX_ISS].numIndices, GL_UNSIGNED_INT, (void*)0);

    // Resto de planetas con órbita solar directa
    int planetasSolares[] = { IDX_MERCURIO, IDX_VENUS, IDX_MARTE, IDX_JUPITER, IDX_SATURNO, IDX_URANO, IDX_NEPTUNO };
    for (int i = 0; i < 7; i++) {
        int idx = planetasSolares[i];
        model = glm::rotate(glm::mat4(1.0f), c[idx].anguloTraslacion, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(c[idx].px, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(c[idx].escala));
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(uColor, c[idx].color.r, c[idx].color.g, c[idx].color.b);
        glDrawElements(GL_TRIANGLES, c[idx].numIndices, GL_UNSIGNED_INT, (void*)0);
    }
}

// Re-escalado si modificamos el tamaño de la ventana

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }

// MAIN
int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Sistema Solar", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window); glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress); glEnable(GL_DEPTH_TEST);

    unsigned int shaderProgram = compilarShaders();

    FiguraGrafica moldeEsfera = crearEsfera(1.0f, 36, 18);
    FiguraGrafica moldeOrbita = crearOrbita(100);

    // Array de cuerpos celestes (Requisito 3)
    CuerpoCeleste cuerpos[NUM_CUERPOS];
    inicializarCuerpos(cuerpos, moldeEsfera.vao, moldeEsfera.numIndices);

    // Uniform locations
    int uModel = glGetUniformLocation(shaderProgram, "model");
    int uView = glGetUniformLocation(shaderProgram, "view");
    int uProj = glGetUniformLocation(shaderProgram, "projection");
    int uColor = glGetUniformLocation(shaderProgram, "colorPlaneta");

    // Variables de cámara
    float camDistancia = 70.0f, camAnguloH = 0.0f, camAnguloV = 35.0f;
    int modoCamara = 0;

    // Estado de la simulación
    int pausado = 0;  // ESPACIO: pausa/reanuda el movimiento
    int mostrarOrbitas = 1;  // O: muestra/oculta las órbitas

    // Estado previo de teclas toggle (para detectar flanco de bajada)
    int teclaEspacioAntes = GLFW_RELEASE;
    int teclaOAntes = GLFW_RELEASE;

    float lastFrame = 0.0f, currentFrame, deltaTime;

    while (!glfwWindowShouldClose(window)) {
        currentFrame = glfwGetTime(); deltaTime = currentFrame - lastFrame; lastFrame = currentFrame;

        procesarInput(window, &modoCamara, &camDistancia, &camAnguloH, &camAnguloV,
            &pausado, &mostrarOrbitas,
            &teclaEspacioAntes, &teclaOAntes,
            deltaTime);

        glClearColor(0.01f, 0.01f, 0.015f, 1.0f); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);

        // Solo actualizar ángulos si no está pausado
        if (!pausado)
            actualizarTodos(cuerpos, deltaTime);

        // Posición de la Tierra para dibujar la órbita de la Luna
        glm::vec3 posTierra = glm::vec3(
            cosf(cuerpos[IDX_TIERRA].anguloTraslacion) * cuerpos[IDX_TIERRA].px, 0.0f,
            -sinf(cuerpos[IDX_TIERRA].anguloTraslacion) * cuerpos[IDX_TIERRA].px
        );

        glm::mat4 view = calcularVista(modoCamara, camDistancia, camAnguloH, camAnguloV, cuerpos);

        // Requisito 6: Re-escalado
        int w, h; glfwGetWindowSize(window, &w, &h);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)w / (float)h, 0.1f, 1000.0f);

        glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(projection));

        // Solo dibujar órbitas si están activadas
        if (mostrarOrbitas)
            dibujarOrbitas(uModel, uColor, moldeOrbita, posTierra, cuerpos[IDX_LUNA].px);

        dibujarSistema(uModel, uColor, cuerpos);

        glfwSwapBuffers(window); glfwPollEvents();
    }

    glDeleteVertexArrays(1, &moldeEsfera.vao); glDeleteBuffers(1, &moldeEsfera.vbo); glDeleteBuffers(1, &moldeEsfera.ebo);
    glDeleteVertexArrays(1, &moldeOrbita.vao); glDeleteBuffers(1, &moldeOrbita.vbo);
    glDeleteProgram(shaderProgram);
    glfwTerminate(); return 0;
}