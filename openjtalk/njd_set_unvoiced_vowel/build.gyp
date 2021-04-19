{
    'includes': ["../../common.gypi"],
    "targets": [
        {
            'target_name': "njd_set_unvoiced_vowel",
            'type': "static_library",
            'include_dirs': [
                "./",
                "../njd"
            ],
            'sources': [
                "njd_set_unvoiced_vowel.c",
            ]
        }
    ]
}
