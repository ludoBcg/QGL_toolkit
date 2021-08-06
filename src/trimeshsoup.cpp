
#include <algorithm>
#include <functional>
#include <ios>
	

#include "trimeshsoup.h"

TriMeshSoup::TriMeshSoup()
{
    m_TBComputed = false;
    m_isVertDuplicated = false;

    m_bBoxMin = glm::vec3(0.0f, 0.0f, 0.0f);
    m_bBoxMax = glm::vec3(0.0f, 0.0f, 0.0f);
}


TriMeshSoup::TriMeshSoup(bool _normals, bool _texCoords2D, bool _col)
{
    m_TBComputed = false;
    m_isVertDuplicated = false;

    m_bBoxMin = glm::vec3(0.0f, 0.0f, 0.0f);
    m_bBoxMax = glm::vec3(0.0f, 0.0f, 0.0f);
}


TriMeshSoup::~TriMeshSoup()
{
    clear();
}


void TriMeshSoup::getVertices(std::vector<glm::vec3>& _vertices)
{
    if(_vertices.size() != 0)
        _vertices.clear();

    if(m_vertices.size() != 0)
    {
        _vertices.assign(m_vertices.begin(), m_vertices.end()); // new data storage
    }
    else
    {
        std::cerr << "[WARNING] TriMeshSoup::getVertices: Empty vertices array" << std::endl;
    }
}


void TriMeshSoup::getNormals(std::vector<glm::vec3>& _normals)
{
    if(_normals.size() != 0)
        _normals.clear();

    if(m_normals.size() != 0)
    {
        _normals.assign(m_normals.begin(), m_normals.end());
    }
    else
    {
        std::cerr << "[WARNING] TriMeshSoup::getNormals: Empty normals array" << std::endl;
    }
}


void TriMeshSoup::getIndices(std::vector<uint32_t>& _indices)
{
    if(_indices.size() != 0)
        _indices.clear();

    if(m_indices.size() != 0)
    {
        _indices.assign(m_indices.begin(), m_indices.end());
    }
    else
    {
        std::cerr << "[WARNING] TriMeshSoup::getIndices: Empty indices array" << std::endl;
    }
}


void TriMeshSoup::getColors(std::vector<glm::vec3>& _colors)
{
    if(_colors.size() != 0)
        _colors.clear();

    if(m_colors.size() != 0)
    {
        _colors.assign(m_colors.begin(), m_colors.end()); 
    }
}


void TriMeshSoup::getTexCoords(std::vector<glm::vec2>& _texcoords)
{
    if(_texcoords.size() != 0)
        _texcoords.clear();

    if(m_texcoords.size() != 0)
    {
        _texcoords.assign(m_texcoords.begin(), m_texcoords.end());
    }
}


void TriMeshSoup::getTangents(std::vector<glm::vec3>& _tangents)
{
    if(_tangents.size() != 0)
        _tangents.clear();

    if(m_tangents.size() != 0)
    {
        _tangents.assign(m_tangents.begin(), m_tangents.end());
    }
}


void TriMeshSoup::getBitangents(std::vector<glm::vec3>& _bitangents)
{
    if(_bitangents.size() != 0)
        _bitangents.clear();

    if(m_bitangents.size() != 0)
    {
        _bitangents.assign(m_bitangents.begin(), m_bitangents.end());
    }
}


void TriMeshSoup::getFaceNormals(std::vector<glm::vec3>& _facenormals)
{
    if(_facenormals.size() != 0)
        _facenormals.clear();

    if(m_facenormals.size() != 0)
    {
        _facenormals.assign(m_facenormals.begin(), m_facenormals.end());
    }
}


bool TriMeshSoup::readFile(std::string _filename)
{
    if(_filename.substr(_filename.find_last_of(".") + 1) == "obj")
    {
        importOBJ(_filename);
        return true;
    }
    if(_filename.substr(_filename.find_last_of(".") + 1) == "off")
    {
        importOFF(_filename);
        return true;
    }
    if(_filename.substr(_filename.find_last_of(".") + 1) == "ply")
    {
        importPLY(_filename);
        return true;
    }
    else if(_filename.substr(_filename.find_last_of(".") + 1) == "stl") 
    {
        importSTL(_filename);
        return true;
    }
    else
    {
        std::cerr << "[ERROR] TriMeshSoup::readFile(): Invalid file extension: only .obj, .off, .ply, and .stl are supported" << std::endl;
    }
    return false;
}


bool TriMeshSoup::writeFile(std::string _filename)
{
    if(_filename.substr(_filename.find_last_of(".") + 1) == "obj")
    {
        exportOBJ(_filename);
        return true;
    }
    if(_filename.substr(_filename.find_last_of(".") + 1) == "off")
    {
        exportOFF(_filename);
        return true;
    }
    if(_filename.substr(_filename.find_last_of(".") + 1) == "ply")
    {
        exportPLY(_filename);
        return true;
    }
    else if(_filename.substr(_filename.find_last_of(".") + 1) == "stl") 
    {
        exportSTL(_filename);
        return true;
    }
    else
    {
        std::cerr << " [ERROR] TriMeshSoup::writeFile(): Invalid file extension: only .obj, .off, and .stl are supported" << std::endl;
    }
    return false;
}


void TriMeshSoup::computeAABB()
{
    if(m_vertices.size() != 0)
    {
        // init values
        glm::vec3 min = m_vertices[0];
        glm::vec3 max = m_vertices[0];

        for (unsigned int i = 0; i <m_vertices.size(); i++) 
        {
            if(m_vertices[i].x < min.x) { min.x = m_vertices[i].x; }
            if(m_vertices[i].y < min.y) { min.y = m_vertices[i].y; }
            if(m_vertices[i].z < min.z) { min.z = m_vertices[i].z; }
            if(m_vertices[i].x > max.x) { max.x = m_vertices[i].x; }
            if(m_vertices[i].y > max.y) { max.y = m_vertices[i].y; }
            if(m_vertices[i].z > max.z) { max.z = m_vertices[i].z; }
        }
        m_bBoxMin = min;
        m_bBoxMax = max;
    }
    else
    {
        std::cerr << "[WARNING] TriMeshSoup::computeAABB: Empty vertices array" << std::endl;
        m_bBoxMin = glm::vec3(0.0f, 0.0f, 0.0f);
        m_bBoxMax = glm::vec3(0.0f, 0.0f, 0.0f);
    }
}



