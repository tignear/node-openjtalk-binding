# node-openjtalk-binding [![npm version](https://badge.fury.io/js/node-openjtalk-binding.svg)](https://badge.fury.io/js/node-openjtalk-binding)
Simple binding of OpenJTalk.

## Requirements
- Able to run [node-gyp](https://github.com/nodejs/node-gyp#installation)
- Able to run automake.
  - If you use Linux or Mac OS.
- c++ 14 can be compiled. 
- If you use Mac OS, 10.14 or later is required because std::get cannot use before 10.14.

## Install 
```sh
#npm
npm install node-openjtalk-binding
#yarn
yarn add node-openjtalk-binding
```

## Example

```js
const { synthesis, dictionary_dir } = require("node-openjtalk-binding");
const path = require("path");
const { promises: fs } = require("fs");
synthesis("竹やぶ焼けた。", {
  htsvoice: path.resolve(__dirname, "hts_voice_nitech_jp_atr503_m001-1.05", "nitech_jp_atr503_m001.htsvoice"),
  dictionary: dictionary_dir,
}).then(wave => {
  const wav = Buffer.alloc(wave.data.byteLength + 44);
  createWAV(new DataView(wav.buffer), wave);
  return fs.writeFile(path.resolve(__dirname, "example.wav"), wav);
});

/**
 * 
 * @param {DataView} view 
 * @param {import("node-openjtalk-binding").WaveObject} wave
 */
function createWAV(view, wave) {
  const blockSize = wave.numChannels * wave.bitDepth / 8;
  view.setUint32(0, 0x52494646);//"RIFF"
  view.setUint32(4, wave.data.byteLength + 44 - 8, true);
  view.setUint32(8, 0x57415645);//"WAVE"
  view.setUint32(12, 0x666D7420);//"fmt "
  view.setUint32(16, 16, true);//16(LE)
  view.setUint16(20, 1, true);//LINEAR PCM
  view.setUint16(22, wave.numChannels, true);
  view.setUint32(24, wave.sampleRate, true);
  view.setUint32(28, wave.sampleRate * blockSize, true);
  view.setUint16(32, blockSize, true);
  view.setUint16(34, wave.bitDepth, true);
  view.setUint32(36, 0x64617461);//"data"
  view.setUint32(40, wave.data.byteLength, true);
  let i = 44;
  for (const x of wave.data) {
    view.setInt16(i, x, true);
    i += 2;
  }
}
```

## HTSVoice
[Google it!](https://www.google.com/search?q=htsvoice)