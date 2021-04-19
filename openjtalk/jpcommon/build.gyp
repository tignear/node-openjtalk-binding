{
    'includes': ["../../common.gypi"],
    "targets": [
        {
            'target_name': "jpcommon",
            'type': "static_library",
            'include_dirs': [
                "./",
            ],
            'sources': [
                "jpcommon_label.c",
                "jpcommon_node.c",
                "jpcommon.c",
            ]
        }
    ]
}
