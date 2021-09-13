/*********************************************************************************************************************
 *
 * trimesh.cpp
 *
 * Triangle mesh class
 * 
 * QGL_toolkit demo
 * Ludovic Blache
 *
 *********************************************************************************************************************/

#include <algorithm>
#include <functional>
#include <ios>
	

#include "trimesh.h"

TriMesh::TriMesh()
{
    m_bBoxMin = glm::vec3(0.0f, 0.0f, 0.0f);
    m_bBoxMax = glm::vec3(0.0f, 0.0f, 0.0f);

    m_ambientColor = glm::vec3(0.04f, 0.04f, 0.06f);
    m_diffuseColor = glm::vec3(0.82f, 0.66f, 0.43f);
    m_specularColor = glm::vec3(0.9f, 0.9f, 0.9f);

    m_specPow = 128.0f;
}


TriMesh::~TriMesh()
{
    clear();

    glDeleteBuffers(1, &(m_vertexVBO));
    glDeleteBuffers(1, &(m_normalVBO));
    glDeleteBuffers(1, &(m_colorVBO));
    glDeleteBuffers(1, &(m_indexVBO));
    glDeleteVertexArrays(1, &(m_meshVAO));
}


bool TriMesh::readFile(std::string _filename)
{
    if(_filename.substr(_filename.find_last_of(".") + 1) == "obj")
    {
        importOBJ(_filename);
        return true;
    }
    else
    {
        std::cerr << "[ERROR] TriMesh::readFile(): Invalid file extension: only .obj are supported" << std::endl;
    }
    return false;
}


void TriMesh::computeAABB()
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
        std::cerr << "[WARNING] TriMesh::computeAABB: Empty vertices array" << std::endl;
        m_bBoxMin = glm::vec3(0.0f, 0.0f, 0.0f);
        m_bBoxMax = glm::vec3(0.0f, 0.0f, 0.0f);
    }
}


void TriMesh::computeNormals()
{
    std::int32_t vertexIndex0, vertexIndex1, vertexIndex2;
    glm::vec3 normal;

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

    std::cout << "[INFO] TriMesh::computeNormals(): Normals computed" << std::endl;
}


void TriMesh::createVAO()
{
        
    if(m_vertices.size() == 0)
        std::cerr << "[WARNING] DrawableMesh::createVAO(): No vertex provided" << std::endl;
    if(m_normals.size() == 0)
        std::cerr << "[WARNING] DrawableMesh::createVAO(): No normal provided" << std::endl;
    if(m_indices.size() == 0)
        std::cerr << "[WARNING] DrawableMesh::createVAO(): No index provided" << std::endl;

    // Generates and populates a VBO for vertex coords
    glGenBuffers(1, &(m_vertexVBO));
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexVBO);
    size_t verticesNBytes = m_vertices.size() * sizeof(m_vertices[0]);
    glBufferData(GL_ARRAY_BUFFER, verticesNBytes, m_vertices.data(), GL_STATIC_DRAW);

    // Generates and populates a VBO for vertex normals
    glGenBuffers(1, &(m_normalVBO));
    glBindBuffer(GL_ARRAY_BUFFER, m_normalVBO);
    size_t normalsNBytes = m_normals.size() * sizeof(m_normals[0]);
    glBufferData(GL_ARRAY_BUFFER, normalsNBytes, m_normals.data(), GL_STATIC_DRAW);

    // Generates and populates a VBO for the element indices
    glGenBuffers(1, &(m_indexVBO));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexVBO);
    auto indicesNBytes = m_indices.size() * sizeof(m_indices[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesNBytes, m_indices.data(), GL_STATIC_DRAW);


    // Generates and populates a VBO for vertex colors
    glGenBuffers(1, &(m_colorVBO));
    glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
    if(m_colors.size() != 0)
    {
        size_t colorsNBytes = m_colors.size() * sizeof(m_colors[0]);
        glBufferData(GL_ARRAY_BUFFER, colorsNBytes, m_colors.data(), GL_STATIC_DRAW);
    }
    else
    {
        size_t colorsNBytes = 1.0f * sizeof(m_colors[0]);
        glBufferData(GL_ARRAY_BUFFER, colorsNBytes, nullptr, GL_STATIC_DRAW);
    }


    // Creates a vertex array object (VAO) for drawing the mesh
    glGenVertexArrays(1, &(m_meshVAO));
    glBindVertexArray(m_meshVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexVBO);
    glEnableVertexAttribArray(POSITION);
    glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, m_normalVBO);
    glEnableVertexAttribArray(NORMAL);
    glVertexAttribPointer(NORMAL, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
    glEnableVertexAttribArray(COLOR);
    glVertexAttribPointer(COLOR, 3, GL_FLOAT, GL_FALSE, 0, nullptr);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexVBO);
    glBindVertexArray(m_defaultVAO); // unbinds the VAO

    // Additional information required by draw calls
    m_numVertices = m_vertices.size();
    m_numIndices = m_indices.size();
}


