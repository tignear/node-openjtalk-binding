{
    'includes': ["../../common.gypi"],
    "targets": [
        {
            'target_name': "mecab",
            'type': "static_library",
            'sources': [
                    "src/char_property.cpp",
                    "src/connector.cpp",
                    "src/context_id.cpp",
                    "src/dictionary.cpp",
                    "src/dictionary_compiler.cpp",
                    "src/dictionary_generator.cpp",
                    "src/dictionary_rewriter.cpp",
                    "src/eval.cpp",
                    "src/feature_index.cpp",
                    "src/lbfgs.cpp",
                    "src/learner.cpp",
                    "src/learner_tagger.cpp",
                    "src/libmecab.cpp",
                    "src/mecab.cpp",
                    "src/nbest_generator.cpp",
                    "src/param.cpp",
                    "src/string_buffer.cpp",
                    "src/tagger.cpp",
                    "src/tokenizer.cpp",
                    "src/utils.cpp",
                    "src/viterbi.cpp",
                    "src/writer.cpp"
            ],
            'defines': [
                "MECAB_WITHOUT_MUTEX_LOCK",
                'DIC_VERSION=102',
                'VERSION="1.01"',
                'PACKAGE="open_jtalk"',
                'MECAB_DEFAULT_RC="dummy"',
                "HAVE_STRING_H"
            ],
            'conditions': [
                [
                    'OS == "win"', {
                        'defines': [
                            "HAVE_WINDOWS_H"
                        ]
                    }
                ],
                [
                    'OS != "win"', {
                        'defines': [
                            "HAVE_MMAP",
                            "HAVE_SYS_MMAN_H",
                            "HAVE_UNISTD_H",
                            "HAVE_SYS_STAT_H",
                            "HAVE_FCNTL_H",
                            "HAVE_DIRENT_H"
                        ]
                    }
                ]
            ],
            'include_dirs': [
                "./src",
            ],
        },
        {
            'target_name': "mecab-dict-index",
            'type': "executable",
            'dependencies': ["mecab"],
            "libraries": [
                "<(PRODUCT_DIR)/mecab<(STATIC_LIB_SUFFIX)",
            ],
            "conditions": [
                ['OS == "win"', {
                    "libraries": ["Advapi32.lib"]
                }]
            ],
            'sources': [
                "src/mecab-dict-index.cpp",
            ]
        },
    ]
}
