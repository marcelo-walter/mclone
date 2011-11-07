# Makefile for use in MClone project (http://mclone.googlecode.com).
#
# Author:
# John Gamboa


# Goal names
MCLONE_SIMULATOR := bin/MCloneSimulator
MCLONE_VIEWER := bin/MCloneViewer


# Compiler directives
CC := g++

ifeq ($(CC),gcc)
	CFLAGS := -Wall
else ifeq ($(CC),g++)
	CFLAGS := -Wall
else
	CFLAGS := -DENABLE_CUDA
endif




INCLUDE_PATHS := -I./src/ -I/usr/local/include/ImageMagick/
LINKING_PATHS := -L./lib/voronoi3D/
LINKING_FLAGS := -lMagick++ -lm -lvoronoi -lGL -lGLU -lglut

VIEWER_FLAG := -DGRAPHICS

OBJS := obj/main.o obj/Anim.o obj/Growth.o obj/primitives.o obj/texture.o obj/vectorField.o obj/wingEdge.o obj/cells.o obj/cellsList.o obj/fileManager.o obj/Matrix4.o obj/Object.o obj/Parameters.o obj/PatternObject.o obj/Point3D.o obj/edge.o obj/edgelist.o obj/face.o obj/facevector.o obj/geolist.o obj/halfedge.o obj/hashtable.o obj/interfacedistance.o obj/my_opengl.o obj/set.o obj/sortedset.o obj/surface.o obj/vector.o obj/vertex.o obj/vlist.o obj/forces.o obj/morph.o obj/planar.o obj/relax.o obj/simulation.o obj/distPoints.o obj/genericUtil.o obj/heap.o obj/heapTri.o obj/interfaceVoronoi.o obj/intersect.o obj/matrix.o obj/matrixUtil.o obj/printInfo.o obj/random.o obj/transformations.o obj/vect.o obj/drawing.o obj/graph.o obj/menus.o obj/trackball.o obj/ui.o obj/utilGraph.o obj/heapstruct.o

# Default Goal (i.e., the first one): Just redirects to "MCLONE_SIMULATOR".
all : $(MCLONE_SIMULATOR) $(MCLONE_VIEWER)

# Simulator mode
$(MCLONE_SIMULATOR) : $(OBJS) | bin
	g++ $(LINKING_PATHS) -o $(MCLONE_SIMULATOR) $(OBJS) $(LINKING_FLAGS) $(CFLAGS)

# View mode
$(MCLONE_VIEWER) : $(OBJS) | bin
	g++ $(VIEWER_FLAG) $(LINKING_PATHS) -o $(MCLONE_VIEWER) $(OBJS) $(LINKING_FLAGS) $(CFLAGS)


# Indicates that these "prerequisites" aren't files
.PHONY : clean all


# These rules are meant just to create the folders needed
obj :
	mkdir obj
bin :
	mkdir bin

# Since "make -B" isn't working, for any reason, I think the cleaning rule has
# become very important
clean :
	rm -rf ./obj
	rm -rf ./bin



