#ifndef SHADER_HPP
#define SHADER_HPP

#include "util.h"

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <fstream>

using std::cout;
using std::endl;
using std::ifstream;
using std::map;
using std::string;
using std::stringstream;

class Shader
{
private:
    GLuint id;         // The unique ID / handle for the shader
    string typeString; // String representation of the shader type (i.e. "Vertex" or such)
    string source;     // The shader source code (i.e. the GLSL code itself)

public:
    // Constructor
    Shader(const GLenum &type)
    {
        // Get the type of the shader
        if (type == GL_VERTEX_SHADER)
        {
            typeString = "Vertex";
        }
        else if (type == GL_FRAGMENT_SHADER)
        {
            typeString = "Fragment";
        }
        else
        {
            typeString = "Geometry";
        }

        // Create the vertex shader id / handle
        // Note: If you segfault here you probably don't have a valid rendering context.
        id = glCreateShader(type);
    }

    GLuint getId()
    {
        return id;
    }

    string getSource()
    {
        return source;
    }

    // Method to load the shader contents from a file
    void loadFromString(const string &sourceString)
    {
        // Keep hold of a copy of the source
        source = sourceString;

        // Get the source as a pointer to an array of characters
        const char *sourceChars = source.c_str();

        // Associate the source with the shader id
        glShaderSource(id, 1, &sourceChars, NULL);
    }

    // Method to load the shader contents from a string
    void loadFromFile(const string &filename)
    {
        ifstream file;

        file.open( filename.c_str() );

        if (!file.good() )
        {
            cout << "Failed to open file: " << filename << endl;
            exit(-1);
        }

        // Create a string stream
        stringstream stream;

        // Dump the contents of the file into it
        stream << file.rdbuf();

        // Close the file
        file.close();

        // Convert the StringStream into a string
        source = stream.str();

        // Get the source string as a pointer to an array of characters
        const char *sourceChars = source.c_str();

        // Associate the source with the shader id
        glShaderSource(id, 1, &sourceChars, NULL);
    }


    // Method to compile a shader and display any problems if compilation fails
    void compile()
    {
        // Compile the shader
        glCompileShader(id);

        // Check the compilation status and report any errors
        GLint shaderStatus;
        glGetShaderiv(id, GL_COMPILE_STATUS, &shaderStatus);

        // If the shader failed to compile, display the info log and quit out
        if (shaderStatus == GL_FALSE)
        {
            GLint infoLogLength;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);

            GLchar *strInfoLog = new GLchar[infoLogLength + 1];
            glGetShaderInfoLog(id, infoLogLength, NULL, strInfoLog);

            cout << typeString << " Shader Compilation\t\tFailed: " << strInfoLog << endl;
            delete[] strInfoLog;
            exit(-1);
        }
        else
        {
            cout << typeString << " Shader Compilation\t\tOK" << endl;
        }
    }
};


class ShaderProgram
{
private:
    GLuint programId;   // The unique ID / handle for the shader program
    GLuint shaderCount; // How many shaders are attached to the shader program

    // Map of attributes and their binding locations
    map<string, int> attributeLocList;

    // Map of uniforms and their binding locations
    map<string, int> uniformLocList;

public:
    // Constructor
    ShaderProgram()
    {
        // Generate a unique Id / handle for the shader program
        // Note: We MUST have a valid rendering context before generating
        // the programId or it causes a segfault!
        programId = glCreateProgram();

        // Initially, we have zero shaders attached to the program
        shaderCount = 0;
    }


    // Destructor
    ~ShaderProgram()
    {
        // Delete the shader program from the graphics card memory to
        // free all the resources it's been using
        glDeleteProgram(programId);
    }


    // Method to attach a shader to the shader program
    void attachShader(Shader shader)
    {
        // Attach the shader to the program
        // Note: We identify the shader by its unique Id value
        glAttachShader( programId, shader.getId() );

        // Increment the number of shaders we have associated with the program
        shaderCount++;
    }