void TriMeshSoup::computeNormals()
{
    
    if(m_isVertDuplicated)
        std::cerr << "[WARNING] TriMeshSoup::computeNormals(): Vertices are already duplicated, vertex normal cannot be properly calculated" << std::endl;
    else
        std::cerr << "[INFO] TriMeshSoup::computeNormals(): Vertices will be duplicated to calculate face normal vertex attribute" << std::endl;

    
    std::int32_t vertexIndex0, vertexIndex1, vertexIndex2;
    glm::vec3 normal;

    if(!m_isVertDuplicated)
    {
        m_normals.clear();
        m_normals.resize(m_vertices.size(), glm::vec3(0.0f, 0.0f, 0.0f));

        // 1. compute per-vertex normals by averaging the unnormalized face normals
        for (unsigned int i = 0; i < m_indices.size(); i += 3) 
        {
            vertexIndex0 = m_indices[i];
            vertexIndex1 = m_indices[i + 1];
            vertexIndex2 = m_indices[i + 2];

            //compute current face normal
            glm::vec3 faceNormal = glm::cross(m_vertices[vertexIndex1] - m_vertices[vertexIndex0],  m_vertices[vertexIndex2] - m_vertices[vertexIndex0]);
            if( faceNormal != glm::vec3(0.0f) )
                faceNormal = glm::normalize(faceNormal);

            // add current face normal to adjacent vertices
            m_normals[vertexIndex0] += faceNormal;
            m_normals[vertexIndex1] += faceNormal;
            m_normals[vertexIndex2] += faceNormal;
        }
        for (unsigned int i = 0; i < m_normals.size(); i++) 
        {
            // vertex normal is avg of adjacent faces' normals
            m_normals[i] = glm::normalize(m_normals[i]);
        }

        // 2. duplicate vertices so we can compute face normals
        //duplicateVertices(); // @@
    }
    else
    {
        // 3. compute face normals for flat shading

        m_facenormals.clear();
        m_facenormals.resize(m_vertices.size(), glm::vec3(0.0f, 0.0f, 0.0f));

        for (unsigned int i = 0; i < m_indices.size(); i += 3) 
        {
            vertexIndex0 = m_indices[i];
            vertexIndex1 = m_indices[i + 1];
            vertexIndex2 = m_indices[i + 2];
            glm::vec3 faceNormal = glm::cross(m_vertices[vertexIndex1] - m_vertices[vertexIndex0],  m_vertices[vertexIndex2] - m_vertices[vertexIndex0]);
            if( faceNormal != glm::vec3(0.0f) )
                faceNormal = glm::normalize(faceNormal);

            // add face normals
            m_facenormals[vertexIndex0] = faceNormal;
            m_facenormals[vertexIndex1] = faceNormal;
            m_facenormals[vertexIndex2] = faceNormal;
        }
    }
    

    std::cout << "[INFO] TriMeshSoup::computeNormals(): Normals computed" << std::endl;
}


void TriMeshSoup::computeTB()
{
    if( m_normals.size() == 0)
    {
        std::cerr << "[WARNING] TriMeshSoup::computeTB(): normals not available" << std::endl; 
        computeNormals();
    }

    if( m_texcoords.size() != 0)
    {
        m_tangents.resize( m_normals.size() );
        m_bitangents.resize( m_normals.size() );
        //for each triangle
        for (int i = 0; i < m_indices.size(); i += 3)
        {
            int vertexIndex0 = m_indices[i];
            int vertexIndex1 = m_indices[i + 1];
            int vertexIndex2 = m_indices[i + 2];

            glm::vec3 tangent, bitangent;
            compTandBT( m_vertices[vertexIndex1] - m_vertices[vertexIndex0], 
                        m_vertices[vertexIndex2] - m_vertices[vertexIndex0], 
                        glm::vec3( m_texcoords[vertexIndex1].x,  m_texcoords[vertexIndex1].y, 0.0f ) - glm::vec3( m_texcoords[vertexIndex0].x,  m_texcoords[vertexIndex0].y, 0.0f ), 
                        glm::vec3( m_texcoords[vertexIndex2].x,  m_texcoords[vertexIndex2].y, 0.0f ) - glm::vec3( m_texcoords[vertexIndex0].x,  m_texcoords[vertexIndex0].y, 0.0f ), 
                        tangent, bitangent);
            m_tangents[vertexIndex0] = tangent;
            m_bitangents[vertexIndex0] = bitangent;
            compTandBT( m_vertices[vertexIndex2] - m_vertices[vertexIndex1], 
                        m_vertices[vertexIndex0] - m_vertices[vertexIndex1], 
                        glm::vec3( m_texcoords[vertexIndex2].x,  m_texcoords[vertexIndex2].y, 0.0f ) - glm::vec3( m_texcoords[vertexIndex1].x,  m_texcoords[vertexIndex1].y, 0.0f ), 
                        glm::vec3( m_texcoords[vertexIndex0].x,  m_texcoords[vertexIndex0].y, 0.0f ) - glm::vec3( m_texcoords[vertexIndex1].x,  m_texcoords[vertexIndex1].y, 0.0f ),
                        tangent, bitangent);
            m_tangents[vertexIndex1] = tangent;
            m_bitangents[vertexIndex1] = bitangent;
            compTandBT( m_vertices[vertexIndex0] - m_vertices[vertexIndex2], 
                        m_vertices[vertexIndex1] - m_vertices[vertexIndex2], 
                        glm::vec3( m_texcoords[vertexIndex0].x,  m_texcoords[vertexIndex0].y, 0.0f ) - glm::vec3( m_texcoords[vertexIndex2].x,  m_texcoords[vertexIndex2].y, 0.0f ), 
                        glm::vec3( m_texcoords[vertexIndex1].x,  m_texcoords[vertexIndex1].y, 0.0f ) - glm::vec3( m_texcoords[vertexIndex2].x,  m_texcoords[vertexIndex2].y, 0.0f ),
                        tangent, bitangent);
            m_tangents[vertexIndex2] = tangent;
            m_bitangents[vertexIndex2] = bitangent;
        }
    }
    else
    {
        std::cerr << "[WARNING] TriMeshSoup::computeTB(): texcoords not available" << std::endl; 
    }
}



