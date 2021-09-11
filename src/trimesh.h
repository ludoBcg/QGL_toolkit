#ifndef TRIMESHSOUP_H
#define TRIMESHSOUP_H

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>


#define QT_NO_OPENGL_ES_2
#include <GL/glew.h>


#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>


#include <map>


// The attribute locations we will use in the vertex shader
enum AttributeLocation 
{
    POSITION = 0,
    NORMAL = 1,
    COLOR = 2
};


/*!
* \class TriMesh
* \brief Triangle soup mesh (i.e. no adjacency information)
* Read OBJ files and store data in dynamic arrays.
*/
class TriMesh 
{
    public:

        /*------------------------------------------------------------------------------------------------------------+
        |                                        CONSTRUCTORS / DESTRUCTORS                                           |
        +------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn TriMesh
        * \brief Default constructor of TriMesh
        */
        TriMesh();

        /*!
        * \fn ~TriMesh
        * \brief Destructor of TriMesh
        */
        ~TriMesh();


        /*------------------------------------------------------------------------------------------------------------+
        |                                              GETTERS/SETTERS                                                |
        +-------------------------------------------------------------------------------------------------------------*/

        ///*! \fn getVertices */
        //void getVertices(std::vector<glm::vec3>& _vertices);
        ///*! \fn getNormals */
        //void getNormals(std::vector<glm::vec3>& _normals);
        ///*! \fn getIndices */
        //void getIndices(std::vector<uint32_t>& _indices);

        ///*! \fn getColors */
        //void getColors(std::vector<glm::vec3>& _colors);

        /*!
        * \fn getBBoxMin
        * \brief get min point of the bounding box
        * \return 3D coords of the min point of the BBox
        */
        glm::vec3 getBBoxMin() { return m_bBoxMin; }
        /*!
        * \fn getBBoxMax
        * \brief get max point of the bounding box
        * \return 3D coords of the max point of the BBox
        */
        glm::vec3 getBBoxMax() { return m_bBoxMax; }



        /*! \fn setProgram */
        inline void setProgram(const std::string& _vertShaderFilename, const std::string& _fragShaderFilename) { m_program = loadShaderProgram(_vertShaderFilename, _fragShaderFilename); }

        /*! \fn setSpeculatPower */
        inline void setSpeculatPower(float _specPow) { m_specPow = _specPow; }

        /*! \fn setAmbientColor */
        inline void setAmbientColor(int _r, int _g, int _b) { m_ambientColor = glm::vec3( (float)_r/255.0f, (float)_g/255.0f, (float)_b/255.0f ); }
        /*! \fn setDiffuseColor */
        inline void setDiffuseColor(int _r, int _g, int _b) { m_diffuseColor = glm::vec3( (float)_r/255.0f, (float)_g/255.0f, (float)_b/255.0f ); }
        /*! \fn setSpecularColor */
        inline void setSpecularColor(int _r, int _g, int _b) { m_specularColor = glm::vec3( (float)_r/255.0f, (float)_g/255.0f, (float)_b/255.0f ); }

        /*------------------------------------------------------------------------------------------------------------+
        |                                                   MISC.                                                     |
        +-------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn readFile
        * \brief read a mesh from a file
        * \param _filename : name of the file to read
        * \return false if file extension is not supported, true if it is
        */
        bool readFile(std::string _filename);


        /*!
        * \fn computeAABB
        * \brief compute Axis Oriented Bounding Box
        */
        void computeAABB();

        /*!
        * \fn computeNormals
        * \brief recompute the triangle normals and update vertex normals
        */
        void computeNormals();


        /*!
        * \fn createVAO
        * \brief Create mesh VAO and VBOs.
        */
        void createVAO();


        /*!
        * \fn draw
        * \brief Draw the content of the mesh VAO
        * \param _mv : modelview matrix
        * \param _mvp : modelview-projection matrix
        * \param _lightPos : 3D coords of light position
        * \param _lightCol : RGB color of the light
        */
        void draw(glm::mat4 _mv, glm::mat4 _mvp, glm::vec3 _lightPos, glm::vec3 _lightCol);

 
        /*!
        * \fn loadShaderProgram
        * \brief load shader program from shader files
        * \param _vertShaderFilename : vertex shader filename
        * \param _fragShaderFilename : fragment shader filename
        */
        GLuint loadShaderProgram(const std::string& _vertShaderFilename, const std::string& _fragShaderFilename);


    protected:


        /*------------------------------------------------------------------------------------------------------------+
        |                                                ATTRIBUTES                                                   |
        +-------------------------------------------------------------------------------------------------------------*/
                                        
        std::vector<glm::vec3> m_vertices;      /*!< vertices positions array (3D coords) */
        std::vector<glm::vec3> m_normals;       /*!< vertices normal vectors array (3D coords) */
        std::vector<uint32_t> m_indices;        /*!< vertices indices array (uint) */

        std::vector<glm::vec3> m_colors;        /*!< vertices RGB colors array (3D coords) */
        std::vector<glm::vec2> m_texcoords;     /*!< vertices uvs array (2D coords) */

        glm::vec3 m_bBoxMin;                    /*!< 3D coordinates of the min corner of the bounding box */
        glm::vec3 m_bBoxMax;                    /*!< 3D coordinates of the max corner of the bounding box */


GLuint m_program;           /*!< handle of the program object (i.e. shaders) for shaded surface rendering */

GLuint m_meshVAO;           /*!< mesh VAO (i.e. array in which the generated vertex array object names are stored) */
GLuint m_defaultVAO;        /*!< default VAO */

GLuint m_vertexVBO;         /*!< name of vertex 3D coords VBO */
GLuint m_normalVBO;         /*!< name of normal vector VBO */
GLuint m_colorVBO;          /*!< name of rgb color VBO */
GLuint m_indexVBO;          /*!< name of index VBO */

int m_numVertices;          /*!< number of vertices in the VBOs */
int m_numIndices;           /*!< number of indices in the index VBO */

float m_specPow;            /*!< specular power */

glm::vec3 m_ambientColor;   /*!< ambient color */
glm::vec3 m_diffuseColor;   /*!< diffuse color */
glm::vec3 m_specularColor;  /*!< specular color */

//bool m_vertexProvided;      /*!< flag to indicate if vertex coords are available or not */
//bool m_normalProvided;      /*!< flag to indicate if normals are available or not */
//bool m_colorProvided;       /*!< flag to indicate if colors are available or not */
//bool m_indexProvided;       /*!< flag to indicate if indices are available or not */

        /*------------------------------------------------------------------------------------------------------------+
        |                                                    MISC.                                                    |
        +-------------------------------------------------------------------------------------------------------------*/


        /*!
        * \fn importOBJ
        * \brief read OBJ file
        * \param _filename: name of file
        */
        bool importOBJ(const std::string &_filename);

        /*!
        * \fn clear
        * \brief Clear the content of all the attribute vectors
        */
        void clear();

        /*!
        * \fn readShaderSource
        * \brief read shader program and copy it in a string
        * \param _filename : shader file name
        * \return string containing shader program
        */
        std::string readShaderSource(const std::string& _filename); 
        /*!
        * \fn showShaderInfoLog
        * \brief print out shader info log (i.e. compilation errors)
        * \param _shader : shader
        */
        void showShaderInfoLog(GLuint _shader);
        /*!
        * \fn showProgramInfoLog
        * \brief print out program info log (i.e. linking errors)
        * \param _program : program
        */
        void showProgramInfoLog(GLuint _program); 

};
#endif // TRIMESHSOUP_H