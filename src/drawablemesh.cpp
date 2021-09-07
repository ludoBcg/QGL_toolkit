#include "drawablemesh.h"


DrawableMesh::DrawableMesh()
{
    /* TODO */
    //m_defaultVAO = 0;
    m_ambientColor = glm::vec3(0.04f, 0.04f, 0.06f);
    m_diffuseColor = glm::vec3(0.82f, 0.66f, 0.43f);
    m_specularColor = glm::vec3(0.9f, 0.9f, 0.9f);

    m_specPow = 128.0f;

    m_vertexProvided = false;
    m_normalProvided = false;
    m_colorProvided = false;
    m_indexProvided = false;
}


DrawableMesh::~DrawableMesh()
{
    glDeleteBuffers(1, &(m_vertexVBO));
    glDeleteBuffers(1, &(m_normalVBO));
    glDeleteBuffers(1, &(m_colorVBO));
    glDeleteBuffers(1, &(m_indexVBO));
    glDeleteVertexArrays(1, &(m_meshVAO));
}


void DrawableMesh::createVAO(TriMesh* _triMesh)
{
    fillVAO(_triMesh, true);
}


void DrawableMesh::updateVAO(TriMesh* _triMesh)
{
    fillVAO(_triMesh, false);
}


void DrawableMesh::fillVAO(TriMesh* _triMesh, bool _create)
{
    // mandatory data
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<uint32_t> indices;      // !! uint32_t !!

    // optional data
    std::vector<glm::vec3> colors;


    _triMesh->getVertices(vertices);
    _triMesh->getNormals(normals);
    _triMesh->getIndices(indices);

    _triMesh->getColors(colors);

    // update flags according to data provided
    vertices.size() ?  m_vertexProvided = true :  m_vertexProvided = false;
    normals.size() ?  m_normalProvided = true :  m_normalProvided = false;
    indices.size() ?  m_indexProvided = true :  m_indexProvided = false;
    colors.size() ?  m_colorProvided = true :  m_colorProvided = false;

                
    if(!m_vertexProvided)
        std::cerr << "[WARNING] DrawableMesh::createVAO(): No vertex provided" << std::endl;
    if(!m_normalProvided)
        std::cerr << "[WARNING] DrawableMesh::createVAO(): No normal provided" << std::endl;
    if(!m_indexProvided)
        std::cerr << "[WARNING] DrawableMesh::createVAO(): No index provided" << std::endl;

    // Generates and populates a VBO for vertex coords
    if(_create)
        glGenBuffers(1, &(m_vertexVBO));
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexVBO);
    size_t verticesNBytes = vertices.size() * sizeof(vertices[0]);
    glBufferData(GL_ARRAY_BUFFER, verticesNBytes, vertices.data(), GL_STATIC_DRAW);

    // Generates and populates a VBO for vertex normals
    if(_create)
        glGenBuffers(1, &(m_normalVBO));
    glBindBuffer(GL_ARRAY_BUFFER, m_normalVBO);
    size_t normalsNBytes = normals.size() * sizeof(normals[0]);
    glBufferData(GL_ARRAY_BUFFER, normalsNBytes, normals.data(), GL_STATIC_DRAW);

    // Generates and populates a VBO for the element indices
    if(_create)
        glGenBuffers(1, &(m_indexVBO));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexVBO);
    auto indicesNBytes = indices.size() * sizeof(indices[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesNBytes, indices.data(), GL_STATIC_DRAW);


    // Generates and populates a VBO for vertex colors
    if(_create)
        glGenBuffers(1, &(m_colorVBO));
    glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
    if(m_colorProvided)
    {
        size_t colorsNBytes = colors.size() * sizeof(colors[0]);
        glBufferData(GL_ARRAY_BUFFER, colorsNBytes, colors.data(), GL_STATIC_DRAW);
    }
    else
    {
        size_t colorsNBytes = 1.0f * sizeof(colors[0]);
        glBufferData(GL_ARRAY_BUFFER, colorsNBytes, nullptr, GL_STATIC_DRAW);
    }


    // Creates a vertex array object (VAO) for drawing the mesh
    if(_create)
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
    m_numVertices = vertices.size();
    m_numIndices = indices.size();

    // Clear temporary vectors
    vertices.clear();
    normals.clear();
    indices.clear();
    colors.clear();
}


void DrawableMesh::draw(glm::mat4 _mv, glm::mat4 _mvp, glm::vec3 _lightPos, glm::vec3 _lightCol)
{

    // Activate program
    glUseProgram(m_program);

    // ...

    // Pass uniforms
    glUniformMatrix4fv(glGetUniformLocation(m_program, "u_mv"), 1, GL_FALSE, &_mv[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_program, "u_mvp"), 1, GL_FALSE, &_mvp[0][0]);
    glUniform3fv(glGetUniformLocation(m_program, "u_lightPosition"), 1, &_lightPos[0]);

    glUniform3fv(glGetUniformLocation(m_program, "u_lightColor"), 1, &_lightCol[0]);

    glUniform3fv(glGetUniformLocation(m_program, "u_ambientColor"), 1, &m_ambientColor[0]);
    glUniform3fv(glGetUniformLocation(m_program, "u_diffuseColor"), 1, &m_diffuseColor[0]);
    glUniform3fv(glGetUniformLocation(m_program, "u_specularColor"), 1, &m_specularColor[0]);
    glUniform1f(glGetUniformLocation(m_program, "u_specularPower"), m_specPow);
 
    // ...

    // Draw!
    glBindVertexArray(m_meshVAO);                       // bind the VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexVBO);  // do not forget to bind the index buffer AFTER !

    glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);

    glBindVertexArray(m_defaultVAO);



    glUseProgram(0);
}



GLuint DrawableMesh::loadShaderProgram(const std::string& _vertShaderFilename, const std::string& _fragShaderFilename)
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


std::string DrawableMesh::readShaderSource(const std::string& _filename)
{
    std::ifstream file(_filename);
    std::stringstream stream;
    stream << file.rdbuf();

    return stream.str();
}


void DrawableMesh::showShaderInfoLog(GLuint _shader)
{
    GLint infoLogLength = 0;
    glGetShaderiv(_shader, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> infoLog(infoLogLength);
    glGetShaderInfoLog(_shader, infoLogLength, &infoLogLength, &infoLog[0]);
    std::string infoLogStr(infoLog.begin(), infoLog.end());
    std::cerr << infoLogStr << std::endl;
}


void DrawableMesh::showProgramInfoLog(GLuint _program)
{
    GLint infoLogLength = 0;
    glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> infoLog(infoLogLength);
    glGetProgramInfoLog(_program, infoLogLength, &infoLogLength, &infoLog[0]);
    std::string infoLogStr(infoLog.begin(), infoLog.end());
    std::cerr << infoLogStr << std::endl;
}
