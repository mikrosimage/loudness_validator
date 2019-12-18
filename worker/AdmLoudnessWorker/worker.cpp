#ifndef ADM_LOUDNESS_WORKER
#define ADM_LOUDNESS_WORKER

#include <stdio.h>
#include <string.h>
#include <iostream>

#include <admLoudnessAnalyser/AdmLoudnessAnalyser.hpp>

std::map<std::string, float> parseElementGains(const std::string& elementGainsStr) {
  std::map<std::string, float> elementGains;
  const size_t arrayInPos = elementGainsStr.find("[");
  if(arrayInPos ==  std::string::npos) {
    throw std::runtime_error("Invalid gains mapping string format: missing '[' as first character.");
  }
  const size_t arrayOutPos = elementGainsStr.find("]");
  if(arrayOutPos ==  std::string::npos) {
    throw std::runtime_error("Invalid gains mapping string format:  missing ']' as last character.");
  }
  const std::string gainPairsStr = elementGainsStr.substr(arrayInPos + 1, arrayOutPos - 1);

  size_t nextPosition = 0;
  std::string separator = ", ";
  while(nextPosition < gainPairsStr.size()){
    size_t separatorPos = gainPairsStr.find(separator, nextPosition);

    if(separatorPos == std::string::npos){
      separatorPos = gainPairsStr.size();
    }

    const std::string gainPair = gainPairsStr.substr(nextPosition + 1, separatorPos - (nextPosition + 2)); // "element_id=gain"
    const size_t equalPos = gainPair.find("=");
    const std::string elemId = gainPair.substr(0, equalPos); // "element_id"
    const std::string gainDbStr = gainPair.substr(equalPos + 1, gainPair.size()); // "gain"
    elementGains[elemId] = pow(10.0, std::atof(gainDbStr.c_str()) / 20.0);
    std::cout << "Gain:                  " << elementGains[elemId] << " (" << gainDbStr << " dB) applied to " << elemId << std::endl;

    nextPosition = separatorPos + separator.size();
  }

  return elementGains;
}

void displayUsage() {
  std::cout << "Parameters:      TYPE                           DESCRIPTION" << std::endl;
  std::cout << std::endl;
  std::cout << "  input          (string)                       BW64/ADM audio file path" << std::endl;
  std::cout << "  output         (string) (optional)            Destination file (analyse) / directory (correction)" << std::endl;
  std::cout << "  display        (boolean) (optional)           Display loudness analyse values" << std::endl;
  std::cout << "  correction     (boolean) (optional)           Enable correction (if an output directory is specified)" << std::endl;
  std::cout << "  limiter        (boolean) (optional)           Enable peak limiter (if correction is enabled)" << std::endl;
  std::cout << "  element_id     (string) (optional)            Select the AudioProgramme to be analysed by `element_id`" << std::endl;
  std::cout << "  gain_mapping   (array_of_strings) (optional)  Array of `ELEMENT_ID=GAIN` strings, where `GAIN` is the gain value (in dB) to apply to ADM element defined by its `ELEMENT_ID`" << std::endl;
  std::cout << std::endl;
}

