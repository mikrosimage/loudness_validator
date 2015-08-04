%module ploud

%module(directors="1") ploud

%include "std_string.i"
%include "std_vector.i"

// Create instantiations of a template classes
%template(IntVector) std::vector< int >;
%template(FloatVector) std::vector< float >;

%{
#include <loudnessAnalyser/LoudnessAnalyser.hpp>
#include <loudnessAnalyser/Filter.hpp>
#include <loudnessAnalyser/Histogram.hpp>
#include <loudnessAnalyser/Loudness.hpp>
#include <loudnessAnalyser/Process.hpp>
#include <loudnessAnalyser/TruePeakMeter.hpp>
%}

%include <loudnessAnalyser/LoudnessAnalyser.hpp>
%include <loudnessAnalyser/Filter.hpp>
%include <loudnessAnalyser/Histogram.hpp>
%include <loudnessAnalyser/Loudness.hpp>
%include <loudnessAnalyser/Process.hpp>
%include <loudnessAnalyser/TruePeakMeter.hpp>
