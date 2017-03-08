LIBS = sfml-window-s sfml-system-s opengl32 glu32 winmm gdi32
LDIR = .
IDIR = .
DEFINES = SFML_STATIC

_LIBS = $(LIBS:%=-l%)
_DEFINES = $(DEFINES:%=-D%)
CCFLAGS = $(_DEFINES) $(_LIBS) -I $(IDIR) -L $(LDIR) -static-libstdc++ -std=c++0x

ODIR = obj
_OBJ = main.o scene.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp
	g++ -c -o $@ $< $(CCFLAGS)
	
all: $(OBJ)
	g++ $(OBJ) -o tipe.exe $(CCFLAGS)