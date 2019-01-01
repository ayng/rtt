#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <cstdio>

void checkError(const char* tag) {
    GLenum err;
    bool hasError = false;
    while ((err = glGetError()) != GL_NO_ERROR) {
        hasError = true;
        std::cerr << tag << ": gl error: " << err << std::endl;
    }
    if (hasError) {
        exit(1);
    }
}

int main() {
    GLFWwindow* window;

    glfwSetErrorCallback([](int error, const char* description) {
        std::cerr << "GLFW error " << error << ": " << description << std::endl;
    });

    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window,
        [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            (void)scancode;
            (void)mods;
            if ((key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
        });

    std::cout << glGetString(GL_VERSION) << std::endl;

    float vertices[] = {
        -0.5, -0.5, 0.0, 
         0.0,  0.5, 0.0, 
         0.5, -0.5, 0.0, 
    };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    float quadVerts[] = {
        -1.0, -1.0, 0.0,   0.0, 0.0,
        -1.0,  1.0, 0.0,   0.0, 1.0,
         1.0,  1.0, 0.0,   1.0, 1.0,

        -1.0, -1.0, 0.0,   0.0, 0.0,
         1.0,  1.0, 0.0,   1.0, 1.0,
         1.0, -1.0, 0.0,   1.0, 0.0,
    };

    GLuint quadVAO;
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    GLuint quadVBO;
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
            5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
            5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    GLuint prog;
    {
        const GLchar* vs_text = R"(
        #version 410
        layout (location = 0) in vec3 pos;
        void main() {
            gl_Position = vec4(pos, 1.0);
        }
        )";
        const GLchar* fs_text = R"(
        #version 410
        out vec4 color;
        void main() {
            color = vec4(1.0, 0.0, 0.0, 1.0);
        }
        )";

        GLuint vs, fs;
        GLint ok;

        vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vs_text, NULL);
        glCompileShader(vs);

        glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
        if (ok == GL_FALSE) {
            std::cerr << "vertex shader compilation error\n";
            return -1;
        }
        checkError("vs");

        fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fs_text, NULL);
        glCompileShader(fs);

        glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
        if (ok == GL_FALSE) {
            std::cerr << "fragment shader compilation error\n";
            return -1;
        }
        checkError("fs");

        prog = glCreateProgram();
        glAttachShader(prog, vs);
        glAttachShader(prog, fs);
        glLinkProgram(prog);
        glGetProgramiv(prog, GL_LINK_STATUS, &ok);
        if (ok == GL_FALSE) {
            std::cerr << "program not linked\n";
            return -1;
        }

    }

    GLuint texProg;
    {
        const GLchar* vs_text = R"(
        #version 410
        layout (location = 0) in vec3 pos;
        layout (location = 1) in vec2 aUV;
        out vec2 uv;
        void main() {
            uv = aUV;
            gl_Position = vec4(pos, 1.0);
        }
        )";
        const GLchar* fs_text = R"(
        #version 410
        uniform sampler2D tex;
        in vec2 uv;
        out vec4 color;
        void main() {
            //color = 1.0 - texture(tex, uv);
            color = texture(tex, uv);
            //color = vec4(uv, 0.0, 1.0);
        }
        )";
        GLuint vs, fs;
        GLint ok;

        vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vs_text, NULL);
        glCompileShader(vs);

        glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
        if (ok == GL_FALSE) {
            std::cerr << "vertex shader compilation error\n";
            return -1;
        }
        checkError("vs");

        fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fs_text, NULL);
        glCompileShader(fs);

        glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
        if (ok == GL_FALSE) {
            std::cerr << "fragment shader compilation error\n";
            return -1;
        }
        checkError("fs");
        texProg = glCreateProgram();
        glAttachShader(texProg, vs);
        glAttachShader(texProg, fs);
        glLinkProgram(texProg);
        glGetProgramiv(texProg, GL_LINK_STATUS, &ok);
        if (ok == GL_FALSE) {
            std:: cerr << "program not linked\n";
            return -1;
        }
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLuint depthTex;
    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

    {
        GLenum ok;
        ok = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (ok != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "framebuffer not complete\n";
            return -1;
        }
    }
    
    checkError("fbo");

    GLint texLoc;
    texLoc = glGetUniformLocation(texProg, "tex");
    if (texLoc == -1) {
        std::cerr << "failed to get tex uniform location\n";
        return -1;
    }

    glClearColor(.1f, .1f, .3f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClear(GL_DEPTH_BUFFER_BIT |GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, 1024, 1024);
        glUseProgram(prog);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        checkError("texture fb");

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthTex);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, 640 * 2, 480 * 2);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(texProg);
        glUniform1i(texLoc, 1);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        checkError("main fb");


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
