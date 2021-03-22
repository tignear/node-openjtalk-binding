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
                        "inputs": ["./htsengine"],
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
                    "outputs": ["./openjtalk/bin/open_jtalk.exe"],
                    'action': ["cmd","/Q" "/C", "./tools/openjtalk.cmd"]
                }]}],  # OS == "win"
                ['OS != "win"', {"actions": [
                    {
                        "action_name": "make",
                        "inputs": ["./openjtalk"],
                        "outputs": ["./openjtalk/lib/*.o", "./openjtalk/lib/*.a"],
                        'action':["sh", "tools/openjtalk.sh"]
                    }
                ]}]],  # OS != "win"
        },
    ]
}
