# ------------------------
# Makefile para proyecto MISP
# ------------------------

CXX = g++
CXXFLAGS = -O2
SRC_DIR = ./src
BIN_DIR = .

# Lista de ejecutables
EXECS = test_greedy1 test_greedy2 test_greedy3 test_greedyrand1 Experimento

# ------------------------
# Regla por defecto
# ------------------------
all: $(EXECS)

# Compilar greedy1
test_greedy1: $(SRC_DIR)/test_greedy1.cpp $(SRC_DIR)/greedy1.cpp $(SRC_DIR)/utils.cpp
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@

# Compilar greedy2
test_greedy2: $(SRC_DIR)/test_greedy2.cpp $(SRC_DIR)/greedy2.cpp $(SRC_DIR)/utils.cpp
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@

# Compilar greedy3
test_greedy3: $(SRC_DIR)/test_greedy3.cpp $(SRC_DIR)/greedy3.cpp $(SRC_DIR)/utils.cpp
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@

# Compilar greedyrand1
test_greedyrand1: $(SRC_DIR)/test_greedyrand1.cpp $(SRC_DIR)/greedyrand1.cpp $(SRC_DIR)/utils.cpp
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@

# Compilar Experimento (usa todos los greedy + utils)
Experimento: $(SRC_DIR)/Experimento.cpp $(SRC_DIR)/greedy1.cpp $(SRC_DIR)/greedy2.cpp $(SRC_DIR)/greedy3.cpp $(SRC_DIR)/greedyrand1.cpp $(SRC_DIR)/utils.cpp
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@

# ------------------------
# Limpiar ejecutables
# ------------------------
clean:
	rm -f $(BIN_DIR)/$(EXECS)


# mingw32-make 
# o hacer: Set-Alias make mingw32-make
# para usar make en PowerShell
# make