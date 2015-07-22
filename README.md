# PLoud Validator

PLoudValidator is a software to validate audio files with correct loudness.
It can validate various specifications:

* EBU R 128
* ATSC A85
* CST-Ficam RT-017 specification for France.

It can also correct file to EBU R 128.
Comming soon for others specifications.


## License

See [**COPYING.md**](COPYING.md)


## Compilation

#### Getting the source

```
git clone git://github.com/mikrosimage/loudness_validator.git  
cd loudness_validator
```

#### Compilation

###### Build all tools in release mode.
```
scons
```

###### Build in debug mode.
```
scons mode=debug
```

###### Build with a specific path to libraries.
```
scons BOOST_ROOT=/path/to/boost SNDFILE_ROOT=/path/to/sndfile
```


## Run applications

#### Analyser
```
./build/release/loudness-analyser
```

Analyse the loudness of the given file.

#### Corrector
```
./build/release/loudness-corrector
```

Correct the loudness of the given file.

#### Validator
```
./build/release/loudness-validator
```

A GUI which enables to analyse, visualize and correct the loudness.


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


## Dependencies

#### Libraries
libloudnessAnalyser and loudnessTools require libsndfile and boost accumulators.

#### Apps
loudnessValidator requires also Qt4.

[libsndfile](http://www.mega-nerd.com/libsndfile/)  
[Qt4](http://qt.nokia.com/products/)  


## More information 

#### EBU R-128  
[open link](http://tech.ebu.ch/loudness)

#### ATSC A85  
[download pdf](www.atsc.org/cms/standards/a_85-2011a.pdf)

#### France CST-Ficam RT-017 specification  
[download pdf](http://www.arpp-pub.org/IMG/pdf/140911_-_Pub_TV_et_intensite_sonore_v-def-2.pdf)

#### EBU test material  
[download zip archive](http://tech.ebu.ch/webdav/site/tech/shared/testmaterial/ebu-loudness-test-setv03.zip)
