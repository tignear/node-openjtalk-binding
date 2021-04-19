{
    'includes': ["../../common.gypi"],
    "targets": [
        {
            'target_name': "njd_set_accent_phrase",
            'type': "static_library",
            'include_dirs': [
                "./",
                "../njd"
            ],
            'sources': [
                "njd_set_accent_phrase.c",
            ]
        }
    ]
}
