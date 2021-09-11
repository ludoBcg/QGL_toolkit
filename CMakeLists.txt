# check CMAKE version
cmake_minimum_required(VERSION 3.0)

# create project
project(QGL_toolkit)

# add files
set(SRCS
	src/main.cpp
	src/viewer.cpp
	src/window.cpp
	src/trimesh.cpp
    )
    
set(HEADERS
	src/viewer.h
	src/window.h
	src/trimesh.h
	src/QGLtoolkit/camera.h
	src/QGLtoolkit/cameraFrame.h
	src/QGLtoolkit/frame.h
	src/QGLtoolkit/qglviewer.h
	src/QGLtoolkit/quaternion.h
    )
	

	
################################# EXTERNAL LIBS ######################

# Add include directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src" )

# Define variable for linked libraries
set(PROJECT_LIBRARIES)


# OpenGL
find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
  include_directories(SYSTEM ${OPENGL_INCLUDE_DIR})
  set(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} ${OPENGL_LIBRARIES})
endif(OPENGL_FOUND)

# GLEW
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/external/glew/src" PROJECT_SRCS)
include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/external/glew/include")
add_definitions(-DGLEW_STATIC -DGLEW_NO_GLU)

# GLM
include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/external/glm")



################################# QT #################################
# find QT packages
set(CMAKE_PREFIX_PATH C:/Qt/5.11.2/msvc2015_64/lib/cmake)
find_package(Qt5Widgets)
find_package(Qt5OpenGL)
set(CMAKE_AUTOMOC ON)
find_package(OpenGL)


SET(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} )
	
# Add executable for project
add_executable(${PROJECT_NAME} ${SRCS} ${HEADERS} ${PROJECT_SRCS})

qt5_use_modules(QGL_toolkit Widgets OpenGL)

target_link_libraries(QGL_toolkit ${PROJECT_LIBRARIES} ${QT_LIBRARIES} ${OPENGL_LIBRARIES})

# Install executable
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

