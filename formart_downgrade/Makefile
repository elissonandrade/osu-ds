# Variáveis do compilador
CC = g++
CFLAGS = -Wall -std=c++17
LDFLAGS = -static # Adiciona o flag para compilação estática

# Nome do executável
TARGET = format_downgrader

# Arquivos fonte
SOURCES = main_readfile.cpp read_file.cpp TreeNode.cpp BinaryFileWriter.cpp OldOsuExporter.cpp
# Arquivos objeto
OBJECTS = $(SOURCES:.cpp=.o)

# Regras
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Regra de compilação
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza
clean:
	rm -f $(OBJECTS) $(TARGET)
