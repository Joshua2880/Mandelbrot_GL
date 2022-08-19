
#include <fstream>
#include <iostream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "glm/gtc/type_ptr.hpp"

#ifdef M_DOUBLE
    #define MGL_FLOAT double
#else 
    #define MGL_FLOAT float
#endif

extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

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

#ifdef M_DOUBLE
glm::vec<2, double> dims = {800.0, 600.0};
glm::vec<2, double> centre{ 0.0, 0.0 };
double zoom{ -1.0 };
#else
glm::vec2 dims = { 800.0f, 600.0f };
glm::vec2 centre{ 0.0f, 0.0f };
float zoom{ -1.0f };
#endif
MGL_FLOAT aspect_ratio;

void FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    dims = { width, height };
    aspect_ratio = dims.x / dims.y;
}

void ScrollCallback(GLFWwindow *window, double d_x, double d_y)
{
    glm::vec<2, double> mouse_pos{};
    glfwGetCursorPos(window, &mouse_pos.x, &mouse_pos.y);

    MGL_FLOAT new_zoom = zoom + static_cast<MGL_FLOAT>(d_y);

#ifdef M_DOUBLE
    centre += (pow(2.0, -zoom) - pow(2.0, -new_zoom)) * (2.0 / dims.y * glm::vec<2, double>(mouse_pos.x, -mouse_pos.y) - glm::vec<2, double>(aspect_ratio, -1.0));

#else
    centre += (powf(2.0f, -zoom) - powf(2.0f, -new_zoom)) * (2.0f / dims.y * glm::vec2(mouse_pos.x, -mouse_pos.y) - glm::vec2(aspect_ratio, -1.0f));

#endif

    zoom = new_zoom;
}

int main()
{
    if (!glfwInit())
    {
        std::cout << "error: failed to initialise glfw\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
    glfwSetScrollCallback(window, ScrollCallback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cout << "error: failed to initialise glad\n";
        glfwTerminate();
        return -1;
    }

    uint8_t const *vendor = glGetString(GL_VENDOR);
    uint8_t const *renderer = glGetString(GL_RENDERER);

    std::cout << vendor << " : " << renderer << std::endl;

    glViewport(0, 0, static_cast<int32_t>(dims.x), static_cast<int32_t>(dims.y));
    aspect_ratio = dims.x / dims.y;

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

#ifdef M_DOUBLE
    std::string f_shader_source = FileToString("res/shaders/mandelbrot_d.frag");
#else
    std::string f_shader_source = FileToString("res/shaders/mandelbrot.frag");
#endif

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
         1.0f,  1.0f,
         1.0f, -1.0f,
        -1.0f, -1.0f,
        -1.0f,  1.0f
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
        
#ifdef M_DOUBLE
        glUniform1d(glGetUniformLocation(shader, "u_aspect_ratio"), aspect_ratio);
        glUniform1d(glGetUniformLocation(shader, "u_zoom"), pow(2.0, -zoom));
        glUniform2dv(glGetUniformLocation(shader, "u_centre"), 1, glm::value_ptr(centre));
#else
        glUniform1f(glGetUniformLocation(shader, "u_aspect_ratio"), aspect_ratio);
        glUniform1f(glGetUniformLocation(shader, "u_zoom"), zoom);
        glUniform2fv(glGetUniformLocation(shader, "u_centre"), 1, glm::value_ptr(centre));
#endif

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}