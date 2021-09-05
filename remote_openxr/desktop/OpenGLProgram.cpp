#include"OpenGLProgram.h"

void OpenGLProgram::createTriangleProgram() {
    unsigned int vertexShader;
    unsigned int fragmentShader;

    unsigned int VBO;

    const char* vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "layout (location = 1) in vec3 aColor;\n"
                                     "uniform mat4 ModelMatrix;\n"
                                     "uniform mat4 ViewMatrix;\n"
                                     "uniform mat4 ProjMatrix;\n"
                                     "out vec3 oColor;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = ProjMatrix * ViewMatrix * ModelMatrix * vec4(aPos, 1.0);\n"
                                     "   oColor = aColor;\n"
                                     "}\0";

    const char* fragmentShaderSource = "#version 330 core\n"
                                       "in vec3 oColor;\n"
                                       "out vec4 FragColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   FragColor = vec4(oColor, 1.0f);\n"
                                       "}\n\0";

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::fragment:ATION_FAILED\n" << infoLog << std::endl;
    }

    triangleProgram = glCreateProgram();

    glAttachShader(triangleProgram, vertexShader);
    glAttachShader(triangleProgram, fragmentShader);
    glLinkProgram(triangleProgram);

    glGetProgramiv(triangleProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(triangleProgram, 512, NULL, infoLog);
        std::cout << "ERROR::programm::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {-0.1f, -0.1f, 0.0f, 1.0f, 0.0f, 0.0f, 0.1f, -0.1f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.1f, 0.0f, 0.0f, 0.0f, 1.0f};

    glGenVertexArrays(1, &triangleVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(triangleVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer
    // object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the triangleVAO afterwards so other triangleVAO calls won't accidentally modify this triangleVAO, but this
    // rarely happens. Modifying other VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor
    // VBOs) when it's not directly necessary.
    glBindVertexArray(0);
}

void OpenGLProgram::createFullScreenRectProgram() {
    unsigned int vertexShader = 0;
    unsigned int fragmentShader;

    const char* vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "layout (location = 1) in vec2 aTexCoord;\n"
                                     "out vec2 TexCoord;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                     "   TexCoord = aTexCoord;\n"
                                     "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
                                       "in vec2 TexCoord;\n"
                                       "out vec4 FragColor;\n"
                                       "uniform sampler2D texture0;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   vec3 texColor = vec3(texture2D(texture0, TexCoord));\n"
                                       "   FragColor = vec4(texColor, 1.0f);\n"
                                       "}\n\0";

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::fragment:ATION_FAILED\n" << infoLog << std::endl;
    }

    fullScreenRectProgram = glCreateProgram();

    glAttachShader(fullScreenRectProgram, vertexShader);
    glAttachShader(fullScreenRectProgram, fragmentShader);
    glLinkProgram(fullScreenRectProgram);

    glGetProgramiv(fullScreenRectProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(fullScreenRectProgram, 512, NULL, infoLog);
        std::cout << "ERROR::programm::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        // positions          // texture coords
        1.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top right
        1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
        -1.0f, 1.0f,  0.0f, 0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
        0,
        1,
        3, // first triangle
        1,
        2,
        3 // second triangle
    };
    unsigned int VBO, EBO;
    glGenVertexArrays(1, &rectVAO);
    glBindVertexArray(rectVAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void OpenGLProgram::initWindow(HWND hWndGL) {
    static PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), // Size Of This Pixel Format Descriptor
        1,                             // Version Number
        PFD_DRAW_TO_WINDOW |           // Format Must Support Window
            PFD_SUPPORT_OPENGL |       // Format Must Support OpenGL
            PFD_DOUBLEBUFFER,          // Must Support Double Buffering
        PFD_TYPE_RGBA,                 // Request An RGBA Format
        32,                            // Select Our Color Depth
        0,
        0,
        0,
        0,
        0,
        0, // Color Bits Ignored
        0, // No Alpha Buffer
        0, // Shift Bit Ignored
        0, // No Accumulation Buffer
        0,
        0,
        0,
        0,              // Accumulation Bits Ignored
        16,             // 16Bit Z-Buffer (Depth Buffer)
        0,              // No Stencil Buffer
        0,              // No Auxiliary Buffer
        PFD_MAIN_PLANE, // Main Drawing Layer
        0,              // Reserved
        0,
        0,
        0 // Layer Masks Ignored
    };

    g_hDCGL = GetDC(hWndGL);
    GLuint PixelFormat = ChoosePixelFormat(g_hDCGL, &pfd);
    SetPixelFormat(g_hDCGL, PixelFormat, &pfd);
    HGLRC hRC = wglCreateContext(g_hDCGL);
    wglMakeCurrent(g_hDCGL, hRC);
    GLenum x = glewInit();
}

void OpenGLProgram::initOpenGLProgram(ID3D11Device* m_device,ID3D11Texture2D* m_dxColorbuffer) {
    // Register the shared DX texture with OGL
    if (WGLEW_NV_DX_interop) {
        // Acquire a handle to the D3D device for use in OGL
        glHandleDX9Device = wglDXOpenDeviceNV(m_device);
        glGenRenderbuffers(1, glSharedTextur);
        if (glHandleDX9Device) {
            glHandleSharedTexture[0] =
                wglDXRegisterObjectNV(glHandleDX9Device, m_dxColorbuffer, glSharedTextur[0], GL_TEXTURE_2D, WGL_ACCESS_READ_WRITE_NV);

            // attach the Direct3D buffers to an FBO
            if (fbo > 0) {
                glDeleteFramebuffers(1, &fbo);
            }
            glGenFramebuffers(1, &fbo);

            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glBindTexture(GL_TEXTURE_2D, glSharedTextur[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glSharedTextur[0], 0);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }

    createTriangleProgram();
    createFullScreenRectProgram();
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glDisable(GL_DEPTH_TEST);

    g_matModel = DirectX::XMMatrixIdentity();
    g_matView = DirectX::XMMatrixLookAtLH(
        DirectX::XMVectorSet(0.0f, 0.0f, 2, 1.0f), DirectX::XMVectorZero(), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
    g_matProjection = DirectX::XMMatrixPerspectiveFovLH(fov, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 1000.0f);

    translationMat = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
    scalingMat = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
    rotationMat = DirectX::XMMatrixRotationQuaternion(xr::math::LoadXrQuaternion(m_quaternion));
}

void OpenGLProgram::setUniform(unsigned int handle, const char* name, DirectX::XMMATRIX m) {
    std::map<std::string, int>::iterator pos;
    pos = uniformLocations.find(name);

    if (pos == uniformLocations.end()) {
        uniformLocations[name] = glGetUniformLocation(handle, name);
    }

    GLint loc = uniformLocations[name];
    glUniformMatrix4fv(loc, 1, GL_FALSE, &m.r[0].m128_f32[0]);
}
void OpenGLProgram::updateModelMatrix() {
    g_matModel = DirectX::XMMatrixIdentity();
    // T∗R∗S
    g_matModel = translationMat * rotationMat * scalingMat;
}

void OpenGLProgram::render() {
    // Lock the shared surface
    wglDXLockObjectsNV(glHandleDX9Device, 1, &glHandleSharedTexture[0]);
    //++++++++++++++++++ Pass 1 ++++++++++++++++++++++++++++++
    glUseProgram(triangleProgram);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    setUniform(triangleProgram, "ModelMatrix", g_matModel);
    setUniform(triangleProgram, "ViewMatrix", g_matView);
    setUniform(triangleProgram, "ProjMatrix", g_matProjection);

    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, glSharedTextur[0]);

    glBindVertexArray(triangleVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //++++++++++++++++++ Pass 2 ++++++++++++++++++++++++++++++
    glUseProgram(fullScreenRectProgram);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    // bind Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, glSharedTextur[0]);
    // render container
    glBindVertexArray(rectVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Unlock the shared surface
    wglDXUnlockObjectsNV(glHandleDX9Device, 1, &glHandleSharedTexture[0]);
    SwapBuffers(g_hDCGL);
}
//============================================
//    actions 
void OpenGLProgram::scale(float newValue) {
    m_scale = xrMathHelper::XrVector3f_Add(m_scale, newValue);
    if (m_scale.x <= 0.01)
        m_scale = XrVector3f{0.5, 0.5, 0.5};
    if (m_scale.x >= 1)
        m_scale = XrVector3f{1, 1, 1};

   scalingMat = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
   updateModelMatrix();
}
void OpenGLProgram::translation(XrVector3f direction) {
    m_position = xrMathHelper::XrVector3f_Add(m_position, xrMathHelper::XrVector3f_Multi(direction, m_MovingSpeed));
    translationMat = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
    updateModelMatrix();

}
void OpenGLProgram::rotation(XrVector3f delta) {

    m_OrbitAngleY = delta.x * m_OrbitingSpeed;
    m_OrbitAngleX = delta.y * m_OrbitingSpeed;
    m_OrbitAngleZ = delta.z * m_OrbitingSpeed;

    float angle = sqrt(m_OrbitAngleX * m_OrbitAngleX + m_OrbitAngleY * m_OrbitAngleY + m_OrbitAngleZ * m_OrbitAngleZ) / 2.0;
    if (angle != 0.0) {
        XrVector3f axis = XrVector3f{m_OrbitAngleX / angle, m_OrbitAngleY / angle, m_OrbitAngleZ / angle};
        XrQuaternionf newQuaternion = xr::math::Quaternion::RotationAxisAngle(axis, -angle);
        m_quaternion = xrMathHelper::XrQuaternionf_Multiply(m_quaternion, newQuaternion);
  
       DirectX::XMVECTOR quat = xr::math::LoadXrQuaternion(m_quaternion);
       rotationMat = DirectX::XMMatrixRotationQuaternion(quat);
       updateModelMatrix();
    }
}