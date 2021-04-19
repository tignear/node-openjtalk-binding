{
    'target_defaults': {
        'msbuild_settings': {
            'ClCompile': {
                'AdditionalOptions': ['/std:c++17', "/utf-8", "/Ob3", "/Zc:__cplusplus", "/DNDEBUG"],
                'PreprocessorDefinitions': ["NOMINMAX"],
                'ExceptionHandling': "Sync",
                'Optimization': "MaxSpeed",
                'IntrinsicFunctions': 'true',
                'FavorSizeOrSpeed': 'Speed',
                'OmitFramePointers': 'true',
                'EnableFiberSafeOptimizations': 'true',
                'WholeProgramOptimization': 'true'
            },
            'Link': {
                'LinkTimeCodeGeneration': 'UseLinkTimeCodeGeneration',  # /LTCG
            },
            'Lib': {
                'LinkTimeCodeGeneration': 'true',  # /LTCG
            }
        },
        'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
            'OTHER_CPLUSPLUSFLAGS': ['-std=c++17', '-stdlib=libc++', "-O3"],
            'MACOSX_DEPLOYMENT_TARGET': '10.14',
        },
        "cflags_cc": [
            "-std=c++17",
            "-fPIC",
            "-O3",
            "-fexceptions",
        ],
    },

}
