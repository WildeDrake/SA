# ------------------------
# Makefile para proyecto MISP
# ------------------------

CXX = g++
CXXFLAGS = -O2 -std=c++1z
SRC_DIR = ./src
BIN_DIR = .

# Lista de ejecutables
EXECS = test_ACO Experimento

# ------------------------
# Regla por defecto
# ------------------------
all: $(EXECS)

# Compilar ACO
test_ACO: $(SRC_DIR)/test_ACO.cpp $(SRC_DIR)/ACO.cpp $(SRC_DIR)/utils.cpp
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@

# Compilar Experimento
Experimento: $(SRC_DIR)/Experimento.cpp $(SRC_DIR)/ACO.cpp $(SRC_DIR)/utils.cpp
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@

# ------------------------
# Limpiar ejecutables
# ------------------------s
clean:
	rm -f $(BIN_DIR)/$(EXECS)


# mingw32-make 
# make
# o directamente
# g++ -O2 -std=c++1z src/test_ACO.cpp src/ACO.cpp src/utils.cpp -o ./test_ACO
# g++ -O2 -std=c++1z Experimento.cpp ACO.cpp utils.cpp -o ./Experimento



# ./test_ACO -i ..\dataset_grafos_no_dirigidos\new_1000_dataset\erdos_n1000_p0c0.1_1.graph -n 80 -a 1.0 -b 2.0 -e 0.1 -min 0.01 -max 6.0 -t 10 -irace 0 -print 1
# ./Experimento