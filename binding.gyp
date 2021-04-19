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
                "-fexceptions",
            ],
            'xcode_settings': {
                'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
            },
            'msbuild_settings': {
                'ClCompile': {
                    'ExceptionHandling': "Sync",
                }
            }
        }
    ]
}
