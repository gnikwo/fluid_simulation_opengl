#include "Shader.h"

#include <iostream>
#include <fstream>

using namespace std;
using namespace glm;

Shader::Shader(std::string shaderName): m__programID(),
										m__vertexID(),
										m__tessControlID(),
										m__tessEvaluationID(),
										m__geometryID(),
										m__fragmentID(),
                                        m__useVertex(true),
                                        m__useTessControl(false),
                                        m__useTessEval(false),
                                        m__useGeometry(false),
                                        m__useFragment(true),
										m__vertexShaderFile("Shaders/" + shaderName + "/" + shaderName + ".vert"),
										m__tessControlShaderFile("Shaders/" + shaderName + "/" + shaderName + ".tese"),
										m__tessEvaluationShaderFile("Shaders/" + shaderName + "/" + shaderName + ".tesc"),
										m__geometryShaderFile("Shaders/" + shaderName + "/" + shaderName + ".geom"),
										m__fragmentShaderFile("Shaders/" + shaderName + "/" + shaderName + ".frag")
{

	cout << this << " [Shader] constructor" << endl;

}


Shader::~Shader()
{



}


bool Shader::load()
{

	cout << this << " [Shader] load" << endl;

	// Destruction d'un �ventuel ancien Shader
    if(m__useVertex)
	    if (glIsShader(m__vertexID) == GL_TRUE)
		    glDeleteShader(m__vertexID);

    if(m__useTessControl)
        if (glIsShader(m__tessControlID) == GL_TRUE)
            glDeleteShader(m__tessControlID);

    if(m__useTessEval)
        if (glIsShader(m__tessEvaluationID) == GL_TRUE)
            glDeleteShader(m__tessEvaluationID);

    if(m__useGeometry)
        if (glIsShader(m__geometryID) == GL_TRUE)
            glDeleteShader(m__geometryID);

    if(m__useFragment)
        if (glIsShader(m__fragmentID) == GL_TRUE)
            glDeleteShader(m__fragmentID);



	if (glIsProgram(m__programID) == GL_TRUE)
		glDeleteProgram(m__programID);


    if(m__useVertex)
	    cout << this << " [Shader] load() vertex : " << m__vertexShaderFile << endl;

    if(m__useTessControl)
	    cout << this << " [Shader] load() tessControl : " << m__tessControlShaderFile << endl;

    if(m__useTessEval)
	    cout << this << " [Shader] load() tessEval : " << m__tessEvaluationShaderFile << endl;

    if(m__useGeometry)
	    cout << this << " [Shader] load() geometry : " << m__geometryShaderFile << endl;

    if(m__useFragment)
	    cout << this << " [Shader] load() fragment : " << m__fragmentShaderFile << endl;


	// Compilation des shaders
    if(m__useVertex)
	    if (!compilerShader(m__vertexID, GL_VERTEX_SHADER, m__vertexShaderFile))
		    return false;

    if(m__useTessControl)
	    if (!compilerShader(m__tessControlID, GL_TESS_CONTROL_SHADER, m__tessControlShaderFile))
		    return false;

    if(m__useTessEval)
	    if (!compilerShader(m__tessEvaluationID, GL_TESS_EVALUATION_SHADER, m__tessEvaluationShaderFile))
		    return false;

    if(m__useGeometry)
	    if (!compilerShader(m__geometryID, GL_GEOMETRY_SHADER, m__geometryShaderFile))
		    return false;

    if(m__useFragment)
	    if (!compilerShader(m__fragmentID, GL_FRAGMENT_SHADER, m__fragmentShaderFile))
		    return false;


	// Cr�ation du programme
	m__programID = glCreateProgram();


	// Association des shaders
    if(m__useVertex)
	    glAttachShader(m__programID, m__vertexID);

    if(m__useTessControl)
	    glAttachShader(m__programID, m__tessControlID);

    if(m__useTessEval)
	    glAttachShader(m__programID, m__tessEvaluationID);

    if(m__useGeometry)
	    glAttachShader(m__programID, m__geometryID);

    if(m__useFragment)
	    glAttachShader(m__programID, m__fragmentID);


	// Verrouillage des entr�es shader
	glBindAttribLocation(m__programID, 0, "in_Vertex");
	glBindAttribLocation(m__programID, 1, "in_TexCoord0");


	// Linkage du programme
	glLinkProgram(m__programID);


	// V�rification du linkage
	GLint erreurLink(0);
	glGetProgramiv(m__programID, GL_LINK_STATUS, &erreurLink);


	// S'il y a eu une erreur
	if (erreurLink != GL_TRUE)
	{

		// R�cup�ration de la taille de l'erreur
		GLint tailleErreur(0);
		glGetProgramiv(m__programID, GL_INFO_LOG_LENGTH, &tailleErreur);


		// Allocation de m�moire
		char *erreur = new char[tailleErreur + 1];


		// R�cup�ration de l'erreur
		glGetShaderInfoLog(m__programID, tailleErreur, &tailleErreur, erreur);
		erreur[tailleErreur] = '\0';


		// Affichage de l'erreur
		cout << erreur << endl;


		// Lib�ration de la m�moire et retour du bool�en false
		delete[] erreur;
		glDeleteProgram(m__programID);

		return false;

	} else { // Sinon c'est que tout s'est bien pass�

		return true;

	}

}


