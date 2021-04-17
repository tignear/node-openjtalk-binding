/**
 * Simple binding of OpenJTalk.
 * @module node-openjtalk-binding
 */

const { synthesis: _synthesis } = require("bindings")("addon");
const path = require("path");
const dictionary = path.resolve(__dirname, "openjtalk", "mecab-naist-jdic");
const { promises: fs } = require("fs");
/**
 * @typedef {Object} OpenJTalkOptions
 * @property {!string|Uint8Array|ArrayBuffer} htsvoice Path to htsvoice. Or data ArrayBuffer,Buffer.
 * @property {?string} dictionary Path to dictionary. NOT be URL nor Buffer. Must be encoded by UTF-8. The default is to use dictionary_dir.
 * @property {?number} sampling_frequency Must be int. 1<=sampling_frequency.
 * @property {?number} frame_period Must be int. 1<=frame_period.
 * @property {?number} all_pass_constant 0.0<=all_pass_constant<=1.0.
 * @property {?number} postfiltering_coefficient Default is 0.0. 0.0<=postfiltering_coefficient<=1.0.
 * @property {?number} speech_speed_rate Default is 1.0. 0<=speech_speed_rate. Warning: Do not set a very small value as it consumes CPU time.
 * @property {?number} additional_half_tone Default is 0.0.
 * @property {?number} voiced_unvoiced_threshold Default is 0.5. 0.0<=voiced_unvoiced_threshold<=1.0.
 * @property {?number} weight_of_GV_for_spectrum Default is 1.0. 0.0<=weight_of_GV_for_spectrum.
 * @property {?number} weight_of_GV_for_log_F0 Default is 1.0. 0.0<=weight_of_GV_for_log_F0.
 * @property {?number} volume_in_dB Default is 0.0.
 * @property {?number} audio_buffer_size Disabled as default. 0 regarded disabled. 0<=audio_buffer_size
 */

/**
 * @typedef {Object} WaveObject
 * @property {!Buffer} raw_data  Synthesized PCM by host byte order. 
 * @property {!Int16Array} data Synthesized PCM.
 * @property {!16} bitDepth LINEAR16.
 * @property {!number} sampleRate Equals to OpenJTalkOptions#sampling_frequency if presented for synthesis function. Else automatically determined value.
 * @property {!1} numChannels monaural.
 */

/**
 * Synthesis voice with OpenJTalk
 * @async
 * @param {string} text Text to synthesize.
 * @param {OpenJTalkOptions} options OpenJTalk synthesize option.
 * @return {Promise<WaveObject>} Synthesized PCM.
 */
exports.synthesis = async function synthesis(text, options) {
  let htsvoice = options.htsvoice;
  if (typeof htsvoice === "string") {
    htsvoice = await fs.readFile(htsvoice);
  }
  if (htsvoice instanceof Uint8Array) {
    htsvoice = htsvoice.buffer;
  }
  return new Promise((resolve, reject) => {
    function cb(err, /** @type {Buffer} */ buffer, /** @type {number} */ sampleRate) {
      if (err) {
        reject(err);
        return;
      }
      /**
       * @type {WaveObject}
       */
      const wave = {
        raw_data: buffer,
        data: new Int16Array(buffer.buffer),
        bitDepth: 16,
        numChannels: 1,
        sampleRate
      };
      resolve(wave);
    }
    try {
      _synthesis(cb, text, { dictionary, ...options, htsvoice });
    } catch (err) {
      reject(err);
    }
  });
}

/** 
 * Path to builded dictionary.
 * @type {string} 
 * */
exports.dictionary_dir = dictionary;