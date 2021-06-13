{
    'includes': ["common.gypi"],
    'variables': {
        "conditions": [
            ['OS == "win"', {
                "STATIC_LIB_SUFFIX%": ".lib",
                "STATIC_LIB_PREFIX%": "",
            }],
            ['OS != "win"', {
                "STATIC_LIB_SUFFIX%": ".a",
                "STATIC_LIB_PREFIX%": "lib",
            }]
        ]
    },
    'targets': [
        {
            'target_name': 'openjtalk',
            'type': 'none',
            'dependencies': [
                "openjtalk/jpcommon/build.gyp:*",
                "openjtalk/mecab/build.gyp:mecab",
                "openjtalk/mecab2njd/build.gyp:*",
                "openjtalk/njd/build.gyp:*",
                "openjtalk/njd_set_accent_phrase/build.gyp:*",
                "openjtalk/njd_set_accent_type/build.gyp:*",
                "openjtalk/njd_set_digit/build.gyp:*",
                "openjtalk/njd_set_long_vowel/build.gyp:*",
                "openjtalk/njd_set_pronunciation/build.gyp:*",
                "openjtalk/njd_set_unvoiced_vowel/build.gyp:*",
                "openjtalk/njd2jpcommon/build.gyp:*",
                "openjtalk/njd_set_unvoiced_vowel/build.gyp:*",
                "openjtalk/text2mecab/build.gyp:*",
                "openjtalk/open_jtalk/build.gyp:*",
                "openjtalk/mecab-naist-jdic/build.gyp:*",
            ]
        },
        {
            "target_name": "addon",
            "sources": ["addon.cc"],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
                "./openjtalk/open_jtalk",
                "./openjtalk/mecab/src",
                "./openjtalk/njd",
                "./openjtalk/jpcommon",
                "./htsengine/include"
            ],
            "defines": ["NAPI_CPP_EXCEPTIONS"],
            'dependencies': [
                'openjtalk', "htsengine/build.gyp:*"
            ],
            "libraries": [
                "<(PRODUCT_DIR)/open_jtalk<(STATIC_LIB_SUFFIX)",
                "<(PRODUCT_DIR)/mecab<(STATIC_LIB_SUFFIX)",
                "<(PRODUCT_DIR)/njd<(STATIC_LIB_SUFFIX)",
                "<(PRODUCT_DIR)/jpcommon<(STATIC_LIB_SUFFIX)",
                "<(PRODUCT_DIR)/htsengine<(STATIC_LIB_SUFFIX)",
                "<(PRODUCT_DIR)/text2mecab<(STATIC_LIB_SUFFIX)",
                "<(PRODUCT_DIR)/mecab2njd<(STATIC_LIB_SUFFIX)",
                "<(PRODUCT_DIR)/njd_set_pronunciation<(STATIC_LIB_SUFFIX)",
                "<(PRODUCT_DIR)/njd_set_digit<(STATIC_LIB_SUFFIX)",
                "<(PRODUCT_DIR)/njd_set_accent_phrase<(STATIC_LIB_SUFFIX)",
                "<(PRODUCT_DIR)/njd_set_accent_type<(STATIC_LIB_SUFFIX)",
                "<(PRODUCT_DIR)/njd_set_unvoiced_vowel<(STATIC_LIB_SUFFIX)",
                "<(PRODUCT_DIR)/njd_set_long_vowel<(STATIC_LIB_SUFFIX)",
                "<(PRODUCT_DIR)/njd2jpcommon<(STATIC_LIB_SUFFIX)"
            ],
            "cflags_cc":[
                "-std=c++17",
                "-fPIC",
                "-O3",
                "-fexceptions",
            ],
            'xcode_settings': {
                'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                'OTHER_CPLUSPLUSFLAGS': ['-std=c++17', '-stdlib=libc++', "-O3"],
                'MACOSX_DEPLOYMENT_TARGET': '10.14',
            },
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
        },
        {  # https://github.com/mapbox/node-pre-gyp#2-add-a-new-target-to-bindinggyp
            "target_name": "action_after_build_node",
            "type": "none",
            "dependencies": ["addon"],
            "copies": [
                {
                    "files": [
                        "<(PRODUCT_DIR)/addon.node",
                    ],
                    "destination": "<(module_path)"
                }
            ]
        },
        {
            "target_name": "action_after_build_dic",
            "type": "none",
            "dependencies": ["openjtalk"],
            "copies": [
                {
                    "files": [
                        "openjtalk/mecab-naist-jdic/char.bin",
                        "openjtalk/mecab-naist-jdic/matrix.bin",
                        "openjtalk/mecab-naist-jdic/sys.dic",
                        "openjtalk/mecab-naist-jdic/unk.dic", ],
                    "destination": "<(module_path)/dictionary"
                }
            ]

        }
    ]
}