    // Method to link the shader program and display the link status
    void linkProgram()
    {
        // If we have at least two shaders (like a vertex shader and a fragment shader)...
        if (shaderCount >= 2)
        {
            // Perform the linking process
            glLinkProgram(programId);

            // Check the status
            GLint linkStatus;
            glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
            if (linkStatus == GL_FALSE)
            {
                cout << RED << "Shader Program Linking\t\t\tFailed" << RESET << endl;
                exit(-1);
            }
            else
            {
                cout << "Shader Program Linking\t\t\tOK" << endl << endl;
            }
        }
        else
        {
            cout << "Can't link shaders - you need at least 2, but attached shader count is only: " << shaderCount << endl;
            exit(-1);
        }
    }


    // Method to enable the shader program
    void use()
    {
        glUseProgram(programId);
    }


    // Method to disable the shader program
    void disable()
    {
        glUseProgram(0);
    }


    // Returns the bound location of a named attribute
    GLuint attribute(const string &attribute)
    {
        // You could do this function with the single line:
        //
        //      return attributeLocList[attribute];
        //
        // BUT, if you did, and you asked it for a named attribute
        // which didn't exist, like, attributeLocList["ThisAttribDoesn'tExist!"]
        // then the method would return an invalid value which will likely cause
        // the program to segfault. So we're making sure the attribute asked
        // for exists, and if it doesn't we can alert the user and stop rather than bombing out later.

        // Create an iterator to look through our attribute map and try to find the named attribute
        map<string, int>::iterator it = attributeLocList.find(attribute);

        // Found it? Great -return the bound location! Didn't find it? Alert user and halt.
        if ( it != attributeLocList.end() )
        {
            return attributeLocList[attribute];
        }
        else
        {
            cout << "Could not find attribute in shader program: " << attribute << endl;
            exit(-1);
        }
    }


    // Method to returns the bound location of a named uniform
    GLuint uniform(const string &uniform)
    {
        // Note: You could do this method with the single line:
        //
        //      return uniformLocList[uniform];
        //
        // But we're not doing that. Explanation in the attribute() method above.

        // Create an iterator to look through our uniform map and try to find the named uniform
        static map<string, int>::iterator it = uniformLocList.find(uniform);

        // Found it? Great - pass it back! Didn't find it? Alert user and halt.
        if ( it != uniformLocList.end() )
        {
            return uniformLocList[uniform];
        }
        else
        {
            cout << "Could not find uniform in shader program: " << uniform << endl;
            exit(-1);
        }
    }


    // Method to add an attrbute to the shader and return the bound location
    int addAttribute(const string &attributeName)
    {
        attributeLocList[attributeName] = glGetAttribLocation( programId, attributeName.c_str() );

        // Check to ensure that the shader contains an attribute with this name
        if (attributeLocList[attributeName] == -1)
        {
            cout << "Could not add attribute: " << attributeName << " - location returned -1!" << endl;
            exit(-1);
        }
        else
        {
            cout << "Attribute location " << BLUE << attributeLocList[attributeName] << RESET <<
                 " bound to " << BLUE << attributeName << RESET << endl;
        }

        return attributeLocList[attributeName];
    }


    // Method to add a uniform to the shader and return the bound location
    int addUniform(const string &uniformName)
    {
        uniformLocList[uniformName] = glGetUniformLocation( programId, uniformName.c_str() );

        // Check to ensure that the shader contains a uniform with this name
        if (uniformLocList[uniformName] == -1)
        {
            cout << "Could not add uniform: " << uniformName << " - location returned -1!" << endl;
            exit(-1);
        }
        else
        {
            cout << "Uniform location " << BLUE << uniformLocList[uniformName] << RESET <<
                 " bound to " << BLUE << uniformName << RESET << endl;
        }

        return uniformLocList[uniformName];
    }

};

#endif