obj/main.o: src/main.cpp src/main.h src/data/Types.h src/data/Macros.h \
 src/control/vectorField.h src/data/Types.h src/control/wingEdge.h \
 src/control/Growth.h src/control/Anim.h src/control/Growth.h \
 src/control/texture.h src/control/primitives.h src/simulation/relax.h \
 src/simulation/simulation.h src/simulation/forces.h \
 src/simulation/morph.h src/util/heap.h src/util/heapTri.h \
 src/util/printInfo.h src/util/distPoints.h src/util/genericUtil.h \
 src/util/random.h src/data/Object.h src/data/Types.h src/data/cells.h \
 src/data/cellsList.h src/data/Macros.h src/data/Parameters.h \
 src/data/Matrix4.h src/data/Point3D.h src/data/fileManager.h \
 src/distance/interfacedistance.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/Anim.o: src/control/Anim.cpp src/control/Anim.h src/data/Types.h \
 src/data/Macros.h src/control/Growth.h src/control/primitives.h \
 src/util/genericUtil.h src/util/printInfo.h src/data/fileManager.h \
 src/data/Types.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/Growth.o: src/control/Growth.cpp src/control/Growth.h src/data/Types.h \
 src/data/Macros.h src/control/primitives.h src/util/genericUtil.h \
 src/util/transformations.h src/data/Point3D.h src/data/Types.h \
 src/data/Matrix4.h src/data/Point3D.h src/data/fileManager.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/primitives.o: src/control/primitives.cpp src/control/primitives.h \
 src/data/Types.h src/data/Macros.h src/control/Anim.h \
 src/control/Growth.h src/data/cellsList.h src/data/Types.h \
 src/data/Macros.h src/data/fileManager.h src/data/Object.h \
 src/data/Point3D.h src/data/Matrix4.h src/data/Point3D.h \
 src/util/genericUtil.h src/util/transformations.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/texture.o: src/control/texture.cpp src/control/texture.h src/data/Types.h \
 src/data/Macros.h /usr/local/include/ImageMagick/magick/api.h \
 /usr/local/include/ImageMagick/magick/MagickCore.h \
 /usr/local/include/ImageMagick/magick/magick-config.h \
 /usr/local/include/ImageMagick/magick/magick-type.h \
 /usr/local/include/ImageMagick/magick/accelerate.h \
 /usr/local/include/ImageMagick/magick/morphology.h \
 /usr/local/include/ImageMagick/magick/geometry.h \
 /usr/local/include/ImageMagick/magick/animate.h \
 /usr/local/include/ImageMagick/magick/annotate.h \
 /usr/local/include/ImageMagick/magick/draw.h \
 /usr/local/include/ImageMagick/magick/image.h \
 /usr/local/include/ImageMagick/magick/color.h \
 /usr/local/include/ImageMagick/magick/pixel.h \
 /usr/local/include/ImageMagick/magick/colorspace.h \
 /usr/local/include/ImageMagick/magick/constitute.h \
 /usr/local/include/ImageMagick/magick/exception.h \
 /usr/local/include/ImageMagick/magick/semaphore.h \
 /usr/local/include/ImageMagick/magick/blob.h \
 /usr/local/include/ImageMagick/magick/stream.h \
 /usr/local/include/ImageMagick/magick/cache-view.h \
 /usr/local/include/ImageMagick/magick/composite.h \
 /usr/local/include/ImageMagick/magick/compress.h \
 /usr/local/include/ImageMagick/magick/effect.h \
 /usr/local/include/ImageMagick/magick/layer.h \
 /usr/local/include/ImageMagick/magick/locale_.h \
 /usr/local/include/ImageMagick/magick/hashmap.h \
 /usr/local/include/ImageMagick/magick/monitor.h \
 /usr/local/include/ImageMagick/magick/profile.h \
 /usr/local/include/ImageMagick/magick/string_.h \
 /usr/local/include/ImageMagick/magick/quantum.h \
 /usr/local/include/ImageMagick/magick/resample.h \
 /usr/local/include/ImageMagick/magick/resize.h \
 /usr/local/include/ImageMagick/magick/timer.h \
 /usr/local/include/ImageMagick/magick/type.h \
 /usr/local/include/ImageMagick/magick/artifact.h \
 /usr/local/include/ImageMagick/magick/attribute.h \
 /usr/local/include/ImageMagick/magick/cache.h \
 /usr/local/include/ImageMagick/magick/cipher.h \
 /usr/local/include/ImageMagick/magick/client.h \
 /usr/local/include/ImageMagick/magick/coder.h \
 /usr/local/include/ImageMagick/magick/colormap.h \
 /usr/local/include/ImageMagick/magick/compare.h \
 /usr/local/include/ImageMagick/magick/configure.h \
 /usr/local/include/ImageMagick/magick/decorate.h \
 /usr/local/include/ImageMagick/magick/delegate.h \
 /usr/local/include/ImageMagick/magick/deprecate.h \
 /usr/local/include/ImageMagick/magick/quantize.h \
 /usr/local/include/ImageMagick/magick/registry.h \
 /usr/local/include/ImageMagick/magick/display.h \
 /usr/local/include/ImageMagick/magick/distort.h \
 /usr/local/include/ImageMagick/magick/enhance.h \
 /usr/local/include/ImageMagick/magick/feature.h \
 /usr/local/include/ImageMagick/magick/fourier.h \
 /usr/local/include/ImageMagick/magick/fx.h \
 /usr/local/include/ImageMagick/magick/gem.h \
 /usr/local/include/ImageMagick/magick/random_.h \
 /usr/local/include/ImageMagick/magick/histogram.h \
 /usr/local/include/ImageMagick/magick/identify.h \
 /usr/local/include/ImageMagick/magick/image-view.h \
 /usr/local/include/ImageMagick/magick/list.h \
 /usr/local/include/ImageMagick/magick/log.h \
 /usr/local/include/ImageMagick/magick/magic.h \
 /usr/local/include/ImageMagick/magick/magick.h \
 /usr/local/include/ImageMagick/magick/matrix.h \
 /usr/local/include/ImageMagick/magick/memory_.h \
 /usr/local/include/ImageMagick/magick/module.h \
 /usr/local/include/ImageMagick/magick/version.h \
 /usr/local/include/ImageMagick/magick/mime.h \
 /usr/local/include/ImageMagick/magick/montage.h \
 /usr/local/include/ImageMagick/magick/option.h \
 /usr/local/include/ImageMagick/magick/paint.h \
 /usr/local/include/ImageMagick/magick/policy.h \
 /usr/local/include/ImageMagick/magick/prepress.h \
 /usr/local/include/ImageMagick/magick/property.h \
 /usr/local/include/ImageMagick/magick/resource_.h \
 /usr/local/include/ImageMagick/magick/segment.h \
 /usr/local/include/ImageMagick/magick/shear.h \
 /usr/local/include/ImageMagick/magick/signature.h \
 /usr/local/include/ImageMagick/magick/splay-tree.h \
 /usr/local/include/ImageMagick/magick/statistic.h \
 /usr/local/include/ImageMagick/magick/token.h \
 /usr/local/include/ImageMagick/magick/transform.h \
 /usr/local/include/ImageMagick/magick/threshold.h \
 /usr/local/include/ImageMagick/magick/utility.h \
 /usr/local/include/ImageMagick/magick/xml-tree.h \
 /usr/local/include/ImageMagick/magick/xwindow.h src/control/primitives.h \
 src/simulation/relax.h src/data/Object.h src/data/Types.h \
 src/data/Parameters.h src/util/interfaceVoronoi.h src/util/genericUtil.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/vectorField.o: src/control/vectorField.cpp src/control/vectorField.h \
 src/data/Types.h src/data/Macros.h src/control/texture.h \
 src/distance/interfacedistance.h src/simulation/relax.h \
 src/data/Object.h src/data/Types.h src/data/fileManager.h \
 src/data/Matrix4.h src/data/Point3D.h src/util/genericUtil.h \
 src/util/matrix.h src/util/printInfo.h src/util/distPoints.h \
 src/viewer/graph.h src/viewer/ui.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/wingEdge.o: src/control/wingEdge.cpp src/control/wingEdge.h \
 src/data/Types.h src/data/Macros.h src/util/genericUtil.h \
 src/data/Matrix4.h src/data/Point3D.h src/data/Types.h src/data/Object.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/cells.o: src/data/cells.cpp src/data/cells.h src/data/Types.h \
 src/data/Macros.h src/data/cellsList.h src/data/Macros.h \
 src/data/Object.h src/data/Parameters.h src/simulation/relax.h \
 src/data/Types.h src/simulation/simulation.h src/util/random.h \
 src/util/heap.h src/util/genericUtil.h src/util/distPoints.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/cellsList.o: src/data/cellsList.cpp src/data/cellsList.h src/data/Types.h \
 src/data/Macros.h src/data/Macros.h src/data/cells.h src/data/Object.h \
 src/data/fileManager.h src/util/random.h src/util/genericUtil.h \
 src/data/Types.h src/util/distPoints.h src/simulation/relax.h \
 src/simulation/simulation.h src/control/primitives.h \
 src/control/texture.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/fileManager.o: src/data/fileManager.cpp src/data/fileManager.h \
 src/data/Types.h src/data/Macros.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/Matrix4.o: src/data/Matrix4.cpp src/data/Matrix4.h src/data/Point3D.h \
 src/data/Types.h src/data/Macros.h src/util/genericUtil.h \
 src/data/Types.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/Object.o: src/data/Object.cpp src/data/Object.h src/data/Types.h \
 src/data/Macros.h src/util/genericUtil.h src/data/Types.h \
 src/data/fileManager.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/Parameters.o: src/data/Parameters.cpp src/data/Parameters.h \
 src/data/Types.h src/data/Macros.h src/util/genericUtil.h \
 src/data/Types.h src/data/fileManager.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/PatternObject.o: src/data/PatternObject.cpp src/data/PatternObject.h \
 src/data/Types.h src/data/Macros.h src/util/genericUtil.h \
 src/data/Types.h src/util/interfaceVoronoi.h \
 src/distance/interfacedistance.h src/simulation/relax.h \
 src/data/Object.h src/data/Parameters.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/Point3D.o: src/data/Point3D.cpp src/data/Point3D.h src/data/Types.h \
 src/data/Macros.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/edge.o: src/distance/edge.cpp src/distance/edge.h src/distance/halfedge.h \
 src/distance/vertex.h src/distance/heapstruct.h src/distance/face.h \
 src/distance/vector.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/edgelist.o: src/distance/edgelist.cpp src/distance/edgelist.h \
 src/distance/edge.h src/distance/halfedge.h src/distance/vertex.h \
 src/distance/heapstruct.h src/distance/face.h src/distance/vector.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/face.o: src/distance/face.cpp src/distance/face.h src/distance/halfedge.h \
 src/distance/vertex.h src/distance/heapstruct.h src/distance/edge.h \
 src/distance/vector.h src/distance/my_opengl.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/facevector.o: src/distance/facevector.cpp src/distance/facevector.h \
 src/distance/face.h src/distance/halfedge.h src/distance/vertex.h \
 src/distance/heapstruct.h src/distance/edge.h src/distance/vector.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/geolist.o: src/distance/geolist.cpp src/distance/geolist.h \
 src/distance/vertex.h src/distance/halfedge.h src/distance/face.h \
 src/distance/vector.h src/distance/edge.h src/distance/heapstruct.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/halfedge.o: src/distance/halfedge.cpp src/distance/halfedge.h \
 src/distance/vertex.h src/distance/heapstruct.h src/distance/face.h \
 src/distance/vector.h src/distance/edge.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/hashtable.o: src/distance/hashtable.cpp src/distance/hashtable.h \
 src/distance/edge.h src/distance/halfedge.h src/distance/vertex.h \
 src/distance/heapstruct.h src/distance/face.h src/distance/vector.h \
 src/distance/edgelist.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/interfacedistance.o: src/distance/interfacedistance.cpp \
 src/distance/interfacedistance.h src/distance/surface.h \
 src/distance/halfedge.h src/distance/vertex.h src/distance/heapstruct.h \
 src/distance/face.h src/distance/vector.h src/distance/edge.h \
 src/distance/hashtable.h src/distance/edgelist.h \
 src/distance/facevector.h src/distance/set.h src/distance/sortedset.h \
 src/distance/geolist.h src/data/Object.h src/data/Types.h \
 src/data/Macros.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/my_opengl.o: src/distance/my_opengl.cpp src/distance/my_opengl.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/set.o: src/distance/set.cpp src/distance/set.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/sortedset.o: src/distance/sortedset.cpp src/distance/sortedset.h \
 src/distance/heapstruct.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/surface.o: src/distance/surface.cpp src/distance/surface.h \
 src/distance/halfedge.h src/distance/vertex.h src/distance/heapstruct.h \
 src/distance/face.h src/distance/vector.h src/distance/edge.h \
 src/distance/hashtable.h src/distance/edgelist.h \
 src/distance/facevector.h src/distance/set.h src/distance/sortedset.h \
 src/distance/geolist.h src/distance/vlist.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/vector.o: src/distance/vector.cpp src/distance/vector.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/vertex.o: src/distance/vertex.cpp src/distance/vertex.h \
 src/distance/halfedge.h src/distance/face.h src/distance/vector.h \
 src/distance/edge.h src/distance/heapstruct.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/vlist.o: src/distance/vlist.cpp src/distance/vlist.h \
 src/distance/surface.h src/distance/halfedge.h src/distance/vertex.h \
 src/distance/heapstruct.h src/distance/face.h src/distance/vector.h \
 src/distance/edge.h src/distance/hashtable.h src/distance/edgelist.h \
 src/distance/facevector.h src/distance/set.h src/distance/sortedset.h \
 src/distance/geolist.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/forces.o: src/simulation/forces.cpp src/simulation/forces.h \
 src/data/Types.h src/data/Macros.h src/simulation/relax.h \
 src/util/heapTri.h src/control/wingEdge.h src/data/cellsList.h \
 src/data/Types.h src/data/Macros.h src/data/Object.h \
 src/data/Parameters.h src/data/Matrix4.h src/data/Point3D.h \
 src/util/genericUtil.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/morph.o: src/simulation/morph.cpp src/simulation/morph.h src/data/Types.h \
 src/data/Macros.h src/simulation/simulation.h src/simulation/relax.h \
 src/simulation/planar.h src/control/wingEdge.h src/control/primitives.h \
 src/control/Growth.h src/data/cellsList.h src/data/Types.h \
 src/data/Macros.h src/data/Object.h src/data/fileManager.h \
 src/data/Point3D.h src/data/Matrix4.h src/data/Point3D.h \
 src/util/genericUtil.h src/util/distPoints.h src/util/transformations.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/planar.o: src/simulation/planar.cpp src/simulation/planar.h \
 src/data/Types.h src/data/Macros.h src/simulation/forces.h \
 src/simulation/relax.h src/util/heapTri.h src/util/genericUtil.h \
 src/data/Object.h src/data/Types.h src/data/Point3D.h src/data/Matrix4.h \
 src/data/Point3D.h src/data/cellsList.h src/data/Macros.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/relax.o: src/simulation/relax.cpp src/simulation/relax.h src/data/Types.h \
 src/data/Macros.h src/simulation/forces.h src/data/cellsList.h \
 src/data/Types.h src/data/Macros.h src/data/cells.h src/data/Object.h \
 src/data/Parameters.h src/data/Matrix4.h src/data/Point3D.h \
 src/data/fileManager.h src/control/primitives.h src/control/wingEdge.h \
 src/util/intersect.h src/util/genericUtil.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/simulation.o: src/simulation/simulation.cpp src/simulation/simulation.h \
 src/data/Types.h src/data/Macros.h src/control/vectorField.h \
 src/control/wingEdge.h src/control/primitives.h src/control/Growth.h \
 src/util/heap.h src/util/genericUtil.h src/util/distPoints.h \
 src/util/transformations.h src/data/cellsList.h src/data/Types.h \
 src/data/Macros.h src/data/Object.h src/data/Point3D.h \
 src/data/Matrix4.h src/data/Point3D.h src/data/cells.h \
 src/data/Parameters.h src/data/fileManager.h src/simulation/relax.h \
 src/simulation/morph.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/distPoints.o: src/util/distPoints.cpp src/util/distPoints.h \
 src/data/Types.h src/data/Macros.h src/util/genericUtil.h \
 src/data/Point3D.h src/data/Types.h src/data/Object.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/genericUtil.o: src/util/genericUtil.cpp src/util/genericUtil.h \
 src/data/Types.h src/data/Macros.h src/control/primitives.h \
 src/data/cellsList.h src/data/Types.h src/data/Macros.h \
 src/data/Parameters.h src/data/Matrix4.h src/data/Point3D.h \
 src/simulation/simulation.h src/simulation/relax.h \
 src/util/transformations.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/heap.o: src/util/heap.cpp src/util/heap.h src/data/Types.h \
 src/data/Macros.h src/util/genericUtil.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/heapTri.o: src/util/heapTri.cpp src/util/heapTri.h src/data/Types.h \
 src/data/Macros.h src/util/genericUtil.h src/data/Matrix4.h \
 src/data/Point3D.h src/data/Types.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/interfaceVoronoi.o: src/util/interfaceVoronoi.cpp \
 src/util/interfaceVoronoi.h src/data/Types.h src/data/Macros.h \
 src/simulation/relax.h src/simulation/forces.h src/control/wingEdge.h \
 src/data/Object.h src/data/Types.h src/data/cellsList.h \
 src/data/Macros.h src/util/genericUtil.h src/util/intersect.h \
 src/viewer/drawing.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/intersect.o: src/util/intersect.cpp src/util/intersect.h src/data/Types.h \
 src/data/Macros.h src/simulation/relax.h src/control/wingEdge.h \
 src/data/Object.h src/data/Types.h src/util/genericUtil.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/matrix.o: src/util/matrix.cpp src/util/matrix.h src/data/Types.h \
 src/data/Macros.h src/util/matrixUtil.h src/distance/interfacedistance.h \
 src/control/vectorField.h src/data/Object.h src/data/Types.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/matrixUtil.o: src/util/matrixUtil.cpp src/util/matrixUtil.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/printInfo.o: src/util/printInfo.cpp src/util/printInfo.h src/data/Types.h \
 src/data/Macros.h src/data/cellsList.h src/data/Types.h \
 src/data/Macros.h src/data/Object.h src/control/primitives.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/random.o: src/util/random.cpp src/util/random.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/transformations.o: src/util/transformations.cpp \
 src/util/transformations.h src/data/Types.h src/data/Macros.h \
 src/control/primitives.h src/control/Anim.h src/control/Growth.h \
 src/data/Matrix4.h src/data/Point3D.h src/data/Types.h \
 src/data/fileManager.h src/util/genericUtil.h \
 src/simulation/simulation.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/vect.o: src/util/vect.cpp src/util/vect.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/drawing.o: src/viewer/drawing.cpp src/viewer/drawing.h src/data/Types.h \
 src/data/Macros.h src/viewer/graph.h src/viewer/ui.h \
 src/util/interfaceVoronoi.h src/util/transformations.h \
 src/util/genericUtil.h src/data/cellsList.h src/data/Types.h \
 src/data/Macros.h src/data/Object.h src/data/Parameters.h \
 src/data/fileManager.h src/control/Growth.h src/control/primitives.h \
 src/control/texture.h src/control/vectorField.h src/simulation/relax.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/graph.o: src/viewer/graph.cpp src/viewer/graph.h src/data/Types.h \
 src/data/Macros.h src/viewer/drawing.h src/viewer/trackball.h \
 src/viewer/ui.h src/viewer/menus.h src/control/vectorField.h \
 src/control/Growth.h src/control/primitives.h src/util/transformations.h \
 src/util/genericUtil.h src/util/printInfo.h src/data/Object.h \
 src/data/Types.h src/data/Matrix4.h src/data/Point3D.h src/data/cells.h \
 src/data/fileManager.h src/simulation/simulation.h src/util/vect.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/menus.o: src/viewer/menus.cpp src/viewer/menus.h src/viewer/graph.h \
 src/data/Types.h src/data/Macros.h src/viewer/drawing.h \
 src/viewer/utilGraph.h src/control/vectorField.h src/control/texture.h \
 src/control/Growth.h src/control/Anim.h src/control/Growth.h \
 src/control/primitives.h src/data/cellsList.h src/data/Types.h \
 src/data/Macros.h src/data/Parameters.h src/data/PatternObject.h \
 src/data/Object.h src/data/fileManager.h src/util/interfaceVoronoi.h \
 src/util/genericUtil.h src/util/printInfo.h \
 src/distance/interfacedistance.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/trackball.o: src/viewer/trackball.cpp src/viewer/trackball.h \
 src/data/Types.h src/data/Macros.h src/util/vect.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/ui.o: src/viewer/ui.cpp src/viewer/ui.h src/data/Types.h \
 src/data/Macros.h src/viewer/graph.h src/viewer/trackball.h \
 src/viewer/drawing.h src/viewer/utilGraph.h src/control/primitives.h \
 src/control/vectorField.h src/simulation/simulation.h \
 src/simulation/planar.h src/simulation/morph.h src/data/Object.h \
 src/data/Types.h src/data/fileManager.h src/util/genericUtil.h \
 src/util/printInfo.h src/util/vect.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/utilGraph.o: src/viewer/utilGraph.cpp src/viewer/utilGraph.h \
 src/viewer/graph.h src/data/Types.h src/data/Macros.h \
 src/viewer/drawing.h src/control/primitives.h src/util/genericUtil.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

obj/heapstruct.o: src/distance/heapstruct.cc src/distance/a48.h \
 src/distance/heapstruct.h | obj
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

