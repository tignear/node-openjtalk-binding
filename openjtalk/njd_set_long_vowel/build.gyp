{
    'includes': ["../../common.gypi"],
    "targets": [
        {
            'target_name': "njd_set_long_vowel",
            'type': "static_library",
            'include_dirs': [
                "./",
                "../njd"
            ],
            'sources': [
                "njd_set_long_vowel.c",
            ]
        }
    ]
}
