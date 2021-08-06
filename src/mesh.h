#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

/*!
* \class Mesh
* \brief Abstract class for mesh data structure
* This class only contains pure virtual functions (must be overdridden in dervided classes)
*/
class Mesh
{
    public:

        /*------------------------------------------------------------------------------------------------------------+
        |                                              GETTERS/SETTERS                                                |
        +-------------------------------------------------------------------------------------------------------------*/

        virtual void getVertices(std::vector<glm::vec3>& _vertices) = 0;
        virtual void getNormals(std::vector<glm::vec3>& _normals) = 0;
        virtual void getIndices(std::vector<uint32_t>& _indices) = 0;

        virtual void getColors(std::vector<glm::vec3>& _colors) = 0;
        virtual void getTexCoords(std::vector<glm::vec2>& _texcoords) = 0;
        virtual void getTangents(std::vector<glm::vec3>& _tangents) = 0;
        virtual void getBitangents(std::vector<glm::vec3>& _bitangents) = 0;

        virtual void getFaceNormals(std::vector<glm::vec3>& _facenormals) = 0;

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

        virtual bool readFile(std::string _filename) = 0;
        virtual bool writeFile(std::string _filename) = 0;

        virtual void computeAABB() = 0;

        virtual void duplicateVertices()
        { 
            std::cerr << "[WARNING] Mesh::duplicateVertices(): this virtual function should not be executed (it should only be executed by sub-classes overriding)" << std::endl; 
        }

        virtual void computeNormals() = 0;
        virtual void computeTB() = 0;

        virtual void lapSmooth(unsigned int _nbIter = 1, float _fact = 1.0f )
        { 
            std::cerr << "[WARNING] Mesh::lapSmooth(): this virtual function should not be executed (it should only be executed by sub-classes overriding)" << std::endl; 
        }

    protected:

        /*------------------------------------------------------------------------------------------------------------+
        |                                                ATTRIBUTES                                                   |
        +-------------------------------------------------------------------------------------------------------------*/

        bool m_TBComputed;                      /*!< Flag that indicates if tangent and bitangents had been computed */

        glm::vec3 m_bBoxMin;                    /*!< 3D coordinates of the min corner of the bounding box */
        glm::vec3 m_bBoxMax;                    /*!< 3D coordinates of the max corner of the bounding box */

        /*------------------------------------------------------------------------------------------------------------+
        |                                               OTHER METHODS                                                 |
        +-------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn compTandBTt
        * \brief Compute tangent and bitangent vectors from delta uv and delta pos
        *        http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
        * \param _delta_pos1 : vector along first edge of triangle
        * \param _delta_pos2 : vector along second edge of triangle
        * \param _delta_uv1 : differences in uv coords along first edge of triangle
        * \param _delta_uv1 : differences in uv coords along second edge of triangle
        * \param _tangent : result tangent vector
        * \param _bitangent : result bitangent vector
        */
        void compTandBT(glm::vec3 _delta_pos1, glm::vec3 _delta_pos2, glm::vec3 _delta_uv1, glm::vec3 _delta_uv2, glm::vec3& _tangent, glm::vec3& _bitangent)
        {
            glm::normalize(_delta_pos1);
            glm::normalize(_delta_pos2);
            glm::normalize(_delta_uv1);
            glm::normalize(_delta_uv2);

	        float r = 1.0f / (_delta_uv1.x * _delta_uv2.y - _delta_uv1.y * _delta_uv2.x);
            _tangent = (_delta_pos1 * _delta_uv2.y   - _delta_pos2 * _delta_uv1.y)*r;
            _bitangent = (_delta_pos2 * _delta_uv1.x   - _delta_pos1 * _delta_uv2.x)*r;

            glm::normalize(_tangent);
            glm::normalize(_bitangent);
        }


};
#endif // MESH_H