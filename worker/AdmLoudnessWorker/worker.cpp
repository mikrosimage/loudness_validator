
#include "worker.hpp"

#include <admLoudnessAnalyser/AdmLoudnessAnalyser.hpp>
#include <adm_engine/parser.hpp>

std::map<std::string, float> parseElementGains(const std::string& elementGainsStr, const Logger& logger) {
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
    std::stringstream gainInfo;
    gainInfo << "Gain:                  " << elementGains[elemId] << " (" << gainDbStr << " dB) applied to " << elemId << std::endl;
    logger("debug", gainInfo.str().c_str());

    nextPosition = separatorPos + separator.size();
  }

  return elementGains;
}


int analyse(const std::string& inputFilePath,
            const char* outputPathStr,
            const char* elementIdToRenderStr,
            const char* elementGainsCStr,
            const char** message,
            const Logger& logger,
            const char*** output_paths,
            const bool displayValues = false,
            const bool enableCorrection = false,
            const bool enableLimiter = false) {

  const std::string outputPath = (outputPathStr)? outputPathStr : "";
  const std::string elementIdToRender = (elementIdToRenderStr)? elementIdToRenderStr : "";

  if(Loudness::admanalyser::AdmLoudnessAnalyser::getPathType(inputFilePath) != Loudness::admanalyser::EPathType::file) {
    std::stringstream errorMessage;
    errorMessage << "Specified input file does not exist or is not a regular file: ";
    errorMessage << inputFilePath;
    logger("error", errorMessage.str().c_str());
    set_str_on_ptr(message, errorMessage.str().c_str());
    displayUsage(logger);
    return 1;
  }

  if(outputPath.empty() && enableCorrection) {
    const std::string errorMessage = "An output file must be specified to enable correction.";
    logger("error", errorMessage.c_str());
    set_str_on_ptr(message, errorMessage.c_str());
    displayUsage(logger);
    return 1;
  }
  if(enableLimiter && !enableCorrection) {
    const std::string errorMessage = "Correction must be enabled to enable limiter.";
    logger("error", errorMessage.c_str());
    set_str_on_ptr(message, errorMessage.c_str());
    displayUsage(logger);
    return 1;
  }

  const std::string outputLayout("0+2+0");
  std::map<std::string, float> elementGains;
  if(elementGainsCStr) {
    elementGains = parseElementGains(elementGainsCStr, logger);
  }

  std::stringstream processInfo;
  processInfo << std::endl;
  processInfo << "Input file:            " << inputFilePath << std::endl;
  processInfo << "Output layout:         " << outputLayout << std::endl;
  processInfo << "Output path:           " << (outputPath.empty()? "-" : outputPath) << std::endl;
  processInfo << "ADM element to render: " << (elementIdToRender.empty()? "-" : elementIdToRender) << std::endl;
  processInfo << "Display result:        " << (displayValues? "true" : "false") << std::endl;
  processInfo << "Correction enabled:    " << (enableCorrection? "true" : "false") << std::endl;
  if(enableCorrection) {
    processInfo << "Peak limiter enabled:  " << (enableLimiter? "true" : "false") << std::endl;
  }
  logger("debug", processInfo.str().c_str());

  try {
    Loudness::admanalyser::AdmLoudnessAnalyser analyser(inputFilePath, outputLayout, elementGains, outputPath, elementIdToRender);
    const std::shared_ptr<adm::Document> admDocument = analyser.process(displayValues, enableCorrection, enableLimiter);
    const std::string admDocumentAsString = admengine::getAdmDocumentAsString(admDocument);

    const std::vector<std::string> processOutputPaths = analyser.getOutputPaths();
    output_paths[0] = (const char **)malloc(sizeof(int) * processOutputPaths.size() + 1);
    for(size_t i = 0; i < processOutputPaths.size(); i++) {
      set_str_on_ptr(&output_paths[0][i], processOutputPaths[i].c_str());
    }
    output_paths[0][processOutputPaths.size()] = 0;

    set_str_on_ptr(message, admDocumentAsString.c_str());
  } catch(const std::exception& e) {
    const std::string error(e.what());
    logger("error", error.c_str());
    set_str_on_ptr(message, error.c_str());
    return 1;
  }
  return 0;
}

char* get_name() {
	return (char*)"ADM Loudness Worker";
}

char* get_short_description() {
	return (char*)"Processes BW64/ADM audio file rendering and loudness analyse.";
}

char* get_description() {
	return (char*)"This worker processes ADM rendering and loudness analyse from specified BW64/ADM audio file. \
It can also process loudness correction, with an optional peak limiter.";
}

char* get_version() {
	return (char*)"1.3.8";
}

unsigned int get_parameters_size() {
    return sizeof(worker_parameters) / sizeof(Parameter);
}

void get_parameters(Parameter* parameters) {
    memcpy(parameters, worker_parameters, sizeof(worker_parameters));
}

int process(
    JobHandle job_handle,
    GetParameterValueCallback parameters_value_getter,
    Logger logger,
    const char** message,
    const char*** output_paths
  ) {
    // Print message through the Rust internal logger
    logger("debug", "Start ADM loudness analyser worker process...");

    // Retrieve job parameter value
    char* inputFilePath = parameters_value_getter(job_handle, "input");
    if(inputFilePath == NULL) {
    	displayUsage(logger);
    	return 1;
    }

    char* outputFilePath = parameters_value_getter(job_handle, "output");
    char* elementIdToRender = parameters_value_getter(job_handle, "element_id");
    char* displayValuesStr = parameters_value_getter(job_handle, "display");
    char* enableCorrectionStr = parameters_value_getter(job_handle, "correction");
    char* enableLimiterStr = parameters_value_getter(job_handle, "limiter");
    char* elementGainsCStr = parameters_value_getter(job_handle, "gain_mapping");

    bool displayValues = displayValuesStr && strcmp(displayValuesStr, "true") == 0;
    bool enableCorrection = enableCorrectionStr && strcmp(enableCorrectionStr,"true") == 0;
    bool enableLimiter = enableLimiterStr && strcmp(enableLimiterStr,"true") == 0;

    return analyse(inputFilePath,
                   outputFilePath,
                   elementIdToRender,
                   elementGainsCStr,
                   message,
                   logger,
                   output_paths,
                   displayValues,
                   enableCorrection,
                   enableLimiter);
}
