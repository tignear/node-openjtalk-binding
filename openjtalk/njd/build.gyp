{
    'includes': ["../../common.gypi"],
    "targets": [
        {
            'target_name': "njd",
            'type': "static_library",
            'include_dirs': [
                "./",
            ],
            'sources': [
                "njd.c",
                "njd_node.c"
            ]
        }
    ]
}
