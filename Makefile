# Makefile for use in MClone project (http://mclone.googlecode.com).
#
# Author:
# John Gamboa


# Goal names
MCLONE_SIMULATOR := MCloneSimulator
MCLONE_VIEWER := MCloneViewer

# Compiler directives
CXX := g++
CFLAGS := -Wall

INCLUDE_PATHS := -I./src/ -I/usr/local/include/ImageMagick/
LINKING_PATHS := -L./lib/voronoi3D/
LINKING_FLAGS := -lMagick++ -lm -lvoronoi -lGL -lGLU -lglut
VIEWER_FLAG := -DGRAPHICS
OBJS := $(patsubst %.cpp,%.o,$(wildcard ./src/*/*.cpp)) $(patsubst %.cc,%.o,$(wildcard ./src/*/*.cc))

# Default Goal (i.e., the first one): Just redirects to "MCLONE_SIMULATOR".
all : $(MCLONE_SIMULATOR) $(MCLONE_VIEWER)

# Indicates that these "prerequisites" aren't files
.PHONY : clean all Simulator Viewer

# Alias to build only the Simulator or only the Viewer modes.

# Simulator mode
$(MCLONE_SIMULATOR) : $(SIMULATOR_OBJECTS)
	g++ $(LINKING_PATHS) -o $(MCLONE_SIMULATOR) $(OBJS) $(LINKING_FLAGS)

# View mode
$(MCLONE_VIEWER) : $(VIEWER_OBJECTS)
	g++ $(VIEWER_FLAG) $(LINKING_PATHS) -o $(MCLONE_SIMULATOR) $(OBJS) \
	$(LINKING_FLAGS)

./obj/%.o: src/*/%.cpp
	g++ $(INCLUDE_PATHS) -MMD -MP -MF"$(@:%.o=./obj/%.d)" \
	-MT"$(./obj/:%.o=%.d)" -o "$@" "$<"

./obj/%.o: src/*/%.cc
	g++ $(INCLUDE_PATHS) -MMD -MP -MF"$(@:%.o=./obj/%.d)" \
	-MT"$(./obj/:%.o=%.d)" -o "$@" "$<"

clean :
	rm -rf ./obj/*.o
