#ifndef DRAWABLEMESH_H
#define DRAWABLEMESH_H

#define QT_NO_OPENGL_ES_2
#include <GL/glew.h>


#include "mesh.h"

// The attribute locations we will use in the vertex shader
enum AttributeLocation 
{
    POSITION = 0,
    NORMAL = 1,
    COLOR = 2,
    UV = 3,
    TANGENT = 4,
    BITANGENT = 5,
    FACENORMAL = 6
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

        /*! \fn setWireColor */
        inline void setWireColor(int _r, int _g, int _b) { m_wireColor = glm::vec3( (float)_r/255.0f, (float)_g/255.0f, (float)_b/255.0f ); }

        /*! \fn setAmbientFlag */
        inline void setAmbientFlag(bool _useAmbient) { m_useAmbient = _useAmbient; }
        /*! \fn setDiffuseFlag */
        inline void setDiffuseFlag(bool _useDiffuse) { m_useDiffuse = _useDiffuse; }
        /*! \fn setSpecularFlag */
        inline void setSpecularFlag(bool _useSpecular) { m_useSpecular = _useSpecular; }
        /*! \fn setTexFlag */
        inline void setTexFlag(bool _useTex) 
        { 
            if(!m_uvProvided && _useTex)
                std::cerr << "[WARNING] DrawableMesh::setTexFlag(): No UV coords available" << std::endl;
            m_useTex = _useTex; 
        }
        /*! \fn setNormalMapFlag */
        inline void setNormalMapFlag(bool _useNormalMap) 
        {
            if(_useNormalMap)
            {
                if(!m_uvProvided)
                    std::cerr << "[WARNING] DrawableMesh::setNormalMapFlag(): No UV coords available" << std::endl;
                if(!m_tangentProvided)
                    std::cerr << "[WARNING] DrawableMesh::setNormalMapFlag(): No tangent coords available" << std::endl;
                if(!m_bitangentProvided)
                    std::cerr << "[WARNING] DrawableMesh::setNormalMapFlag(): No bitangent coords available" << std::endl;
            }
            m_useNormalMap = _useNormalMap; 
        }
        /*! \fn setShowNormalFlag */
        inline void setShowNormalFlag(bool _showNormals) { m_showNormals = _showNormals; }
        /*! \fn setFlatShadingFlag */
        inline void setFlatShadingFlag(bool _flatShading) { m_flatShading = _flatShading; }
        /*! \fn setUseGammaCorrecFlag */
        inline void setUseGammaCorrecFlag(bool _useGammaCorrec) { m_useGammaCorrec = _useGammaCorrec; }
        /*! \fn setUseFaceNormalsFlag */
        inline void setUseFaceNormalsFlag(bool useFaceNormals) { m_useFaceNormals = useFaceNormals; }
        /*! \fn setUseMeshColFlag */
        inline void setUseMeshColFlag(bool useMeshCol) { m_useMeshCol = useMeshCol; }

        /*! \fn getAmbientFlag */
        inline bool getAmbientFlag() { return m_useAmbient; }
        /*! \fn getDiffuseFlag */
        inline bool getDiffuseFlag() { return m_useDiffuse; }
        /*! \fn getSpecularFlag */
        inline bool getSpecularFlag() { return m_useSpecular; }
        /*! \fn getTexFlag */
        inline bool getTexFlag() { return m_useTex; }
        /*! \fn getNormalMapFlag */
        inline bool getNormalMapFlag() { return m_useNormalMap; }
        /*! \fn getShowNormalFlag */
        inline bool getShowNormalFlag() { return m_showNormals; }
        /*! \fn getFlatShadingFlag */
        inline bool getFlatShadingFlag() { return m_flatShading; }
        /*! \fn getUseGammaCorrecFlag */
        inline bool getUseGammaCorrecFlag() { return m_useGammaCorrec; }
        /*! \fn getUseFaceNormalsFlag */
        inline bool getUseFaceNormalsFlag() { return m_useFaceNormals; }
        /*! \fn getUseMeshColFlag */
        inline bool getUseMeshColFlag() { return m_useMeshCol; }


        /*------------------------------------------------------------------------------------------------------------+
        |                                               OTHER METHODS                                                 |
        +-------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn createVAO
        * \brief Create mesh VAO and VBOs, calling fillVAO().
        * \param _triMesh : Mesh to update mesh VAO and VBOs from
        */
        void createVAO(Mesh* _triMesh);

        /*!
        * \fn updateVAO
        * \brief Update mesh VAO and VBOs, calling fillVAO().
        * \param _triMesh : Mesh to update mesh VAO and VBOs from
        */
        void updateVAO(Mesh* _triMesh);

