{
    'variables': {
    },
    'targets': [
        {
            'target_name': 'htsengine',
            'type': 'none',
            "conditions": [['OS == "win"', {"actions": [
                {
                    "action_name": "htsengine",
                    "inputs": ["./htsengine/Makefile.mak"],
                    "outputs": ["./htsengine/lib/hts_engine_API.lib"],
                    'action': ["cmd", "/C", "tools/htsengine.cmd"]
                }]}],  # OS == "win"
                ['OS != "win"', {"actions": [
                    {
                        "action_name": "make",
                        "inputs": ["./htsengine/Makefile.am"],
                        "outputs": ["./htsengine/lib/*.o", "./htsengine/lib/*.a"],
                        'action':["sh", "tools/htsengine.sh"]
                    }
                ]}]],  # OS != "win"
        },
        {
            'target_name': 'openjtalk',
            'type': 'none',
            'dependencies': [
                'htsengine',
            ],
            "conditions": [['OS == "win"', {"actions": [
                {
                    "action_name": "openjtalk",
                    "inputs": ["./openjtalk/Makefile.mak"],
                    "outputs": ["./openjtalk/static-lib/open_jtalk.lib"],
                    'action': ["cmd", "/Q" "/C", "./tools/openjtalk.cmd"]
                }]}],  # OS == "win"
                ['OS != "win"', {"actions": [
                    {
                        "action_name": "make",
                        "inputs": ["./openjtalk/Makefile.am"],
                        "outputs": ["./openjtalk/static-lib/libOpenJTalk.a"],
                        'action':["sh", "tools/openjtalk.sh"]
                    }
                ]}]],  # OS != "win"
        },
        {
            "target_name": "addon",
            "sources": ["addon.cc"],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
                "./openjtalk/static-lib",
                "./openjtalk/mecab/src",
                "./openjtalk/njd",
                "./openjtalk/jpcommon",
                "./htsengine/include"
            ],
            "defines": ["NAPI_CPP_EXCEPTIONS"],
            'dependencies': [
                'openjtalk',
            ],
            "cflags_cc":[
                "-std=c++17",
                "-fexceptions",
                "-fPIC",
                "-O2"
            ],
            'msvs_settings': {
                'VCCLCompilerTool': {
                    'AdditionalOptions': ['/std:c++17', "/utf-8", "/D NOMINMAX", "/EHsc","/O2","/Zc:__cplusplus"],
                },
            },
            'xcode_settings': {
                'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                'OTHER_CPLUSPLUSFLAGS': ['-std=c++17', '-stdlib=libc++',"-O2"],
                'OTHER_LDFLAGS': ['-stdlib=libc++'],
                'MACOSX_DEPLOYMENT_TARGET': '10.14',
            },
            "conditions": [['OS == "win"', {"libraries": [
                "<(module_root_dir)/openjtalk/static-lib/open_jtalk.lib",
                "<(module_root_dir)/openjtalk/mecab/src/mecab.lib",
                "<(module_root_dir)/openjtalk/njd/njd.lib",
                "<(module_root_dir)/openjtalk/jpcommon/jpcommon.lib",
                "<(module_root_dir)/htsengine/lib/hts_engine_API.lib",
                "<(module_root_dir)/openjtalk/text2mecab/text2mecab.lib",
                "<(module_root_dir)/openjtalk/mecab2njd/mecab2njd.lib",
                "<(module_root_dir)/openjtalk/njd_set_pronunciation/njd_set_pronunciation.lib",
                "<(module_root_dir)/openjtalk/njd_set_digit/njd_set_digit.lib",
                "<(module_root_dir)/openjtalk/njd_set_accent_phrase/njd_set_accent_phrase.lib",
                "<(module_root_dir)/openjtalk/njd_set_accent_type/njd_set_accent_type.lib",
                "<(module_root_dir)/openjtalk/njd_set_unvoiced_vowel/njd_set_unvoiced_vowel.lib",
                "<(module_root_dir)/openjtalk/njd_set_long_vowel/njd_set_long_vowel.lib",
                "<(module_root_dir)/openjtalk/njd2jpcommon/njd2jpcommon.lib"
            ]}],  # OS == "win"
                ['OS != "win"', {"libraries": [
                    "<(module_root_dir)/openjtalk/static-lib/libOpenJTalk.a",
                    "<(module_root_dir)/openjtalk/mecab/src/libmecab.a",
                    "<(module_root_dir)/openjtalk/njd/libnjd.a",
                    "<(module_root_dir)/openjtalk/jpcommon/libjpcommon.a",
                    "<(module_root_dir)/htsengine/lib/libHTSEngine.a",
                    "<(module_root_dir)/openjtalk/text2mecab/libtext2mecab.a",
                    "<(module_root_dir)/openjtalk/mecab2njd/libmecab2njd.a",
                    "<(module_root_dir)/openjtalk/njd_set_pronunciation/libnjd_set_pronunciation.a",
                    "<(module_root_dir)/openjtalk/njd_set_digit/libnjd_set_digit.a",
                    "<(module_root_dir)/openjtalk/njd_set_accent_phrase/libnjd_set_accent_phrase.a",
                    "<(module_root_dir)/openjtalk/njd_set_accent_type/libnjd_set_accent_type.a",
                    "<(module_root_dir)/openjtalk/njd_set_unvoiced_vowel/libnjd_set_unvoiced_vowel.a",
                    "<(module_root_dir)/openjtalk/njd_set_long_vowel/libnjd_set_long_vowel.a",
                    "<(module_root_dir)/openjtalk/njd2jpcommon/libnjd2jpcommon.a"
                ]}]],
        }
    ]
}
