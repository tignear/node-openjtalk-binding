{
    'includes': ["../../common.gypi"],
    "targets": [
        {
            'target_name': "njd2jpcommon",
            'type': "static_library",
            'include_dirs': [
                "./",
                "../jpcommon",
                "../njd"
            ],
            'sources': [
                "njd2jpcommon.c",
            ]
        }
    ]
}
