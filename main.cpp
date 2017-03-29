#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>

#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX

#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <SOIL.h>

// Includes GLM
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef BUFFER_OFFSET

#define BUFFER_OFFSET(offset) ((char*)NULL + (offset))

#endif

#include "Shader.h"
#include "Controller.h"

using namespace std;
using namespace glm;

void envoyerMat4(string nom, GLuint programID, glm::mat4 matrice)
{

	int localisation = glGetUniformLocation(programID, nom.c_str());

	glUniformMatrix4fv(localisation, 1, GL_FALSE, glm::value_ptr(matrice));

}

void envoyerVec2(std::string nom, GLuint programID, glm::vec2 vecteur)
{

	// Localisation de la matrice
	int localisation = glGetUniformLocation(programID, nom.c_str());

	// Envoi des valeurs
	glUniform2f(localisation, vecteur.x, vecteur.y);

}


void envoyer1I(std::string nom, GLuint programID, GLuint id)
{

	// Localisation de la matrice
	int localisation = glGetUniformLocation(programID, nom.c_str());

	// Envoi des valeurs
	glUniform1i(localisation, id);

}


void envoyerFloat(std::string nom, GLuint programID, float val)
{

	// Localisation de la matrice
	int localisation = glGetUniformLocation(programID, nom.c_str());

	// Envoi des valeurs
	glUniform1f(localisation, val);

}

bool compilerShader(GLuint &shader, GLenum type, string const &fichierSource)
{

	shader = glCreateShader(type);

	if (shader == 0)
	{
		std::cout << "Erreur, le type de shader (" << type << ") n'existe pas" << std::endl;
		return false;
	}

	ifstream fichier(fichierSource.c_str());

	if (!fichier)
	{
		std::cout << "Erreur le fichier " << fichierSource << " est introuvable" << std::endl;
		glDeleteShader(shader);

		return false;
	}

	std::string ligne;
	std::string codeSource;

	while (getline(fichier, ligne))
		codeSource += ligne + '\n';

	fichier.close();

	const GLchar* chaineCodeSource = codeSource.c_str();

	glShaderSource(shader, 1, &chaineCodeSource, 0);

	glCompileShader(shader);

	GLint erreurCompilation(0);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &erreurCompilation);

	if (erreurCompilation != GL_TRUE)
	{

		GLint tailleErreur(0);
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &tailleErreur);

		char *erreur = new char[tailleErreur + 1];

		glGetShaderInfoLog(shader, tailleErreur, &tailleErreur, erreur);
		erreur[tailleErreur] = '\0';

		std::cout << erreur << std::endl;

		delete[] erreur;
		glDeleteShader(shader);

		return false;
	}

	else
		return true;

}


int main(int argc, char **argv) {

	if (!glfwInit()) {

		std::cout << "Failed to initialize GLFW\n";
		exit(EXIT_FAILURE);

	}

	//=====================

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    int height = mode->height;
    int width = mode->width;

    GLFWwindow* window = glfwCreateWindow(width, height, "Test fluids", NULL, NULL);
	if (!window)
	{

		std::cout << "Erreur lors de la creation de la fenetre" << std::endl;

		glfwTerminate();
		exit(EXIT_FAILURE);

	}

	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

	//=====================

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); //OpenGL 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// On initialise GLEW
	GLenum initialisationGLEW(glewInit());

	// Si l'initialisation a échoué :
	if (initialisationGLEW != GLEW_OK)
	{

		// On affiche l'erreur grâce à la fonction : glewGetErrorString(GLenum code)
		std::cout << "Erreur d'initialisation de GLEW : " << glewGetErrorString(initialisationGLEW) << std::endl;

		glfwTerminate();
		exit(EXIT_FAILURE);

	}

    glfwSetWindowShouldClose(window, false);

    //===========INITIALIZATION===========

    Shader* fluidShader = new Shader("fluid");
    fluidShader->useGeometryShader(true);
    fluidShader->load();

    Shader* defaultShader = new Shader("default");
    defaultShader->load();

    //====Objects====

	int sizeVerticesBytes = sizeof(float) * 18;

	float vertices[] = {-1.0, -1.0, 0.0,   -1.0, 1.0, 0.0,    1.0, -1.0, 0.0,
                        -1.0, 1.0, 0.0,    1.0, 1.0, 0.0,     1.0, -1.0, 0.0};

	GLuint idVAO;
	GLuint idVBO;

	if (glIsBuffer(idVBO) == GL_TRUE)
		glDeleteBuffers(1, &idVBO);

	glGenBuffers(1, &idVBO);

	glBindBuffer(GL_ARRAY_BUFFER, idVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeVerticesBytes, 0, GL_STATIC_DRAW);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVerticesBytes, vertices);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (glIsVertexArray(idVAO) == GL_TRUE)
		glDeleteVertexArrays(1, &idVAO);

	glGenVertexArrays(1, &idVAO);

	glBindVertexArray(idVAO);

		glBindBuffer(GL_ARRAY_BUFFER, idVBO);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
			glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

    //====Texture====

    GLuint fboId;
    glGenFramebuffers(1, &fboId);

    GLuint fluid;
    glGenTextures(1, &fluid);
    glBindTexture(GL_TEXTURE_3D, fluid);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, 5, 5, 5, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, fluid, 0, 0);
    static const GLenum draw_buffers[] =
    {
        GL_COLOR_ATTACHMENT0
    };

    glDrawBuffers(1, draw_buffers);

    //====================================

    Controller* controller = new Controller(); // a Controller to bind the ESCAPE key to the Window

    int framerate = 60/1000;

    while(!glfwWindowShouldClose(window))
    {

	    int startTime = int(glfwGetTime() * 1000);

        glfwMakeContextCurrent(window);

        controller->check(window);

        glBindFramebuffer(GL_FRAMEBUFFER, fboId);

            glViewport(0, 0, width, height);

            glClearColor(0.2, 0.2, 0.2, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);

            glUseProgram(fluidShader->getProgramID());

                glBindVertexArray(idVAO);

                    for(int layer=0; layer < 5; layer++)
                    {

                        envoyer1I("layer", fluidShader->getProgramID(), layer);

                        glDrawArrays(GL_TRIANGLES, 0, 6);

                    }

                glBindVertexArray(0);

            glUseProgram(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, width, height);

        glClearColor(0.2, 0.2, 0.2, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        glUseProgram(defaultShader->getProgramID());

            glBindVertexArray(idVAO);

                envoyer1I("fluid", defaultShader->getProgramID(), 0);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_3D, fluid);

                glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindVertexArray(0);

        glUseProgram(0);

        glfwSwapBuffers(window);

        glfwPollEvents();

        int endTime = int(glfwGetTime() * 1000);
        int elapsedTime = endTime - startTime;

        if(elapsedTime < framerate)
            this_thread::sleep_for(std::chrono::milliseconds(framerate - elapsedTime));

    };

    return EXIT_SUCCESS;
}
