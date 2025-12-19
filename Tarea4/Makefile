# ------------------------
# Makefile para proyecto MISP
# ------------------------

CXX = g++
CXXFLAGS = -O2 -std=c++1z
SRC_DIR = ./src
BIN_DIR = .

# Lista de ejecutables
EXECS = test_MMAS Experimento

# ------------------------
# Regla por defecto
# ------------------------
all: $(EXECS)

# Compilar MMAS
test_MMAS: $(SRC_DIR)/test_MMAS.cpp $(SRC_DIR)/MMAS.cpp $(SRC_DIR)/utils.cpp
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@

# Compilar Experimento
Experimento: $(SRC_DIR)/Experimento.cpp $(SRC_DIR)/MMAS.cpp $(SRC_DIR)/utils.cpp
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@

# ------------------------
# Limpiar ejecutables
# ------------------------s
clean:
	rm -f $(BIN_DIR)/$(EXECS)


# mingw32-make 
# make
# o directamente
# g++ -O2 -std=c++1z src/test_MMAS.cpp src/MMAS.cpp src/utils.cpp -o ./test_MMAS
# g++ -O2 -std=c++1z Experimento.cpp MMAS.cpp utils.cpp -o ./Experimento



# ./test_MMAS -i ..\dataset_grafos_no_dirigidos\new_1000_dataset\erdos_n1000_p0c0.1_1.graph -n 80 -a 1.0 -b 2.0 -e 0.1 -min 0.01 -max 6.0 -t 10 -irace 0 -print 1
# ./Experimento