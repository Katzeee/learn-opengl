// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include <array>
#include <assimp/Importer.hpp>
#include <cmath>
#include <filesystem>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "camera/camera.hpp"
#include "context/context.hpp"
#include "input/input_system.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "shader/shader.hpp"


void FrameBufferSizeCB(GLFWwindow *window, int width, int height) {
  glViewport(global_context.imgui_width_, 0, width - global_context.imgui_width_, height);
  global_context.window_width_ = width;
  global_context.window_height_ = height;
  global_context.camera_->SetAspect(static_cast<float>(width - global_context.imgui_width_) / height);
}

static auto InCommand(xac::ControlCommand command) -> bool {
  return static_cast<uint32_t>(command) & global_context.control_commad_;
}

auto main() -> int {
#pragma region window init
  if (!glfwInit()) {
    return -1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __linux__
  glfwWindowHintString(GLFW_X11_CLASS_NAME, "opengl test");
  glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "opengl test");
#endif

  GLFWwindow *window =
      glfwCreateWindow(global_context.window_width_, global_context.window_height_, "Hello OpenGL", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  glfwSetFramebufferSizeCallback(window, FrameBufferSizeCB);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
#pragma endregion

  global_context.Init();
  xac::InputSystem::Init(window);
  float delta_time_per_frame;

#pragma region imgui init
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 450 core");
  ImGui::StyleColorsDark();
#pragma endregion

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "Failed to initialize GALD!" << std::endl;
    return -1;
  }

  glViewport(global_context.imgui_width_, 0, global_context.window_width_ - global_context.imgui_width_,
             global_context.window_height_);
  global_context.camera_->SetAspect(static_cast<float>(global_context.window_width_ - global_context.imgui_width_) /
                                    global_context.window_height_);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);

  // clang-format off
  std::vector<unsigned int> light_box_indices{
     0,  1,  2,  2,  3,  0, 
     4,  5,  6,  6,  7,  4,
     8,  9, 10, 10, 11,  8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20,
  };

  std::vector<unsigned int> ground_indices{
     0,  1,  2,  2,  3,  0, 
  };

  std::vector<std::vector<unsigned int>> cage_face_indices{
    { 0,  1,  2,  2,  3,  0}, 
    { 4,  5,  6,  6,  7,  4},
    { 8,  9, 10, 10, 11,  8},
    {12, 13, 14, 14, 15, 12},
    {16, 17, 18, 18, 19, 16},
    {20, 21, 22, 22, 23, 20},
  };

  std::vector<xac::Mesh::Vertex> plain_vertices{
    {{-5.0f,  0.0f, -5.0f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}}, 
    {{ 5.0f,  0.0f, -5.0f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}}, 
    {{ 5.0f,  0.0f,  5.0f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}}, 
    {{-5.0f,  0.0f,  5.0f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}}, 
  };

  std::vector<xac::Mesh::Vertex> box_vertices{
    // positions              normals        texture coords
    {{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}}, 
    {{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}}, 
    {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}}, 
    {{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}}, 
                                               
    {{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}}, 
    {{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}}, 
    {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}}, 
    {{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}}, 
                                               
    {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}}, 
    {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}, 
    {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}, 
    {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, 
                                               
    {{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}}, 
    {{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}, 
    {{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}, 
    {{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, 
                                               
    {{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}}, 
    {{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}}, 
    {{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}}, 
    {{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}}, 
                                               
    {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}}, 
    {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}}, 
    {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}}, 
    {{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}}, 
  };
  // clang-format on

#pragma region setup objs
  xac::Model m_herta("../resources/models/Herta/heita.obj");
  xac::Mesh m_light_cube{box_vertices, light_box_indices, {}, "light"};
  xac::Mesh m_cube = m_light_cube;
  xac::Mesh m_ground{plain_vertices, ground_indices, {}, "ground"};

  auto light_shader = std::make_shared<xac::Shader>("../15-stencil-test/shader/light_vert.glsl",
                                                    "../15-stencil-test/shader/light_frag.glsl");
  auto obj_shader = std::make_shared<xac::Shader>("../15-stencil-test/shader/obj_vert.glsl",
                                                  "../15-stencil-test/shader/obj_frag.glsl");

  auto t_ground_diffuse = xac::LoadTextureFromFile("../resources/textures/container2.png");
  auto t_ground_specular = xac::LoadTextureFromFile("../resources/textures/container2_specular.png");
  auto t_white = xac::LoadTextureFromFile("../resources/textures/white.png");

  m_light_cube.SetShader(light_shader);
  m_herta.SetShader(obj_shader);
  m_ground.SetShader(obj_shader);
  m_cube.SetShader(obj_shader);

  std::array<std::shared_ptr<xac::Mesh>, 6> m_cage_faces;
  for (int i = 0; i < 6; i++) {
    m_cage_faces[i] = std::make_shared<xac::Mesh>(box_vertices, cage_face_indices[i], xac::Material{});
    m_cage_faces[i]->SetShader(obj_shader);
  }

#pragma endregion

  struct PointLight {
    glm::vec4 color = glm::vec4{1};
    float intensity = 5;
  };
  struct DirectLight {
    glm::vec4 color = glm::vec4{1};
    float intensity = 1;
    glm::vec3 direction = glm::vec3{1, 1, 1};
  };

#pragma region imgui variables
  glm::vec4 background_color{};
  PointLight p_lights[2];
  DirectLight d_lights[1];
  glm::vec3 rotation_axis{1, 1, 1};
  float rotation_degree{60};
  int gl_func_item = 1;
  int debug_item = 0;

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
#pragma endregion

  // HINT: Render loop start
  while (!glfwWindowShouldClose(window)) {
#pragma region render imgui
    {
      ImGui::NewFrame();
      ImGui::SetNextWindowPos({0, 0});
      ImGui::SetNextWindowSize(
          {static_cast<float>(global_context.imgui_width_), static_cast<float>(global_context.window_height_)});
      ImGui::Begin("Hello, World!", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
      ImGui::SetWindowFontScale(1.2);

      ImGui::ColorEdit4("BG_color", reinterpret_cast<float *>(&background_color), ImGuiColorEditFlags_AlphaPreview);

      if (ImGui::TreeNodeEx("Rotating light", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::ColorEdit3("p_light1", reinterpret_cast<float *>(&p_lights[0].color));
        ImGui::Separator();
        ImGui::ColorEdit3("p_light2", reinterpret_cast<float *>(&p_lights[1].color));
        ImGui::DragFloat3("rotation_axis", reinterpret_cast<float *>(&rotation_axis), 0.005f, -1.0f, 1.0f);
        ImGui::Separator();
        ImGui::ColorEdit3("d_light1", reinterpret_cast<float *>(&d_lights[0].color));
        ImGui::PushItemWidth(80);
        ImGui::SliderFloat("X", &d_lights[0].direction.x, -1.0f, 1.0f);
        ImGui::SameLine();
        ImGui::SliderFloat("Y", &d_lights[0].direction.y, -1.0f, 1.0f, "%.3f", ImGuiSliderFlags_None);
        ImGui::SameLine();
        ImGui::SliderFloat("Z", &d_lights[0].direction.z, -1.0f, 1.0f);
        ImGui::PopItemWidth();
        ImGui::TreePop();
      }
      ImGui::SliderFloat("Camera speed", &global_context.camera_->speed_, 5.0f, 30.0f);
      ImGui::SliderFloat("rotation_degree", &rotation_degree, 0, 360);
      if (ImGui::TreeNodeEx("Depth test", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Combo("Debug Mode", &debug_item, "NODEBUG\0NORMAL\0DEPTH\0");
        ImGui::Combo("Depth test", &gl_func_item,
                     "GL_NEVER\0GL_LESS\0GL_EQUAL\0GL_LEQUAL\0GL_GREATER\0GL_NOTEQUAL\0GL_GEQUAL\0GL_ALWAYS\0");
        ImGui::TreePop();
      }

      if (ImGui::CollapsingHeader("Section 2", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::TreeNodeEx("Cursor", ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::Text("xoffset: %f\nyoffset: %f", xac::InputSystem::cursor_x_offset_,
                      xac::InputSystem::cursor_y_offset_);
          ImVec2 mouse_position_absolute = ImGui::GetMousePos();
          ImGui::Text("Position: %f, %f", mouse_position_absolute.x, mouse_position_absolute.y);
          ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("Command", ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::Text("FORWARD: %d, BACKWARD: %d", xac::InCommand(xac::ControlCommand::FORWARD),
                      xac::InCommand(xac::ControlCommand::BACKWARD));
          ImGui::Text("LEFT: %4d, RIGHT: %4d", xac::InCommand(xac::ControlCommand::LEFT),
                      xac::InCommand(xac::ControlCommand::RIGHT));
          ImGui::Text("DOWN: %4d, UP: %7d", xac::InCommand(xac::ControlCommand::DOWN),
                      xac::InCommand(xac::ControlCommand::UP));
          ImGui::TreePop();
        }
        ImGui::Text("Frame rate: %f", 1.0 / delta_time_per_frame);
      }
      ImGui::End();
      ImGui::Render();
    }
#pragma endregion

    glDepthFunc(GL_NEVER + gl_func_item);
    static float last_frame_time = 0.0f;
    auto cur_frame_time = static_cast<float>(glfwGetTime());
    delta_time_per_frame = cur_frame_time - last_frame_time;
    last_frame_time = cur_frame_time;

    global_context.camera_->Tick(delta_time_per_frame);
    xac::InputSystem::Tick();

    glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

#pragma region render light
    glm::mat4 light_model(1.0f);
    float rotate_speed = 0.5;
    // make light rotate
    float decimal = rotate_speed * cur_frame_time - std::floor(rotate_speed * cur_frame_time);
    float phi = glm::radians(360 * decimal);
    glm::vec3 light_pos(5 * std::cos(phi), 0, 5 * std::sin(phi));
    light_pos = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_degree), rotation_axis) * glm::vec4(light_pos, 1);
    // Because you do the transformation by the order scale->rotate->translate,
    // glm functions are doing right multiply, so
    // the model matrix should reverse it, that is translate->rotate->scale
    light_model = glm::translate(light_model, light_pos);
    light_model = glm::scale(light_model, glm::vec3(0.2f));

    light_shader->Use();
    light_shader->SetMat4("V", global_context.camera_->GetViewMatrix());
    light_shader->SetMat4("P", global_context.camera_->GetProjectionMatrix());

    light_shader->Use();
    light_shader->SetMat4("M", light_model);
    light_shader->SetVec4("color", p_lights[0].color);
    m_light_cube.Draw();

    light_shader->Use();
    glm::vec3 light2_pos{3, 2, 0};
    auto light2_model = glm::scale(glm::translate(glm::mat4{1}, light2_pos), glm::vec3{0.2});
    light_shader->SetMat4("M", light2_model);
    light_shader->SetVec4("color", p_lights[1].color);
    m_light_cube.Draw();

    light_shader->Use();
    glm::vec3 d_light_pos = d_lights[0].direction * 40.0f;
    auto d_light_model = glm::scale(glm::translate(glm::mat4{1}, d_light_pos), glm::vec3{3.0f});
    light_shader->SetMat4("M", d_light_model);
    light_shader->SetVec4("color", d_lights[0].color);
    m_light_cube.Draw();

#pragma endregion

#pragma region common settings for obj shader
    obj_shader->Use();
    obj_shader->SetVec3("p_lights[0].ws_position", light_pos);
    obj_shader->SetVec3("p_lights[1].ws_position", light2_pos);
    obj_shader->SetVec3("p_lights[0].color", p_lights[0].color);
    obj_shader->SetFloat("p_lights[0].intensity", p_lights[0].intensity);
    obj_shader->SetVec3("p_lights[1].color", p_lights[1].color);
    obj_shader->SetFloat("p_lights[1].intensity", p_lights[1].intensity);

    obj_shader->SetVec3("d_lights[0].direction", d_lights[0].direction);
    obj_shader->SetVec3("d_lights[0].color", d_lights[0].color);
    obj_shader->SetFloat("d_lights[0].intensity", d_lights[0].intensity);
    obj_shader->SetVec3("ws_cam_pos", global_context.camera_->GetPosition());
#pragma endregion

#pragma region render ground
    obj_shader->Use();
    auto ground_model = glm::mat4(1);
    // you should do scale and rotation at origin!
    ground_model = glm::translate(ground_model, {0, -2.8, 0});
    ground_model = glm::rotate(ground_model, glm::radians(30.0f), {0, 1, 0});
    ground_model = glm::scale(ground_model, {15, 1, 15});
    obj_shader->SetMat4("Model", ground_model);
    obj_shader->SetMat4("View", global_context.camera_->GetViewMatrix());
    obj_shader->SetMat4("Proj", global_context.camera_->GetProjectionMatrix());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t_white);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    obj_shader->SetInt("texture_diffuse0", 0);
    obj_shader->SetInt("texture_specular0", 1);
    m_ground.Draw();
#pragma endregion

#pragma region render cage
    glStencilMask(0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    obj_shader->Use();
    auto cage_model = glm::scale(glm::translate(glm::mat4{1}, {10, 5, 15}), glm::vec3{15});
    obj_shader->SetMat4("Model", cage_model);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    obj_shader->SetVec4("base_color", {0.5, 0.5, 0.9, 1.0});
    m_cage_faces[3]->Draw();
    glStencilFunc(GL_ALWAYS, 2, 0xFF);
    obj_shader->SetVec4("base_color", {0.5, 0.9, 0.5, 1.0});
    m_cage_faces[1]->Draw();
    glStencilFunc(GL_ALWAYS, 3, 0xFF);
    obj_shader->SetVec4("base_color", {0.9, 0.9, 0.5, 1.0});
    m_cage_faces[0]->Draw();
    obj_shader->SetVec4("base_color", {0.3, 0.3, 0.8, 1.0});
    m_cage_faces[2]->Draw();
    m_cage_faces[4]->Draw();
    m_cage_faces[5]->Draw();
    // HINT: only write stencil buffer when draw cage
    glad_glStencilMask(0x00);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    // HINT: clear depth buffer to draw objects behind cage
    glClear(GL_DEPTH_BUFFER_BIT);
#pragma endregion

#pragma region render cubes

    glStencilFunc(GL_EQUAL, 2, 0xFF);
    obj_shader->Use();
    obj_shader->SetVec4("base_color", glm::vec4{1});
    // you should do scale and rotation at origin!
    auto cube_model = glm::mat4(1);
    cube_model = glm::translate(cube_model, {-8.0, 0, -10.0});
    cube_model = glm::rotate(cube_model, glm::radians(30.0f), {0, 1, 0});
    cube_model = glm::scale(cube_model, {5, 5, 5});
    obj_shader->SetMat4("Model", cube_model);
    obj_shader->SetMat4("View", global_context.camera_->GetViewMatrix());
    obj_shader->SetMat4("Proj", global_context.camera_->GetProjectionMatrix());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t_ground_diffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, t_ground_specular);
    obj_shader->SetInt("texture_diffuse0", 0);
    obj_shader->SetInt("texture_specular0", 1);
    m_cube.Draw();

    cube_model = glm::mat4(1);
    cube_model = glm::translate(cube_model, {28.0, 0, 20.0});
    cube_model = glm::rotate(cube_model, glm::radians(45.0f), {0, 1, 0});
    cube_model = glm::scale(cube_model, {5, 5, 5});
    obj_shader->SetMat4("Model", cube_model);
    obj_shader->SetMat4("View", global_context.camera_->GetViewMatrix());
    obj_shader->SetMat4("Proj", global_context.camera_->GetProjectionMatrix());

    m_cube.Draw();
#pragma endregion

#pragma region render herta
    glStencilFunc(GL_EQUAL, 1, 0xFF);

    static int last_debug_item = 0;
    if (last_debug_item != debug_item) {
      switch (debug_item) {
        case 0:  // NO_DEBUG
          obj_shader->ClearDefine();
          obj_shader->CompileShaders();
          break;
        case 1:  // DEBUG_NORMAL
          obj_shader->ClearDefine();
          obj_shader->AddDefine("DEBUG_NORMAL");
          obj_shader->CompileShaders();
          break;
        case 2:  // DEBUG_DEPTH
          obj_shader->ClearDefine();
          obj_shader->AddDefine("DEBUG_DEPTH");
          obj_shader->CompileShaders();
          break;
      }
      last_debug_item = debug_item;
    }
    obj_shader->Use();
    obj_shader->SetMat4("Model", glm::scale(glm::translate(glm::mat4(1), glm::vec3(-3, -3, 0)), glm::vec3(0.4)));
    m_herta.Draw();
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glDepthFunc(GL_NEVER + gl_func_item);
    // HINT: or else can't clear stencil buffer bit
    glad_glStencilMask(0xFF);
#pragma endregion

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