void TriMeshSoup::duplicateVertices()
{
    m_isVertDuplicated = false;

    // Check if data is available 
    if(m_indices.size() == 0 || m_vertices.size() ==0 )
    {
        std::cerr << "[WARNING] TriMeshSoup::duplicateVertices(): Vertex data incomplete" << std::endl;
        return;
    }
    bool hasNormals = (m_normals.size() != 0 );
    bool hasColors = (m_colors.size() != 0 );
    bool hasUVs = (m_texcoords.size() != 0 );


    // Check consistency of vertex attributes
    if( m_vertices.size() != m_normals.size() && hasNormals)
    {
        std::cerr << "[WARNING] TriMeshSoup::duplicateVertices(): arrays of vertex coords and normal coords have different sizes" << std::endl;
        return;
    }
    if( m_vertices.size() != m_colors.size() && hasColors)
    {
        std::cerr << "[WARNING] TriMeshSoup::duplicateVertices(): arrays of vertex coords and colors have different sizes" << std::endl;
        return;
    }
    if( m_vertices.size() != m_texcoords.size() && hasUVs)
    {
        std::cerr << "[WARNING] TriMeshSoup::duplicateVertices(): arrays of vertex coords and UV coords have different sizes" << std::endl;
        return;
    }


    // Store vertex attributes in temporary arrays
    std::vector<glm::vec3> temp_vertices = m_vertices;
    std::vector<glm::vec3> temp_normals = m_normals;
    std::vector<glm::vec3> temp_colors = m_colors;
    std::vector<glm::vec2> temp_texcoords = m_texcoords;
    // clear arrays
    m_vertices.clear();
    m_normals.clear();
    m_colors.clear();
    m_texcoords.clear();


    // duplicate vertices with all their attributes for each adjacent triangle
    for(unsigned int i=0; i<m_indices.size(); i++)
    {
        m_vertices.push_back(temp_vertices[ m_indices[i] ]);

        if(hasNormals)
            m_normals.push_back(temp_normals[ m_indices[i] ]);

        if(hasColors)
            m_colors.push_back(temp_colors[ m_indices[i] ]);

        if(hasUVs)
            m_texcoords.push_back(temp_texcoords[ m_indices[i] ]);

        // update indices
        m_indices[i] = i;
    }

    // clear temporary arrays
    temp_vertices.clear();
    temp_normals.clear();
    temp_colors.clear();
    temp_texcoords.clear();

    std::cout << "[INFO] TriMeshSoup::duplicateVertices(): Vertices duplicated" << std::endl;

    m_isVertDuplicated = true;
}



