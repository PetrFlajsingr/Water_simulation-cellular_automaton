#include <iostream>
#include <geGL/geGL.h>
#include <SDL2CPP/Window.h>
#include <SDL2CPP/MainLoop.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/component_wise.hpp>
#include <Model.h>
#include <Utilities.h>
#include <Camera.h>
#include <geGL/StaticCalls.h>
#include <Cell.h>

using namespace ge::gl;

auto camera = Camera(glm::vec3(0.0, 0.0, 5.0));

bool SDLHandler(const SDL_Event &event) {
    static bool mousePressed = false;
    if (event.type == SDL_KEYDOWN) {
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
    } else if (event.type == SDL_MOUSEBUTTONDOWN and event.button.button == SDL_BUTTON_LEFT) {
        mousePressed = true;
        return true;
    } else if (event.type == SDL_MOUSEMOTION and mousePressed) {
        camera.ProcessMouseMovement(-event.motion.xrel, event.motion.yrel);
        return true;
    } else if (event.type == SDL_MOUSEBUTTONUP and event.button.button == SDL_BUTTON_LEFT) {
        mousePressed = false;
        return true;
    }
    return false;
}

int main() {
    auto tankSize = glm::uvec3(2,2,2);
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
                                               Utilities::readFile(
                                                       "/home/kuro/CLionProjects/GMU_water_simulation_cellular_automata/Shaders/basic.vert"));
    auto fs = std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER,
                                               Utilities::readFile(
                                                       "/home/kuro/CLionProjects/GMU_water_simulation_cellular_automata/Shaders/basic.frag"));
    auto csH = std::make_shared<ge::gl::Shader>(GL_COMPUTE_SHADER, Utilities::readFile(
            "/home/kuro/CLionProjects/GMU_water_simulation_cellular_automata/Shaders/basic-horizontal.comp"));
    auto csV = std::make_shared<ge::gl::Shader>(GL_COMPUTE_SHADER, Utilities::readFile(
            "/home/kuro/CLionProjects/GMU_water_simulation_cellular_automata/Shaders/basic-vertical.comp"));


    auto graphicsProgram = std::make_shared<ge::gl::Program>(vs, fs);
    auto computeHorizontalProgram = std::make_shared<ge::gl::Program>(csH);
    auto computeVerticalProgram = std::make_shared<ge::gl::Program>(csV);

    auto cube = Model("/home/kuro/CLionProjects/GMU_water_simulation_cellular_automata/Resources/Models/cube.obj");
    auto cells = std::vector<Cell>(glm::compMul(tankSize));
    cells[5].setFluidVolume(1.0);
/*    cells[510].setFluidVolume(1.0);
    cells[502].setFluidVolume(1.0);
    cells[503].setFluidVolume(1.0);*/

    GLuint vbo;
    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, cube.verticesCount() * sizeof(Model::VertexData), cube.getVertices().data(),
                      GL_STATIC_DRAW);

    GLuint drawIdsBuffer;
    glCreateBuffers(1, &drawIdsBuffer);
    glNamedBufferData(drawIdsBuffer, cube.indicesCount() * sizeof(uint32_t), cube.getIndices().data(),
                      GL_STATIC_DRAW);

    std::array<GLuint, 2> cellBuffers{};
    glCreateBuffers(2, cellBuffers.data());
    glNamedBufferData(cellBuffers[0], cells.size() * sizeof(Cell), cells.data(), GL_DYNAMIC_COPY);
    glNamedBufferData(cellBuffers[1], cells.size() * sizeof(Cell), cells.data(), GL_DYNAMIC_COPY);

    std::vector<glm::vec4> positions{glm::compMul(tankSize)};
    GLuint positionsBuffer;
    glCreateBuffers(1, &positionsBuffer);
    glNamedBufferData(positionsBuffer, positions.size() * sizeof(glm::vec4), positions.data(), GL_DYNAMIC_DRAW);

    GLuint indirectBuffer;
    std::array<uint32_t, 5> command{cube.indicesCount(), 2, 0, 0, 0};
    glCreateBuffers(1, &indirectBuffer);
    glNamedBufferData(indirectBuffer, sizeof(uint32_t) * command.size(), command.data(), GL_DYNAMIC_DRAW);



    // vertex array object
    GLuint vao;
    glCreateVertexArrays(1, &vao);
    //attrib 0. is vertex positions
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Model::VertexData, pos));
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, static_cast<GLsizei>(sizeof(Model::VertexData)));
    glVertexArrayAttribBinding(vao, 0, 0);
    glEnableVertexArrayAttrib(vao, 0);

    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Model::VertexData, color));
    glVertexArrayVertexBuffer(vao, 0, vbo, offsetof(Model::VertexData, color),
                              static_cast<GLsizei>(sizeof(Model::VertexData)));
    glVertexArrayAttribBinding(vao, 1, 0);
    glEnableVertexArrayAttrib(vao, 1);

    glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Model::VertexData, texCoord));
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, static_cast<GLsizei>(sizeof(Model::VertexData)));
    glVertexArrayAttribBinding(vao, 2, 0);
    glEnableVertexArrayAttrib(vao, 2);

    glVertexArrayAttribFormat(vao, 3, 3, GL_FLOAT, GL_FALSE, offsetof(Model::VertexData, normal));
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, static_cast<GLsizei>(sizeof(Model::VertexData)));
    glVertexArrayAttribBinding(vao, 3, 0);
    glEnableVertexArrayAttrib(vao, 3);

    glClearColor(0, 0, 0, 1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    mainLoop->setIdleCallback([&]() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        computeHorizontalProgram->use();
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, cellBuffers[0]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, cellBuffers[1]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, positionsBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, indirectBuffer);


        Cell *ptrRD;
        Cell *ptrWR;
        ptrRD = (Cell *) glMapNamedBuffer(cellBuffers[1], GL_READ_ONLY);
        ptrWR = (Cell *) glMapNamedBuffer(cellBuffers[0], GL_READ_ONLY);

        glUnmapNamedBuffer(cellBuffers[0]);glUnmapNamedBuffer(cellBuffers[1]);


        glDispatchCompute(tankSize.x/2, tankSize.y/2, tankSize.z/2);

        ptrRD = (Cell *) glMapNamedBuffer(cellBuffers[1], GL_READ_ONLY);
        ptrWR = (Cell *) glMapNamedBuffer(cellBuffers[0], GL_READ_ONLY);

        glUnmapNamedBuffer(cellBuffers[0]);glUnmapNamedBuffer(cellBuffers[1]);

        std::swap(cellBuffers[0], cellBuffers[1]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, cellBuffers[0]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, cellBuffers[1]);
        computeVerticalProgram->use();
        glDispatchCompute(tankSize.x/2, tankSize.y/2, tankSize.z/2);

        ptrRD = (Cell *) glMapNamedBuffer(cellBuffers[1], GL_READ_ONLY);
        ptrWR = (Cell *) glMapNamedBuffer(cellBuffers[0], GL_READ_ONLY);

        glUnmapNamedBuffer(cellBuffers[0]);glUnmapNamedBuffer(cellBuffers[1]);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);



        glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

        graphicsProgram->use();

        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawIdsBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, positionsBuffer);
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
        glUniformMatrix4fv(5, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(6, 1, GL_FALSE, glm::value_ptr(proj));
        glUniform3fv(7, 1, glm::value_ptr(camera.Position));
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
        glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        window->swap();
        std::swap(cellBuffers[0], cellBuffers[1]);
    });

    (*mainLoop)();

    return 0;
}
