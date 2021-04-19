{
    'includes': ["../common.gypi"],
    "targets": [
        {
            'target_name': "htsengine",
            'type': "static_library",
            'include_dirs': [
                "include",
                "lib",
            ],
            'sources': [
                "./lib/HTS_engine.c",
                "./lib/HTS_gstream.c",
                "./lib/HTS_label.c",
                "./lib/HTS_misc.c",
                "./lib/HTS_model.c",
                "./lib/HTS_pstream.c",
                "./lib/HTS_sstream.c",
                "./lib/HTS_vocoder.c"
            ]
        }
    ]
}
