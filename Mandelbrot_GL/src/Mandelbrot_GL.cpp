
#include <fstream>
#include <iostream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

std::string FileToString(std::string path)
{
    std::fstream file{ path };
    if (!file.is_open())
    {
        std::cout << "error: could not open file " << path << std::endl;
        return "";
    }

    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    
}

int main()
{
    if (!glfwInit())
    {
        std::cout << "error: failed to initialise glfw\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Mandelbrot GL", nullptr, nullptr);
    if (!window)
    {
        std::cout << "error: failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cout << "error: failed to initialise glad\n";
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, 800, 600);

    int32_t success;
    char info_log[512];

    std::string v_shader_source = FileToString("res/shaders/basic.vert");
    char const *v_shader_source_cstr = v_shader_source.c_str();

    uint32_t v_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v_shader, 1, &v_shader_source_cstr, nullptr);
    glCompileShader(v_shader);

    glGetShaderiv(v_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(v_shader, sizeof(info_log), nullptr, info_log);
        std::cout << "error: couldn't compile vertex shader\n" << info_log << std::endl;
    }

    std::string f_shader_source = FileToString("res/shaders/mandelbrot.frag");
    char const *f_shader_source_cstr = f_shader_source.c_str();

    uint32_t f_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f_shader, 1, &f_shader_source_cstr, nullptr);
    glCompileShader(f_shader);

    glGetShaderiv(f_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(f_shader, sizeof(info_log), nullptr, info_log);
        std::cout << "error: couldn't compile fragment shader\n" << info_log << std::endl;
    }

    uint32_t shader = glCreateProgram();
    glAttachShader(shader, v_shader);
    glAttachShader(shader, f_shader);
    glLinkProgram(shader);

    glGetProgramiv(shader, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(shader, sizeof(info_log), nullptr, info_log);
        std::cout << "error: couldn't link shader\n" << info_log << std::endl;
    }

    glDeleteShader(v_shader);
    glDeleteShader(f_shader);

    float constexpr vertices[]{
         0.5f,  0.5f,
         0.5f, -0.5f,
        -0.5f, -0.5f,
        -0.5f,  0.5f
    };

    uint32_t constexpr indices[]{
        0, 1, 2,
        0, 2, 3
    };

    uint32_t vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    uint32_t vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);

    uint32_t ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}