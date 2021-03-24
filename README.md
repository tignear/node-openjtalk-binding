# node-openjtalk-binding
Simple binding of OpenJTalk.

## Requirements
- [node-gyp](https://github.com/nodejs/node-gyp#installation)
- automake
- c++17 support

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
 * @param {import("../addon").WaveObject} wave
 */
function createWAV(view, wave) {
  view.setUint32(0, 0x52494646);//"RIFF"
  view.setUint32(4, wave.data.byteLength + 44 - 8, true);
  view.setUint32(8, 0x57415645);//"WAVE"
  view.setUint32(12, 0x666D7420);//"fmt "
  view.setUint32(16, 16, true);//16(LE)
  view.setUint16(20, 1, true);//LINEAR PCM
  view.setUint16(22, 1, true);//monaural
  view.setUint32(24, 48000, true);//48kHz
  view.setUint32(28, 48000, true);
  view.setUint16(32, 2, true);
  view.setUint16(34, 16, true);
  view.setUint32(36, 0x64617461);//"data"
  view.setUint32(40, wave.data.byteLength, true);
  let i = 44;
  for (const x of wave.data) {
    view.setInt16(i, x, true);
    i += 2;
  }
}
```