        /*!
        * \fn createVAO
        * \brief Fill mesh VAO and VBOs
        * \param _triMesh : Mesh to fill mesh VAO and VBOs from
        * \param _create : true to init the VBOs and VAO, false to update them 
        */
        void fillVAO(Mesh* _triMesh, bool _create);

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
        * \fn loadTex
        * \brief load the texture from a file
        * \param _filename : name of texture image
        */
        inline void loadTex(const std::string& _filename) { m_tex = load2DTexture(_filename, true); }
        /*!
        * \fn loadNormalMap
        * \brief load the normal map texture from a file
        * \param _filename : name of texture image
        */
        inline void loadNormalMap(const std::string& _filename) { m_normalMap = load2DTexture(_filename, true); }
        /*!
        * \fn loadMetalMap
        * \brief load the metal map (PBR) texture from a file
        * \param _filename : name of texture image
        */
        inline void loadMetalMap(const std::string& _filename) { m_metalMap = load2DTexture(_filename, true); }
        /*!
        * \fn loadGlossMap
        * \brief load the gloss mao (PBR) texture from a file
        * \param _filename : name of texture image
        */
        inline void loadGlossMap(const std::string& _filename) { m_glossMap = load2DTexture(_filename, true); }
        /*!
        * \fn loadAmbientMap
        * \brief load the ambient map texture from a file
        * \param _filename : name of texture image
        */
        inline void loadAmbientMap(const std::string& _filename) { m_ambientMap = load2DTexture(_filename, true); }

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

        /*! 
        * \fn toggleWireframeRenderFlag 
        * \brief reverse wireframe rendering flag
        */
        inline void toggleWireframeRenderFlag() { m_wireframeRenderOn = !m_wireframeRenderOn; }


        /*! 
        * \fn toggleWireframeShadingFlag 
        * \brief reverse wireframe shading flag
        */
        inline void toggleWireframeShadingFlag() { m_wireframeShadingOn = !m_wireframeShadingOn; }
        

    protected:

        /*------------------------------------------------------------------------------------------------------------+
        |                                                ATTRIBUTES                                                   |
        +-------------------------------------------------------------------------------------------------------------*/

        GLuint m_program;           /*!< handle of the program object (i.e. shaders) for shaded surface rendering */
        GLuint m_programWF;         /*!< handle of the program object (i.e. shaders) for wireframe rendering */

        GLuint m_meshVAO;           /*!< mesh VAO (i.e. array in which the generated vertex array object names are stored) */
        GLuint m_defaultVAO;        /*!< default VAO */

        GLuint m_vertexVBO;         /*!< name of vertex 3D coords VBO */
        GLuint m_normalVBO;         /*!< name of normal vector VBO */
        GLuint m_colorVBO;          /*!< name of rgb color VBO */
        GLuint m_tangentVBO;        /*!< name of tangent vector VBO */
        GLuint m_bitangentVBO;      /*!< name of bitangent vector VBO */
        GLuint m_uvVBO;             /*!< name of UV coords VBO */
        GLuint m_indexVBO;          /*!< name of index VBO */
        GLuint m_facenormalVBO;     /*!< name of face normal VBO */

        int m_numVertices;          /*!< number of vertices in the VBOs */
        int m_numIndices;           /*!< number of indices in the index VBO */

        GLuint m_tex;               /*!< name of texture */
        GLuint m_normalMap;         /*!< name of normal map texture */
        GLuint m_metalMap;          /*!< name of metal map texture */
        GLuint m_glossMap;          /*!< name of gloss map texture */
        GLuint m_ambientMap;        /*!< name of ambient map texture */
        GLuint m_cubeMap;           /*!< name of cube map texture */

        float m_specPow;            /*!< specular power */

        glm::vec3 m_ambientColor;   /*!< ambient color */
        glm::vec3 m_diffuseColor;   /*!< diffuse color */
        glm::vec3 m_specularColor;  /*!< specular color */

        glm::vec3 m_wireColor;      /*!< line color for wireframe rendering */

        bool m_useAmbient;          /*!< flag to use ambient shading or not */
        bool m_useDiffuse;          /*!< flag to use diffuse shading or not */
        bool m_useSpecular;         /*!< flag to use specular shading or not */
        bool m_useTex;              /*!< flag to use texture mapping or not  */
        bool m_useNormalMap;        /*!< flag to use normal mapping or not */
        bool m_showNormals;         /*!< flag to display normals as colors or not */
        bool m_flatShading;         /*!< flag to use flat shading or not */
        bool m_useGammaCorrec;      /*!< flag to apply gamma correction or not */
        bool m_useFaceNormals;      /*!< flag to use face normals or not */
        bool m_useMeshCol;          /*!< flag to use mesh color or not */

        bool m_vertexProvided;      /*!< flag to indicate if vertex coords are available or not */
        bool m_normalProvided;      /*!< flag to indicate if normals are available or not */
        bool m_colorProvided;       /*!< flag to indicate if colors are available or not */
        bool m_tangentProvided;     /*!< flag to indicate if tangents are available or not */
        bool m_bitangentProvided;   /*!< flag to indicate if bitangnets are available or not */
        bool m_uvProvided;          /*!< flag to indicate if uv coords are available or not */
        bool m_indexProvided;       /*!< flag to indicate if indices are available or not */
        bool m_facenormalProvided;   /*!< flag to indicate if face normals are available or not */

        bool m_shadedRenderOn;      /*!< flag to indicate if shaded rendering is on */
        bool m_wireframeRenderOn;   /*!< flag to indicate if wireframe rendering is on */
        bool m_wireframeShadingOn;  /*!< flag to indicate if wireframe shading is on */

        /*------------------------------------------------------------------------------------------------------------+
        |                                               OTHER METHODS                                                 |
        +-------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn load2DTexture
        * \brief load a 2D image to be used as texture
        * \param _filename : name of texture image
        * \param _repeat : repeat (true) or clamptoedge (false)
        */
        GLuint load2DTexture(const std::string& _filename, bool _repeat = false);

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