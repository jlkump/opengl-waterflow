#include "shader.hpp"
#include <RootDir.h>

///////////////////////
///	Private Methods ///
///////////////////////

// Code for creating a Shader object are from Tomasz Ga'aj's
// OpenGL CMake tutorial, modified to this project's use-case

bool Shader::GetUniformLocation(const std::string& uniform_name) 
{
    if (uniform_ids_.count(uniform_name)) 
    {
        // In this case, the uniform is present, so just use it.
        // No need to call gl functions.
        return true;
    }

    GLuint uniform_location = glGetUniformLocation(program_id_, uniform_name);
    if (uniform_location != -1)
    {
        uniform_ids_[uniform_name] = uniform_location;
        return true;
    }
    else
    {
        fprintf(stderr, "Error! Can't find uniform %s\n", uniform_name.c_str());
        return false;
    }
}

void Shader::LinkProgram()
{
    glLinkProgram(program_id_);

    GLint status;
    glGetProgramiv(program_id, GL_LINK_STATUS, &status);

    // Error checking
    // If is_linked_ is false, calls to SetActive() will do nothing.
    is_linked_ = !(status == GL_FALSE)
    if (!is_linked_)
    {
        fprintf(stderr, "Failed to link shader program!\n");

        GLint log_len;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_len);

        if (log_len > 0)
        {
            char* log = (char*)malloc(log_len);
            GLsizei written;
            glGetProgramInfoLog(program_id, log_len, &written, log);

            fprintf(stderr, "Program log: \n%s", log);
            free(log);
        }
    }
}

std::string Shader::LoadFile(const std::string& filename)
{
    if (filename.empty())
    {
        return "";
    }

    std::string filetext;
    std::string line;

    std::ifstream inFile(ROOT_DIR "resources/shaders/" + filename);

    if (!inFile)
    {
        fprintf(stderr, "Could not open file %s\n", filename.c_str());
        inFile.close();

        return "";
    }
    else
    {
        while (getline(inFile, line))
        {
            filetext.append(line + "\n");
        }

        inFile.close();

        return filetext;
    }
}

//////////////////////
///	Public Methods ///
//////////////////////

Shader::Shader(const std::string& vertex_shader_file_name, const std::string& fragment_shader_file_name)
{
    const std::string shader_codes[2] = { LoadFile(vertex_shader_file_name),
                                         LoadFile(fragment_shader_file_name) };
    const std::string filenames[2] = { vertex_shader_file_name,
                                       fragment_shader_file_name };
    program_id_ = glCreateProgram();

    if (program_id == 0)
    {
        fprintf(stderr, "Error while creating OpenGL program object.\n");
        printf("Press any key to continue...\n");
        getchar();
        return;
    }

    // Process each shader's text, compile it, and connect it to
    // a unified program to be run on the GPU.
    // Lastly, link the program once finished with the shaders
    for (int i = 0; i < sizeof(shader_codes) / sizeof(std::string); ++i)
    {
        if (shader_codes[i].empty())
        {
            continue;
        }

        GLuint shader_type = 0;
        if (i == 0)
            shader_type = GL_VERTEX_SHADER;
        else
        if (i == 1)
            shader_type = GL_FRAGMENT_SHADER;

        GLuint shader_obj = glCreateShader(shader_type);

        if (shader_obj == 0)
        {
            fprintf(stderr, "Error while creating %s.\n", filenames[i].c_str());
            continue;
        }

        const char* shader_code[1] = { shader_codes[i].c_str() };

        glShaderSource(shader_obj, 1, shader_code, nullptr);
        glCompileShader(shader_obj);

        GLint result;
        glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &result);

        if (result == GL_FALSE)
        {
            fprintf(stderr, "%s compilation failed!\n", filenames[i].c_str());

            GLint log_len;
            glGetShaderiv(shader_obj, GL_INFO_LOG_LENGTH, &log_len);

            if (log_len > 0)
            {
                char* log = (char*)malloc(log_len);

                GLsizei written;
                glGetShaderInfoLog(shader_obj, log_len, &written, log);

                fprintf(stderr, "Shader log: \n%s", log);
                free(log);
            }

            continue;
        }

        glAttachShader(program_id_, shader_obj);
        glDeleteShader(shader_obj);
    }

    LinkProgram();
}

bool Shader::SetActive()
{
    bool valid_program = program_id_ != 0 && is_linked_;
    if (valid_program)
    {
        glUseProgram(program_id_);
    }
    return valid_program;
}

bool Shader::SetUniformTexture(const std::string& uniform_name, Texture& texture, GLint texture_unit)
{
    bool success = GetUniformLocation(uniform_name);
    if (success) 
    {
        texture.ActiveBind();
        glUniform1i(uniform_ids_[uniform_name], texture_unit);
    }
}
