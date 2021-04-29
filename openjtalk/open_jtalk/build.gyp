{
    'includes': ["../../common.gypi"],
    "targets": [
        {
            'target_name': "open_jtalk",
            'type': "static_library",
            'include_dirs': [
                "../text2mecab",
                "../mecab/src",
                "../mecab2njd",
                "../njd",
                "../njd_set_pronunciation",
                "../njd_set_digit",
                "../njd_set_accent_phrase",
                "../njd_set_accent_type",
                "../njd_set_unvoiced_vowel",
                "../njd_set_long_vowel",
                "../njd2jpcommon",
                "../jpcommon",
                "../../htsengine/include"
            ],
            'sources': [
                "open_jtalk.cpp",
            ]
        }
    ]
}
