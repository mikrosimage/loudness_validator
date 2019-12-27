#ifndef ADM_LOUDNESS_WORKER
#define ADM_LOUDNESS_WORKER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <sstream>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Worker parameter type
 */
typedef struct Parameter {
    char* identifier;
    char* label;
    unsigned int kind_size;
    char** kind;
    int required;
} Parameter;

/**
 * Job parameters handler
 */
typedef void* JobHandle;
/**
 * Get job parameter value callback
 */
typedef char* (*GetParameterValueCallback)(JobHandle, const char*);
/**
 * Rust Logger
 */
typedef void* (*Logger)(const char*, const char*);

/**
 * Get worker name
 */
char* get_name();

/**
 * Get worker short description
 */
char* get_short_description();

/**
 * Get worker long description
 */
char* get_description();

/**
 * Get worker version
 */
char* get_version();

/**
 * Get number of worker parameters
 */
unsigned int get_parameters_size();

/**
 * Retrieve worker parameters
 * @param parameters    Output parameters array pointer
 */
void get_parameters(Parameter* parameters);

/**
 * Worker main process function
 * @param job_handle               Job parameters handler
 * @param parameters_value_getter  Get job parameter value callback
 * @param logger                   Rust Logger
 */
int process(
    JobHandle job_handle,
    GetParameterValueCallback parameters_value_getter,
    Logger logger,
    const char** message,
    const char*** output_paths
  );

/**
 * Set the C string to the pointer
 * @param message  Pointer on the const char*
 * @param value    c string with 0 ending
 */
void set_str_on_ptr(const char** message, const char* value) {
    size_t length = strlen(value) + 1;
    *message = (const char *)malloc(length);
    memcpy((void*)*message, value, length);
}

#ifdef __cplusplus
}
#endif

// Worker parameters
char* string_kind[1] = { (char*)"string" };
char* boolean_kind[1] = { (char*)"boolean" };
char* array_of_strings_kind[1] = { (char*)"array_of_strings" };

Parameter worker_parameters[7] = {
    {
        .identifier = (char*)"input",
        .label = (char*)"BW64/ADM audio file path",
        .kind_size = 1,
        .kind = string_kind,
        .required = 1
    },
    {
        .identifier = (char*)"output",
        .label = (char*)"Output file path with updated ADM with loudness values",
        .kind_size = 1,
        .kind = string_kind,
        .required = 0
    },
    {
        .identifier = (char*)"display",
        .label = (char*)"Display loudness analyse values",
        .kind_size = 1,
        .kind = boolean_kind,
        .required = 0
    },
    {
        .identifier = (char*)"correction",
        .label = (char*)"Enable correction (if an output file is specified)",
        .kind_size = 1,
        .kind = boolean_kind,
        .required = 0
    },
    {
        .identifier = (char*)"limiter",
        .label = (char*)"Enable peak limiter (if correction is enabled)",
        .kind_size = 1,
        .kind = boolean_kind,
        .required = 0
    },
    {
        .identifier = (char*)"element_id",
        .label = (char*)"Select the AudioProgramme to be analysed by `element_id`",
        .kind_size = 1,
        .kind = string_kind,
        .required = 0
    },
    {
        .identifier = (char*)"gain_mapping",
        .label = (char*)"Array of `ELEMENT_ID=GAIN` strings, where `GAIN` is the gain value (in dB) to apply to ADM element defined by its `ELEMENT_ID`",
        .kind_size = 1,
        .kind = array_of_strings_kind,
        .required = 0
    }
};

void displayUsage(const Logger& logger) {
    std::stringstream usage;
    usage << std::endl;
    usage << "Parameters:      TYPE                           DESCRIPTION" << std::endl;
    usage << "  input          (string)                       BW64/ADM audio file path" << std::endl;
    usage << "  output         (string) (optional)            Destination file (analyse) / directory (correction)" << std::endl;
    usage << "  display        (boolean) (optional)           Display loudness analyse values" << std::endl;
    usage << "  correction     (boolean) (optional)           Enable correction (if an output directory is specified)" << std::endl;
    usage << "  limiter        (boolean) (optional)           Enable peak limiter (if correction is enabled)" << std::endl;
    usage << "  element_id     (string) (optional)            Select the AudioProgramme to be analysed by `element_id`" << std::endl;
    usage << "  gain_mapping   (array_of_strings) (optional)  Array of `ELEMENT_ID=GAIN` strings, where `GAIN` is the gain value (in dB) to apply to ADM element defined by its `ELEMENT_ID`" << std::endl;
    logger("debug", usage.str().c_str());
}

#endif
