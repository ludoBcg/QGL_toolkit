#ifndef TRIMESHSOUP_H
#define TRIMESHSOUP_H

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>


#include <map>


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
        * \fn TriMesh
        * \brief Constructor of TriMesh with various vertex properties.
        *        Other properties can still be requested when needed.
        * \param _normals : true if mesh needs normals
        * \param _texCoords2D : true if mesh needs texCoords2D
        * \param _col : true if mesh needs colors
        */
        TriMesh(bool _normals, bool _texCoords2D, bool _col);

        /*!
        * \fn ~TriMesh
        * \brief Destructor of TriMesh
        */
        ~TriMesh();


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
        * \fn computeAABB
        * \brief compute Axis Oriented Bounding Box
        */
        void computeAABB();

        /*!
        * \fn computeNormals
        * \brief recompute the triangle normals and update vertex normals
        */
        void computeNormals();



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
        * \fn clear
        * \brief Clear the content of all the attribute vectors
        */
        void clear();

};
#endif // TRIMESHSOUP_H