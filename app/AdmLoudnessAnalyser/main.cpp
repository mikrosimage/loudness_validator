#include <iostream>

#include <adm_engine/adm_helper.hpp>
#include <adm_engine/renderer.hpp>

#include <loudnessAnalyser/LoudnessAnalyser.hpp>

adm::LoudnessMetadata analyseLoudness(const std::unique_ptr<bw64::Bw64Reader>& bw64File, const admengine::Renderer& renderer, const bool displayResult) {
    // Analyse loudness according to EBU R-128
    Loudness::analyser::LoudnessLevels level = Loudness::analyser::LoudnessLevels::Loudness_EBU_R128();
    Loudness::analyser::LoudnessAnalyser analyser(level);

    // init
    const size_t nbChannelsToAnalyse = renderer.getNbOutputChannels();
    const size_t nbInputChannels = bw64File->channels();
    const size_t sampleRate = bw64File->sampleRate();
    analyser.initAndStart(nbChannelsToAnalyse, sampleRate);

    // Create interlaced buffer
    float readFileBuffer[admengine::BLOCK_SIZE * nbInputChannels] = {0.0,}; // nb of samples * nb input channels

    while (!bw64File->eof()) {
        // Read a data block
        float admRenderBuffer[admengine::BLOCK_SIZE * nbChannelsToAnalyse] = {0.0,}; // nb of samples * nb output channels
        const size_t nbFrames = bw64File->read(readFileBuffer, admengine::BLOCK_SIZE);
        const size_t renderedSamples = renderer.processBlock(nbFrames, readFileBuffer, admRenderBuffer);


        // Create planar buffer of float data
        float** loudnessInputBuffer = new float*[nbChannelsToAnalyse];
        for (size_t c = 0; c < nbChannelsToAnalyse; ++c) {
            loudnessInputBuffer[c] = new float[nbFrames];
        }

        size_t channel_id = 0;
        size_t frame_id = 0;
        for (size_t s = 0; s < renderedSamples; ++s) {
            channel_id = s % nbChannelsToAnalyse;
            frame_id = s / nbChannelsToAnalyse;
            loudnessInputBuffer[channel_id][frame_id] = admRenderBuffer[s];
        }
        analyser.processSamples(loudnessInputBuffer, nbFrames);

        // free audio buffer
        delete[] loudnessInputBuffer;
    }
    bw64File->seek(0);

    if(displayResult) {
        analyser.printPloudValues();
    }

    adm::LoudnessMetadata loudnessMetadata;
    loudnessMetadata.set(adm::LoudnessMethod("ITU-R BS.1770"));
    loudnessMetadata.set(adm::LoudnessRecType("EBU R128"));
    loudnessMetadata.set(adm::IntegratedLoudness(analyser.getIntegratedLoudness()));
    loudnessMetadata.set(adm::MaxTruePeak(analyser.getTruePeakValue()));
    loudnessMetadata.set(adm::MaxMomentary(analyser.getMomentaryLoudness()));

    if(analyser.isShortProgram()) {
        loudnessMetadata.set(adm::LoudnessRange(analyser.getIntegratedRange()));
        loudnessMetadata.set(adm::MaxShortTerm(analyser.getMaxShortTermLoudness()));
    }
    return loudnessMetadata;
}

void writetoFile(const std::unique_ptr<bw64::Bw64Reader>& inputFile,
                 const std::shared_ptr<adm::Document>& document,
                 const std::shared_ptr<bw64::ChnaChunk>& chnaChunk,
                 const std::string& outputFilePath) {
    std::shared_ptr<bw64::AxmlChunk> axml = admengine::createAxmlChunk(document);
    auto outputFile = bw64::writeFile(outputFilePath, inputFile->channels(), inputFile->sampleRate(), inputFile->bitDepth(), chnaChunk, axml);
    std::vector<float> buffer(admengine::BLOCK_SIZE * inputFile->channels());
    inputFile->seek(0);
    while (!inputFile->eof()) {
        auto readFrames = inputFile->read(&buffer[0], admengine::BLOCK_SIZE);
        outputFile->write(&buffer[0], readFrames);
    }
    inputFile->seek(0);
}


int main(int argc, char const *argv[])
{
    // std::cout << "Welcome to ADM Loundess Analyser!" << std::endl;
    if (argc != 2 && argc != 3) {
        std::cout << "Usage: " << argv[0] << " INPUT [OUTPUT]" << std::endl;
        std::cout << "   with INPUT   BW64/ADM audio input file" << std::endl;
        std::cout << "   with OUTPUT  BW64/ADM audio output file (optional)" << std::endl;
        exit(1);
    }

    auto bw64File = bw64::readFile(argv[1]);
    const std::string outputDirectory(".");
    const std::string outputLayout("0+2+0");
    admengine::Renderer renderer(bw64File, outputLayout);

    auto admDocument = renderer.getDocument();
    auto chnaChunk = renderer.getAdmChnaChunk();
    auto audioProgrammes = renderer.getDocumentAudioProgrammes();
    if(audioProgrammes.size()) {
        for(auto audioProgramme : audioProgrammes) {
            // std::cout << "### Render audio programme: " << admengine::toString(audioProgramme) << std::endl;
            renderer.initAudioProgrammeRendering(audioProgramme);
            const adm::LoudnessMetadata loudnessMetadata = analyseLoudness(bw64File, renderer, argc == 2);

            // Update audio programme
            admDocument->remove(audioProgramme);
            audioProgramme->set(loudnessMetadata);
            admDocument->add(audioProgramme);
        }
    } else {
        std::cerr << "No ADM audio programme found." << std::endl;
        exit(1);
    }

    if(argc == 3) {
        const std::string outputFilePath(argv[2]);
        // std::cout << "### Write to file: " << outputFilePath << std::endl;
        writetoFile(bw64File, admDocument, chnaChunk, outputFilePath);
    }
    return 0;
}