int analyse(const std::string& inputFilePath,
            const char* outputPathStr,
            const char* elementIdToRenderStr,
            const char* elementGainsCStr,
            const bool displayValues = false,
            const bool enableCorrection = false,
            const bool enableLimiter = false) {

  const std::string outputPath = (outputPathStr)? outputPathStr : "";
  const std::string elementIdToRender = (elementIdToRenderStr)? elementIdToRenderStr : "";

  if(outputPath.empty() && enableCorrection) {
    const std::string errorMessage = "An output file must be specified to enable correction.";
    std::cerr << "Error: " << errorMessage << std::endl;
    displayUsage();
    return 1;
  }
  if(enableLimiter && !enableCorrection) {
    const std::string errorMessage = "Correction must be enabled to enable limiter.";
    std::cerr << "Error: " << errorMessage << std::endl;
    displayUsage();
    return 1;
  }

  const std::string outputLayout("0+2+0");
  std::map<std::string, float> elementGains;
  if(elementGainsCStr) {
    elementGains = parseElementGains(elementGainsCStr);
  }

  std::cout << "Input file:            " << inputFilePath << std::endl;
  std::cout << "Output layout:         " << outputLayout << std::endl;
  std::cout << "Output path:           " << (outputPath.empty()? "-" : outputPath) << std::endl;
  std::cout << "ADM element to render: " << (elementIdToRender.empty()? "-" : elementIdToRender) << std::endl;
  std::cout << "Display result:        " << (displayValues? "true" : "false") << std::endl;
  std::cout << "Correction enabled:    " << (enableCorrection? "true" : "false") << std::endl;
  if(enableCorrection) {
    std::cout << "Peak limiter enabled:  " << (enableLimiter? "true" : "false") << std::endl;
  }

  try {
      Loudness::admanalyser::AdmLoudnessAnalyser analyser(inputFilePath, outputLayout, outputFilePath, elementIdToRender);
      analyser.process(displayValues, enableCorrection, enableLimiter);
  } catch(std::exception e) {
      std::cerr << "Error: " << e.what() << std::endl;
      return 1;
  }
  return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get worker name
 */
char* get_name() {
	return (char*)"ADM Loudness Worker";
}

/**
 * Get worker short description
 */
char* get_short_description() {
	return (char*)"Processes BW64/ADM audio file rendering and loudness analyse.";
}

/**
 * Get worker long description
 */
char* get_description() {
	return (char*)"This worker processes ADM rendering and loudness analyse from specified BW64/ADM audio file. \
It can also process loudness correction, with an optional peak limiter.";
}

/**
 * Get worker version
 */
char* get_version() {
	return (char*)"1.3.8";
}

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

// Worker parameters
char* string_kind[1] = { (char*)"string" };
char* boolean_kind[1] = { (char*)"boolean" };

Parameter worker_parameters[6] = {
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
    }
};

/**
 * Get number of worker parameters
 */
unsigned int get_parameters_size() {
    return sizeof(worker_parameters) / sizeof(Parameter);
}

/**
 * Retrieve worker parameters
 * @param parameters    Output parameters array pointer
 */
void get_parameters(Parameter* parameters) {
    memcpy(parameters, worker_parameters, sizeof(worker_parameters));
}

typedef void* JobParameters;
typedef char* (*GetParameterValueCallback)(JobParameters, const char*);
typedef void* (*Logger)(const char*);
typedef int* (*CheckError)();

/**
 * Worker main process function
 * @param job                      Job parameters handler
 * @param parametersValueGetter    Get job parameter value callback
 * @param checkError               Check error callback
 * @param logger                   Rust logger callback
 */
int process(JobParameters job, GetParameterValueCallback parametersValueGetter, CheckError checkError, Logger logger) {
    // Print message through the Rust internal logger
    logger("Start ADM loudness analyser worker process...");

    // Retrieve job parameter value
    char* inputFilePath = parametersValueGetter(job, "input");
    if(inputFilePath == NULL) {
    	checkError();
    	displayUsage();
    	return 1;
    }

    char* outputFilePath = parametersValueGetter(job, "output");
    char* elementIdToRender = parametersValueGetter(job, "element_id");
    char* displayValuesStr = parametersValueGetter(job, "display");
    char* enableCorrectionStr = parametersValueGetter(job, "correction");
    char* enableLimiterStr = parametersValueGetter(job, "limiter");
    char* elementGainsCStr = parametersValueGetter(job, "gain_mapping");

    bool displayValues = displayValuesStr && strcmp(displayValuesStr, "true") == 0;
    bool enableCorrection = enableCorrectionStr && strcmp(enableCorrectionStr,"true") == 0;
    bool enableLimiter = enableLimiterStr && strcmp(enableLimiterStr,"true") == 0;

    return analyse(inputFilePath, outputFilePath, elementIdToRender, elementGainsCStr, displayValues, enableCorrection, enableLimiter);
}

#ifdef __cplusplus
}
#endif

#endif