// Read an Mesh from an .obj file. This function can read texture
// coordinates and/or normals, in addition to vertex positions.
bool TriMeshSoup::importOBJ(const std::string &_filename)
{
    struct uvec3Less 
    {
        bool operator() (const glm::uvec3 &a, const glm::uvec3 &b) const
        {
            return (a.x < b.x) |
                    ((a.x == b.x) & (a.y < b.y)) |
                    ((a.x == b.x) & (a.y == b.y) & (a.z < b.z));
        }
    };

    const std::string VERTEX_LINE("v ");
    const std::string TEXCOORD_LINE("vt ");
    const std::string NORMAL_LINE("vn ");
    const std::string FACE_LINE("f ");

    std::string line;
    glm::vec3 vertex;
    glm::vec3 normal;
    glm::vec3 texcoord;
    std::uint32_t vindex[3];
    std::uint32_t tindex[3];
    std::uint32_t nindex[3];

    // Open OBJ file
    std::ifstream f(_filename.c_str());
    if(!f.is_open()) 
    {
        std::cerr << "[ERROR] TriMeshSoup::importOBJ(): Could not open " << _filename << std::endl;
        return false;
    }

    // First pass: read vertex data into temporary mesh
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<uint32_t> indices;
    std::vector<glm::vec3> texcoords;
    while(!f.eof()) 
    {
        std::getline(f, line);
        if (line.substr(0, 2) == VERTEX_LINE) 
        {
            std::istringstream ss(line.substr(2));
            ss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        }
        else if (line.substr(0, 3) == TEXCOORD_LINE) 
        {
            std::istringstream ss(line.substr(3));
            ss >> texcoord.x >> texcoord.y;// >> texcoord.z;
            texcoords.push_back(texcoord);
        }
        else if (line.substr(0, 3) == NORMAL_LINE) 
        {
            std::istringstream ss(line.substr(3));
            ss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else 
        {
            // Ignore line
        }
    }

    // Rewind file
    f.clear();
    f.seekg(0);

    // Clear old mesh and pre-allocate space for new mesh data
    m_vertices.clear();
    m_vertices.reserve(vertices.size());
    m_texcoords.clear();
    m_texcoords.reserve(texcoords.size());
    m_normals.clear();
    m_normals.reserve(normals.size());
    m_indices.clear();

    // Set up dictionary for mapping unique tuples to indices
    std::map<glm::uvec3, unsigned, uvec3Less> visited;
    unsigned next_index = 0;
    glm::uvec3 key;

    // Second pass: read faces and construct per-vertex texcoords/normals.
    // Note: OBJ-indices start at one, so we need to subtract indices by one.
    while (!f.eof()) 
    {
        std::getline(f, line);
        if (line.substr(0, 2) == FACE_LINE) 
        {
            if (std::sscanf(line.c_str(), "f %d %d %d", &vindex[0], &vindex[1], &vindex[2]) == 3) 
            {
                for (unsigned i = 0; i < 3; ++i) 
                {
                    key = glm::uvec3(vindex[i], 0, 0);
                    if (visited.count(key) == 0) 
                    {
                        visited[key] = next_index++;
                        m_vertices.push_back(vertices[vindex[i] - 1]);
                    }
                    m_indices.push_back(visited[key]);
                }
            }
            else if (std::sscanf(line.c_str(), "f %d/%d %d/%d %d/%d", &vindex[0], &tindex[0], &vindex[1], &tindex[1], &vindex[2], &tindex[2]) == 6) 
            {
                for (unsigned i = 0; i < 3; ++i) 
                {
                    key = glm::uvec3(vindex[i], tindex[i], 0);
                    if (visited.count(key) == 0) 
                    {
                        visited[key] = next_index++;
                        m_vertices.push_back(vertices[vindex[i] - 1]);
                        m_texcoords.push_back( glm::vec2(texcoords[tindex[i] - 1].x, texcoords[tindex[i] - 1].y) );
                    }
                    m_indices.push_back(visited[key]);
                }
            }
            else if (std::sscanf(line.c_str(), "f %d//%d %d//%d %d//%d", &vindex[0], &nindex[0], &vindex[1], &nindex[1], &vindex[2], &nindex[2]) == 6) 
            {
                for (unsigned i = 0; i < 3; ++i) 
                {
                    key = glm::uvec3(vindex[i], nindex[i], 0);
                    if (visited.count(key) == 0) 
                    {
                        visited[key] = next_index++;
                        m_vertices.push_back(vertices[vindex[i] - 1]);
                        m_normals.push_back(normals[nindex[i] - 1]);
                    }
                    m_indices.push_back(visited[key]);
                }
            }
            else if(std::sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &vindex[0], &tindex[0], &nindex[0], &vindex[1], &tindex[1], &nindex[1], &vindex[2], &tindex[2], &nindex[2]) == 9) 
            {
                for(unsigned i = 0; i < 3; ++i) 
                {
                    key = glm::uvec3(vindex[i], tindex[i], nindex[i]);
                    if (visited.count(key) == 0) 
                    {
                        visited[key] = next_index++;
                        m_vertices.push_back(vertices[vindex[i] - 1]);
                        m_texcoords.push_back( glm::vec2(texcoords[tindex[i] - 1].x, texcoords[tindex[i] - 1].y) );
                        m_normals.push_back(normals[nindex[i] - 1]);
                    }
                    m_indices.push_back(visited[key]);
                }
            }
        }
    }

    // Compute normals (if OBJ-file did not contain normals)
    if(m_normals.size() == 0) 
    {
        std::cout << "[INFO] TriMeshSoup::importOBJ(): Normals not provided, compute them " << std::endl;
        computeNormals();
    }

    return true;
}


void TriMeshSoup::exportOBJ(const std::string &_filename)
{
    // Open the file
    FILE* file = fopen(_filename.c_str(), "w");
    if( !file) 
    {
        std::cerr << " [ERROR] TriMeshSoup::exportOBJ(): Cannot open file to write" << _filename << std::endl;
    }

    unsigned int nb_triangles = m_indices.size()/3;
    if( m_indices.size() % 3 != 0)
    {
        std::cerr << " [ERROR] TriMeshSoup::exportOBJ(): Number of vertices is not a multiple of 3" << std::endl;
    }

    unsigned int i;

    fprintf(file, "# generated by Mesh_viewer\n" );

    // Write vertices
    fprintf(file, "\n");
    fprintf(file, "# %d vertices\n", m_vertices.size() );
    for (i = 0; i <m_vertices.size(); i++) 
    {
        fprintf(file, "v %f %f %f\n", m_vertices[i].x, m_vertices[i].y, m_vertices[i].z);
    }

    // Write normals
    fprintf(file, "\n");
    fprintf(file, "# %d normals\n", m_normals.size() );
    for (i = 0; i <m_normals.size(); i++) 
    {
        fprintf(file, "vn %f %f %f\n", m_normals[i].x, m_normals[i].y, m_normals[i].z);
    }

    // Write texcoords
    fprintf(file, "\n");
    fprintf(file, "# %d texcoords\n", m_texcoords.size() );
    for (i = 0; i <m_texcoords.size(); i++) 
    {
        fprintf(file, "vt %f %f \n", m_texcoords[i].x, m_texcoords[i].y);
    }

    // Write facets (triangles)
    fprintf(file, "\n");
    fprintf(file, "# %d faces (triangles)\n", nb_triangles );

    for (i = 0; i <nb_triangles; i++) 
    {
        int vertId0 = m_indices[ i * 3 ] + 1; // !! in wavefront files, vertex indices starts at 1
        int vertId1 = m_indices[ i * 3 + 1 ] + 1;
        int vertId2 = m_indices[ i * 3 + 2 ] + 1;

        if(m_texcoords.size() != 0 && m_normals.size() != 0)
            fprintf(file, "f %d/%d/%d %d/%d/%d %d/%d/%d \n", vertId0, vertId0, vertId0, vertId1, vertId1, vertId1, vertId2, vertId2, vertId2);
        else if(m_texcoords.size() != 0 || m_normals.size() != 0)
            fprintf(file, "f %d//%d %d//%d %d//%d \n", vertId0, vertId0, vertId1, vertId1, vertId2, vertId2);
        else
            fprintf(file, "f %d %d %d \n", vertId0, vertId1, vertId2);
    }

    fclose(file);
}

