EnsureSConsVersion( 2, 3, 0 )

loudnessAssessmentVersionMajor = "0"
loudnessAssessmentVersionMinor = "0"
loudnessAssessmentVersionMicro = "1"

loudnessAssessmentVersion = [
    loudnessAssessmentVersionMajor,
    loudnessAssessmentVersionMinor,
    loudnessAssessmentVersionMicro ]

loudnessAssessmentVersionStr = ".".join( loudnessAssessmentVersion )

buildMode = ARGUMENTS.get( 'mode', 'release' )
if not ( buildMode in [ 'debug', 'release' ] ) :
        raise Exception( "Can't select build mode ['debug', 'release']" )

# get libsndfile install path
sndfile_root = ARGUMENTS.get( 'SNDFILE_ROOT', '' )
sndfile_include = ''
sndfile_lib = ''
if sndfile_root:
    sndfile_include = sndfile_root + '/include'
    sndfile_lib = sndfile_root + '/lib'

# get libboost install path
boost_root = ARGUMENTS.get( 'BOOST_ROOT', '' )
boost_include = ''
boost_lib = ''
if boost_root:
    boost_include = boost_root + '/include'

env = Environment()

env.Append(
        CPPPATH = [
                '#src',
                sndfile_include,
                boost_include,
        ],
        CXXFLAGS = [
                '-Wall',
                '-fPIC',
                '-DLOUDNESS_ASSESSMENT_VERSION_MAJOR=' + loudnessAssessmentVersionMajor,
                '-DLOUDNESS_ASSESSMENT_VERSION_MINOR=' + loudnessAssessmentVersionMinor,
                '-DLOUDNESS_ASSESSMENT_VERSION_MICRO=' + loudnessAssessmentVersionMicro,
        ],
        LIBPATH = [
                '#src',
                sndfile_lib,
        ],
        SHLIBVERSION = loudnessAssessmentVersionStr,
        )

if buildMode == 'release':
    env.Append( CXXFLAGS=['-O3'] )
else:
    env.Append( CXXFLAGS=['-g'] )

Export( 'env' )

VariantDir( 'build/' + buildMode + '/src', 'src', duplicate = 0 )
VariantDir( 'build/' + buildMode + '/app', 'app', duplicate = 0 )

SConscript( 'src/SConscript', variant_dir = 'build/' + buildMode + '/src' )
SConscript( 'app/SConscript', variant_dir = 'build/' + buildMode + '/app' )

