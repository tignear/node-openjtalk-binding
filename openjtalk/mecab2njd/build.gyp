{
    'includes': ["../../common.gypi"],
    "targets": [
        {
            'target_name': "mecab2njd",
            'type': "static_library",
            'include_dirs': [
                "./",
                "../njd"
            ],
            'sources': [
                "mecab2njd.c",
            ]
        }
    ]
}
