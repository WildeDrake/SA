# ------------------------
# Makefile para proyecto MISP
# ------------------------

CXX = g++
CXXFLAGS = -O2 -std=c++1z
SRC_DIR = ./src
BIN_DIR = .

# Lista de ejecutables
EXECS = testILS Experimento

# ------------------------
# Regla por defecto
# ------------------------
all: $(EXECS)

# Compilar ILS
testILS: $(SRC_DIR)/test_ILS.cpp $(SRC_DIR)/ILS.cpp $(SRC_DIR)/greedyrand1.cpp $(SRC_DIR)/utils.cpp
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@

# Compilar Experimento
Experimento: $(SRC_DIR)/Experimento.cpp $(SRC_DIR)/ILS.cpp $(SRC_DIR)/greedyrand1.cpp $(SRC_DIR)/utils.cpp
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@

# ------------------------
# Limpiar ejecutables
# ------------------------
clean:
	rm -f $(BIN_DIR)/$(EXECS)


# mingw32-make 
# make
# o directamente 
# g++ -O2 src/testILS.cpp src/ILS.cpp src/greedyrand1.cpp src/utils.cpp -o ./test_ILS
# g++ -O2 src/Experimento.cpp src/ILS.cpp src/greedyrand1.cpp src/utils.cpp -o ./Experimento