bool Shader::compilerShader(GLuint &shader, GLenum type, string const &fichierSource)
{

	// Cr�ation du shader
	shader = glCreateShader(type);


	// V�rification du shader
	if (shader == 0)
	{
		std::cout << "Erreur, le type de shader (" << type << ") n'existe pas" << std::endl;
		return false;
	}


	// Flux de lecture
	ifstream fichier(fichierSource.c_str());


	// Test d'ouverture
	if (!fichier)
	{
		std::cout << "Erreur le fichier " << fichierSource << " est introuvable" << std::endl;
		glDeleteShader(shader);

		return false;
	}


	// Strings permettant de lire le code source
	std::string ligne;
	std::string codeSource;


	// Lecture
	while (getline(fichier, ligne))
		codeSource += ligne + '\n';


	// Fermeture du fichier
	fichier.close();


	// R�cup�ration de la chaine C du code source
	const GLchar* chaineCodeSource = codeSource.c_str();


	// Envoi du code source au shader
	glShaderSource(shader, 1, &chaineCodeSource, 0);


	// Compilation du shader
	glCompileShader(shader);


	// V�rification de la compilation
	GLint erreurCompilation(0);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &erreurCompilation);


	// S'il y a eu une erreur
	if (erreurCompilation != GL_TRUE)
	{

		// R�cup�ration de la taille de l'erreur
		GLint tailleErreur(0);
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &tailleErreur);


		// Allocation de m�moire
		char *erreur = new char[tailleErreur + 1];


		// R�cup�ration de l'erreur
		glGetShaderInfoLog(shader, tailleErreur, &tailleErreur, erreur);
		erreur[tailleErreur] = '\0';


		// Affichage de l'erreur
		std::cout << erreur << std::endl;


		// Lib�ration de la m�moire et retour du bool�en false
		delete[] erreur;
		glDeleteShader(shader);

		return false;
	}


	// Sinon c'est que tout s'est bien pass�
	else
		return true;

}


void Shader::envoyerMat4(string nom, glm::mat4 matrice)
{

	// Localisation de la matrice
	int localisation = glGetUniformLocation(m__programID, nom.c_str());

	// Envoi des valeurs
	glUniformMatrix4fv(localisation, 1, GL_FALSE, glm::value_ptr(matrice));

}


void Shader::envoyerMat3(string nom, glm::mat3 matrice)
{

	// Localisation de la matrice
	int localisation = glGetUniformLocation(m__programID, nom.c_str());

	// Envoi des valeurs
	glUniformMatrix3fv(localisation, 1, GL_FALSE, glm::value_ptr(matrice));

}


void Shader::envoyerVec4(std::string nom, glm::vec4 vecteur)
{

	// Localisation de la matrice
	int localisation = glGetUniformLocation(m__programID, nom.c_str());

	// Envoi des valeurs
	glUniform4f(localisation, vecteur.x, vecteur.y, vecteur.z, vecteur.w);

}


void Shader::envoyerVec3(std::string nom, glm::vec3 vecteur)
{

	// Localisation de la matrice
	int localisation = glGetUniformLocation(m__programID, nom.c_str());

	// Envoi des valeurs
	glUniform3f(localisation, vecteur.x, vecteur.y, vecteur.z);

}


void Shader::envoyer1I(std::string nom, GLuint id)
{

	// Localisation de la matrice
	int localisation = glGetUniformLocation(m__programID, nom.c_str());

	// Envoi des valeurs
	glUniform1i(localisation, id);

}


void Shader::envoyerFloat(std::string nom, float val)
{

	// Localisation de la matrice
	int localisation = glGetUniformLocation(m__programID, nom.c_str());

	// Envoi des valeurs
	glUniform1f(localisation, val);

}


GLuint Shader::getProgramID()
{

	return m__programID;

}


//=====Shaders=====
string Shader::getVertexShaderFile()
{

	return m__vertexShaderFile;

}


void Shader::setVertexShaderFile(string val)
{

	m__vertexShaderFile = val;

}


string Shader::getTessControlShaderFile()
{

	return m__tessControlShaderFile;

}


void Shader::setTessControlShaderFile(string val)
{

	m__tessControlShaderFile = val;

}


string Shader::getTessEvaluationShaderFile()
{

	return m__tessEvaluationShaderFile;

}


void Shader::setTessEvaluationShaderFile(string val)
{

	m__tessEvaluationShaderFile = val;

}



string Shader::getGeometryShaderFile()
{

	return m__geometryShaderFile;

}


void Shader::setGeometryShaderFile(string val)
{

	m__geometryShaderFile = val;

}



string Shader::getFragmentShaderFile()
{

	return m__fragmentShaderFile;

}


void Shader::setFragmentShaderFile(string val)
{

	m__fragmentShaderFile = val;

}