// https://people.sc.fsu.edu/~jburkardt/data/off/off.html
// https://segeval.cs.princeton.edu/public/off_format.html
bool TriMeshSoup::importOFF(const std::string &_filename)
{
    
    // read ASCII

    bool isCOFF = false;
    bool isNOFF = false;
    bool isCNOFF = false;
    
    // Clear old mesh 
    clear();

    glm::vec3 vertex, normal, color;

    // open file
    std::ifstream ifile(_filename.c_str() );

    if (!ifile.is_open() || !ifile.good())
    {
        std::cerr << "[ERROR] TriMeshSoup::importOFF(): cannot open file "  << _filename  << std::endl;
        return false;
    }

    // read header line
    std::string header;
    std::getline(ifile,header);
    
    if(header == "COFF")
    {
        isCOFF = true;
        isNOFF = false;
        isCNOFF = false;
    }
    else if(header == "NOFF")
    {
        isCOFF = false;
        isNOFF = true;
        isCNOFF = false;
    }
    else if(header == "CNOFF")
    {
        isCOFF = false;
        isNOFF = false;
        isCNOFF = true;
    }
    else if(header == "OFF")
    {
        isCOFF = false;
        isNOFF = false;
        isCNOFF = false;
    }
    else
    {
        std::cerr << "[ERROR] TriMeshSoup::importOFF(): wrong header, should be OFF, COFF, NOFF, or CNOFF. "  << _filename  << std::endl;
    }

    // read number of vertices, faces, edges, and vertices per face
    unsigned int nV, nF, nE, nVF;
    nV = 0;
    nF = 0;

    // read nb of vertices and facets
    while(nV == 0 && nF ==0)
    {
        std::getline(ifile,header);
        std::stringstream linestream(header);

        // if not a comment line
        if (header.substr(0, 2) != "# ")
            linestream >> nV >> nF >> nE; //nE unused
    }


    // for each vertex coord line
    std::string line;
    for (unsigned int i = 0; i<nV && std::getline(ifile, line); i++)
    {
        // get line
        std::stringstream linestream(line);

        // read vertex coords and add into array
        if(linestream >> vertex.x >> vertex.y >> vertex.z)
            m_vertices.push_back(vertex);

        // read normals
        if(isNOFF || isCNOFF)
        {
            // read vertex color, normlaze it ([0;255] -> [0;1]), and add into array
            if(linestream >> normal.x >> normal.y >> normal.z)
                m_normals.push_back(normal);
        }
        // read color
        if(isCOFF || isCNOFF)
        {
            // read vertex color, normlaze it ([0;255] -> [0;1]), and add into array
            if(linestream >> color.x >> color.y >> color.z)
                m_colors.push_back(color / 256.0f );
        }
    }



    // read faces
    // #N <v1> <v2> .. <v(n-1)>
    for (int unsigned j = 0 ; j<nF  && std::getline(ifile, line); j++)
    {
        // get line
        //std::getline(ifile, line);
        std::stringstream linestream(line);
        unsigned int v1 , v2, v3;

        // read vertex number
        if(linestream >> nVF)
        {
            if(nVF == 3 )
            {
                // read indices if the face is a triangle
                if(linestream >> v1 >> v2 >> v3)
                {
                    m_indices.push_back(v1);
                    m_indices.push_back(v2);
                    m_indices.push_back(v3);
                }
            }
        }

    }

    // close file  
    ifile.close();

    // Compute normals
    if(m_normals.size() == 0) 
    {
        std::cout << "[INFO] TriMeshSoup::importOFF(): Normals not provided, compute them " << std::endl;
        computeNormals();
    }

    // File was successfully parsed.
    return true;

}


void TriMeshSoup::exportOFF(const std::string &_filename)
{
    // Open the file
    FILE* file = fopen(_filename.c_str(), "w");
    if( !file) 
    {
        std::cerr << "[ERROR] TriMeshSoup::exportOFF(): Cannot open file to write" << _filename << std::endl;
    }

    unsigned int nb_triangles = m_indices.size()/3;
    if( m_indices.size() % 3 != 0)
    {
        std::cerr << "[ERROR] TriMeshSoup::exportOFF(): Number of vertices is not a multiple of 3" << std::endl;
    }

    bool hasColors = ( m_colors.size() == m_vertices.size() );

    // Write header
    if(hasColors)
    {
        // colors: [0;1] -> [0;255]
        std::transform(m_colors.begin(), m_colors.end(), m_colors.begin(), [](glm::vec3 &_c){ return _c * 256.0f; });
        fprintf(file, "COFF\n");
    }
    else
        fprintf(file, "OFF\n");


    fprintf(file, "%d %d %d\n", m_vertices.size(), nb_triangles, 0 );

    // Write vertices
    for (unsigned int i = 0; i <m_vertices.size(); i++) 
    {
        if(hasColors)
            fprintf(file, "%f %f %f %d %d %d\n", m_vertices[i].x, m_vertices[i].y, m_vertices[i].z,  (int)m_colors[i].x, (int)m_colors[i].y, (int)m_colors[i].z);
        else
            fprintf(file, "%f %f %f\n", m_vertices[i].x, m_vertices[i].y, m_vertices[i].z);
    }
  
    // Write facets (triangles)
    for (unsigned int i = 0; i <nb_triangles; i++) 
    {
        int vertId0 = m_indices[ i * 3 ];
        int vertId1 = m_indices[ i * 3 + 1 ];
        int vertId2 = m_indices[ i * 3 + 2 ];

        fprintf(file, "%d %d %d %d \n", 3, vertId0, vertId1, vertId2);
    }

    fclose(file);
}