void TriMesh::draw(glm::mat4 _mv, glm::mat4 _mvp, glm::vec3 _lightPos, glm::vec3 _lightCol)
{

    // Activate program
    glUseProgram(m_program);

    // Pass uniforms
    glUniformMatrix4fv(glGetUniformLocation(m_program, "u_mv"), 1, GL_FALSE, &_mv[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_program, "u_mvp"), 1, GL_FALSE, &_mvp[0][0]);
    glUniform3fv(glGetUniformLocation(m_program, "u_lightPosition"), 1, &_lightPos[0]);

    glUniform3fv(glGetUniformLocation(m_program, "u_lightColor"), 1, &_lightCol[0]);

    glUniform3fv(glGetUniformLocation(m_program, "u_ambientColor"), 1, &m_ambientColor[0]);
    glUniform3fv(glGetUniformLocation(m_program, "u_diffuseColor"), 1, &m_diffuseColor[0]);
    glUniform3fv(glGetUniformLocation(m_program, "u_specularColor"), 1, &m_specularColor[0]);
    glUniform1f(glGetUniformLocation(m_program, "u_specularPower"), m_specPow);
 

    // Draw!
    glBindVertexArray(m_meshVAO);                       // bind the VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexVBO);  // do not forget to bind the index buffer AFTER !

    glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);

    glBindVertexArray(m_defaultVAO);


    glUseProgram(0);
}


GLuint TriMesh::loadShaderProgram(const std::string& _vertShaderFilename, const std::string& _fragShaderFilename)
{
    // Load and compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    std::string vertexShaderSource = readShaderSource(_vertShaderFilename);
    const char *vertexShaderSourcePtr = vertexShaderSource.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderSourcePtr, nullptr);

    glCompileShader(vertexShader);
    GLint compiled = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) 
    {
        std::cerr << "[ERROR] DrawableMesh::loadShaderProgram(): Vertex shader compilation failed:" << std::endl;
        showShaderInfoLog(vertexShader);
        glDeleteShader(vertexShader);
        return 0;
    }

    // Load and compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragmentShaderSource = readShaderSource(_fragShaderFilename);
    const char *fragmentShaderSourcePtr = fragmentShaderSource.c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderSourcePtr, nullptr);

    glCompileShader(fragmentShader);
    compiled = 0;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) 
    {
        std::cerr << "[ERROR] DrawableMesh::loadShaderProgram(): Fragment shader compilation failed:" << std::endl;
        showShaderInfoLog(fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    // Create program object
    GLuint program = glCreateProgram();

    // Attach shaders to the program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // Link program
    glLinkProgram(program);

    // Check linking status
    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) 
    {
        std::cerr << "[ERROR] DrawableMesh::loadShaderProgram(): Linking failed:" << std::endl;
        showProgramInfoLog(program);
        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    // Clean up
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);

    return program;
}


// Read an Mesh from an .obj file. This function can read texture
// coordinates and/or normals, in addition to vertex positions.
bool TriMesh::importOBJ(const std::string &_filename)
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
        std::cerr << "[ERROR] TriMesh::importOBJ(): Could not open " << _filename << std::endl;
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
        std::cout << "[INFO] TriMesh::importOBJ(): Normals not provided, compute them " << std::endl;
        computeNormals();
    }

    return true;
}


void TriMesh::clear()
{
    m_vertices.clear();
    m_normals.clear();
    m_indices.clear();

    m_colors.clear();
}


std::string TriMesh::readShaderSource(const std::string& _filename)
{
    std::ifstream file(_filename);
    std::stringstream stream;
    stream << file.rdbuf();

    return stream.str();
}


void TriMesh::showShaderInfoLog(GLuint _shader)
{
    GLint infoLogLength = 0;
    glGetShaderiv(_shader, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> infoLog(infoLogLength);
    glGetShaderInfoLog(_shader, infoLogLength, &infoLogLength, &infoLog[0]);
    std::string infoLogStr(infoLog.begin(), infoLog.end());
    std::cerr << infoLogStr << std::endl;
}


void TriMesh::showProgramInfoLog(GLuint _program)
{
    GLint infoLogLength = 0;
    glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> infoLog(infoLogLength);
    glGetProgramInfoLog(_program, infoLogLength, &infoLogLength, &infoLog[0]);
    std::string infoLogStr(infoLog.begin(), infoLog.end());
    std::cerr << infoLogStr << std::endl;
}
