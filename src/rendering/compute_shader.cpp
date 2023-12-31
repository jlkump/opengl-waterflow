#include "compute_shader.hpp"

///////////////////////
///	Private Methods ///
///////////////////////

// Code for creating a Shader object are from Tomasz Ga'aj's
// OpenGL CMake tutorial, modified to this project's use-case
// Specifically, this code is modified for a compute shader,
// which must be run as its own program on the GPU.

//////////////////////
///	Public Methods ///
//////////////////////

ComputeShader::ComputeShader(const std::string& compute_shader_file_name, const glm::ivec3& work_group_dim) 
    : Shader(), work_group_dim_(work_group_dim)
{
	const std::string shader_code = LoadFile(compute_shader_file_name);

    printf("Work Group Sizes: %d, %d, %d\n", work_group_dim_.x, work_group_dim_.y, work_group_dim_.z);

	program_id_ = glCreateProgram();
    if (program_id_ == 0)
    {
        fprintf(stderr, "Error while creating OpenGL program object.\n");
        printf("Press any key to continue...\n");
        getchar();
        return;
    }

    GLuint comp_shader_obj = glCreateShader(GL_COMPUTE_SHADER);
    if (shader_code.empty() || comp_shader_obj == 0) 
    {
        fprintf(stderr, "Error constructing compute shader.\n");
        return;
    }

    const char* code[] = { shader_code.c_str() };

    glShaderSource(comp_shader_obj, 1, code, nullptr);
    glCompileShader(comp_shader_obj);

    GLint result;
    glGetShaderiv(comp_shader_obj, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE)
    {
        fprintf(stderr, "Compute shader compilation failed!\n");

        GLint log_len;
        glGetShaderiv(comp_shader_obj, GL_INFO_LOG_LENGTH, &log_len);

        if (log_len > 0)
        {
            char* log = (char*)malloc(log_len);

            GLsizei written;
            glGetShaderInfoLog(comp_shader_obj, log_len, &written, log);

            fprintf(stderr, "Shader log: \n%s", log);
            free(log);
        }
    }

    glAttachShader(program_id_, comp_shader_obj);
    glDeleteShader(comp_shader_obj);
    LinkProgram();
}

void ComputeShader::Barrier()
{
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void ComputeShader::Dispatch()
{
    glDispatchCompute(work_group_dim_.x, work_group_dim_.y, work_group_dim_.z);
}

GLuint ComputeShader::GenerateAndBindSSBO(const void* data, unsigned int data_size, GLuint target_binding)
{
    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, data_size, data, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, target_binding, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0); // Unbind when done
    return ssbo;
}

void ComputeShader::UpdateSSBO(GLuint ssbo, const void* data, unsigned int data_size, unsigned int data_offset)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset, data_size, data);
    // Might need this, not sure?
    // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, target_binding, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind when done
}