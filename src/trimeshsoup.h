#ifndef TRIMESHSOUP_H
#define TRIMESHSOUP_H

#include <map>


#include "mesh.h"


/*!
* \class TriMeshSoup
* \brief Triangle soup mesh (i.e. no adjacency information)
* Read and write STL and OBJ files and store data in dynamic arrays
* Duplicate vertices data to handle multiple UV coords and/or normals
*/
class TriMeshSoup : public Mesh 
{
    public:

        /*------------------------------------------------------------------------------------------------------------+
        |                                        CONSTRUCTORS / DESTRUCTORS                                           |
        +------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn TriMeshSoup
        * \brief Default constructor of TriMeshSoup
        */
        TriMeshSoup();

        /*!
        * \fn TriMeshSoup
        * \brief Constructor of TriMeshSoup with various vertex properties.
        *        Other properties can still be requested when needed.
        * \param _normals : true if mesh needs normals
        * \param _texCoords2D : true if mesh needs texCoords2D
        * \param _col : true if mesh needs colors
        */
        TriMeshSoup(bool _normals, bool _texCoords2D, bool _col);

        /*!
        * \fn ~TriMeshSoup
        * \brief Destructor of TriMeshSoup
        */
        ~TriMeshSoup();


        /*------------------------------------------------------------------------------------------------------------+
        |                                              GETTERS/SETTERS                                                |
        +-------------------------------------------------------------------------------------------------------------*/

        /*! \fn getVertices */
        void getVertices(std::vector<glm::vec3>& _vertices);
        /*! \fn getNormals */
        void getNormals(std::vector<glm::vec3>& _normals);
        /*! \fn getIndices */
        void getIndices(std::vector<uint32_t>& _indices);

        /*! \fn getColors */
        void getColors(std::vector<glm::vec3>& _colors);
        /*! \fn getTexCoords */
        void getTexCoords(std::vector<glm::vec2>& _texcoords);
        /*! \fn getTangents */
        void getTangents(std::vector<glm::vec3>& _tangents);
        /*! \fn getBitangents */
        void getBitangents(std::vector<glm::vec3>& _bitangents);

        /*! \fn getFaceNormals */
        void getFaceNormals(std::vector<glm::vec3>& _facenormals);

        /*------------------------------------------------------------------------------------------------------------+
        |                                               OTHER METHODS                                                 |
        +-------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn readFile
        * \brief read a mesh from a file
        * \param _filename : name of the file to read
        * \return false if file extension is not supported, true if it is
        */
        bool readFile(std::string _filename);

        /*!
        * \fn writeFile
        * \brief write a mesh into a file (Wavefront OBJ and STL supported)
        * \param _filename : name of the file to write
        * \return false if file extension is not supported, true if it is
        */
        bool writeFile(std::string _filename);

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
        * \fn computeTB
        * \brief Compute tangent and bitangent vectors for all vertices of the mesh
        *        http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
        */
        void computeTB();

        /*
        * \fn duplicateVertices
        * \brief Duplicates vertex attributes for each adjacent triangle.
        */
        void duplicateVertices();


    protected:


        /*------------------------------------------------------------------------------------------------------------+
        |                                                ATTRIBUTES                                                   |
        +-------------------------------------------------------------------------------------------------------------*/
                                        
        std::vector<glm::vec3> m_vertices;      /*!< vertices positions array (3D coords) */
        std::vector<glm::vec3> m_normals;       /*!< vertices normal vectors array (3D coords) */
        std::vector<uint32_t> m_indices;        /*!< vertices indices array (uint) */

        std::vector<glm::vec3> m_colors;        /*!< vertices RGB colors array (3D coords) */
        std::vector<glm::vec2> m_texcoords;     /*!< vertices uvs array (2D coords) */
        std::vector<glm::vec3> m_tangents;      /*!< vertices tangent vectors array (3D coords) */
        std::vector<glm::vec3> m_bitangents;    /*!< vertices bitangent vectors array (3D coords) */

        std::vector<glm::vec3> m_facenormals;   /*!< face normak vectors array (3D coords) */

        bool m_isVertDuplicated;                /*!< flag if vertices have been duplicated */


        /*------------------------------------------------------------------------------------------------------------+
        |                                               OTHER METHODS                                                 |
        +-------------------------------------------------------------------------------------------------------------*/


        /*!
        * \fn importOBJ
        * \brief read OBJ file
        * \param _filename: name of file
        */
        bool importOBJ(const std::string &_filename);

        /*!
        * \fn exportOBJ
        * \brief Writes the mesh into a file, using the Wavefront OBJ format.
        *        BEWARE: This is a dummy writing of the attribute vectors.
        *                The normals and UVs (if they exist) are duplicated for each vertex and are written as it is.
        * \param _filename: name of file to write
        */
        void exportOBJ(const std::string &_filename);

        /*!
        * \fn importOFF
        * \brief read OFF file
        * \param _filename: name of file
        */
        bool importOFF(const std::string &_filename);

        /*!
        * \fn exportOFF
        * \brief Writes the mesh into a file, using the OFF format.
        * \param _filename: name of file to write
        */
        void exportOFF(const std::string &_filename);


        /*!
        * \fn importPLY
        * \brief read PLY file
        * \param _filename: name of file
        */
        bool importPLY(const std::string &_filename);

        /*!
        * \fn exportPLY
        * \brief Writes the mesh into a file, using the PLY format.
        * \param _filename: name of file to write
        */
        void exportPLY(const std::string &_filename);

        /*!
        * \fn importSTL
        * \brief Create a mesh by reading an STL file ( calling readSTL() ) and filling trimesh with the data.
        *        Check if the file is ASCII or binary and calls the appropriate function ( i.e. either readSTLAscii() or readSTLBinary() ).
        * \param _filename: name of file to read
        * \return true if the file can be opened, false if not
        */
        bool importSTL(const std::string &_filename);

        /*!
        * \fn readSTLAscii
        * \brief Read an ASCII STL file.
        *        BEWARE: Read face normals (if available) and duplicate them as vertices normals.
        *        This function is called by readSTL().
        * \param _file: file to read
        * \param _vertex_data: vertex 3D position buffer to fill
        * \param _normal_data: normals buffer to fill
        */
        void readSTLAscii(FILE * const _file, std::vector<float> * const _vertex_data,  std::vector<float> * const _normal_data);

        /*!
        * \fn readSTLBinary
        * \brief Read a binary STL file.
        *        BEWARE: Read 3D coords only.
        *        This function is called by readSTL().
        * \param _file: file to read
        * \param _vertex_data: vertex 3D position buffer to fill
        */
        void readSTLBinary(FILE * const _file, std::vector<float> * const _vertex_data);

        /*!
        * \fn readSTL
        * \brief Open an STL file and read its content. 
        *        Check if the file is ASCII or binary and calls the appropriate function ( i.e. either readSTLAscii() or readSTLBinary() ).
        * \param _filename: name of file to read
        * \param _vertex_data: vertex 3D position buffer to fill
        * \param _normal_data: normals buffer to fill
        * \return true if the file can be opened, false if not
        */
        bool readSTL(const std::string &_filename, std::vector<float> * const _vertex_data, std::vector<float> * const _normal_data);

        /*!
        * \fn exportSTL
        * \brief write mesh in an binary STL file
        * \param _filename: name of file
        */
        void exportSTL(const std::string &_filename);

        /*!
        * \fn clear
        * \brief Clear the content of all the attribute vectors
        */
        void clear();

};
#endif // TRIMESHSOUP_H