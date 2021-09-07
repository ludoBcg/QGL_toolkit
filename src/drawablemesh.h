#ifndef DRAWABLEMESH_H
#define DRAWABLEMESH_H

#define QT_NO_OPENGL_ES_2
#include <GL/glew.h>


#include "trimesh.h"

// The attribute locations we will use in the vertex shader
enum AttributeLocation 
{
    POSITION = 0,
    NORMAL = 1,
    COLOR = 2/*,
    UV = 3,
    TANGENT = 4,
    BITANGENT = 5,
    FACENORMAL = 6*/
};


/*!
* \class DrawableMesh
* \brief Drawable mesh
* Render a TriMesh using a Blinn-Phong shading model and texture mapping
*/
class DrawableMesh
{
    public:

        /*------------------------------------------------------------------------------------------------------------+
        |                                        CONSTRUCTORS / DESTRUCTORS                                           |
        +------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn DrawableMesh
        * \brief Default constructor of DrawableMesh
        */
        DrawableMesh();


        /*!
        * \fn ~DrawableMesh
        * \brief Destructor of DrawableMesh
        */
        ~DrawableMesh();


        /*------------------------------------------------------------------------------------------------------------+
        |                                              GETTERS/SETTERS                                                |
        +-------------------------------------------------------------------------------------------------------------*/

        /*! \fn setProgram */
        inline void setProgram(const std::string& _vertShaderFilename, const std::string& _fragShaderFilename) { m_program = loadShaderProgram(_vertShaderFilename, _fragShaderFilename); }

        /*! \fn setSpeculatPower */
        inline void setSpeculatPower(float _specPow) { m_specPow = _specPow; }

        /*! \fn setAmbientColor */
        inline void setAmbientColor(int _r, int _g, int _b) { m_ambientColor = glm::vec3( (float)_r/255.0f, (float)_g/255.0f, (float)_b/255.0f ); }
        /*! \fn setAmbientColor */
        inline void setDiffuseColor(int _r, int _g, int _b) { m_diffuseColor = glm::vec3( (float)_r/255.0f, (float)_g/255.0f, (float)_b/255.0f ); }
        /*! \fn setAmbientColor */
        inline void setSpecularColor(int _r, int _g, int _b) { m_specularColor = glm::vec3( (float)_r/255.0f, (float)_g/255.0f, (float)_b/255.0f ); }

        /*! \fn setAmbientFlag */
        inline void setAmbientFlag(bool _useAmbient) { m_useAmbient = _useAmbient; }
        /*! \fn setDiffuseFlag */
        inline void setDiffuseFlag(bool _useDiffuse) { m_useDiffuse = _useDiffuse; }
        /*! \fn setSpecularFlag */
        inline void setSpecularFlag(bool _useSpecular) { m_useSpecular = _useSpecular; }

        /*! \fn getAmbientFlag */
        inline bool getAmbientFlag() { return m_useAmbient; }
        /*! \fn getDiffuseFlag */
        inline bool getDiffuseFlag() { return m_useDiffuse; }
        /*! \fn getSpecularFlag */
        inline bool getSpecularFlag() { return m_useSpecular; }
  


        /*------------------------------------------------------------------------------------------------------------+
        |                                               OTHER METHODS                                                 |
        +-------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn createVAO
        * \brief Create mesh VAO and VBOs, calling fillVAO().
        * \param _triMesh : Mesh to update mesh VAO and VBOs from
        */
        void createVAO(TriMesh* _triMesh);

        /*!
        * \fn updateVAO
        * \brief Update mesh VAO and VBOs, calling fillVAO().
        * \param _triMesh : Mesh to update mesh VAO and VBOs from
        */
        void updateVAO(TriMesh* _triMesh);

        /*!
        * \fn createVAO
        * \brief Fill mesh VAO and VBOs
        * \param _triMesh : Mesh to fill mesh VAO and VBOs from
        * \param _create : true to init the VBOs and VAO, false to update them 
        */
        void fillVAO(TriMesh* _triMesh, bool _create);

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

        /*! 
        * \fn toggleShadedRenderFlag 
        * \brief reverse shaded rendering flag
        */
        inline void toggleShadedRenderFlag() { m_shadedRenderOn = !m_shadedRenderOn; }

        

    protected:

        /*------------------------------------------------------------------------------------------------------------+
        |                                                ATTRIBUTES                                                   |
        +-------------------------------------------------------------------------------------------------------------*/

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

        bool m_useAmbient;          /*!< flag to use ambient shading or not */
        bool m_useDiffuse;          /*!< flag to use diffuse shading or not */
        bool m_useSpecular;         /*!< flag to use specular shading or not */

        bool m_vertexProvided;      /*!< flag to indicate if vertex coords are available or not */
        bool m_normalProvided;      /*!< flag to indicate if normals are available or not */
        bool m_colorProvided;       /*!< flag to indicate if colors are available or not */
        bool m_indexProvided;       /*!< flag to indicate if indices are available or not */

        bool m_shadedRenderOn;      /*!< flag to indicate if shaded rendering is on */


        /*------------------------------------------------------------------------------------------------------------+
        |                                               OTHER METHODS                                                 |
        +-------------------------------------------------------------------------------------------------------------*/

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
#endif // DRAWABLEMESH_H