// binary PLY not supported
// http://paulbourke.net/dataformats/ply/
bool TriMeshSoup::importPLY(const std::string &_filename)
{
    // read ASCII

    bool isASCII = false;
    bool isBinLE = false;
    bool isBinBE = false;
    unsigned int cptLinesHeader = 2; // ply + format

    // Clear old mesh 
    clear();

    int nbVert, nbFaces;

    // open file
    std::ifstream ifile(_filename.c_str() );

    if (!ifile.is_open() || !ifile.good())
    {
        std::cerr << "[ERROR] TriMeshSoup::importPLY(): cannot open file "  << _filename  << std::endl;
        return false;
    }

    // read header

    // ply
    std::string header, line;
    std::getline(ifile,header);
    if(header != "ply")
        std::cerr << "[ERROR] TriMeshSoup::importPLY(): wrong header, should start with ply "  << std::endl;

    
    std::string keyword, structure, type1, type2, valueS;
    float valueF;
    int valueI;
    std::vector<std::string> vertProperties;
    std::vector<std::string> vertPropertiesTypes;

    // read format
    std::stringstream linestream(header);
    linestream >> keyword; 
    // format ascii 1.0
    if(keyword == "format")
    {
        linestream >> type1 >> valueF; 
        if(type1 == "ascii" && valueF == 1.0)
        {
            isASCII = true;
        }
        /*else if(type1 == "binary_little_endian" && valueF == 1.0)
        {
            isBinLE = true;
        }
        else if(type1 == "binary_big_endian" && valueF == 1.0)
        {
            isBinBE = true;
        }*/
        else
            std::cerr << "[ERROR] TriMeshSoup::importPLY(): only file format are ascii/binary_little_endian/binary_big_endian 1.0" << std::endl;
    }


    bool vertProp = false;
    bool faceProp = false;
    int cptVertProp = 0;

    //read header line by line until "end_header"
    while(keyword != "end_header")
    {
        cptLinesHeader++;
        std::getline(ifile,header);
        std::stringstream linestream(header);
        linestream >> keyword;

        // ignore comments
        if(keyword != "comment")
        {
            // element description
            if(keyword == "element")
            {
                linestream >> structure;
                if(structure == "vertex")
                {
                    // element vertex nbvertices
                    vertProp = true;
                    faceProp = false;
                    cptVertProp = 0;

                    linestream >> nbVert;
                }
                else if(structure == "face")
                {
                    // element face nbfaces
                    vertProp = false;
                    faceProp = true;

                    linestream >> nbFaces;
                }
                else
                {
                    vertProp = false;
                    faceProp = false;
                    std::cerr << "[ERROR] TriMeshSoup::importPLY(): wrong header, only elements supported are vertex and face " << std::endl;
                }
            }
            else if (keyword == "property")
            {
                if(vertProp)
                {
                    // property float x/y/z
                    // vertex properties
                    linestream >> type1 >> valueS;
                    vertProperties.push_back(valueS);
                    vertPropertiesTypes.push_back(type1);
                }
                else if(faceProp)
                {
                    // property list uchar int vertex_indices
                    // face properties
                    linestream >> structure >> type1 >> type2 >> valueS;
                    if(structure != "list")
                        std::cerr << "[ERROR] TriMeshSoup::importPLY(): wrong header, only face structure supported is list " << std::endl;
                    if(valueS != "vertex_indices")
                        std::cerr << "[ERROR] TriMeshSoup::importPLY(): wrong header, only face structure supported is list of vertex indices " << std::endl;
                }
            }

        }
    }

    
    bool hasNormal = false;
    bool hasFlag = false;
    bool hasColor = false;
    bool hasAlpha = false;
    glm::vec3 vertex, normal, color;
    int flag, alpha; //unused
    int valence, id1, id2, id3;

    // check for vertex coords presence
    std::vector<std::string>::iterator it1 = find (vertProperties.begin(), vertProperties.end(), "x");
    std::vector<std::string>::iterator it2 = find (vertProperties.begin(), vertProperties.end(), "y");
    std::vector<std::string>::iterator it3 = find (vertProperties.begin(), vertProperties.end(), "z");
    if( it1 == vertProperties.end() || it2 == vertProperties.end() ||  it3 == vertProperties.end() )
    {
        std::cerr << "[ERROR] TriMeshSoup::importPLY(): vertex coordinates not provided " << std::endl;
        return false;
    }

    // check for vertex normal presence
    it1 = find (vertProperties.begin(), vertProperties.end(), "nx");
    it2 = find (vertProperties.begin(), vertProperties.end(), "ny");
    it3 = find (vertProperties.begin(), vertProperties.end(), "nz");
    if( it1 != vertProperties.end() && it2 != vertProperties.end() &&  it3 != vertProperties.end() )
        hasNormal = true;

    it1 = find (vertProperties.begin(), vertProperties.end(), "flags");
    if( it1 != vertProperties.end() )
        hasFlag = true;

    // check for vertex color presence
    it1 = find (vertProperties.begin(), vertProperties.end(), "red");
    it2 = find (vertProperties.begin(), vertProperties.end(), "green");
    it3 = find (vertProperties.begin(), vertProperties.end(), "blue");
    if( it1 != vertProperties.end() && it2 != vertProperties.end() &&  it3 != vertProperties.end() )
        hasColor = true;

    it1 = find (vertProperties.begin(), vertProperties.end(), "alpha");
    if( it1 != vertProperties.end() )
        hasAlpha = true;


    // read vertices
    for (unsigned int i = 0; i<nbVert && std::getline(ifile, line); i++)
    {
        // get line
        std::stringstream linestream(line);

        // read vertex coords and add into array
        if(linestream >> vertex.x >> vertex.y >> vertex.z)
            m_vertices.push_back(vertex);

        // read color
        if(hasNormal)
        {
            // read vertex normal, and add into array
            if(linestream >> normal.x >> normal.y >> normal.z)
                m_normals.push_back(normal);
        }

        // read flag
        if(hasFlag)
            linestream >> flag;

        // read color
        if(hasColor)
        {
            // read vertex color, normlaze it ([0;255] -> [0;1]), and add into array
            if(linestream >> color.x >> color.y >> color.z)
                m_colors.push_back(color / 256.0f );
        }

        // read alpha
        if(hasAlpha)
            linestream >> alpha;
    }

    // read faces
    for (unsigned int i = 0; i<nbFaces && std::getline(ifile, line); i++)
    {
        // get line
        std::stringstream linestream(line);

        //read valence
        linestream >> valence;

        if(valence != 3)
        {
            std::cerr << "[ERROR] TriMeshSoup::importPLY(): only triangles faces suported " << std::endl;
            return false;
        }
        // read vertices' indices
        if(linestream >> id1 >> id2 >> id3)
        {
            m_indices.push_back(id1);
            m_indices.push_back(id2);
            m_indices.push_back(id3);
        }
    }


    // check if size of lists are consistent
    if(m_vertices.size() != nbVert || m_indices.size() != nbFaces*3 ) 
        std::cerr << "[ERROR] TriMeshSoup::importPLY(): insonsistent data " << std::endl;

    return true;
}


