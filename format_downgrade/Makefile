# Variáveis do compilador
CC = g++
CFLAGS = -Wall -std=c++17
LIBS = -L/mingw64/lib -lmpg123 -lsamplerate -lshlwapi
LDFLAGS = -static # Adiciona o flag para compilação estática
INCLUDE = -I/mingw64/include

# Nome do executável
TARGET = format_downgrader

# Arquivos fonte
SOURCES = main_readfile.cpp read_file.cpp TreeNode.cpp BinaryFileWriter.cpp OldOsuExporter.cpp Mp3ToRawConverter.cpp
# Arquivos objeto
OBJECTS = $(SOURCES:.cpp=.o)

# Regras
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS) $(LIBS)

# Regra de compilação
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE)

# Limpeza
clean:
	rm -f $(OBJECTS) $(TARGET)
