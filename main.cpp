#include <iostream>
#include <geGL/geGL.h>
#include <SDL2CPP/Window.h>
#include <SDL2CPP/MainLoop.h>
#include <glm/gtc/type_ptr.hpp>
#include <Model.h>
#include <Utilities.h>
#include <Camera.h>
#include <geGL/StaticCalls.h>

using namespace ge::gl;

auto camera = Camera();

bool SDLHandler(const SDL_Event &event){
    static bool mousePressed = false;
    if(event.type == SDL_KEYDOWN and event.key.repeat){
        switch (event.key.keysym.sym) {
            case SDLK_UP:
            case SDLK_w:
                camera.ProcessKeyboard(Camera_Movement::FORWARD, 0.1);
                return true;
            case SDLK_DOWN:
            case SDLK_s:
                camera.ProcessKeyboard(Camera_Movement::BACKWARD, 0.1);
                return true;
            case SDLK_LEFT:
            case SDLK_a:
                camera.ProcessKeyboard(Camera_Movement::LEFT, 0.1);
                return true;
            case SDLK_RIGHT:
            case SDLK_d:
                camera.ProcessKeyboard(Camera_Movement::RIGHT, 0.1);
                return true;
            default:
                return false;
        }
    } else if(event.type == SDL_MOUSEBUTTONDOWN and event.button.button == SDL_BUTTON_LEFT){
        mousePressed = true;
        return true;
    } else if(event.type == SDL_MOUSEMOTION and mousePressed){
        camera.ProcessMouseMovement(-event.motion.xrel, event.motion.yrel);
        return true;
    } else if(event.type == SDL_MOUSEBUTTONUP and event.button.button == SDL_BUTTON_LEFT){
        mousePressed = false;
        return true;
    }
    return false;
}

int main() {
    auto proj = glm::perspective(glm::radians(45.0f),
                                 static_cast<float>(640) / static_cast<float>(480),
                                 0.1f,
                                 100.0f);

    /*Create Window*/
    auto mainLoop = std::make_shared<sdl2cpp::MainLoop>();
    auto window = std::make_shared<sdl2cpp::Window>(640, 480);
    window->createContext("rendering");
    mainLoop->addWindow("mainWindow", window);
    mainLoop->setEventHandler(SDLHandler);


    /*init OpenGL*/
    ge::gl::init(SDL_GL_GetProcAddress);
    ge::gl::setHighDebugMessage();

    auto vs = std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER,
                                               Utilities::readFile("/home/kuro/CLionProjects/GMU_water_simulation_cellular_automata/Shaders/basic.vert"));
    auto fs = std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER,
                                               Utilities::readFile("/home/kuro/CLionProjects/GMU_water_simulation_cellular_automata/Shaders/basic.frag"));
    auto program = std::make_shared<ge::gl::Program>(vs, fs);

    auto cube = Model("/home/kuro/CLionProjects/GMU_water_simulation_cellular_automata/Resources/Models/cube.obj");

    GLuint vbo;
    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, cube.verticesCount() * sizeof(Model::VertexData), cube.getVertices().data(),
                              GL_STATIC_DRAW);

    GLuint drawIdsBuffer;
    glCreateBuffers(1, &drawIdsBuffer);
    glNamedBufferData(drawIdsBuffer, cube.indicesCount() * sizeof(uint32_t), cube.getIndices().data(),
                              GL_STATIC_DRAW);

    // vertex array object
    GLuint vao;
    glCreateVertexArrays(1,&vao);
    //attrib 0. is vertex positions
    glVertexArrayAttribFormat  (vao,0,3,GL_FLOAT,GL_FALSE, offsetof(Model::VertexData, pos));
    glVertexArrayVertexBuffer  (vao,0,vbo,0,static_cast<GLsizei>(sizeof(Model::VertexData)));
    glVertexArrayAttribBinding (vao,0,0);
    glEnableVertexArrayAttrib  (vao,0);

    glVertexArrayAttribFormat  (vao,1,3,GL_FLOAT,GL_FALSE, offsetof(Model::VertexData, color));
    glVertexArrayVertexBuffer  (vao,0,vbo,offsetof(Model::VertexData, color),static_cast<GLsizei>(sizeof(Model::VertexData)));
    glVertexArrayAttribBinding (vao,1,0);
    glEnableVertexArrayAttrib  (vao,1);

    glVertexArrayAttribFormat  (vao,2,2,GL_FLOAT,GL_FALSE,offsetof(Model::VertexData, texCoord));
    glVertexArrayVertexBuffer  (vao,0,vbo,0,static_cast<GLsizei>(sizeof(Model::VertexData)));
    glVertexArrayAttribBinding (vao,2,0);
    glEnableVertexArrayAttrib  (vao,2);

    glVertexArrayAttribFormat  (vao,3,3,GL_FLOAT,GL_FALSE,offsetof(Model::VertexData, normal));
    glVertexArrayVertexBuffer  (vao,0,vbo,0,static_cast<GLsizei>(sizeof(Model::VertexData)));
    glVertexArrayAttribBinding (vao,3,0);
    glEnableVertexArrayAttrib  (vao,3);

    glClearColor(0,0,0,1);

    glEnable(GL_DEPTH_TEST);

    mainLoop->setIdleCallback([&]() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        program->use();

        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawIdsBuffer);
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
        glUniformMatrix4fv(5, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(6, 1, GL_FALSE, glm::value_ptr(proj));
        glUniform3fv(7, 1, glm::value_ptr(camera.Position));
        glDrawElementsInstanced(GL_TRIANGLES, cube.indicesCount(), GL_UNSIGNED_INT, nullptr, 125);
        glBindVertexArray(0);
        window->swap();
    });

    (*mainLoop)();

    return 0;
}