// export ascii
void TriMeshSoup::exportPLY(const std::string &_filename)
{
    // Open the file
    FILE* file = fopen(_filename.c_str(), "w");
    if( !file) 
    {
        std::cerr << "[ERROR] TriMeshSoup::exportPLY(): Cannot open file to write" << _filename << std::endl;
    }

    unsigned int nb_triangles = m_indices.size()/3;
    if( m_indices.size() % 3 != 0)
    {
        std::cerr << "[ERROR] TriMeshSoup::exportPLY(): Number of vertices is not a multiple of 3" << std::endl;
    }

    bool hasColors = ( m_colors.size() == m_vertices.size() );
    bool hasNormals = ( m_normals.size() == m_vertices.size() );

    // Write header
    fprintf(file, "ply\n");
    fprintf(file, "format ascii 1.0\n");
    fprintf(file, "comment generated by Mesh_viewer\n");

    // write vertex properties
    fprintf(file, "element vertex %d\n", m_vertices.size() );
    fprintf(file, "property float x\n");
    fprintf(file, "property float y\n");
    fprintf(file, "property float z\n");

    if(hasNormals)
    {
        fprintf(file, "property float nx\n");
        fprintf(file, "property float ny\n");
        fprintf(file, "property float nz\n");
    }
    if(hasColors)
    {
        // colors: [0;1] -> [0;255]
        std::transform(m_colors.begin(), m_colors.end(), m_colors.begin(), [](glm::vec3 &_c){ return _c * 256.0f; });
        fprintf(file, "property uchar red\n");
        fprintf(file, "property uchar green\n");
        fprintf(file, "property uchar blue\n");
        fprintf(file, "property uchar alpha\n");
    }

    // write face properties
    fprintf(file, "element face %d\n", (int)(m_indices.size() / 3) );
    fprintf(file, "property list uchar int vertex_indices\n");

    // end header
    fprintf(file, "end_header\n");


    // Write vertices
    for (unsigned int i = 0; i <m_vertices.size(); i++) 
    {
        fprintf(file, "%f %f %f ", m_vertices[i].x, m_vertices[i].y, m_vertices[i].z);

        if(hasNormals)
            fprintf(file, "%f %f %f ", m_normals[i].x, m_normals[i].y, m_normals[i].z);
        if(hasColors)
            fprintf(file, "%d %d %d 255", (int)m_colors[i].x, (int)m_colors[i].y, (int)m_colors[i].z);

        fprintf(file, "\n");
        
    }
  

    // Write facets (triangles)
    for (unsigned int i = 0; i <nb_triangles; i++) 
    {
        int vertId0 = m_indices[ i * 3 ];
        int vertId1 = m_indices[ i * 3 + 1 ];
        int vertId2 = m_indices[ i * 3 + 2 ];

        fprintf(file, "3 %d %d %d \n", vertId0, vertId1, vertId2);
    }

    fclose(file);
}


void TriMeshSoup::readSTLAscii(FILE * const _file, std::vector<float> * const _vertex_data,  std::vector<float> * const _normal_data)
{
    assert( _file );
    assert( _vertex_data );
    assert( 0 == _vertex_data->size() );

    const uint32_t max_line_length = 256;

    char line[ max_line_length ];
    while ( std::fgets( line, max_line_length, _file ) ) 
    {
        if ( std::strstr( line, "facet normal" ) ) 
        {
            double normal[3];

            char str1[10];
            char str2[10];
            std::sscanf( line, " %10s %10s %lf %lf %lf", str1, str2, &normal[0], &normal[1], &normal[2] );
            _normal_data->push_back( normal[0] );
            _normal_data->push_back( normal[1] );
            _normal_data->push_back( normal[2] );

        }
        if ( std::strstr( line, "vertex" ) ) 
        {

            double vertex[3];
            char str[16];

            std::sscanf( line, " %15s %lf %lf %lf", str, &vertex[0], &vertex[1], &vertex[2] );

            _vertex_data->push_back( vertex[0] );
            _vertex_data->push_back( vertex[1] );
            _vertex_data->push_back( vertex[2] );

        }
    }
}


void TriMeshSoup::readSTLBinary(FILE * const _file, std::vector<float> * const _vertex_data)
{ 
    assert( _file );
    assert( _vertex_data );
    assert( 0 == _vertex_data->size() );

    const uint32_t header_length = 80;

    uint8_t header[ header_length ];
    std::fread( header, sizeof( uint8_t ), header_length, _file );
    uint32_t num_triangles = 0;
    std::fread( &num_triangles, sizeof( uint32_t ), 1, _file );
    for ( uint32_t i = 0; i < num_triangles; ++i ) 
    {
        float normal[3];
        std::fread( normal, sizeof( float ), 3, _file );
        for ( uint32_t j = 0; j < 3; ++j )
        {
            float vertex[3];
            std::fread( vertex, sizeof( float ), 3, _file );
            _vertex_data->push_back( vertex[ 0 ] );
            _vertex_data->push_back( vertex[ 1 ] );
            _vertex_data->push_back( vertex[ 2 ] );
        }
        uint16_t metadata;
        std::fread( &metadata, sizeof( uint16_t ), 1, _file );
    }
}


