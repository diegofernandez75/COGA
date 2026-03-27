# ============================================================
#  Makefile - Grua OpenGL
#  Compatible con MinGW (Windows) y GCC/Clang (Linux/macOS)
# ============================================================

# --- Compilador ---
CXX      = g++
CC       = gcc

# --- Nombre del ejecutable ---
TARGET   = grua_bin

# --- Directorios ---
SRC_DIR  = Grua
LIB_DIR  = Grua/glad

# --- Archivos fuente ---w
SRCS_CPP = $(SRC_DIR)/main.cpp \
            $(SRC_DIR)/camara.cpp \
            $(SRC_DIR)/objetos.cpp

SRCS_C   = $(SRC_DIR)/glad.c

OBJS     = main.o camara.o objetos.o glad.o

# --- Flags de inclusion de headers ---
INCLUDES = -I$(LIB_DIR) \
           -I$(LIB_DIR)/glad \
           -I$(LIB_DIR)/glfw \
           -I$(LIB_DIR)/glm

# --- Flags de compilacion ---
CXXFLAGS = $(INCLUDES) -std=c++17 -Wall -O2
CFLAGS   = $(INCLUDES) -O2

# ============================================================
#  Deteccion automatica de plataforma
# ============================================================
ifeq ($(OS),Windows_NT)
    # Windows con MinGW
    LDFLAGS  = -L$(LIB_DIR) -lglfw3dll -lopengl32 -lgdi32
    COPY_DLL = copy $(LIB_DIR)\glfw3.dll glfw3.dll
    EXE      = $(TARGET).exe
else
    # Linux / macOS
    LDFLAGS  = -lglfw -lGL -ldl -lpthread -lm
    COPY_DLL = @echo "No DLL needed on this platform."
    EXE      = $(TARGET)
endif

# ============================================================
#  Reglas
# ============================================================
all: $(EXE)
	@echo ""
	@echo ">>> Compilacion exitosa: $(EXE)"
	@echo ">>> Asegurate de que glfw3.dll esta junto al ejecutable (Windows)"

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)
	$(COPY_DLL)

main.o: $(SRC_DIR)/main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

camara.o: $(SRC_DIR)/camara.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

objetos.o: $(SRC_DIR)/objetos.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

glad.o: $(SRC_DIR)/glad.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	del /Q *.o $(EXE) 2>nul || true
else
	rm -f $(OBJS) $(EXE)
endif

.PHONY: all clean