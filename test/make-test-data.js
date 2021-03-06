const { synthesis } = require("../addon");
const path = require("path");
const { promises: fs } = require("fs");
fs.readFile(path.resolve(__dirname, "./test.txt"), "utf-8").then(text =>
  synthesis(text, {
    htsvoice: path.resolve(__dirname, "../", "hts_voice_nitech_jp_atr503_m001-1.05", "nitech_jp_atr503_m001.htsvoice"),
  })).then(wave =>{
    const wav = Buffer.alloc(wave.data.byteLength + 44);
    createWAV(new DataView(wav.buffer), wave);
    fs.writeFile(path.resolve(__dirname, "test.wav"), wav);
    const bin = Buffer.alloc(wave.data.byteLength);
    let i = 0;
    for(const d of wave.data){
      bin.writeInt16LE(d,i);
      i+=2;
    }
    fs.writeFile(path.resolve(__dirname, "test.bin"), bin);
  });

/**
 * 
 * @param {DataView} view 
 * @param {import("../addon").WaveObject} wave
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