//! Reads mesh from STL file (ASCII or binary). The format, ASCII or
//! binary, is detected automatically by checking the content of the file.
bool TriMeshSoup::readSTL(const std::string &_filename, std::vector<float> * const _vertex_data, std::vector<float> * const _normal_data)
{
    FILE *file = std::fopen( _filename.c_str(), "rb" );
    if ( file ) 
    {
        uint8_t buf[ 5 ];
        std::fread( buf, sizeof( uint8_t ), 5, file );
        std::rewind( file );
        if ( 0 == std::strncmp( "solid", (const char *)buf, 5 ) ) 
        {
            readSTLAscii( file, _vertex_data, _normal_data );
        }
        else 
        {
            readSTLBinary( file, _vertex_data );
        }
        std::fclose( file );
    }
    else 
    {
        std::cerr << "[ERROR] TriMeshSoup::readSTL(): Cannot open file " << _filename << std::endl;
        return false;
    }
    return true;
}

// Reads mesh from STL file (ASCII or binary)
// https://en.wikipedia.org/wiki/STL_(file_format)
bool TriMeshSoup::importSTL(const std::string &_filename)
{
    if (!m_vertices.empty())
        m_vertices.clear();
    if (!m_normals.empty())
        m_normals.clear();

    // Read STL file.
    std::vector<float> buffer_vertices;
    std::vector<float> buffer_normals;

    bool status = readSTL(_filename.c_str(), &buffer_vertices, &buffer_normals);
    if (!status)
        return false;

    const int32_t num_vertices = buffer_vertices.size() / 3;
    const int32_t num_normals = buffer_normals.size() / 3;

    // Store vertex positions.
    m_vertices.reserve(num_vertices);

    for (int32_t i = 0; i < num_vertices; ++i)
    {
        glm::vec3 vertex;
        vertex[0] = buffer_vertices[3 * i + 0];
        vertex[1] = buffer_vertices[3 * i + 1];
        vertex[2] = buffer_vertices[3 * i + 2];
        m_vertices.push_back(vertex);

        m_indices.push_back(i);
    }

    // Store face normals.
    if (!buffer_normals.empty() && (num_normals == num_vertices/3) )
    {
        std::cout<<"read normals"<<std::endl;
        std::cout<<num_normals<<" "<<num_vertices<<std::endl;
        for (int32_t i = 0; i < num_normals; ++i)
        { 
            glm::vec3 normal;
            normal[0] = buffer_normals[3 * i + 0];
            normal[1] = buffer_normals[3 * i + 1];
            normal[2] = buffer_normals[3 * i + 2];

            // one normal per facet, duplicate it to get one normal per vertex
            m_normals.push_back( normal );
            m_normals.push_back( normal );
            m_normals.push_back( normal );
        }
    }
    else
    {
        std::cout<<"compute normals"<<std::endl;
        m_normals.reserve(num_vertices);
        for (int32_t i = 0; i < num_vertices; i += 3)
        { 
            glm::vec3 v01 = m_vertices[i + 1] - m_vertices[i];
            glm::vec3 v02 = m_vertices[i + 2] - m_vertices[i];
            glm::vec3 normal = glm::normalize( (glm::cross(v01, v02)) );
            m_normals.push_back(normal);
            m_normals.push_back(normal);
            m_normals.push_back(normal);
        }
    }

    return true;
}


void TriMeshSoup::exportSTL(const std::string &_filename)
{
    if( m_vertices.empty() || m_indices.empty() )
        std::cerr<<"[ERROR] TriMeshSoup::exportSTL(): empty data"<<std::endl;

    struct STL_TRIANGLE
    {
        float n[3];
        float v1[3];
        float v2[3];
        float v3[3];
        std::uint16_t abc;
    };

    std::uint8_t header[80] = "Exported STL";
    std::uint32_t num_triangles =  m_indices.size() / 3;

    FILE *file = std::fopen(_filename.c_str(), "wb+");
    if( !file) 
    {
        std::cerr << "[ERROR] TriMeshSoup::exportSTL(): Cannot open file to write" << _filename << std::endl;
    }

    std::fwrite(&header[0], sizeof(header), 1, file);
    std::fwrite(&num_triangles, sizeof(num_triangles), 1, file);

    for (int i = 0; i < m_indices.size(); i += 3)
    {
        const glm::vec3 &v1 = m_vertices[ m_indices[i + 0] ];
        const glm::vec3 &v2 = m_vertices[ m_indices[i + 1] ];
        const glm::vec3 &v3 = m_vertices[ m_indices[i + 2] ];

        STL_TRIANGLE triangle;
        // face normal is set to zero
        std::memset(&triangle.n[0], 0, sizeof(float) * 3);
        std::memcpy(&triangle.v1[0], &v1[0], sizeof(float) * 3);
        std::memcpy(&triangle.v2[0], &v2[0], sizeof(float) * 3);
        std::memcpy(&triangle.v3[0], &v3[0], sizeof(float) * 3);
        std::memset(&triangle.abc, 0, sizeof(std::uint16_t));

        size_t triangleNBytes = 50;  // Actual size without padding
        std::fwrite(&triangle, triangleNBytes, 1, file);
    }
    std::fclose(file);
}


void TriMeshSoup::clear()
{
    m_vertices.clear();
    m_normals.clear();
    m_indices.clear();

    m_colors.clear();
    m_texcoords.clear();
    m_tangents.clear();
    m_bitangents.clear();
}
