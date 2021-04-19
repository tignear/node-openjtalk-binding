{
    'includes': ["../../common.gypi"],
    "targets": [
        {
            'target_name': "text2mecab",
            'type': "static_library",
            'include_dirs': [
                "./",
            ],
            'sources': [
                "text2mecab.c",
            ]
        }
    ]
}
