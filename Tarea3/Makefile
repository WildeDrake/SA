# ------------------------
# Makefile para proyecto MISP
# ------------------------

CXX = g++
CXXFLAGS = -O2
SRC_DIR = ./src
BIN_DIR = .

# Lista de ejecutables
EXECS = test_brkga

# ------------------------
# Regla por defecto
# ------------------------
all: $(EXECS)

# Compilar BRKGA
test_brkga: $(SRC_DIR)/test_brkga.cpp $(SRC_DIR)/BRKGA.cpp $(SRC_DIR)/utils.cpp
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@

# ------------------------
# Limpiar ejecutables
# ------------------------
clean:
	rm -f $(BIN_DIR)/$(EXECS)


# mingw32-make 
# make
# o directamente
# g++ -O2 src/test_brkga.cpp src/BRKGA.cpp src/utils.cpp -o ./test_brkga