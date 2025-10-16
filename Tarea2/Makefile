# ------------------------
# Makefile para proyecto MISP
# ------------------------

CXX = g++
CXXFLAGS = -O2
SRC_DIR = ./src
BIN_DIR = .

# Lista de ejecutables
EXECS = test_GRASP

# ------------------------
# Regla por defecto
# ------------------------
all: $(EXECS)

# Compilar GRASP
test_GRASP: $(SRC_DIR)/testGRASP.cpp $(SRC_DIR)/GRASP.cpp $(SRC_DIR)/greedyrand1.cpp $(SRC_DIR)/utils.cpp
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@

# ------------------------
# Limpiar ejecutables
# ------------------------
clean:
	rm -f $(BIN_DIR)/$(EXECS)


# mingw32-make 
# make
# o directamente 
# g++ -O2 src/testGRASP.cpp src/GRASP.cpp src/greedyrand1.cpp src/utils.cpp -o ./test_GRASP