# ------------------------
# Makefile para proyecto MISP
# ------------------------

CXX = g++
CXXFLAGS = -O2
SRC_DIR = ./src
BIN_DIR = .

# Lista de ejecutables
EXECS = test_brkga Experimento

# ------------------------
# Regla por defecto
# ------------------------
all: $(EXECS)

# Compilar BRKGA
test_brkga: $(SRC_DIR)/test_brkga.cpp $(SRC_DIR)/BRKGA.cpp $(SRC_DIR)/utils.cpp
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@

# Compilar Experimento
Experimento: $(SRC_DIR)/Experimento.cpp $(SRC_DIR)/BRKGA.cpp $(SRC_DIR)/utils.cpp
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$@

# ------------------------
# Limpiar ejecutables
# ------------------------s
clean:
	rm -f $(BIN_DIR)/$(EXECS)


# mingw32-make 
# make
# o directamente
# g++ -O2 src/test_brkga.cpp src/BRKGA.cpp src/utils.cpp -o ./test_brkga
# ./test_brkga -i ../dataset_grafos_no_dirigidos/new_1000_dataset/erdos_n1000_p0c0.1_1.graph -s 100 -m 0.05 -t 60 -print 1 -pe 0.2 -pm 0.1 -rhoe 0.7
# g++ -O2 Experimento.cpp BRKGA.cpp utils.cpp -o ./Experimento
# ./Experimento