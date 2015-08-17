# Compilation

## Getting the source

```
git clone git://github.com/mikrosimage/loudness_validator.git  
cd loudness_validator
```


## Build

#### Build all tools in release mode
```
scons
```

#### More build info ('Local Options' section)
```
scons --help
```


## Dependencies

#### Libraries

* __loudnessAnalyser__  
Depends on boost accumulators.

* __loudnessCorrector__  
Depends on loudnessAnalyser and boost accumulators.

* __loudnessTools__  
Depends on loudnessAnalyser, loudnessCorrector, and [libsndfile](http://www.mega-nerd.com/libsndfile/).


#### Apps

* __loudness-analyser__  
Depends on loudnessAnalyser and loudnessTools.

* __loudness-corrector__  
Depends on loudnessCorrector and loudnessTools.

* __loudness-validator__  
Depends on loudnessAnalyser, loudnessCorrector, loudnessTools and [Qt4](http://qt.nokia.com/products/).


## Tested compilers

* GCC 4.4.1 on OpenSUSE 11.2
* GCC 4.6.2 on OpenSUSE 11.2
* CLang 3.1 on OpenSUSE 11.2

* GCC 4.5.2 on Windows XP


## Tested libraries

* Qt 4.5.3  
* Qt 4.8.6

* libsndfile 1.0.24-22.2
* libsndfile 1.0.25
