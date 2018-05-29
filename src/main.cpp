#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <exception>
#include <iostream>
#include <string> 
#include <array>
#include <chrono>
#include <cmath>

#include <qfs-cpp/qfs.hpp>
#include "shader.hpp"

#include "main.h"

/* tmp globals */

int main (int, char**)
{
/* SDL/GL INIT */

	glfwSetErrorCallback([](int error, const char* description){std::cerr << "Error " << error << ": " << description << "\n";});
	if (!glfwInit()) return 1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Geo", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	gladLoadGL();

/* IMGUI INIT */

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfwGL3_Init(window, false);

	glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
	glfwSetScrollCallback(window, glfw_scroll_callback);
	glfwSetKeyCallback(window, glfw_key_callback);
	glfwSetCharCallback(window, glfw_char_callback);
	glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);

	ImGui::StyleColorsDark();
	//ImGuiStyle& style = ImGui::GetStyle();
	//style.Alpha = 1.f;

/* VISUALIZE INIT */

	std::string res = qfs::real_path(qfs::dir(qfs::exe_path()) + "../res/");

	//Shader program(res + "geo.vsh", res + "geo.fsh");
	int graph_style = 0;

	enum
	{
		GRAPH_POLAR,
		GRAPH_CARTESIAN,
		GRAPH_COMPLEX
	};

	glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 3.0f);  
	glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 camera_direction = glm::normalize(camera_pos - camera_target);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 

	glm::vec3 cameraRight = glm::normalize(glm::cross(up, camera_direction));
	glm::vec3 cameraUp = glm::cross(camera_direction, cameraRight);

	glm::mat4 view = glm::mat4();
	view = glm::lookAt(camera_pos, camera_pos - glm::vec3(0.0f, 0.0f, 1.0f), up);
	glm::mat4 model = glm::mat4();
	glm::mat4 proj = glm::mat4();


	glClearColor(0.1, 0.1, 0.1, 1);
	glEnable(GL_MULTISAMPLE);  
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader shader(res + "geo.vs", res + "geo.fs");//, res + "geo.gs");
	Shader lnshader(res + "grid.vs", res + "grid.fs", res + "grid.gs");
	Shader circshader(res + "poly.vs", res + "poly.fs", res + "poly.gs");
	Shader sumshader(res + "sum.vs", res + "sum.fs", res + "sum.gs");
	
	std::vector<float> lnvertices;//{1, 0, 1, 0, 0, 0, 1, 0};

	bool is_triangle = false;

	for (int i = -64; i <= 64; ++i)
	{
		lnvertices.push_back((float)(i));
		if (i != 0)
		{
			if (i % 2 == 0)
			{
				lnvertices.push_back(0.3);
				lnvertices.push_back(0.3);
				lnvertices.push_back(0.3);
			}
			else
			{
				lnvertices.push_back(0.16);
				lnvertices.push_back(0.16);
				lnvertices.push_back(0.16);
			}
		}
		else
		{
			lnvertices.push_back(1);
			lnvertices.push_back(1);
			lnvertices.push_back(0);
		}
	}

	unsigned int lnVBO, lnVAO;
	glGenVertexArrays(1, &lnVAO);
	glGenBuffers(1, &lnVBO);

	glBindVertexArray(lnVAO);

	glBindBuffer(GL_ARRAY_BUFFER, lnVBO);
	glBufferData(GL_ARRAY_BUFFER, lnvertices.size() * sizeof(float), lnvertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(1 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0); 

	std::vector<float> circvertices /*{0, 0, 3, 0.5, 0.5, 0.5}*/ ;

	for (int i = 1; i <= 64; ++i)
	{
		circvertices.push_back(0);
		circvertices.push_back(0);
		circvertices.push_back(i);
		if (i != 1)
		{
			if (i % 2 == 0)
			{
				circvertices.push_back(0.3);
				circvertices.push_back(0.3);
				circvertices.push_back(0.3);
			}
			else
			{
				circvertices.push_back(0.16);
				circvertices.push_back(0.16);
				circvertices.push_back(0.16);
			}
		}
		else
		{
			circvertices.push_back(1);
			circvertices.push_back(1);
			circvertices.push_back(0);
		}
	}

	unsigned int circVBO, circVAO;
	glGenVertexArrays(1, &circVAO);
	glGenBuffers(1, &circVBO);

	glBindVertexArray(circVAO);

	glBindBuffer(GL_ARRAY_BUFFER, circVBO);
	glBufferData(GL_ARRAY_BUFFER, circvertices.size() * sizeof(float), circvertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0); 

	std::vector<glm::vec2> eqnvertices;

	unsigned int eqnVBO, eqnVAO;
	glGenVertexArrays(1, &eqnVAO);
	glGenBuffers(1, &eqnVBO);

	glBindVertexArray(eqnVAO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0); 

	std::vector<glm::vec2> sumvertices;

	unsigned int sumVBO, sumVAO;
	glGenVertexArrays(1, &sumVAO);
	glGenBuffers(1, &sumVBO);

	glBindVertexArray(sumVAO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0); 



/* U-R LOOP */

	enum cartesian_equation_e
	{
		CART_STANDARD,
		CART_PARAMETRIC
	};
	int cartesian_equation_type = CART_STANDARD;

//
	float pol_final = 1;

	float coefficient_outer = 0;
	float coefficient_inner = 0;
	int cos_or_sin = 0; //0 is cos, 1 is sine. even is cos, odd is sine

//
	float par_initial = 0;
	float par_final = 1;
	std::vector<float> par_coefficients_x;
	int suggested_coef_x = 0;
	int suggested_coef_y = 0;
	std::vector<float> par_coefficients_y;

	std::vector<glm::vec2> par_vertices;
//
	int suggested_coef_num = 0;
	std::vector<float> std_coefficients;

	std::vector<glm::vec2> std_vertices;

	float riemann_width = 1;
//
	int zoom = 50; /* 1 unit of screen space = zoom units of pixel space */

	while (!glfwWindowShouldClose(window))
	{	
/* EVENT */
;
		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		float offset_w = display_w * 1.f / zoom; //width of screen in coordinate space = 12.8, half = 6.4
		float offset_h = display_h * 1.f/ zoom;

		//proj = glm::perspective(glm::radians(45.f), display_w * 1.f/display_h, 0.1f, 100.0f);  
		proj = glm::ortho(-offset_w/2, offset_w/2, -offset_h/2, offset_h/2, 0.1f, 100.0f);

/* CONTROLS*/

		ImGui::Begin("Geometric Visualizer", nullptr);
		{
			ImGui::InputInt("Zoom", &zoom);

			ImGui::RadioButton("Polar", &graph_style, GRAPH_POLAR); ImGui::SameLine();
			ImGui::RadioButton("Cartesian", &graph_style, GRAPH_CARTESIAN); ImGui::SameLine();
			ImGui::RadioButton("Complex", &graph_style, GRAPH_COMPLEX);

			if (graph_style != GRAPH_COMPLEX)
			{
				ImGui::NewLine();
				ImGui::Text("Equation Editor");
				ImGui::Separator();

				if (graph_style == GRAPH_POLAR)
				{
					ImGui::Text("r = A * cos or sin (B * theta)");
					ImGui::Text("for theta in range(0, C) * 2 * pi:");

					ImGui::InputFloat("C", &pol_final);
					ImGui::InputFloat("A", &coefficient_outer);
					ImGui::InputFloat("B", &coefficient_inner);

					ImGui::RadioButton("Cos", &cos_or_sin, 0); ImGui::SameLine();
					ImGui::RadioButton("Sin", &cos_or_sin, 1);

					if (ImGui::Button("Visualize!")) 
					{
						is_triangle = true;
						eqnvertices.resize(0);
						sumvertices.resize(0);

						int a = 0;
						for (float x = 0; x < pol_final; x += 0.001)
						{
							float angle = coefficient_inner * x * 2 * 3.14159265358979323f;
							float r = cos_or_sin % 2 == 0? coefficient_outer * cos(angle)
							: coefficient_outer * sin(angle);
							eqnvertices.push_back(glm::vec2(r * cos(x * 2 * 3.14159265358979323f), r * sin(x * 2 * 3.14159265358979323f)));
							//if (a % 10 == 0) sumvertices.push_back(glm::vec2(r * cos(x * 2 * 3.14159265358979323f), r * sin(x * 2 * 3.14159265358979323f)));
							//a += 1;
						}

						glBindVertexArray(eqnVAO);
						glBindBuffer(GL_ARRAY_BUFFER, eqnVBO);
						glBufferData(GL_ARRAY_BUFFER, eqnvertices.size() * sizeof(glm::vec2), eqnvertices.data(), GL_DYNAMIC_DRAW);

						glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
						glEnableVertexAttribArray(0);
						
						glBindBuffer(GL_ARRAY_BUFFER, 0);
						glBindVertexArray(0);

						glBindVertexArray(sumVAO);
						glBindBuffer(GL_ARRAY_BUFFER, sumVBO);
						glBufferData(GL_ARRAY_BUFFER, sumvertices.size() * sizeof(glm::vec2), sumvertices.data(), GL_DYNAMIC_DRAW);

						glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
						glEnableVertexAttribArray(0);
						
						glBindBuffer(GL_ARRAY_BUFFER, 0);
						glBindVertexArray(0);
					
					}
				}
				else if (graph_style == GRAPH_CARTESIAN)
				{
					ImGui::RadioButton("Standard", &cartesian_equation_type, CART_STANDARD); ImGui::SameLine();
					ImGui::RadioButton("Parametric", &cartesian_equation_type, CART_PARAMETRIC);

					if (cartesian_equation_type == CART_STANDARD)
					{
						ImGui::Text("y = sum(0, N): C_n * x^n:");
						ImGui::InputInt("= N", &suggested_coef_num);
						if (suggested_coef_num < 0) suggested_coef_num = 0;
						ImGui::SameLine(); 
						if (ImGui::SmallButton("Save N")) {std_coefficients.resize(suggested_coef_num);};

						for (int i = 0; i < std_coefficients.size(); ++i)
						{
							ImGui::InputFloat((std::string("C_") + std::to_string(i) + "##STD").c_str(), &std_coefficients[i]);
						}

						if (ImGui::Button("Visualize!")) 
						{
							is_triangle = false;
							eqnvertices.resize(0);
							sumvertices.resize(0);

							int a = 0;
							for (float x = (int) (-offset_w/2) - 1; x < (int) (offset_w/2) + 1; x += 0.1)
							{
								//should_sum = !should_sum;
								float sum = 0;
								for (int i = 0; i < std_coefficients.size(); ++i)
								{
									sum += std_coefficients[i] * pow(x, i);
								}
								eqnvertices.push_back(glm::vec2(x, sum));

								if (a % 10 == 0)sumvertices.push_back(glm::vec2(x, sum));
								a += 1;
							}

							glBindVertexArray(eqnVAO);
							glBindBuffer(GL_ARRAY_BUFFER, eqnVBO);
							glBufferData(GL_ARRAY_BUFFER, eqnvertices.size() * sizeof(glm::vec2), eqnvertices.data(), GL_DYNAMIC_DRAW);

							glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
							glEnableVertexAttribArray(0);
							
							glBindBuffer(GL_ARRAY_BUFFER, 0);
							glBindVertexArray(0);

							glBindVertexArray(sumVAO);
							glBindBuffer(GL_ARRAY_BUFFER, sumVBO);
							glBufferData(GL_ARRAY_BUFFER, sumvertices.size() * sizeof(glm::vec2), sumvertices.data(), GL_DYNAMIC_DRAW);

							glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
							glEnableVertexAttribArray(0);
							
							glBindBuffer(GL_ARRAY_BUFFER, 0);
							glBindVertexArray(0);
						
						}

					}
					else if (cartesian_equation_type == CART_PARAMETRIC)
					{
						ImGui::Text("x = sum(0, M): C_m * t^m");
						ImGui::Text("y = sum(0, N): C_n * t^n");
						ImGui::Text("for t in range(i, f):");

						ImGui::InputFloat("i", &par_initial);
						ImGui::InputFloat("f", &par_final);

						ImGui::InputInt("= M", &suggested_coef_x);
						if (suggested_coef_x < 0) suggested_coef_x = 0;
						ImGui::SameLine(); 
						if (ImGui::SmallButton("Save M")) {par_coefficients_x.resize(suggested_coef_x);};

						for (int i = 0; i < par_coefficients_x.size(); ++i)
						{
							ImGui::InputFloat((std::string("Cx_") + std::to_string(i) + "##PARX").c_str(), &par_coefficients_x[i]);
						}

						ImGui::InputInt("= N", &suggested_coef_y);
						if (suggested_coef_y < 0) suggested_coef_y = 0;
						ImGui::SameLine(); 
						if (ImGui::SmallButton("Save N")) {par_coefficients_y.resize(suggested_coef_y);};
						
						for (int i = 0; i < par_coefficients_y.size(); ++i)
						{
							ImGui::InputFloat((std::string("Cy_") + std::to_string(i) + "##PARY").c_str(), &par_coefficients_y[i]);
						}

						if (ImGui::Button("Visualize!")) 
						{
							eqnvertices.resize(0);
							sumvertices.resize(0);

							for (float t = par_initial; t < par_final; t += 0.1)
							{
								float xsum = 0;
								float ysum = 0;
								for (int i = 0; i < par_coefficients_x.size(); ++i)
								{
									xsum += par_coefficients_x[i] * pow(t, i);
								}
								for (int i = 0; i < par_coefficients_y.size(); ++i)
								{
									ysum += par_coefficients_y[i] * pow(t, i);
								}
								eqnvertices.push_back(glm::vec2(xsum, ysum));
							}

							glBindVertexArray(eqnVAO);
							glBindBuffer(GL_ARRAY_BUFFER, eqnVBO);
							glBufferData(GL_ARRAY_BUFFER, eqnvertices.size() * sizeof(glm::vec2), eqnvertices.data(), GL_DYNAMIC_DRAW);

							glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
							glEnableVertexAttribArray(0);
							
							glBindBuffer(GL_ARRAY_BUFFER, 0);
							glBindVertexArray(0);

							/*glBindVertexArray(sumVAO);
							glBindBuffer(GL_ARRAY_BUFFER, sumVBO);
							glBufferData(GL_ARRAY_BUFFER, sumvertices.size() * sizeof(glm::vec2), sumvertices.data(), GL_DYNAMIC_DRAW);

							glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
							glEnableVertexAttribArray(0);
							
							glBindBuffer(GL_ARRAY_BUFFER, 0);
							glBindVertexArray(0);*/
						
						}
					}
				}
			}

		}
		ImGui::End();

/* VISUALIZE */
		
		glBindVertexArray(lnVAO);
		lnshader.use();
		lnshader.setMat4("mvp_transformation", proj * view * model);
		glDrawArrays(GL_POINTS, 64, 1);

		if (graph_style == GRAPH_POLAR || graph_style == GRAPH_COMPLEX)
		{

			glBindVertexArray(circVAO);
			circshader.use();
			circshader.setMat4("mvp_transformation", proj * view * model);
			circshader.setVec2("screen_to_pixel", glm::vec2(display_w/2, display_h/2));
			circshader.setVec2("screen_to_world", glm::vec2(offset_w/2, offset_h/2));
			glDrawArrays(GL_POINTS, 0, 63);

		}

		if(graph_style == GRAPH_CARTESIAN || graph_style == GRAPH_COMPLEX)
		{
			glBindVertexArray(lnVAO);
			lnshader.use();
			lnshader.setMat4("mvp_transformation", proj * view * model);

			glDrawArrays(GL_POINTS, 0, 64);
			glDrawArrays(GL_POINTS, 65, 64);
		}

		if (eqnvertices.size() > 0)
		{
			glBindVertexArray(sumVAO);
			sumshader.use();
			sumshader.setMat4("transformation", proj * view * model);
			sumshader.setFloat("dx", riemann_width);
			sumshader.setVec2("world_quadrant", glm::vec2(offset_w/2, offset_h/2));
			sumshader.setBool("is_triangle", is_triangle);
			glDrawArrays(GL_POINTS, 0, sumvertices.size());


			glBindVertexArray(eqnVAO);
			shader.use();
			shader.setMat4("mvp_transformation", proj * view * model);
			glDrawArrays(GL_LINE_STRIP, 0, eqnvertices.size());
		}
		
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}